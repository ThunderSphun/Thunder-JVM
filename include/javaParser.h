#pragma once

#include "types.h"
#include "javaData.h"

#include <optional>
#include <iosfwd>

namespace tjvm {
	std::optional<tjvm::Class> parseClass(std::ifstream& file);
	std::optional<List<tjvm::ConstantPool>> parseConstantPool(std::ifstream& file, const u2 amount);
}