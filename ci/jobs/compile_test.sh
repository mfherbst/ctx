#!/bin/bash -e

set_compilers() {
	local COMPILER="$1"
	local COMPNAME=$(echo "${COMPILER}" | cut -d- -f1)
	local COMPVER=$(echo "${COMPILER}" | cut -d- -f2)

	[ -z "$COMPILER" ] && return

	case "$COMPNAME" in
		clang)
			export CC="clang-$COMPVER"
			export CXX="clang++-$COMPVER"
			;;
		*)
			export CC="gcc-$COMPVER"
			export CXX="g++-$COMPVER"
			;;
	esac
}

set_santise_flags() {
	local SANITISE="$1"
	local SANITISE_FLAGS="-O1 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls"

	[ -z "$SANITISE" ] && return

	case "$SANITISE" in
		memory)
			echo "Enabled memory santisier."
			local SANITISE_FLAGS="$SANITISE_FLAGS -fsanitize=memory -fsanitize-memory-track-origin"
			export ASAN_OPTIONS="symbolize=1"
			;;
		thread)
			echo "Enabled thread santisier."
			local SANITISE_FLAGS="$SANITISE_FLAGS -fsanitize=thread"
			;;
		address)
			echo "Enabled address santisier."
			local SANITISE_FLAGS="$SANITISE_FLAGS -fsanitize=address"
			export LSAN_OPTIONS=verbosity=1:log_threads=1
			;;
		undefined)
			echo "Enabled undifined behaviour sanitiser."
			local SANITISE_FLAGS="$SANITISE_FLAGS -fsanitize=undefined"
			;;
	esac
	export CXX_FLAGS="$CXX_FLAGS $SANITISE_FLAGS"
}

configure_debug_exit() {
	cat CMakeFiles/CMakeError.log
	exit 1
}

# --------------------------------------------------------------

set_compilers "${COMPILER}"
set_santise_flags "$SANITISE"

cmake --version
ninja --version
${CC} --version
${CXX} --version
echo "CXX_FLAGS: $CXX_FLAGS"

# --------------------------------------------------------------
# Configure and build
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_C_COMPILER=${CC} \
	-DCMAKE_CXX_FLAGS="${CXX_FLAGS}" .. || configure_debug_exit

cmake --build  . --target all -- -j ${CORES:-1}

# Test
ctest --output-on-failure -j ${CORES:-1}
