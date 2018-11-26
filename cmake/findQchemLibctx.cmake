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
