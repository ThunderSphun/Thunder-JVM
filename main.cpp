#include "javaParser.h"
#include "javaData.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#pragma execution_character_set("utf-8")

const std::filesystem::path entry("testData/Main.class");

void hexFile(std::ifstream& file);

int main() {
#ifdef WIN32
	SetConsoleOutputCP(65001);
#endif

	std::ifstream file(entry, std::ios::binary);
	if (!file) {
		std::cout << "could not open file: " << entry << std::endl;
		return -1;
	}

	std::optional javaEntry = tjvm::parseClass(file);

	if (javaEntry)
		std::cout << "file is a java class" << std::endl;
	else
		std::cout << std::endl << "file is not a java class" << std::endl;

	std::cout << std::endl;

	file.seekg(0);
	hexFile(file);
	file.close();

	if (!javaEntry)
		return -2;

	std::cout << std::endl;

	tjvm::printInfo(javaEntry.value());

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
				std::cout << "\u001b[0;31m.\u001b[0m";
		}

		std::cout << std::endl;
	}
}
