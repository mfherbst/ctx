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

#include <catch.hpp>
#include <ctx/ctx_ptr.h>

namespace ctx {
namespace tests {

TEST_CASE("Test ctx_ptr", "[ctx_ptr]") {
  root_storage stor;
  context ctx(stor);

  SECTION("Test storing integers") {
    stor.update("six", 6);
    ctx.insert("four", make_rcptr<int>(4));
    ctx.insert("five", rc_ptr<int>(new int(5)));

    ctx_ptr<int> ptr_six(ctx, "six");
    ctx_ptr<int> ptr_four(ctx, std::string("four"));
    ctx_ptr<int> ptr_five(ctx, "five");

    REQUIRE(*ptr_six == 6);
    REQUIRE(*ptr_four == 4);
    REQUIRE(*ptr_five == 5);
  }  // integers

  SECTION("Test storing strings") {
    stor.update("six", "6");
    ctx.insert("four", make_rcptr<std::string>("4"));
    ctx.insert("five", rc_ptr<std::string>(new std::string("5")));

    ctx_ptr<std::string> ptr_six(ctx, "six");
    ctx_ptr<std::string> ptr_four(ctx, std::string("four"));
    ctx_ptr<std::string> ptr_five(ctx, "five");

    REQUIRE(*ptr_six == "6");
    REQUIRE(*ptr_four == "4");
    REQUIRE(*ptr_five == "5");
  }  // strings

  SECTION("Test storing doubles") {
    stor.update("six", 6.0);
    ctx.insert("four", make_rcptr<double>(4.0));
    ctx.insert("five", rc_ptr<double>(new double(5.0)));

    ctx_ptr<double> ptr_six(ctx, "six");
    ctx_ptr<double> ptr_four(ctx, std::string("four"));
    ctx_ptr<double> ptr_five(ctx, "five");

    REQUIRE(*ptr_six == 6.);
    REQUIRE(*ptr_four == 4.);
    REQUIRE(*ptr_five == 5.);
  }  // doubles

}  // ctx_ptr

}  // namespace tests
}  // namespace ctx
