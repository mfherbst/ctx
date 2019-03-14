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

#include <catch2/catch.hpp>
#include <libctx/context.h>
#include <libctx/ctx_exception.h>

namespace ctx {
using namespace libctx;
namespace tests {

TEST_CASE("Test context", "[context]") {
  SECTION("Test workflow with insert, erase, update") {
    root_storage stor;

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
    CtxMap stor{
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

    // Insert from a CtxMap submap.
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
    CtxMap stor{{"tree/data", 39}, {"tree/string", "string"}};
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
    root_storage stor2;
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

  SECTION("Test various exception throws") {
    CtxMap stor{{"tree/data", 39}, {"tree/string", "string"}};
    context ctx(stor);

    REQUIRE_THROWS_AS(ctx.get<std::string>("tree/data"), ctx::type_mismatch);
    REQUIRE_THROWS_AS(ctx.get<std::string>("tree/data"), libctx::ctx_exception);

    REQUIRE_THROWS_AS(ctx.get<std::string>("tree/strng"), ctx::out_of_range);
    REQUIRE_THROWS_AS(ctx.get<std::string>("tree/strng"), std::out_of_range);
    REQUIRE_THROWS_AS(ctx.get<std::string>("tree/strng"), libctx::ctx_exception);
  }
}  // testcace context

}  // namespace tests
}  // namespace ctx
