//
// Copyright 2018 Michael F. Herbst
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once
#include "CtxMapIterator.hh"
#include "exceptions.hh"

namespace ctx {

/** CtxMap implements a map from a std::string to objects of arbitrary
 *  type.
 *
 *  This way an arbitrary amount of arbitrary objects can be passed
 *  around using this object and they can be quickly accessed by the
 *  std::string key.
 *
 *  The functionality in this class deliberately resembles Python
 *  dictionaries to a large extent.
 *
 *  In the implementation of the CtxMap, the character '/' has
 *  a special meaning as it allows to access a submap, so "a/b/c",
 *  fills an object into the entry c in submap b of the submap a.
 *  See the submap function for some more details.
 */
class CtxMap {
 public:
  /** Custom comparator to sort key strings. Makes sure that slashes "/"
   *  sort before any other character. */
  struct key_comparator_type {
    bool operator()(const std::string& x, const std::string& y) const;
    typedef std::string first_argument_type;
    typedef std::string second_argument_type;
    typedef bool result_type;
  };

  typedef CtxMapValue entry_value_type;
  typedef std::map<std::string, entry_value_type, key_comparator_type> map_type;
  typedef std::pair<const std::string, entry_value_type> entry_type;
  typedef CtxMapIterator<true> const_iterator;
  typedef CtxMapIterator<false> iterator;

  /** \name Constructors, destructors and assignment */
  ///@{
  /** \brief default constructor
   * Constructs empty map */
  CtxMap() : m_container_ptr{std::make_shared<map_type>()}, m_location{""} {}

  /** \brief Construct parameter map from initialiser list of entry_types */
  CtxMap(std::initializer_list<entry_type> il) : CtxMap{} { update(il); };

  ~CtxMap()        = default;
  CtxMap(CtxMap&&) = default;

  /** \brief Copy constructor
   *
   * Note that the copy is deep, but only in the sense that the pointers to
   * the mapped data is copied, but not the actual data itself.
   *
   * This manifests in the different behaviour of the ``update`` versus the
   * ``at`` function, because the former updates the actual pointer and the
   * latter updates the data referred to by the pointer.
   *
   * Consider as an example:
   * ```
   * CtxMap map{"a", 1};
   * CtxMap copy(map);
   * copy.update("a", 42);
   *
   * std::cout << map.at<int>("a");
   * std::cout << copy.at<int>("a");
   * ```
   * This will print 1 and then 42, but
   * ```
   * CtxMap map{"a", 1};
   * CtxMap copy(map);
   * copy.at<int>("a") = 42;
   *
   * std::cout << map.at<int>("a");
   * std::cout << copy.at<int>("a");
   * ```
   * will print 42 twice.
   * */
  CtxMap(const CtxMap& other);

  /** \brief Assignment operator */
  CtxMap& operator=(CtxMap other);
  ///@}

  /** \name Modifiers */
  ///@{
  /** \brief Insert or update a key.
   *
   * All objects which can be implicitly converted to an EntryValue can be used.
   * This includes:
   *   - Cheaply copyable types (numbers, std::string, enums)
   *   - RValue references
   *   - LValue references of subscribable objects
   *   - Shared pointers
   */
  void update(const std::string& key, entry_value_type e) {
    (*m_container_ptr)[make_full_key(key)] = std::move(e);
  }

  /** \brief Update many entries using an initialiser list
   *
   * TODO More details, have an example
   * */
  void update(std::initializer_list<entry_type> il);

  /** \brief Update many entries using another CtxMap
   *
   * The entries are updated relative to the given key paths.
   * I.e. if key == "blubber" and the map \t map contairs "foo" and
   * "bar", then "blubber/foo" and "blubber/bar" will be updated.
   *
   * Note that the update involves a deep copy of the referring pointers,
   * but not a copy of the actual data in memory. See the details of
   * the copy-constructor for consequences as it involves such a deep
   * copy of the map data as well.
   *
   * Roughly speaking modification of entries via ``at`` affects both
   * ``this`` and ``map``, wherease modification of entries via ``update``
   * only effects the CtxMap object on which the method is called.
   * */
  void update(const std::string& key, const CtxMap& other);

  /** \brief Update many entries using another CtxMap
   *
   * The entries are updated relative to the given key paths.
   * I.e. if key == "blubber" and the map \t map contairs "foo" and
   * "bar", then "blubber/foo" and "blubber/bar" will be updated.
   * */
  void update(const std::string& key, CtxMap&& other);

