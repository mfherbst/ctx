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

set(CTX_SOURCES
  params.cc
  version.cc
)

# Write the config file config.hh
configure_file("config.hh.in"  "config.hh")

# Add the libraries for Debug and Release builds:
drb_add_library(ctx
	DBGSUFFIX  ".g"
	FILES ${CTX_SOURCES}
)

drb_set_target_properties(
	ALL ctx
	PROPERTIES
	VERSION "${PROJECT_VERSION}"
)

drb_target_link_libraries(ALL     ctx ${CTX_DEPENDENCIES})
drb_target_link_libraries(DEBUG   ctx ${CTX_DEPENDENCIES_DEBUG})
drb_target_link_libraries(RELEASE ctx ${CTX_DEPENDENCIES_RELEASE})

# set the installation property for these targets:
install(TARGETS ${ctx_TARGETS}
	EXPORT CtxTargets
	DESTINATION lib
	COMPONENT library
)

# install the includes:
install(DIRECTORY .
	DESTINATION "include/ctx"
	COMPONENT devel
	FILES_MATCHING
	PATTERN "*.hh"
	PATTERN "*.h"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/ctx/config.hh"
	DESTINATION "include/ctx"
	COMPONENT devel
)

# TODO note: The symlink include/libctx -> include/ctx is not
#            generated, so one will not be able to use the installed
#            files from code following the Q-Chem libctx setup convention
#            at the moment.

# Export the target specifications for ctx.
export(EXPORT CtxTargets
	FILE "${ctx_BINARY_DIR}/ctxTargets.cmake"
	NAMESPACE Upstream::
)

# and install them appropriately:
install(EXPORT CtxTargets
	FILE "ctxTargets.cmake"
	NAMESPACE Upstream::
	DESTINATION "share/cmake/ctx"
)
