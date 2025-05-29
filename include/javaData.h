#pragma once

#include "types.h"
#include "javaCP.h"

#include <string>

namespace tjvm {
	enum class ClassFlags : u2 {
		Public		= 0x0001,
		Final		= 0x0010,
		Super		= 0x0020,
		Interface	= 0x0200,
		Abstract	= 0x0400,
		Synthetic	= 0x1000,
		Annotation	= 0x2000,
		Enum		= 0x4000,
	};

	tjvm::ClassFlags operator|(const tjvm::ClassFlags& l, const tjvm::ClassFlags& r);
	tjvm::ClassFlags operator&(const tjvm::ClassFlags& l, const tjvm::ClassFlags& r);
	tjvm::ClassFlags operator^(const tjvm::ClassFlags& l, const tjvm::ClassFlags& r);
	tjvm::ClassFlags operator~(const tjvm::ClassFlags& l);

	tjvm::ClassFlags& operator|=(tjvm::ClassFlags& l, const tjvm::ClassFlags& r);
	tjvm::ClassFlags& operator&=(tjvm::ClassFlags& l, const tjvm::ClassFlags& r);
	tjvm::ClassFlags& operator^=(tjvm::ClassFlags& l, const tjvm::ClassFlags& r);

	struct Class {
		u2 m_minor_version;
		u2 m_major_version;

		List<tjvm::ConstantPool> m_constantPool;

		tjvm::ClassFlags m_accessFlags;
		
		u2 m_thisClass;
		u2 m_superClass;

		List<u2> m_interfaces;
	};

	void printInfo(const tjvm::Class& javaClass);
	std::string getCPString(const List<tjvm::ConstantPool>& cp, const u2 index, const ConstantPool::Tag tag);
}
