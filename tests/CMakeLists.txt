## ---------------------------------------------------------------------
##
## Copyright (C) 2018 by Michael F. Herbst
##
## This file is part of ctx.
##
## ctx is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## ctx is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with ctx. If not, see <http://www.gnu.org/licenses/>.
##
## ---------------------------------------------------------------------

# The sources for the test executable
set(CTX_TESTS_SOURCES
	PamMapTests.cc
	contextTests.cc
	ctx_ptrTests.cc
	ctx_refTests.cc
	paramsTests.cc
	main.cc
)

# Module to setup catch test targets
include(${CATCH_DIR}/ParseAndAddCatchTests.cmake)

# enable testing of this project
enable_testing()

add_executable(ctx_tests ${CTX_TESTS_SOURCES})
target_link_libraries(ctx_tests ctx Catch)
ParseAndAddCatchTests(ctx_tests)

# Check wether we are supposed to check Q-Chem's libctx library
# as well against our test suite
if(TEST_QCHEM_LIBCTX)
	# Test which tests Q-Chem's libctx library using our unit test suite.
	add_executable(qchem_libctx_tests ${CTX_TESTS_SOURCES})

	# We do not actually need our ctx code here, but
	# we set the definition TEST_QCHEM_LIBCTX which switches between
	# libctx and ctx-speciffic code segments.
	target_compile_definitions(qchem_libctx_tests PRIVATE "TEST_QCHEM_LIBCTX")
	set_target_properties(qchem_libctx_tests PROPERTIES INCLUDE_DIRECTORIES "")
	target_link_libraries(qchem_libctx_tests
		${qchem_libctx_target} Catch)

	# Add to test list
	add_test(NAME qchem_libctx_tests COMMAND qchem_libctx_tests)
endif()
