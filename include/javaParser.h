#pragma once

#include "javaData.h"

#include <optional>
#include <fstream>

namespace tjvm {
	std::optional<Class> parseClass(std::ifstream& file);
	List<ConstantPool> parseConstantPool(std::ifstream& file, u2 amount);
}