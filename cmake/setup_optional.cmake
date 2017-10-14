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

####################
#-- C++ standard --#
####################
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 14)
	message(STATUS "Detected C++14 support: Setting CTX_HAVE_CXX14")
	set(CTX_HAVE_CXX14 ON)
endif()
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 17)
	message(STATUS "Detected C++17 support: Setting CTX_HAVE_CXX17")
	set(CTX_HAVE_CXX17 ON)
endif()

##################################
#-- QChem libctx compatibility --#
##################################
if (NOT CTX_ENABLE_LIBCTX_COMPATIBILITY)
	message(STATUS "Disabling full libctx compatibility.")
	set(DISABLE_LIBCTX_COMPATIBILITY ON)
endif()

################################################
#-- Running our unit tests with QChem libctx --#
################################################
if (TEST_QCHEM_LIBCTX)
	include(cmake/findQchemLibctx.cmake)
endif()
