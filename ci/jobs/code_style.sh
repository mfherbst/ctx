#!/bin/bash -e

find_run_clang_tidy() {
	local CLANG_TIDY_VERSION=$(clang-tidy --version | awk '/LLVM version/ {print $3}')
	local MAJOR=$(echo "${CLANG_TIDY_VERSION}" | cut -d. -f1)
	local MINOR=$(echo "${CLANG_TIDY_VERSION}" | cut -d. -f2)
	local SUFFIX="${MAJOR}.${MINOR}"

	for path in /usr/bin /usr/local/clang*/bin \
			/usr/lib/llvm-${SUFFIX}/share/clang \
			/usr/lib/llvm-${SUFFIX}/bin; do
		for name in run-clang-tidy.py run-clang-tidy-${SUFFIX}.py; do
			if [ -x "$path/$name" ]; then
				echo "$path/$name"
				return 0
			fi
		done
	done
	return 1
}

# --------------------------------------------------------------
# Versions
clang-tidy --version
find_run_clang_tidy
clang-format --version

# --------------------------------------------------------------
# Initialise build directory
# This exports the compilation commands for clang-tidy
mkdir build
pushd build
cmake -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_C_COMPILER=${CC} ..
popd

# Run clang-tidy (disabled for now)
#./scripts/run-clang-tidy.py -run-clang-tidy "$(find_run_clang_tidy)" \
#	-j ${CORES:-1} -check-output

# --------------------------------------------------------------
# Run clang-format on all files of the repo
./scripts/run-clang-format.py

# Check if one file has changed
if [ "$(git status --untracked-files=no --porcelain=v2 | wc -l)" -gt 0 ]; then
	echo "Formatting has changed the following files:"
	git status -s --untracked-files=no
	echo
	echo --------------------------------------------------------
	echo
	git diff
	exit 1
fi

exit 0
