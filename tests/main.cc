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

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

// TEST_QCHEM_LIBCTX enables the sections for testing
// Q-Chem's libctx with this test suite
#ifndef TEST_QCHEM_LIBCTX
#include <krims/ExceptionSystem.hh>
#endif

int main(int argc, char* const argv[]) {
#ifndef TEST_QCHEM_LIBCTX
  // Make sure that the program does not get aborted,
  // but all krims exceptions throw instead.
  krims::AssertDbgEffect::set_throw();
#endif

  // Run catch:
  int result = Catch::Session().run(argc, argv);
  return result;
}
