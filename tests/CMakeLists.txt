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
	rc_ptrTests.cc
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
