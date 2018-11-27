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

cmake_minimum_required(VERSION 3.2.0)
cmake_policy(VERSION 3.2.0)

##########################################################################
# Project setup

project(ctx VERSION 2.1.0)
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
