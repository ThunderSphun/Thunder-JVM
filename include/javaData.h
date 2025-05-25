#pragma once

#include "types.h"

namespace tjvm {
	struct cpInfo {
		u1 tag;
		list<u1> info;
	};

	struct Class {
		u2 minor_version;
		u2 major_version;

		list<cpInfo> constantPool;
	};

	void printInfo(const Class& javaClass);
}