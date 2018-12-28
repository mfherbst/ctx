## ---------------------------------------------------------------------
##
## Copyright 2018 Michael F. Herbst
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

include(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES(
	"#include <cxxabi.h>
	#include <string>
	#include <cstddev>
	int main() {
		const char* mangled = typeid(bool).name();
		int status;
		char*  demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
		if(status == 0 and \"bool\" == std::string(demangled)) {
			return 0;
		}
		return 1;
	}"
	HAVE_CXX_ABI
)
