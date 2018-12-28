## ---------------------------------------------------------------------
##
## Copyright 2018 Michael F. Herbst
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## ---------------------------------------------------------------------

# The sources for the test executable
set(CTX_TESTS_SOURCES
	CtxMapTests.cc
	contextTests.cc
	ctx_ptrTests.cc
	ctx_refTests.cc
	paramsTests.cc
	main.cc
)

# Module to setup catch test targets
include(${CTX_CATCH_DIR}/ParseAndAddCatchTests.cmake)

include_directories("${ctx_SOURCE_DIR}/src")
add_executable(ctx_tests ${CTX_TESTS_SOURCES})
target_link_libraries(ctx_tests ctx CtxCatch)
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
