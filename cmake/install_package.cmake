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

# Installs the cmake package information this project provides
#

# Write a basic version file for ctx
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${ctx_BINARY_DIR}/ctxConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

# Adjust a configure file
configure_file(cmake/ctxConfig.cmake.in
	"${ctx_BINARY_DIR}/ctxConfig.cmake"
	COPYONLY
)

# Set an export location:
install(FILES
	"${ctx_BINARY_DIR}/ctxConfig.cmake"
	"${ctx_BINARY_DIR}/ctxConfigVersion.cmake"
	DESTINATION "share/cmake/ctx"
	COMPONENT devel
)

