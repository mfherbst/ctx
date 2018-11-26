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

include(CheckCXXCompilerFlag)

macro(enable_if_cxx_compiles VARIABLE FLAG)
        # Checks whether a cxx compiler supports a flag and if yes
        # adds it to the variable provided.
        #
        string(REGEX REPLACE "[^a-zA-Z0-9]" "" FLAG_CLEAN "${FLAG}")
        CHECK_CXX_COMPILER_FLAG("-Werror ${FLAG}" DRB_HAVE_FLAG_${FLAG_CLEAN})
        if (DRB_HAVE_FLAG_${FLAG_CLEAN})
                set(${VARIABLE} "${${VARIABLE}} ${FLAG}")
        endif()
        unset(FLAG_CLEAN)
endmacro(enable_if_cxx_compiles)

# Standard flags, used without check
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -pedantic")

# Really useful and sensible flags to check for common errors
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wnon-virtual-dtor")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Woverloaded-virtual")
# enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wshadow")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wold-style-cast")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wcast-align")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wconversion")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wuseless-cast")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wsign-conversion")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wmisleading-indentation")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wduplicated-cond")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wduplicated-branches")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wlogical-op")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wnull-dereference")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wdouble-promotion")
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wformat=2")

# These we rather want as warnings, not errors
enable_if_cxx_compiles(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations")
