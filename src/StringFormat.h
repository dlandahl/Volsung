#pragma once

#include <string>
#include <vector>

#include "Yggdrasil.h"

namespace Yggdrasil {

bool starts_with(std::string, std::string);
std::vector<std::string> split_by(std::string, char);
std::string remove_spaces(std::string);

}
