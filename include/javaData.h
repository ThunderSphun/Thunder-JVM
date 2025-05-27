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

	ClassFlags operator|(const ClassFlags& l, const ClassFlags& r);
	ClassFlags operator&(const ClassFlags& l, const ClassFlags& r);
	ClassFlags operator^(const ClassFlags& l, const ClassFlags& r);
	ClassFlags operator~(const ClassFlags& l);

	ClassFlags& operator|=(ClassFlags& l, const ClassFlags& r);
	ClassFlags& operator&=(ClassFlags& l, const ClassFlags& r);
	ClassFlags& operator^=(ClassFlags& l, const ClassFlags& r);

	struct Class {
		u2 m_minor_version;
		u2 m_major_version;

		List<ConstantPool> m_constantPool;

		ClassFlags m_accessFlags;
		
		u2 m_thisClass;
		u2 m_superClass;

		List<u2> m_interfaces;
	};

	void printInfo(const Class& javaClass);
	std::string getCPString(const List<ConstantPool>& cp, u2 index, ConstantPool::Tag tag);
}
