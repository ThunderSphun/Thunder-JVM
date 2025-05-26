#include "javaParser.h"

#include <filesystem>
#include <fstream>
#include <iostream>

const std::filesystem::path entry("testData/Main.class");

void hexFile(std::ifstream& file);

int main() {
	std::ifstream file(entry, std::ios::binary);
	std::optional javaEntry = tjvm::parseClass(file);

	if (!javaEntry) {
		std::cout << "file is not java class" << std::endl;

		file.clear();
		file.seekg(0);

		hexFile(file);

		file.close();

		return -1;
	}

	std::cout << "file is java class" << std::endl;

	std::cout << std::endl;

	tjvm::printInfo(javaEntry.value());

	file.close();

	return 0;
}

void hexFile(std::ifstream& file) {
	while (!file.eof()) {
		char bytes[16];

		file.read(bytes, 16);

		std::streamsize bytesRead = file.gcount();

		for (size_t i = 0; i < 16; i++) {
			if (i < bytesRead)
				std::cout << std::format("{:02X}", bytes[i]) << " ";
			else
				std::cout << "   ";
		}

		std::cout << " ";

		for (size_t i = 0; i < bytesRead; i++) {
			if (bytes[i] >= 32)
				std::cout << std::format("{:c}", bytes[i]);
			else
				std::cout << ".";
		}

		std::cout << std::endl;
	}
}
