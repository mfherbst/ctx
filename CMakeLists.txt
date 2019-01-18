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

cmake_minimum_required(VERSION 3.2.0)
cmake_policy(VERSION 3.2.0)

if (DEFINED QCHEM_SOURCE_DIR)
	include(cmake/SetupBuildQChem.cmake)
	# Stop processing this file, since all setup is done in the
	# included file instead.
	return()
endif()

##########################################################################
# Project setup

project(ctx VERSION 2.3.0)
message("\n# Configuring ${PROJECT_NAME}\n#")

if (CMAKE_VERSION VERSION_GREATER 3.5.0)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

##########################################################################
# Setup compiler flags

# C++ standard of this project is 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/SetupCompiler.cmake)

include(cmake/CheckCxxAbi.cmake)
if (NOT HAVE_CXX_ABI)
	message(FATAL_ERROR "C++ ABI header is required.")
endif()

##########################################################################
# Subdirectories and targets

add_subdirectory(src)

option(CTX_ENABLE_EXAMPLES "Build ctx examples" ON)
if (CTX_ENABLE_EXAMPLES)
	add_subdirectory(examples)
endif()

option(CTX_ENABLE_TESTS "Build ctx tests" ON)
if(CTX_ENABLE_TESTS)
	enable_testing()

	add_subdirectory(external/catch2)

	option(TEST_QCHEM_LIBCTX "Build and test Q-Chems libctx library." OFF)
	if (TEST_QCHEM_LIBCTX)
		include(cmake/findQchemLibctx.cmake)
	endif()

	add_subdirectory(tests)
endif()

##########################################################################
message("#\n# Configuring ${PROJECT_NAME} done\n")