  /** \brief Update many entries using another CtxMap
   *
   * The entries are updated in paths relative to /
   * */
  void update(const CtxMap& other) { update("/", other); }

  /** \brief Update many entries using another CtxMap
   *
   * The entries are updated in paths relative to /
   */
  void update(CtxMap&& other) { update("/", std::move(other)); }

  /** Insert or update a key with a copy of an element */
  template <typename T>
  void update_copy(std::string key, T object) {
    (*m_container_ptr)[make_full_key(key)] =
          entry_value_type{std::make_shared<T>(object)};
  }

  /** Insert a default value for a key, i.e. no existing key will be touched,
   * only new ones inserted (That's why the method is still const)
   */
  void insert_default(const std::string& key, entry_value_type e) const {
    auto itkey = m_container_ptr->find(make_full_key(key));
    if (itkey == std::end(*m_container_ptr)) {
      // Key not found, hence insert default.
      (*m_container_ptr)[make_full_key(key)] = std::move(e);
    }
  }

  /** Insert default values for many entries at once using an initialiser list.
   *
   * Only inserts values that do not already exist in the map
   * (That's why the method is const)
   */
  void insert_default(std::initializer_list<entry_type> il) const {
    for (entry_type t : il) {
      insert_default(t.first, t.second);
    }
  }

  /** \brief Try to remove an element
   *  which is referenced by this string
   *
   *  \return The number of removed elements (i.e. 0 or 1)
   **/
  size_t erase(const std::string& key) {
    return m_container_ptr->erase(make_full_key(key));
  }

  /** \brief Try to remove an element referenced by a key iterator
   *
   *  \return The iterator referencing the key *after* the last
   *          element removed
   **/
  iterator erase(iterator position) {
    // Extract actual map iterator by converting to it explictly:
    typedef map_type::iterator mapiter;
    auto pos_conv = static_cast<typename map_type::iterator>(position);
    mapiter res   = m_container_ptr->erase(pos_conv);
    return iterator(std::move(res), m_location);
  }

  /** \brief Try to remove a range of elements
   *
   *  \return The iterator referencing the key *after* the last
   *          element removed
   **/
  iterator erase(iterator first, iterator last) {
    // Extract actual map iterator by converting to it explictly:
    typedef map_type::iterator mapiter;
    auto first_conv = static_cast<typename map_type::iterator>(first);
    auto last_conv  = static_cast<typename map_type::iterator>(last);
    mapiter res     = m_container_ptr->erase(first_conv, last_conv);
    return iterator(std::move(res), m_location);
  }

  /** \brief Try to remove a full submap path including all
   *         child key entries.
   *
   *  \note  The function is equivalent to ``this->submap(path).clear()``.
   *  \return The number of key-value entries removed from the map
   */
  void erase_recursive(const std::string& path) { erase(begin(path), end(path)); }

  /** Remove all elements from the map
   *
   * \note This takes the location of submaps into account,
   * i.e. only the elements of the submap are deleted and
   * not all elements of the parent. */
  void clear();
  ///@}

  /** \name Obtaining elements and pointers to elements */
  ///@{
  /** Return a reference to the value at a given key
   *  with the specified type.
   *
   * If the value cannot be found an std::out_of_range is thrown.
   * If the type requested is wrong the program is aborted.
   *
   * \note This directly modifies the data in memory, so all
   * CtxMap objects which internally refer to this data
   * will be changed by modifying this value as well. This not
   * only includes submaps, but also deep copies of this
   * CtxMap either done by ``update(std::string, CtxMap)``
   * or by the copy constructor. See the documentation of the
   * copy constructor for details.
   */
  template <typename T>
  T& at(const std::string& key) {
    return at_raw_value(key).get<T>();
  }

  /** \brief Return a reference to the value at a given key
   * with the specified type. (const version)
   * See non-const version for details.
   */
  template <typename T>
  const T& at(const std::string& key) const {
    return at_raw_value(key).get<T>();
  }

  /** \brief Get the value of an element.
   *
   * If the key cannot be found, returns the provided reference instead.
   * If the type requested is wrong the program is aborted.
   */
  template <typename T>
  T& at(const std::string& key, T& default_value);

  /** \brief Get the value of an element (const version).
   * See non-const version for details.
   */
  template <typename T>
  const T& at(const std::string& key, const T& default_value) const;

