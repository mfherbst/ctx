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

# sets these things
#
#       CTX_DEPENDENCIES			everyone needs these libraries
#       CTX_DEPENDENCIES_DEBUG			debug mode needs these extras
#       CTX_DEPENDENCIES_RELEASE		release mode needs these extras
#       CTX_DEPENDENCIES_TEST			tests need these extra libraries
#
#       CTX_DEFINITIONS				definitions for all compilation
#       CTX_DEFINITIONS_DEBUG			definitions for debug mode
#       CTX_DEFINITIONS_RELEASE			definitions for release mode
#

####################
#-- Empty it all --#
####################
set(CTX_DEPENDENCIES "")
set(CTX_DEPENDENCIES_DEBUG "")
set(CTX_DEPENDENCIES_RELEASE "")
set(CTX_DEPENDENCIES_TEST "")
set(CTX_DEFINITIONS "")
set(CTX_DEFINITIONS_DEBUG "")
set(CTX_DEFINITIONS_RELEASE "")

#############
#-- catch --#
#############
if (CTX_ENABLE_TESTS)
	include(cmake/findCatch.cmake)
	set(CTX_DEPENDENCIES_TEST ${CTX_DEPENDENCIES_TEST} ${catch_TARGET})
endif()

#############
#-- krims --#
#############
# Find at least version 0.0.0
set(KRIMS_VERSION 0.0.0)

# By default disable tests and examples
option(KRIMS_ENABLE_TESTS "Build krims tests" OFF)

include(cmake/findKrims.cmake)
foreach (build ${DRB_BUILD_TYPES})
	set(CTX_DEPENDENCIES_${build} ${CTX_DEPENDENCIES_${build}} ${krims_${build}_TARGET})
endforeach()
