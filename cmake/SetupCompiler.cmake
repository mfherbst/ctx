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