  /** \brief Return a pointer to the value of a specific key.
   *
   * The returned object is a RCPWrapper, meaning that it internally
   * may contain a shared pointer, but it may also contain a
   * SubscriptionPointer. It can be used as if it was a normal pointer,
   * but in order to extract the actual shared_ptr out of it an explicit
   * cast to the appropriate shared_ptr type is required.
   *
   * In case this is not possible (i.e. it does contain a
   * SubscriptionPointer instead) an exception is raised in Debug mode.
   * Use the contains_shared_ptr() function to check this.
   */
  template <typename T>
  std::shared_ptr<T> at_ptr(const std::string& key) {
    return at_raw_value(key).get_ptr<T>();
  }

  /** Return a pointer to the value of a specific key. (const version)
   *
   * See non-const version for details
   */
  template <typename T>
  std::shared_ptr<const T> at_ptr(const std::string& key) const {
    return at_raw_value(key).get_ptr<T>();
  }

  //@{
  /** \brief Get the pointer to the value of a key or a default.
   *
   * If the key cannot be found, returns the provided pointer instead.
   * If the type requested is wrong the program is aborted.
   */
  template <typename T>
  std::shared_ptr<T> at_ptr(const std::string& key, std::shared_ptr<T> default_ptr);
  //@}

  //@{
  /** \brief Get the pointer to the value of a key or a default. (const version)
   *
   *  See the non-const version for details.
   */
  template <typename T>
  std::shared_ptr<const T> at_ptr(const std::string& key,
                                  std::shared_ptr<const T> default_ptr) const;

  //@}

  /** Return an CtxMapValue object representing the data behind the specified key
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   * */
  CtxMapValue& at_raw_value(const std::string& key) {
    auto itkey = m_container_ptr->find(make_full_key(key));
    if (itkey == std::end(*m_container_ptr)) {
      throw out_of_range("Key '" + key + "' is not known.");
    }
    return itkey->second;
  }

  /** Return an CtxMapValue object representing the data behind the specified key
   * (const version)
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   * */
  const CtxMapValue& at_raw_value(const std::string& key) const {
    auto itkey = m_container_ptr->find(make_full_key(key));
    if (itkey == std::end(*m_container_ptr)) {
      throw out_of_range("Key '" + key + "' is not known.");
    }
    return itkey->second;
  }
  ///@}

  /** Check weather a key exists */
  bool exists(const std::string& key) const {
    return m_container_ptr->find(make_full_key(key)) != std::end(*m_container_ptr);
  }

  /** Return a string which describes the type of the
   * stored data
   */
  std::string type_name_of(const std::string& key) const {
    return at_raw_value(key).type_name();
  }

  /** \name Submaps */
  ///@{
  /** \brief Get a submap starting pointing at a different location.
   *
   * The CtxMap allows the hierarchical organisation of data
   * in the form of UNIX paths. So the key "bla/blubber/foo" actually
   * emplaces a value in the path "/bla/blubber/foo". If one obtains
   * a submap at location "/bla", the same key is now available under
   * the path "/blubber/foo", whereas "/bla/blubber/foo" no longer exists.
   *
   * All supplied paths are subject to the standard UNIX path normalisation
   * in other words "/bla/../bla/blubber/./foo" is equivalent to
   * "/bla/blubber/foo". The leading "/" may be avoided. Hence "bla/blubber/foo"
   * is in turn equal to the aforementioned paths and yields the same
   * entry.
   *
   * Escaping a submap via ".." is not possible. This means that the submap
   * at location "bla" does still not contain a key "../bla/blubber/foo",
   * but it does contain a key "../blubber/foo", since the leading ".."
   * has no effect (we are at the root of the CtxMap)
   *
   * The submap is a shallow copy of a subpath of *this. This means that
   * both the pointer and the referred values are identical. So unlike the
   * copy constructor both ``update`` and ``at`` of both maps
   * work on the same state. I.e.
   * ```
   * CtxMap map{"/tree/a", 1};
   * CtxMap sub = map.submap("tree");
   * sub.update("a", 42);
   *
   * std::cout << map.at<int>("/tree/a");
   * std::cout << sub.at<int>("a");
   * ```
   * will print 42 twice and
   * ```
   * CtxMap map{"/tree/a", 1};
   * CtxMap sub = map.submap("tree");
   * sub.at<int>("a") =  42;
   *
   * std::cout << map.at<int>("a");
   * std::cout << copy.at<int>("a");
   * ```
   * will print 42 twice as well.
   * */
  CtxMap submap(const std::string& location) {
    // Construct new map, but starting at a different location
    return CtxMap{*this, location};
  }

