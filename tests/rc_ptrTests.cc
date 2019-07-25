//
// Copyright 2019 Michael F. Herbst and contributors
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

#include <catch2/catch.hpp>
#include <libctx/rc_ptr.h>

namespace ctx {
using namespace libctx;
namespace tests {

TEST_CASE("Test rc_ptr", "[rc_ptr]") {
  SECTION("Test empty constructor") {
    rc_ptr<int> a;
    rc_ptr<int> b(nullptr);

    REQUIRE(a.get() == nullptr);
    REQUIRE(b.get() == nullptr);
  }

  SECTION("Test initialisation from pointer") {
    int* i_raw = new int(4);  // NOLINT  Raw pointer needed for test.
    rc_ptr<int> i_ptr(i_raw);
    REQUIRE(i_ptr.get() == i_raw);
  }

  SECTION("Test make_rcptr") {
    auto i_ptr = make_rcptr<int>(4);
    REQUIRE(*i_ptr == 4);
  }

  SECTION("Test move constructor") {
    rc_ptr<int> i_ptr = make_rcptr<int>(42);
    REQUIRE(i_ptr.use_count() == 1);
    REQUIRE(*i_ptr == 42);
    int* addr = i_ptr.get();

    rc_ptr<int> j_ptr(std::move(i_ptr));
    REQUIRE(j_ptr.use_count() == 1);
    REQUIRE(*j_ptr == 42);
    REQUIRE(j_ptr.get() == addr);
  }

  SECTION("Test copy constructor") {
    rc_ptr<int> i_ptr = make_rcptr<int>(42);
    REQUIRE(i_ptr.use_count() == 1);
    REQUIRE(*i_ptr == 42);

    rc_ptr<int> j_ptr(i_ptr);  // NOLINT  Copy is on purpose
    REQUIRE(j_ptr.use_count() == 2);
    REQUIRE(i_ptr.use_count() == 2);
    REQUIRE(*j_ptr == 42);
    REQUIRE(j_ptr.get() == i_ptr.get());
  }

  SECTION("Test initialisation from shared_ptr") {
    std::shared_ptr<int> sh_ptr = std::make_shared<int>(42);
    REQUIRE(sh_ptr.use_count() == 1);
    REQUIRE(*sh_ptr == 42);

    rc_ptr<int> ptr(sh_ptr);  // NOLINT  Copy is on purpose
    REQUIRE(ptr.use_count() == 2);
    REQUIRE(sh_ptr.use_count() == 2);
    REQUIRE(*ptr == 42);
    REQUIRE(ptr.get() == sh_ptr.get());
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  SECTION("Test initialisation from auto_ptr") {
    //
    std::auto_ptr<int> aut_ptr(new int(42));  // NOLINT  auto_ptr on purpose
    REQUIRE(*aut_ptr == 42);
    int* addr = aut_ptr.get();

    rc_ptr<int> ptr(aut_ptr);
    REQUIRE(ptr.use_count() == 1);
    REQUIRE(*ptr == 42);
    REQUIRE(ptr.get() == addr);
    REQUIRE(aut_ptr.get() == nullptr);
  }
#pragma GCC diagnostic pop

  SECTION("Test initialisation from unique_ptr") {
    std::unique_ptr<int> uniq_ptr(new int(42));
    REQUIRE(*uniq_ptr == 42);
    int* addr = uniq_ptr.get();

    rc_ptr<int> ptr(std::move(uniq_ptr));
    REQUIRE(ptr.use_count() == 1);
    REQUIRE(*ptr == 42);
    REQUIRE(ptr.get() == addr);
  }

  SECTION("Test use as shared_ptr") {
    auto get_shared_ptr_value = [](std::shared_ptr<int> ptr) { return *ptr; };

    rc_ptr<int> ptr = make_rcptr<int>(42);
    REQUIRE(get_shared_ptr_value(ptr) == 42);
  }
}  // rc_ptr

}  // namespace tests
}  // namespace ctx
