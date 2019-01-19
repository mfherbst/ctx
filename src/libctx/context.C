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

#include "context.h"

namespace libctx {

void context::erase(const std::string& key) {
  size_t count = m_map_ptr->erase(key);
  if (count != 1) {
    throw out_of_range("Key '" + key + "' is not known.");
  }
}

void context::copy(const std::string& key_from, context& to, const std::string& key_to) {
  if (to.m_map_ptr->exists(key_to)) {
    throw invalid_argument("Target key '" + key_to + "' already exists.");
  }
  to.m_map_ptr->update(key_to, m_map_ptr->at_raw_value(key_from));
}

void context::move(const std::string& key_from, const std::string& key_to) {
  if (key_from == key_to) {
    throw invalid_argument("Source and destination are identical: " + key_from);
  }
  m_map_ptr->update(key_to, m_map_ptr->at_raw_value(key_from));
  m_map_ptr->erase(key_from);
}

void context::get_keys(std::vector<std::string>& keys) const {
  keys.clear();
  for (auto& kv : this->map()) {
    keys.push_back(kv.key());
  }
}

void context::print(std::ostream& os) const {
  os << "Current path: " << m_location << "\n";
  os << map();
}

}  // namespace libctx
