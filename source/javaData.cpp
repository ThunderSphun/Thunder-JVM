#include "javaData.h"

#include <iostream>
#include <format>
#include <bit>
#include <utility>
#include <iosfwd>

tjvm::ConstantPool::~ConstantPool() {
	if (m_tag == Tag::Utf8)
		delete m_utf8.bytes;
}

tjvm::ClassFlags tjvm::operator|(const ClassFlags& l, const ClassFlags& r) {
	return tjvm::ClassFlags(std::to_underlying(l) | std::to_underlying(r));
}

tjvm::ClassFlags tjvm::operator&(const ClassFlags& l, const ClassFlags& r) {
	return tjvm::ClassFlags(std::to_underlying(l) & std::to_underlying(r));
}

tjvm::ClassFlags tjvm::operator^(const ClassFlags& l, const ClassFlags& r) {
	return tjvm::ClassFlags(std::to_underlying(l) ^ std::to_underlying(r));
}

tjvm::ClassFlags tjvm::operator~(const ClassFlags& l) {
	ClassFlags retval = l;

	retval ^= tjvm::ClassFlags::Public;
	retval ^= tjvm::ClassFlags::Final;
	retval ^= tjvm::ClassFlags::Super;
	retval ^= tjvm::ClassFlags::Interface;
	retval ^= tjvm::ClassFlags::Abstract;
	retval ^= tjvm::ClassFlags::Synthetic;
	retval ^= tjvm::ClassFlags::Annotation;
	retval ^= tjvm::ClassFlags::Enum;

	return retval;
}

tjvm::ClassFlags& tjvm::operator|=(ClassFlags& l, const ClassFlags& r) {
	l = l | r;
	return l;
}

tjvm::ClassFlags& tjvm::operator&=(ClassFlags& l, const ClassFlags& r) {
	l = l & r;
	return l;
}

tjvm::ClassFlags& tjvm::operator^=(ClassFlags& l, const ClassFlags& r) {
	l = l ^ r;
	return l;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, List<T>& list) {
	return os << static_cast<const List<T>&>(list);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const List<T>& list) {
	os << "List<" << typeid(T).name() << "> (" << list.getSize() << ") {";

	auto fmt = [](T val) {
		return std::format("{:0{}X}", val, sizeof(T) * 2);
	};

	if (list.getSize() > 0)
		os << fmt(list[0]);

	for (u2 i = 1; i < list.getSize(); i++)
		os << " " << fmt(list[i]);

	return os << "}";
}

template<>
std::ostream& operator<<(std::ostream& os, const List<u1>& list) {
	// keeping typeid(u1).name() for correct expansion of type alias
	os << "List<" << typeid(u1).name() << "> (" << list.getSize() << ") {";

	auto fmt = [](u1 val) {
		return std::format("{:02X}", val);
	};

	if (list.getSize() > 0)
		os << fmt(list[0]);

	for (u2 i = 1; i < list.getSize(); i++)
		os << " " << fmt(list[i]);

	return os << "}";
}

template<>
std::ostream& operator<<(std::ostream& os, const List<tjvm::Utf8>& list) {
	// keeping typeid(tjvm::Utf8).name() for correct expansion of type alias
	os << "List<" << typeid(tjvm::Utf8).name() << "> (" << list.getSize() << ") {";

	for (u2 i = 0; i < list.getSize(); i++)
		os << std::to_string(list[i]);

	return os << "}";
}

