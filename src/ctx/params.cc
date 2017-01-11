//
// Copyright (C) 2017 by the ctx authors
//
// This file is part of ctx.
//
// ctx is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ctx is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with ctx. If not, see <http://www.gnu.org/licenses/>.
//

#include "params.h"

namespace ctx {

params::params(const params& other) : params() {
  const auto& othermap = *other.m_map_ptr;
  for (auto itkey = othermap.begin_keys(); itkey != othermap.end_keys(); ++itkey) {
    m_map_ptr->update(*itkey, other.m_map_ptr->at<std::string>(*itkey));
  }
}

params& params::operator=(params p) {
  m_map_ptr = std::move(p.m_map_ptr);
  m_subtree_cache.clear();
  return *this;
}

bool params::subtree_exists(const std::string& key) const {
  std::string normalised;
  if (key[0] != '/') {
    normalised = "/" + key;
  } else {
    normalised = key;
  }

  // The key identifies a subtree if keys of the kind
  // key/${subkey} exist. So we go through the keys starting
  // with the path "key" and check that this is the case
  for (auto itkey = m_map_ptr->begin_keys(); itkey != m_map_ptr->end_keys(key); ++itkey) {
    if (0 == itkey->compare(0, normalised.length() + 1, normalised + "/")) {
      return true;
    }
  }
  return false;
}

params& params::get_subtree(const std::string& key) {
  const std::string normalised = normalise_key(key);

  // If the subtree cache does not have the required params object for the subtree
  // yet, we need to create it first
  auto it = m_subtree_cache.find(normalised);
  if (it == std::end(m_subtree_cache)) {
    // Create the new parameter object.
    params p(krims::make_unique<krims::ParameterMap>(m_map_ptr->submap(normalised)));

    // Move in inside the subtree
    auto it = m_subtree_cache.emplace(std::move(normalised), std::move(p)).first;
    return it->second;
  } else {
    return it->second;
  }
}

params& params::merge_subtree(const std::string& key, const params& from) {
  assert_throw(!key.empty(), ExcEmptyKey());

  // Perform a full deep copy of the from object
  // and move this copy into ourself.
  params copy(from);
  m_map_ptr->update(key, std::move(copy.map()));
  return get_subtree(key);
}

std::ostream& operator<<(std::ostream& os, const params& p) {
  const char delimiter = '.';

  const auto& map = *p.m_map_ptr;
  for (auto itkey = map.begin_keys(); itkey != map.end_keys(); ++itkey) {
    // Since the ParameterMap uses '/' as the delimiter,
    // we need to replace those characters here.
    // We skip the leading /

    assert_dbg((*itkey)[0] == '/', krims::ExcInternalError());
    for (auto it = std::begin(*itkey) + 1; it != std::end(*itkey); ++it) {
      if (*it == '/') {
        os << delimiter;
      } else {
        os << *it;
      }
    }

    os << " = " << map.at<std::string>(*itkey) << std::endl;
  }

  return os;
}

}  // namespace ctx
