#include "javaParser.h"

#include <filesystem>
#include <fstream>
#include <iostream>

const std::filesystem::path entry("testData/Main.class");

int main() {
	std::ifstream file(entry, std::ios::binary);
	std::optional javaEntry = tjvm::parseClass(file);

	std::cout << std::endl;

	if (!javaEntry) {
		std::cout << "file is not java class" << std::endl;
		return -1;
	}

	std::cout << "file is java class" << std::endl;

	std::cout << std::endl;

	tjvm::printInfo(javaEntry.value());

	return 0;
}