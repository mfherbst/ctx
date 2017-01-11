#include "version.hh"
#include <sstream>

namespace ctx {

std::string version::version_string() {
  std::stringstream ss;
  ss << major << "." << minor << "." << patch;
  return ss.str();
}

}  // namespace ctx
