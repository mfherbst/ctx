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

#include <iostream>
#include <libctx/libctx.h>

void modify_orig(libctx::context ctx);
void modify_new(ctx::CtxMap& ctxmap);

void main_orig() {
  libctx::root_storage stor;
  libctx::context ctx(stor);

  ctx.insert("integer", libctx::rc_ptr<int>(new int(4)));
  ctx.insert("string", libctx::rc_ptr<std::string>(new std::string("bla")));

  libctx::context subctx(ctx, "inner");
  subctx.insert("float", libctx::rc_ptr<double>(new double(4)));

  modify_orig(ctx);

  // Originally printed only keys
  ctx.print(std::cout);
}

void modify_orig(libctx::context ctx) {
  // Does not work!
  // std::cout << "I got " << ctx_ref<int>(ctx, "integer") << std::endl;
  //
  // Instead:
  int value = libctx::ctx_ref<int>(ctx, "integer");
  std::cout << "I got " << value << std::endl;

  // Does not work!
  // ctx_ref<int>(ctx, "integer") = 2*value;
  //
  // Instead
  int& value_ref = libctx::ctx_ref<int>(ctx, "integer");
  value_ref      = 2 * value;
}

//
// --------------------------------------------------------------
//

void main_new() {
  // Powerful CtxMap interface:
  {
    ctx::CtxMap stor{
          {"integer", 4},
          {"string", "bla"},
          {"inner/float", 4.},
    };
  }

  // Or equivalently:
  ctx::CtxMap stor;
  stor.update("integer", 4);
  stor.update("string", "bla");
  auto inner = stor.submap("inner");
  inner.update("float", 4.);

  // Fully backwards compatible, since libctx::context
  // and related objects wrap CtxMap in a compatible way.
  libctx::context ctx(stor);
  modify_orig(ctx);  // NOTE Original version called!

  // Forward compatible, since new libctx::context
  // provides access to ctx::CtxMap
  modify_new(ctx.map());

  // More C++ natural and improved printing
  std::cout << stor << std::endl;
}

void modify_new(ctx::CtxMap& ctxmap) {
  std::cout << "I got " << ctxmap.at<int>("integer") << std::endl;
  ctxmap.at<int>("integer") *= 2;
}

//
// --------------------------------------------------------------
//

int main() {
  main_orig();
  std::cout << "#\n#--------------------------------------\n#" << std::endl;
  main_new();
  return 0;
}