  /** Get a const submap */
  const CtxMap submap(const std::string& location) const {
    // Construct new map, but starting at a different location
    return CtxMap{*this, location};
  }
  ///@}

  /** \name Iterators */
  ///@{
  //@{
  /** Return an iterator to the beginning of the map or the beginning of a
   * specified subpath.
   *
   * The iteration will run over all keys of the subpath including
   * ``path + "/"``, i.e. the root of the subpath itself.
   *
   * \note Equivalent to ``submap(path).begin()`` or ``submap(path).cbegin()``,
   * respectively.
   */
  iterator begin(const std::string& path = "/");
  const_iterator begin(const std::string& path = "/") const { return cbegin(path); }
  const_iterator cbegin(const std::string& path = "/") const;
  //@}

  //@{
  /** Returns the matching end iterator to begin() or cbegin().
   *
   * \note Equivalent to ``submap(path).end()`` or ``submap(path).cend()``,
   * respectively.
   */
  iterator end(const std::string& path = "/");
  const_iterator end(const std::string& path = "/") const { return cend(path); }
  const_iterator cend(const std::string& path = "/") const;
  //@}

  // TODO alias names, i.e. link one name to a different one.
  //      but be careful not to get a cyclic graph.
  //
  // TODO other map operations like
  //        - find ?
  //        - access to data using iterators ?
  //        - erase using iterators ?

 protected:
  /** \brief Construct parameter map from a reference to another CtxMap
   *  and a new location.
   *
   * The newlocation is relative to the original CtxMap ``orig``, since
   * it is processed through ``other.make_full_key()``. In other words this
   * constructor is only suitable to move downwards in the tree and never upwards.
   * Note that this is due to the fact, that make_full_key ignores leading "../".
   *
   * The returned CtxMap is essentially a shallow copy to a subtree
   * of the input map (i.e. a view into a subtree). Since they operate
   * on the same inner data, all changes will be visible to both objects.
   *
   * \note This constructor may remove constness (if newlocation == "/"),
   * so beware how you expose the constructed object to the outside.
   *
   * \note This is an advanced constructor. Use only if you know what you are
   * doing.
   **/
  CtxMap(const CtxMap& other, std::string newlocation)
        : m_container_ptr{other.m_container_ptr},
          m_location{other.make_full_key(newlocation)} {}

 private:
  /** Make the actual container key from a key supplied by the user
   *  Care is taken such that we cannot escape the subtree.
   * */
  std::string make_full_key(const std::string& key) const;

  std::shared_ptr<map_type> m_container_ptr;

  /** The location we are currently on in the tree
   * may not end with a slash (but should be a full key like "/tree")
   */
  std::string m_location;
};

std::ostream& operator<<(std::ostream& o, const CtxMap& map);

//
// -----------------------------------------------------------------
//

template <typename T>
T& CtxMap::at(const std::string& key, T& default_value) {
  auto itkey = m_container_ptr->find(make_full_key(key));
  if (itkey == std::end(*m_container_ptr)) {
    return default_value;  // Key not found
  } else {
    return itkey->second.get<T>();
  }
}

template <typename T>
const T& CtxMap::at(const std::string& key, const T& default_value) const {
  auto itkey = m_container_ptr->find(make_full_key(key));
  if (itkey == std::end(*m_container_ptr)) {
    return default_value;  // Key not found
  } else {
    return itkey->second.get<T>();
  }
}

template <typename T>
std::shared_ptr<T> CtxMap::at_ptr(const std::string& key,
                                  std::shared_ptr<T> default_ptr) {
  auto itkey = m_container_ptr->find(make_full_key(key));
  if (itkey == std::end(*m_container_ptr)) {
    return default_ptr;  // Key not found
  } else {
    return itkey->second.get_ptr<T>();
  }
}

template <typename T>
std::shared_ptr<const T> CtxMap::at_ptr(const std::string& key,
                                        std::shared_ptr<const T> default_ptr) const {
  auto itkey = m_container_ptr->find(make_full_key(key));
  if (itkey == std::end(*m_container_ptr)) {
    return default_ptr;  // Key not found
  } else {
    return itkey->second.get_ptr<T>();
  }
}

}  // namespace ctx
