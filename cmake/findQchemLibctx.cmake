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

# Try to find the libctx library
set(LIBCTX_BUILD_PATH ${PROJECT_SOURCE_DIR}/external/libctx/build/libctx)

find_library(QCHEM_LIBCTX_LIBRARY
	ctx
	PATHS ${LIBCTX_BUILD_PATH}
	DOC "Path to Q-Chem's libctx library"
)
if(${QCHEM_LIBCTX_LIBRARY} MATCHES "-NOTFOUND")
	message(FATAL_ERROR "Could not find Q-Chem's libctx library at ${LIBCTX_BUILD_PATH}.
Did you checkout libctx and build it inside \"${PROJECT_SOURCE_DIR}/external/libctx\"?")
else()
	message(STATUS "Found Q-Chem's libctx at ${QCHEM_LIBCTX_LIBRARY}")
endif()

set(qchem_libctx_target qchem_libctx)
add_library(${qchem_libctx_target} INTERFACE IMPORTED GLOBAL)
set_target_properties(${qchem_libctx_target} PROPERTIES
	INTERFACE_LINK_LIBRARIES "${QCHEM_LIBCTX_LIBRARY}"
	INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/external/libctx"
	INTERFACE_COMPILE_DEFINITIONS ""
)
