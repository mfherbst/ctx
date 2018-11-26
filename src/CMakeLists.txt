## ---------------------------------------------------------------------
##
## Copyright (C) 2018 by Michael F. Herbst and contributers
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

set(CTX_SOURCES
  ctx/demangle.cc
  ctx/PamMapValue.cc
  ctx/PamMap.cc
  libctx/params.C
  libctx/context.C
)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_library(ctx ${CTX_SOURCES})
set_target_properties(ctx PROPERTIES VERSION "${PROJECT_VERSION}")

#
# Installation
#

# set the installation property for these targets:
install(TARGETS ctx
	EXPORT CtxTargets
	DESTINATION lib
	COMPONENT library
)

# install the includes:
install(DIRECTORY ctx
	DESTINATION "include/ctx"
	COMPONENT devel
	FILES_MATCHING
	PATTERN "*.hh"
)
install(DIRECTORY libctx
	DESTINATION "include/libctx"
	COMPONENT devel
	FILES_MATCHING
	PATTERN "*.h"
)

# Export the target specifications for ctx.
export(EXPORT CtxTargets
	FILE "${ctx_BINARY_DIR}/ctxTargets.cmake"
	NAMESPACE Ctx::
)

# and install them appropriately:
install(EXPORT CtxTargets
	FILE "ctxTargets.cmake"
	NAMESPACE Ctx::
	DESTINATION "share/cmake/ctx"
)

# Write a basic version file for ctx
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${ctx_BINARY_DIR}/ctxConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

file(WRITE "${ctx_BINARY_DIR}/ctxConfig.cmake"
	"include(\"${CMAKE_CURRENT_LIST_DIR}/ctxTargets.cmake\")")

# Set an export location:
install(FILES
	"${ctx_BINARY_DIR}/ctxConfig.cmake"
	"${ctx_BINARY_DIR}/ctxConfigVersion.cmake"
	DESTINATION "share/cmake/ctx"
	COMPONENT devel
)
