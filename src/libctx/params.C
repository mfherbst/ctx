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

#include "params.h"
#include <ctx/exceptions.hh>
#include <ctx/CtxMap.hh>

namespace libctx {
using namespace ctx;

params::params() : m_map_ptr{new CtxMap{}}, m_subtree_cache{} {}

params::params(const CtxMap& map) : params() {
  for (auto& kv : map) {
    m_map_ptr->update(kv.key(), kv.value<std::string>());
  }
}

params::params(const params& other)  : params(*other.m_map_ptr) {}

params::~params() {
  delete m_map_ptr;
  m_map_ptr = nullptr;
}

params& params::operator=(params p) {
  delete m_map_ptr;
  m_map_ptr   = p.m_map_ptr;
  p.m_map_ptr = nullptr;

  m_subtree_cache.clear();
  return *this;
}

bool params::subtree_exists(const std::string& key) const {
  const std::string normalised = normalise_key(key);

  // The key identifies a subtree if keys of the kind
  // key/${subkey} exist. So we go through the keys starting
  // with the path "key" and check that this is the case
  for (auto& kv : *m_map_ptr) {
    if (0 == kv.key().compare(0, normalised.length() + 1, normalised + "/")) {
      return true;
    }
  }
  return false;
}

bool params::key_exists(const std::string& key) const {
  // The key indentifies a value if it plainly exists and returns a value:
  return m_map_ptr->exists(key);
}

const params& params::get_subtree(const std::string& key) const {
  if (!subtree_exists(key)) {
    throw out_of_range("Subtree key  '" + key + "' is not known.");
  }

  auto it = m_subtree_cache.find(normalise_key(key));
  if (it == std::end(m_subtree_cache)) {
    throw internal_error("Unexpectedly could not find key '" + key +
                         "' in subtree cache'");
  }
  return it->second;
}

params& params::get_subtree(const std::string& key) {
  if (key.find('/') != std::string::npos) {
    throw invalid_argument("Key should not contain the \"/\" character.");
  }
  const std::string normalised = normalise_key(key);

  // If the subtree cache does not have the required params object for the subtree
  // yet, we need to create it first
  auto it = m_subtree_cache.find(normalised);
  if (it == std::end(m_subtree_cache)) {
    // Create the new parameter object and the new subtree
    CtxMap* submap_ptr = new CtxMap(m_map_ptr->submap(normalised));
    auto it            = m_subtree_cache.insert({normalised, params{}}).first;

    // Replace CtxMap object of the subtree by the one we made as a submap above
    auto& it_params = it->second;
    delete it_params.m_map_ptr;
    it_params.m_map_ptr = submap_ptr;
    submap_ptr          = nullptr;

    return it->second;
  } else {
    return it->second;
  }
}

const std::string& params::get_str(const std::string& key) const {
  return m_map_ptr->at<std::string>(key);
}

void params::set(const std::string& key, const std::string& value) {
  if (key.find('/') != std::string::npos) {
    throw invalid_argument("Key should not contain the \"/\" character.");
  }
  m_map_ptr->update(key, value);
}

void params::erase_value(const std::string& key) { m_map_ptr->erase(key); }

params& params::merge_subtree(const std::string& key, const params& from) {
  if (key.empty()) {
    throw invalid_argument("An empty key is not allowed here.");
  }
  if (key.find('/') != std::string::npos) {
    throw invalid_argument("Key should not contain the \"/\" character.");
  }

  // Perform a full deep copy of the from object
  // and move this copy into ourself.
  params copy(from);
  m_map_ptr->update(key, std::move(copy.map()));
  return get_subtree(key);
}

std::ostream& operator<<(std::ostream& os, const params& p) {
  const char delimiter = '.';

  for (auto& kv : *p.m_map_ptr) {
    // Since the CtxMap uses '/' as the delimiter,
    // we need to replace those characters here.
    // We skip the leading /

    if (kv.key()[0] != '/') {
      throw internal_error("Unexpectedly encountered key '" + kv.key() +
                           "'not starting with a '/'");
    }
    for (auto it = std::begin(kv.key()) + 1; it != std::end(kv.key()); ++it) {
      if (*it == '/') {
        os << delimiter;
      } else {
        os << *it;
      }
    }

    os << " = " << kv.value<std::string>() << std::endl;
  }

  return os;
}

void throw_type_mismatch(const std::string& str) { throw type_mismatch(str); }

}  // namespace libctx
