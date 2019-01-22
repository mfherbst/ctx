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
#include <ctx/CtxMap.hh>
#include <libctx/params.h>

namespace ctx {
using namespace libctx;
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
#ifdef TEST_QCHEM_LIBCTX
    std::vector<int> vint;
    p.get_vec("wsp", vint);
#else
    auto vint  = p.get_vec<int>("wsp");
#endif
    REQUIRE(vint.empty());

    p.set("wsp", "   ");
#ifdef TEST_QCHEM_LIBCTX
    std::vector<double> vdbl;
    p.get_vec("wsp", vdbl);
#else
    auto vdbl  = p.get_vec<double>("wsp");
#endif
    REQUIRE(vdbl.empty());

    p.set("wsp", "	\t\n");
#ifdef TEST_QCHEM_LIBCTX
    std::vector<size_t> vsize;
    p.get_vec("wsp", vsize);
#else
    auto vsize = p.get_vec<size_t>("wsp");
#endif
    REQUIRE(vsize.empty());

#ifdef TEST_QCHEM_LIBCTX
    std::vector<int> vec;
    p.get_vec<int>("nonexistent", vec);
    REQUIRE(vec.empty());
#else
    REQUIRE(p.get_vec<int>("nonexistent").empty());
#endif
  }

  SECTION("Test vector conversion of data strings") {
    params p;

    p.set("d", "1\t\n -2\t");
#ifdef TEST_QCHEM_LIBCTX
    std::vector<int> vint;
    p.get_vec("d", vint);
#else
    auto vint  = p.get_vec<int>("d");
#endif
    REQUIRE(vint.size() == 2);
    REQUIRE(vint[0] == 1);
    REQUIRE(vint[1] == -2);

    p.set("d", "\n  1.1e4   -9 \n 0.4 \n\n 19.44");
#ifdef TEST_QCHEM_LIBCTX
    std::vector<double> vdbl;
    p.get_vec("d", vdbl);
#else
    auto vdbl  = p.get_vec<double>("d");
#endif
    REQUIRE(vdbl.size() == 4);
    REQUIRE(vdbl[0] == 11000.);
    REQUIRE(vdbl[1] == -9);
    REQUIRE(vdbl[2] == 0.4);
    REQUIRE(vdbl[3] == 19.44);

    p.set("d", "	\t\n12 19 27");
#ifdef TEST_QCHEM_LIBCTX
    std::vector<size_t> vsize;
    p.get_vec("d", vsize);
#else
    auto vsize = p.get_vec<size_t>("d");
#endif
    REQUIRE(vsize.size() == 3);
    REQUIRE(vsize[0] == 12);
    REQUIRE(vsize[1] == 19);
    REQUIRE(vsize[2] == 27);
  }

  SECTION("Test workflow with subtrees") {
// Construct a params object and initialise with some data:
#ifdef TEST_QCHEM_LIBCTX
    params p;
    p.get_subtree("tree").set("data", "1");
    p.get_subtree("tree").set("string", "string");
#else
    params p({{"/tree/data", "1"}, {"tree/string", "string"}});
#endif
    p.set("data", 4);
    p.set("soso", 2);
    p.get_subtree("other").set("data", 99);

    // check that it worked
    REQUIRE(p.subtree_exists("tree"));
    REQUIRE(p.subtree_exists("other"));
    REQUIRE(p.key_exists("data"));
    REQUIRE_FALSE(p.subtree_exists("data"));
    REQUIRE_FALSE(p.key_exists("tree"));
    REQUIRE_FALSE(p.key_exists("other"));

    REQUIRE(p.get<int>("data") == 4);
    REQUIRE(p.get_subtree("tree").get<int>("data") == 1);
    REQUIRE(p.get_subtree("tree").get<std::string>("string") == "string");

    // Get a subtree to it
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
    REQUIRE(p.get_subtree("tree").get<int>("data") == 42);
    REQUIRE(p.get_subtree("tree").get<std::string>("string") == "ssss");
    REQUIRE(p.get_subtree("tree").get<double>("new") == 3.);

    // Copy a submap:
    params subcopy(sub);
    REQUIRE(subcopy.get<int>("data") == 42);
    subcopy.set("new", 17);
    REQUIRE(subcopy.get<int>("new") == 17);
    REQUIRE(sub.get<int>("new") == 3);
    REQUIRE(p.get_subtree("tree").get<int>("new") == 3);

    // subsubcontext:
    params& subsub = sub.get_subtree("sub");
    subsub.set("key", "value");
    REQUIRE(sub.get_subtree("sub").get<std::string>("key") == "value");
    REQUIRE(p.get_subtree("tree").get_subtree("sub").get<std::string>("key") == "value");

    sub.erase_value("string");
    REQUIRE(p.get_subtree("tree").get_subtree("sub").get<std::string>("key") == "value");
    REQUIRE(!sub.key_exists("string"));
    REQUIRE(!p.get_subtree("tree").key_exists("string"));
    REQUIRE(p.get_subtree("tree").get<int>("data") == 42);

    p.get_subtree("tree").erase_value("new");
    REQUIRE(!sub.key_exists("new"));
    REQUIRE(!p.get_subtree("tree").key_exists("new"));

    // Subtree merging
    params q;
#ifdef TEST_QCHEM_LIBCTX
    q.set("some", "2");
    q.get_subtree("any").set("below", "3");
    q.get_subtree("any").set("above", "4");
    q.set("extra", "pi");
#else
    q.map().update(
          {{"some", "2"}, {"any/below", "3"}, {"any/above", "4"}, {"extra", "pi"}});
#endif
    REQUIRE(q.get<int>("some") == 2);
    REQUIRE(q.get_subtree("any").get<int>("below") == 3);
    REQUIRE(q.get_subtree("any").get<int>("above") == 4);
    REQUIRE(q.get<std::string>("extra") == "pi");

    auto& merged = sub.merge_subtree("blubb", q);

    // old stuff:
    REQUIRE(p.get<int>("data") == 4);
    REQUIRE(p.get_subtree("other").get<int>("data") == 99);
    REQUIRE(p.get<int>("soso") == 2);
    REQUIRE(p.get_subtree("tree").get<int>("data") == 42);
    REQUIRE(p.get_subtree("tree").get_subtree("sub").get<std::string>("key") == "value");

    // new stuff:
    REQUIRE(p.get_subtree("tree").get_subtree("blubb").get<int>("some") == 2);
    REQUIRE(p.get_subtree("tree").get_subtree("blubb").get_subtree("any").get<int>(
                  "below") == 3);
    REQUIRE(p.get_subtree("tree").get_subtree("blubb").get_subtree("any").get<int>(
                  "above") == 4);
    REQUIRE(p.get_subtree("tree").get_subtree("blubb").get<std::string>("extra") == "pi");

    // new submap:
    REQUIRE(merged.get<int>("some") == 2);
    REQUIRE(merged.get_subtree("any").get<int>("below") == 3);
    REQUIRE(merged.get_subtree("any").get<int>("above") == 4);
    REQUIRE(merged.get<std::string>("extra") == "pi");
  }

  SECTION("Test workflow with subtree cache generation") {
    params p;
    p.map().update({{"some/any", "2"}});
    p.get_subtree("any");
    p.get_subtree("new").set("stuff", 3);
    params& nested = p.get_subtree("nested");
    nested.get_subtree("data");

    REQUIRE(p.subtree_exists("any"));
    REQUIRE(p.subtree_exists("new"));
    REQUIRE(p.subtree_exists("some"));
    REQUIRE(p.subtree_exists("nested"));
    REQUIRE(p.get_subtree("nested").subtree_exists("data"));

    params copy(p);
    REQUIRE(copy.subtree_exists("any"));
    REQUIRE(copy.subtree_exists("new"));
    REQUIRE(copy.subtree_exists("some"));
    REQUIRE(copy.get_subtree("nested").subtree_exists("data"));

    params nested_copy(nested);
    REQUIRE(nested_copy.subtree_exists("data"));
  }
}

}  // namespace tests
}  // namespace ctx
