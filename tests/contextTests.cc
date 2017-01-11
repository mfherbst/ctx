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
#include <ctx/context.h>
namespace ctx {
namespace tests {

TEST_CASE("Test context", "[context]") {
  SECTION("Test workflow with insert, erase, update") {
    krims::ParameterMap stor;

    // Insert into storage outside of context
    stor.update("outside", "inside");

    context ctx(stor);
    REQUIRE(ctx.key_exists("outside"));
    REQUIRE(*ctx.get<std::string>("outside") == "inside");

    ctx.insert("bla", make_rcptr<int>(12234));
    ctx.insert("blubb", make_rcptr<int>(999));
    REQUIRE(ctx.key_exists("bla"));
    REQUIRE(*ctx.get<int>("bla") == 12234);
    REQUIRE(ctx.key_exists("blubb"));
    REQUIRE(*ctx.get<int>("blubb") == 999);

    ctx.erase("bla");
    REQUIRE(!ctx.key_exists("bla"));
    REQUIRE(*ctx.get<int>("blubb") == 999);
    REQUIRE(*ctx.get<std::string>("outside") == "inside");

    ctx.insert("string", make_rcptr<std::string>("14"));
    REQUIRE(*ctx.get<std::string>("string") == "14");
    REQUIRE(*ctx.get<int>("blubb") == 999);
    REQUIRE(*ctx.get<std::string>("outside") == "inside");

    ctx.update("string", make_rcptr<int>(14));

    // Check that outside knows about our changes:
    REQUIRE(stor.at<int>("blubb") == 999);
    REQUIRE(stor.at<std::string>("outside") == "inside");
    REQUIRE(stor.at<int>("string") == 14);
  }

  SECTION("Test workflow with subcontexts") {
    krims::ParameterMap stor{
          {"tree/data", 1}, {"tree/string", "string"}, {"data", 4}, {"other/data", 99}};
    context ctx(stor);

    REQUIRE(ctx.key_exists("data"));
    REQUIRE(ctx.key_exists("tree/data"));
    REQUIRE(ctx.key_exists("tree/string"));
    REQUIRE(*ctx.get<int>("data") == 4);
    REQUIRE(*ctx.get<int>("tree/data") == 1);
    REQUIRE(*ctx.get<std::string>("tree/string") == "string");

    context sub(ctx, "tree");
    REQUIRE(sub.key_exists("string"));
    REQUIRE(sub.key_exists("data"));
    REQUIRE(*sub.get<int>("data") == 1);
    REQUIRE(*sub.get<std::string>("string") == "string");

    // Update sub
    sub.update("string", make_rcptr<std::string>("ssss"));
    sub.update("data", make_rcptr<int>(42));

    REQUIRE(*sub.get<int>("data") == 42);
    REQUIRE(*sub.get<std::string>("string") == "ssss");
    REQUIRE(*ctx.get<int>("data") == 4);
    REQUIRE(*ctx.get<int>("tree/data") == 42);
    REQUIRE(*ctx.get<std::string>("tree/string") == "ssss");

    // Insert inside sub
    sub.insert("new", make_rcptr<double>(3.));
    REQUIRE(*ctx.get<double>("tree/new") == 3.);

    // Insert from a ParameterMap submap.
    stor.submap("tree").update("outside", -12);
    REQUIRE(*ctx.get<int>("tree/outside") == -12);
    REQUIRE(*sub.get<int>("outside") == -12);

    // Copy a subcontext:
    context subcopy(sub);
    REQUIRE(*subcopy.get<int>("data") == 42);
    subcopy.update("new", make_rcptr<int>(17));
    REQUIRE(*sub.get<int>("new") == 17);
    REQUIRE(*ctx.get<int>("tree/new") == 17);

    // subsubcontext:
    context subsub(sub, "sub");
    subsub.insert("key", make_rcptr<std::string>("value"));
    REQUIRE(*sub.get<std::string>("sub/key") == "value");
    REQUIRE(*ctx.get<std::string>("tree/sub/key") == "value");

    sub.erase("string");
    REQUIRE(*ctx.get<std::string>("tree/sub/key") == "value");
    REQUIRE(!sub.key_exists("string"));
    REQUIRE(!ctx.key_exists("tree/string"));
    REQUIRE(*ctx.get<int>("tree/outside") == -12);

    ctx.erase("tree/new");
    REQUIRE(!sub.key_exists("new"));
    REQUIRE(!ctx.key_exists("tree/new"));

    // Erase tree path
    ctx.erase_path("tree");
    REQUIRE_FALSE(ctx.key_exists("tree/outside"));
    REQUIRE_FALSE(ctx.key_exists("tree/data"));
    REQUIRE_FALSE(ctx.key_exists("tree/string"));
    REQUIRE_FALSE(ctx.key_exists("tree/sub/key"));
    REQUIRE(ctx.key_exists("data"));
    REQUIRE(ctx.key_exists("other/data"));
  }

  SECTION("Test context copy function") {
    krims::ParameterMap stor{{"tree/data", 39}, {"tree/string", "string"}};
    context ctx(stor);
    context tree(ctx, "tree");
    context baum(ctx, "baum");

    // Copy data
    ctx.copy("tree/data", "baum/data");
    REQUIRE(*ctx.get<int>("baum/data") == 39);
    REQUIRE(*ctx.get<int>("tree/data") == 39);

    // Check that they are different pointers, but
    // point to the same data
    *baum.get<int>("data") = -39;
    REQUIRE(*ctx.get<int>("baum/data") == -39);
    REQUIRE(*ctx.get<int>("tree/data") == -39);

    baum.update("data", make_rcptr<int>(12));
    REQUIRE(*ctx.get<int>("tree/data") == -39);
    REQUIRE(*ctx.get<int>("baum/data") == 12);

    // Copy data across contexts
    krims::ParameterMap stor2;
    context ctx2(stor2);
    tree.copy("string", ctx2, "d");
    REQUIRE(*ctx.get<std::string>("tree/string") == "string");
    REQUIRE(*ctx2.get<std::string>("d") == "string");

    *ctx2.get<std::string>("d") = "blubber";
    REQUIRE(*ctx.get<std::string>("tree/string") == "blubber");
    REQUIRE(*ctx2.get<std::string>("d") == "blubber");

    ctx2.update("d", make_rcptr<int>(0));
    REQUIRE(*ctx.get<std::string>("tree/string") == "blubber");
    REQUIRE(*ctx2.get<int>("d") == 0);
  }
}  // testcace context

}  // namespace tests
}  // namespace ctx
