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

####################
#-- Empty it all --#
####################
set(CTX_DEPENDENCIES "")
set(CTX_DEPENDENCIES_DEBUG "")
set(CTX_DEPENDENCIES_RELEASE "")
set(CTX_DEPENDENCIES_TEST "")

#############
#-- krims --#
#############
# By default disable tests and examples
option(KRIMS_ENABLE_TESTS "Build krims tests" OFF)

include_krims_cmake_module(FindPackageAutocheckoutFallback)
find_package_autocheckout_fallback(krims 0.1.0)
foreach (build ${DRB_BUILD_TYPES})
	set(CTX_DEPENDENCIES_${build} ${CTX_DEPENDENCIES_${build}} ${krims_${build}_TARGET})
endforeach()
