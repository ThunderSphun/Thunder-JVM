#include "javaData.h"

#include <iostream>

void tjvm::printInfo(const Class& javaClass) {
	std::cout << "version: " << javaClass.major_version << "." << javaClass.minor_version << std::endl;
	std::cout << "constantPool (" << javaClass.constantPool.getSize() << "):" << std::endl;

	for (u2 i = 0; i < javaClass.constantPool.getSize(); i++) {
		std::cout << "\t" << "cpInfo [" << std::format("{:02x}", javaClass.constantPool[i].tag) << "] (" << javaClass.constantPool[i].info.getSize() << "): ";

		for (u2 j = 0; j < javaClass.constantPool[i].info.getSize(); j++) {
			std::cout << std::format("{:02x}", javaClass.constantPool[i].info[j]) << " ";
		}

		std::cout << std::endl;
	}
}