std::ostream& operator<<(std::ostream& os, tjvm::ConstantPool::Tag tag) {
	switch (tag) {
	case tjvm::ConstantPool::Tag::Class				: os << "Class             "; break;
	case tjvm::ConstantPool::Tag::Fieldref			: os << "Fieldref          "; break;
	case tjvm::ConstantPool::Tag::Methodref			: os << "Methodref         "; break;
	case tjvm::ConstantPool::Tag::InterfaceMethodref: os << "InterfaceMethodref"; break;
	case tjvm::ConstantPool::Tag::String			: os << "String            "; break;
	case tjvm::ConstantPool::Tag::Integer			: os << "Integer           "; break;
	case tjvm::ConstantPool::Tag::Float				: os << "Float             "; break;
	case tjvm::ConstantPool::Tag::Long				: os << "Long              "; break;
	case tjvm::ConstantPool::Tag::Double			: os << "Double            "; break;
	case tjvm::ConstantPool::Tag::NameAndType		: os << "NameAndType       "; break;
	case tjvm::ConstantPool::Tag::Utf8				: os << "Utf8              "; break;
	case tjvm::ConstantPool::Tag::MethodHandle		: os << "MethodHandle      "; break;
	case tjvm::ConstantPool::Tag::MethodType		: os << "MethodType        "; break;
	case tjvm::ConstantPool::Tag::InvokeDynamic		: os << "InvokeDynamic     "; break;
	default:
		os << "invalid [" << std::format("{:02X}", (u1) tag) << "]      ";
#ifdef _MSC_VER
		__debugbreak();
#endif
		break;
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const tjvm::ConstantPool& cp) {
	os << "\t[" << cp.m_tag << "] ";

	switch (cp.m_tag) {
	case tjvm::ConstantPool::Tag::Integer			:os << std::format("{}"				,						cp.m_integer.bytes);break;
	case tjvm::ConstantPool::Tag::Float				:os << std::format("{}"				, std::bit_cast<float>(	cp.m_float.bytes));	break;
	case tjvm::ConstantPool::Tag::Long				:os << std::format("{}"				,						(u8) cp.m_long.high_bytes << 32 + cp.m_long.low_bytes);		break;
	case tjvm::ConstantPool::Tag::Double			:os << std::format("{}"				, std::bit_cast<double>((u8) cp.m_double.high_bytes << 32 + cp.m_double.low_bytes));break;

	case tjvm::ConstantPool::Tag::Class				:os << std::format("${:04X}"		, cp.m_class.name_index);			break;
	case tjvm::ConstantPool::Tag::String			:os << std::format("${:04X}"		, cp.m_string.string_index);		break;
	case tjvm::ConstantPool::Tag::MethodType		:os << std::format("${:04X}"		, cp.m_methodType.descriptor_index);break;
	
	case tjvm::ConstantPool::Tag::MethodHandle		:os << std::format("{:02X} ${:04X}"	, cp.m_methodHandle.reference_kind, cp.m_methodHandle.reference_index);break;
	
	case tjvm::ConstantPool::Tag::Fieldref			:os << std::format("${:04X} ${:04X}", cp.m_fieldRef.class_index, cp.m_fieldRef.name_and_type_index);					break;
	case tjvm::ConstantPool::Tag::Methodref			:os << std::format("${:04X} ${:04X}", cp.m_methodRef.class_index, cp.m_methodRef.name_and_type_index);					break;
	case tjvm::ConstantPool::Tag::InterfaceMethodref:os << std::format("${:04X} ${:04X}", cp.m_interfaceMethodRef.class_index, cp.m_interfaceMethodRef.name_and_type_index);break;
	case tjvm::ConstantPool::Tag::NameAndType		:os << std::format("${:04X} ${:04X}", cp.m_nameAndType.name_index, cp.m_nameAndType.descriptor_index);							break;
	case tjvm::ConstantPool::Tag::InvokeDynamic		:os << std::format("${:04X} ${:04X}", cp.m_invokeDynamic.bootstrap_method_attr_index, cp.m_invokeDynamic.name_and_type_index);	break;
	
	case tjvm::ConstantPool::Tag::Utf8				:os << *cp.m_utf8.bytes;break;
	
	default:break;
	}

	return os;
}

void tjvm::printInfo(const Class& javaClass) {
	std::cout << "JAVA CLASS" << std::endl << std::string(75, '-') << std::endl << "CAFEBABE" << std::endl;

	std::cout << "version: " << javaClass.m_major_version << "." << javaClass.m_minor_version << std::endl;
	std::cout << "constantPool (" << javaClass.m_constantPool.getSize() << "):" << std::endl;

	for (u2 i = 0; i < javaClass.m_constantPool.getSize(); i++)
		std::cout << javaClass.m_constantPool[i] << std::endl;

	std::cout << "flags: " << std::format("{:04X}", (u2) javaClass.m_accessFlags) << std::endl;

	std::cout << "this: " << std::format("${:04X}", javaClass.m_thisClass) << " ";
	std::cout << tjvm::getCPString(javaClass.m_constantPool, javaClass.m_thisClass, tjvm::ConstantPool::Tag::Class) << std::endl;

	std::cout << "super: " << std::format("${:04X}", javaClass.m_superClass) << " ";
	if (javaClass.m_superClass == 0)
		std::cout << "Object" << std::endl;
	else
		std::cout << tjvm::getCPString(javaClass.m_constantPool, javaClass.m_superClass, tjvm::ConstantPool::Tag::Class) << std::endl;

	std::cout << "interfaces (" << javaClass.m_interfaces.getSize() << "):" << std::endl;
	for (u2 i = 0; i < javaClass.m_interfaces.getSize(); i++)
		std::cout << "\t" << std::format("${:04X}", javaClass.m_interfaces[i]) << " "
			<< tjvm::getCPString(javaClass.m_constantPool, javaClass.m_interfaces[i], tjvm::ConstantPool::Tag::Class) << std::endl;
}

std::string tjvm::getCPString(const List<tjvm::ConstantPool>& cp, const u2 index, const tjvm::ConstantPool::Tag tag) {
	const tjvm::ConstantPool& ref = cp[index - 1];
	if (ref.m_tag != tag)
		return "!! INCORRECT TAG !!";

	switch (tag) {
	case tjvm::ConstantPool::Tag::Class:
		return tjvm::getCPString(cp, ref.m_class.name_index, tjvm::ConstantPool::Tag::Utf8);

	case tjvm::ConstantPool::Tag::Fieldref:
		return std::format("{} {}",
			tjvm::getCPString(cp, ref.m_fieldRef.class_index,			tjvm::ConstantPool::Tag::Class),
			tjvm::getCPString(cp, ref.m_fieldRef.name_and_type_index,	tjvm::ConstantPool::Tag::NameAndType));
	case tjvm::ConstantPool::Tag::Methodref:
		return std::format("{} {}",
			tjvm::getCPString(cp, ref.m_methodRef.class_index,			tjvm::ConstantPool::Tag::Class),
			tjvm::getCPString(cp, ref.m_methodRef.name_and_type_index,	tjvm::ConstantPool::Tag::NameAndType));
	case tjvm::ConstantPool::Tag::InterfaceMethodref:
		return std::format("{} {}",
			tjvm::getCPString(cp, ref.m_interfaceMethodRef.class_index,			tjvm::ConstantPool::Tag::Class),
			tjvm::getCPString(cp, ref.m_interfaceMethodRef.name_and_type_index,	tjvm::ConstantPool::Tag::NameAndType));

	case tjvm::ConstantPool::Tag::String:
		return tjvm::getCPString(cp, ref.m_string.string_index, tjvm::ConstantPool::Tag::Utf8);

	case tjvm::ConstantPool::Tag::Integer:
		return std::format("{}", ref.m_integer.bytes);
	case tjvm::ConstantPool::Tag::Float:
		return std::format("{}", std::bit_cast<float>(ref.m_float.bytes));

	case tjvm::ConstantPool::Tag::Long:
		return std::format("{}", (u8) ref.m_long.high_bytes << 32 + ref.m_long.low_bytes);
	case tjvm::ConstantPool::Tag::Double:
		return std::format("{}", std::bit_cast<double>((u8) ref.m_double.high_bytes << 32 + ref.m_double.low_bytes));

	case tjvm::ConstantPool::Tag::NameAndType:
		return std::format("{} {}",
			tjvm::getCPString(cp, ref.m_nameAndType.name_index,			tjvm::ConstantPool::Tag::Utf8),
			tjvm::getCPString(cp, ref.m_nameAndType.descriptor_index,	tjvm::ConstantPool::Tag::Utf8));

	case tjvm::ConstantPool::Tag::Utf8:
		return std::to_string(*ref.m_utf8.bytes);

	case tjvm::ConstantPool::Tag::MethodHandle:
		return std::format("{} {}",
			"referenceType",
			"dependend on referenceType");

	case tjvm::ConstantPool::Tag::MethodType:
		return tjvm::getCPString(cp, ref.m_nameAndType.descriptor_index, tjvm::ConstantPool::Tag::Utf8);

	case tjvm::ConstantPool::Tag::InvokeDynamic:
		return std::format("{} {}",
			"bootstrapMethodAttribute",
			tjvm::getCPString(cp, ref.m_invokeDynamic.name_and_type_index, tjvm::ConstantPool::Tag::NameAndType));
	default:
		return "!! NO CONVERSION POSSIBLE !!";
	}
};
