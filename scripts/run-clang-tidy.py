#!/usr/bin/env python3
## vi: tabstop=4 shiftwidth=4 softtabstop=4 expandtab
## ---------------------------------------------------------------------
##
## Copyright 2019 Michael F. Herbst
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## ---------------------------------------------------------------------
import os
import sys
import glob
import common
import argparse
import subprocess


def run_check_output(commandline):
    return_code = 0
    run_clang_tidy = subprocess.Popen(commandline, stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT, universal_newlines=True)
    for line in run_clang_tidy.stdout:
        if "warning:" in line or "error:" in line:
            return_code = 1
        sys.stdout.write(line)

    if run_clang_tidy.returncode and run_clang_tidy.returncode != 0:
        raise SystemExit(run_clang_tidy.returncode)
    raise SystemExit(return_code)


def main():
    parser = argparse.ArgumentParser(
        description="Run clang-tidy on all currently committed c++ files"
    )
    parser.add_argument("-p", metavar="BUILD_PATH",
                        default=os.path.join(common.get_repo_root_path(), "build"),
                        help="Path to the build directory")
    parser.add_argument("-j", metavar="THREADS",
                        help="Number of threads to run in parallel")
    parser.add_argument("-export-fixes", metavar="PATH",
                        help="Path to the file where fixes should be stored")
    parser.add_argument("-run-clang-tidy", metavar="PATH", default="run-clang-tidy",
                        help="Name or path of run-clang-tidy executable to use.")
    parser.add_argument("-clang-tidy", metavar="PATH", default="clang-tidy",
                        help="Name or path of clang-tidy executable to use.")
    parser.add_argument("-fix", action="store_true",
                        help="Attempt to fix the issues")
    parser.add_argument("-check-output", action="store_true",
                        help="Check output for warnings and errors and exit with "
                        "non-zero return code if such are found. Disables "
                        "formatted output.")
    args = parser.parse_args()

    cpp_extensions = [".cpp", ".hpp", ".cxx", ".hxx", ".hh", ".cc", ".h", ".c"]
    ignore_globs = common.read_ignore_globs(".clang-tidy.ignore")
    cpp_files = [
        name for name in common.list_committed_files()
        if os.path.splitext(name)[1] in cpp_extensions and
        not any(glob.fnmatch.fnmatch(name, ignore) for ignore in ignore_globs)
    ]

    commandline = [args.run_clang_tidy, "-p", args.p, "-clang-tidy", args.clang_tidy]
    if args.j:
        commandline.extend(["-j", args.j])
    if args.export_fixes:
        commandline.extend(["-export-fixes", args.export_fixes])
    if args.fix:
        commandline.extend(["-fix", "-format"])
    commandline.extend(cpp_files)

    if args.check_output:
        run_check_output(commandline)
    else:
        subprocess.check_call(commandline)


if __name__ == "__main__":
    main()
