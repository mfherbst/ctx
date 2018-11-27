//
// Copyright (C) 2018 by Michael F. Herbst
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

namespace libctx {

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
    // Create the new parameter object.
    params p(std::unique_ptr<CtxMap>(new CtxMap(m_map_ptr->submap(normalised))));

    // Move in inside the subtree
    auto it = m_subtree_cache.emplace(std::move(normalised), std::move(p)).first;
    return it->second;
  } else {
    return it->second;
  }
}

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
    // Since the GenMap uses '/' as the delimiter,
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

}  // namespace libctx
