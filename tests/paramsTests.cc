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
#include <ctx/params.h>
namespace ctx {
namespace tests {

TEST_CASE("Test params", "[params]") {
  SECTION("Test basic workflow with set, get, erase_value") {
    params p;

    p.set("outside", "inside");
    REQUIRE(p.key_exists("outside"));
    REQUIRE(p.get<std::string>("outside") == "inside");

    p.set("bla", 12234);
    p.set("blubb", 999);
    REQUIRE(p.key_exists("bla"));
    REQUIRE(p.get<int>("bla") == 12234);
    REQUIRE(p.key_exists("blubb"));
    REQUIRE(p.get<int>("blubb") == 999);

    p.erase_value("bla");
    REQUIRE(!p.key_exists("bla"));
    REQUIRE(p.get<int>("blubb") == 999);
    REQUIRE(p.get<std::string>("outside") == "inside");

    p.set("string", "14");
    REQUIRE(p.get<std::string>("string") == "14");
    REQUIRE(p.get<int>("blubb") == 999);
    REQUIRE(p.get<std::string>("outside") == "inside");

    p.set("string", 9);
    REQUIRE(p.get<int>("string") == 9);
    REQUIRE(p.get<int>("blubb") == 999);
    REQUIRE(p.get<std::string>("outside") == "inside");
  }

  SECTION("Test conversion using parameter maps") {
    params p;

    p.set("d", "13");
    REQUIRE(p.get<int>("d") == 13);
    REQUIRE(p.get<double>("d") == 13.);

    p.set("d", "3.14");
    REQUIRE(p.get<double>("d") == 3.14);

    p.set("d", "3.14e3");
    REQUIRE(p.get<double>("d") == 3140.);

    p.set("d", 144.3);
    REQUIRE(p.get<double>("d") == 144.3);
    REQUIRE(p.get<std::string>("d") == "144.3");

    p.set("d", -14);
    REQUIRE(p.get<std::string>("d") == "-14");
  }

  SECTION("Test vector conversion of whitespace strings") {
    params p;

    p.set("wsp", "");
    auto vint = p.get_vec<int>("wsp");
    REQUIRE(vint.empty());

    p.set("wsp", "   ");
    auto vdbl = p.get_vec<double>("wsp");
    REQUIRE(vdbl.empty());

    p.set("wsp", "	\t\n");
    auto vsize = p.get_vec<size_t>("wsp");
    REQUIRE(vsize.empty());

    REQUIRE(p.get_vec<int>("nonexistent").empty());
  }

  SECTION("Test vector conversion of data strings") {
    params p;

    p.set("d", "1\t\n -2\t");
    auto vint = p.get_vec<int>("d");
    REQUIRE(vint.size() == 2);
    REQUIRE(vint[0] == 1);
    REQUIRE(vint[1] == -2);

    p.set("d", "\n  1.1e4   -9 \n 0.4 \n\n 19.44");
    auto vdbl = p.get_vec<double>("d");
    REQUIRE(vdbl.size() == 4);
    REQUIRE(vdbl[0] == 11000.);
    REQUIRE(vdbl[1] == -9);
    REQUIRE(vdbl[2] == 0.4);
    REQUIRE(vdbl[3] == 19.44);

    p.set("d", "	\t\n12 19 27");
    auto vsize = p.get_vec<size_t>("d");
    REQUIRE(vsize.size() == 3);
    REQUIRE(vsize[0] == 12);
    REQUIRE(vsize[1] == 19);
    REQUIRE(vsize[2] == 27);
  }

  SECTION("Test workflow with subtrees") {
    params p;
    p.set("tree/data", 1);
    p.set("tree/string", "string");
    p.set("data", 4);
    p.set("soso", 2);
    p.set("other/data", 99);

    REQUIRE(p.subtree_exists("tree"));
    REQUIRE(p.subtree_exists("other"));
    REQUIRE(p.key_exists("data"));
    REQUIRE_FALSE(p.subtree_exists("data"));
    REQUIRE_FALSE(p.key_exists("tree"));
    REQUIRE_FALSE(p.key_exists("other"));

    REQUIRE(p.get<int>("data") == 4);
    REQUIRE(p.get<int>("tree/data") == 1);
    REQUIRE(p.get<std::string>("tree/string") == "string");

    params& sub = p.get_subtree("tree");
    REQUIRE(sub.key_exists("string"));
    REQUIRE(sub.key_exists("data"));
    REQUIRE_FALSE(sub.key_exists("soso"));
    REQUIRE(sub.get<int>("data") == 1);
    REQUIRE(sub.get<std::string>("string") == "string");

    // Modify inside sub
    sub.set("string", "ssss");
    sub.set("data", 42);
    sub.set("new", 3.);
    REQUIRE(sub.get<int>("data") == 42);
    REQUIRE(sub.get<std::string>("string") == "ssss");
    REQUIRE(sub.get<int>("new") == 3);
    REQUIRE(p.get<int>("data") == 4);
    REQUIRE(p.get<int>("tree/data") == 42);
    REQUIRE(p.get<std::string>("tree/string") == "ssss");
    REQUIRE(p.get<double>("tree/new") == 3.);

    // Copy a submap:
    params subcopy(sub);
    REQUIRE(subcopy.get<int>("data") == 42);
    subcopy.set("new", 17);
    REQUIRE(subcopy.get<int>("new") == 17);
    REQUIRE(sub.get<int>("new") == 3);
    REQUIRE(p.get<int>("tree/new") == 3);

    // subsubcontext:
    params& subsub = sub.get_subtree("sub");
    subsub.set("key", "value");
    REQUIRE(sub.get<std::string>("sub/key") == "value");
    REQUIRE(p.get<std::string>("tree/sub/key") == "value");

    sub.erase_value("string");
    REQUIRE(p.get<std::string>("tree/sub/key") == "value");
    REQUIRE(!sub.key_exists("string"));
    REQUIRE(!p.key_exists("tree/string"));
    REQUIRE(p.get<int>("tree/data") == 42);

    p.erase_value("tree/new");
    REQUIRE(!sub.key_exists("new"));
    REQUIRE(!p.key_exists("tree/new"));

    // Subtree merging
    params q;
    q.map().update(
          {{"some", "2"}, {"any/below", "3"}, {"any/above", "4"}, {"extra", "pi"}});
    REQUIRE(q.get<int>("/some") == 2);
    REQUIRE(q.get<int>("/any/below") == 3);
    REQUIRE(q.get<int>("/any/above") == 4);
    REQUIRE(q.get<std::string>("//extra") == "pi");

    auto& merged = sub.merge_subtree("blubb", q);

    // old stuff:
    REQUIRE(p.get<int>("data") == 4);
    REQUIRE(p.get<int>("other/data") == 99);
    REQUIRE(p.get<int>("soso") == 2);
    REQUIRE(p.get<int>("tree/data") == 42);
    REQUIRE(p.get<std::string>("tree/sub/key") == "value");

    // new stuff:
    REQUIRE(p.get<int>("/tree/blubb/some") == 2);
    REQUIRE(p.get<int>("/tree/blubb/any/below") == 3);
    REQUIRE(p.get<int>("/tree/blubb/any/above") == 4);
    REQUIRE(p.get<std::string>("/tree/blubb/extra") == "pi");

    // new submap:
    REQUIRE(merged.get<int>("some") == 2);
    REQUIRE(merged.get<int>("any/below") == 3);
    REQUIRE(merged.get<int>("any/above") == 4);
    REQUIRE(merged.get<std::string>("extra") == "pi");
  }
}

}  // namespace tests
}  // namespace ctx
