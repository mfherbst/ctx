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

# Setup optional dependencies and features
# alters these things
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
#-- C++ standard --#
####################
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 14)
	message(STATUS "Detected C++14 support: Setting CTX_HAVE_CXX14")
	LIST(APPEND CTX_DEFINITIONS "CTX_HAVE_CXX14")
endif()
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 17)
	message(STATUS "Detected C++17 support: Setting CTX_HAVE_CXX17")
	LIST(APPEND CTX_DEFINITIONS "CTX_HAVE_CXX17")
endif()


if (NOT CTX_ENABLE_LIBCTX_COMPATIBILITY)
	message(STATUS "Disabling full libctx compatibility.")
	LIST(APPEND CTX_DEFINITIONS "DISABLE_LIBCTX_COMPATIBILITY")
endif()
