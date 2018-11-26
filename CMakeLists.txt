## ---------------------------------------------------------------------
##
## Copyright (C) 2017 by the ctx authors
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

cmake_minimum_required(VERSION 3.2.0)
cmake_policy(VERSION 3.2.0)

# Load the DebugReleaseBuild module for a simultaneous Debug and Release
# and release build and initialise it.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/IncludeKrimsCmakeModule.cmake")
include_krims_cmake_module(DebugReleaseBuild)
drb_init()

##########################################################################
# Project setup

# enable the new version feature introduced in cmake 3.0.0
cmake_policy(SET CMP0048 NEW)
project(ctx VERSION 2.1.0)
message("\n# Configuring ${PROJECT_NAME}\n#")

# Global include directories for this project:
include_directories("${ctx_SOURCE_DIR}/src")
include_directories("${ctx_BINARY_DIR}/src")

##########################################################################
# Setup hard and optional dependencies and find components

# Determine and setup compiler flags, enforcing C++11
drb_setup_compiler_flags(11)
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands if cmake supports it.
if (CMAKE_VERSION VERSION_GREATER 3.5.0)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

include(cmake/CheckCxxAbi.cmake)
if (NOT HAVE_CXX_ABI)
	message(FATAL_ERROR "C++ ABI header is required.")
endif()

# setup hard dependencies
include(cmake/setup_dependencies.cmake)


##########################################################################
# Source
option(CTX_ENABLE_LIBCTX_COMPATIBILITY
	"Enable libctx compatibility mode. \
If this is off, it is possible to use Q-Chem's libctx (via libctx namespace) \
and this ctx library (via the ctx namespace) side-by-side." ON)
if (NOT CTX_ENABLE_LIBCTX_COMPATIBILITY)
	message(STATUS "Disabling full libctx compatibility.")
	set(DISABLE_LIBCTX_COMPATIBILITY ON)
endif()
add_subdirectory(src/ctx)

##########################################################################
# Tests
option(CTX_ENABLE_TESTS "Build ctx tests" ON)
if(CTX_ENABLE_TESTS)
	# TODO do this by plain downloading
	include(cmake/findCatch.cmake)

	option(TEST_QCHEM_LIBCTX "Build and test Q-Chems libctx library." OFF)
	if (TEST_QCHEM_LIBCTX)
		include(cmake/findQchemLibctx.cmake)
	endif()

	add_subdirectory(tests)
endif()

##########################################################################
# Finally install package:
include(cmake/install_package.cmake)

# and print a closing message
message("#\n# Configuring ${PROJECT_NAME} done\n")
