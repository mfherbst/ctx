//
// Copyright (C) 2018 by Michael F. Herbst and contributers
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

#include "context.h"

namespace libctx {

void context::erase(const std::string& key) {
  size_t count = m_map_ptr->erase(key);
  if (count != 1) {
    throw out_of_range("Key '" + key + "' is not known.");
  }
}

void context::copy(const std::string& key_from, context& to, const std::string& key_to) {
  if (m_map_ptr->exists(key_to)) {
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
