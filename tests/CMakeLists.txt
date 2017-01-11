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

#
# The sources for the test executable
#
set(CTX_TESTS_SOURCES
	contextTests.cc
	ctx_ptrTests.cc
	ctx_refTests.cc
	paramsTests.cc
	main.cc
)

drb_add_executable(ctx_tests
	DBGSUFFIX  "_DEBUG"
	RELSUFFIX  "_RELEASE"
	FILES ${CTX_TESTS_SOURCES}
)

drb_target_compile_definitions(ALL     ctx_tests
	PRIVATE ${CTX_DEFINITIONS})
drb_target_compile_definitions(DEBUG   ctx_tests
	PRIVATE ${CTX_DEFINITIONS_DEBUG})
drb_target_compile_definitions(RELEASE ctx_tests
	PRIVATE ${CTX_DEFINITIONS_RELEASE})

drb_target_link_libraries(ALL     ctx_tests
	${CTX_DEPENDENCIES} ${CTX_DEPENDENCIES_TEST})
drb_target_link_libraries(DEBUG   ctx_tests
	${ctx_DEBUG_TARGET} ${CTX_DEPENDENCIES_DEBUG})
drb_target_link_libraries(RELEASE ctx_tests
	${ctx_RELEASE_TARGET} ${CTX_DEPENDENCIES_RELEASE})

# Add the test executables to the test suite
FOREACH(target ${ctx_tests_TARGETS})
	add_test(NAME ${target} COMMAND ${target})
ENDFOREACH()

