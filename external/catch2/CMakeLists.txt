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

# Setup catch as target
set(CATCH_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE "${CATCH_DIR}/..")
