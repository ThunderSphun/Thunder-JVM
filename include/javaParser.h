#pragma once

#include "types.h"
#include "javaData.h"

#include <optional>
#include <iosfwd>

namespace tjvm {
	std::optional<Class> parseClass(std::ifstream& file);
	std::optional<List<ConstantPool>> parseConstantPool(std::ifstream& file, u2 amount);
}