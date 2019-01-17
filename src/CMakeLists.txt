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

set(CTX_SOURCES
  ctx/demangle.cc
  ctx/CtxMapValue.cc
  ctx/CtxMap.cc
  libctx/params.C
  libctx/context.C
)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_library(ctx ${CTX_SOURCES})
set_target_properties(ctx PROPERTIES VERSION "${PROJECT_VERSION}")

#
# Installation
#
if (DEFINED CTX_BUILD_FROM_QCHEM AND CTX_BUILD_FROM_QCHEM)
	return()  # No installation if built along Q-Chem
endif()

# set the installation property for these targets:
install(TARGETS ctx
	EXPORT CtxTargets
	DESTINATION lib
	COMPONENT library
)

# install the includes:
install(DIRECTORY ctx
	DESTINATION include
	COMPONENT devel
	FILES_MATCHING
	PATTERN "*.hh"
)
install(DIRECTORY libctx
	DESTINATION include
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
