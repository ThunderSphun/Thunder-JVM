#include "javaData.h"

#include <iostream>
#include <format>
#include <bit>

template<typename T>
std::ostream& operator<<(std::ostream& os, List<T>& list) {
	os << "List<" << typeid(T).name() << "> (" << list.getSize() << ") {";

	if (list.getSize() > 0)
		os << std::format("{:{}X}", list[0], sizeof(T) * 2);

	for (u2 i = 1; i < list.getSize(); i++)
		os << " " << std::format("{:{}X}", list[i], sizeof(T) * 2);

	return os << "}";
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const List<T>& list) {
	os << "List<" << typeid(T).name() << "> (" << list.getSize() << ") {";

	if (list.getSize() > 0)
		os << std::format("{:{}X}", list[0], sizeof(T) * 2);

	for (u2 i = 1; i < list.getSize(); i++)
		os << " " << std::format("{:{}X}", list[i], sizeof(T) * 2);

	return os << "}";
}

tjvm::ConstantPool::~ConstantPool() {
	if (m_tag == Tag::Utf8)
		delete m_utf8.bytes;
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
	default											: os << "                  "; break;
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const tjvm::ConstantPool& cp) {
	os << "\tconstantPool [" << cp.m_tag << "] ";

	switch (cp.m_tag) {
	case tjvm::ConstantPool::Tag::Class:
		os << std::format("${:04x}", cp.m_class.name_index); break;
	case tjvm::ConstantPool::Tag::Fieldref:
		os << std::format("${:04x} ${:04x}", cp.m_fieldRef.class_index, cp.m_fieldRef.name_and_type_index);break;
	case tjvm::ConstantPool::Tag::Methodref:
		os << std::format("${:04x} ${:04x}", cp.m_methodRef.class_index, cp.m_methodRef.name_and_type_index);break;
	case tjvm::ConstantPool::Tag::InterfaceMethodref:
		os << std::format("${:04x} ${:04x}", cp.m_interfaceMethodRef.class_index, cp.m_interfaceMethodRef.name_and_type_index);break;
	case tjvm::ConstantPool::Tag::String:
		os << std::format("${:04x}", cp.m_string.string_index);break;
	case tjvm::ConstantPool::Tag::Integer:
		os << std::format("{}", cp.m_integer.bytes);break;
	case tjvm::ConstantPool::Tag::Float:
		os << std::format("{}", std::bit_cast<float>(cp.m_float.bytes));break;
	case tjvm::ConstantPool::Tag::Long:
		os << std::format("{}", (u8) cp.m_long.high_bytes << 32 | cp.m_long.low_bytes);break;
	case tjvm::ConstantPool::Tag::Double:
		os << std::format("{}", std::bit_cast<double>((u8) cp.m_double.high_bytes << 32 | cp.m_double.low_bytes)); break;
	case tjvm::ConstantPool::Tag::NameAndType:
		os << std::format("${:04x} ${:04x}", cp.m_nameAndType.name_index, cp.m_nameAndType.descriptor_index);break;
	case tjvm::ConstantPool::Tag::Utf8:
		os << *cp.m_utf8.bytes;break;
	case tjvm::ConstantPool::Tag::MethodHandle:
		os << std::format("{:02x} ${:04x}", cp.m_methodHandle.reference_kind, cp.m_methodHandle.reference_index);break;
	case tjvm::ConstantPool::Tag::MethodType:
		os << std::format("${:04x}", cp.m_methodType.descriptor_index);break;
	case tjvm::ConstantPool::Tag::InvokeDynamic:
		os << std::format("${:04x} ${:04x}", cp.m_invokeDynamic.bootstrap_method_attr_index, cp.m_invokeDynamic.name_and_type_index);break;
	default:
		break;
	}

	return os;
}

void tjvm::printInfo(const Class& javaClass) {
	std::cout << "JAVA CLASS" << std::endl << std::string(75, '-') << std::endl << "CAFEBABE" << std::endl;

	std::cout << "version: " << javaClass.m_major_version << "." << javaClass.m_minor_version << std::endl;
	std::cout << "constantPool (" << javaClass.m_constantPool.getSize() << "):" << std::endl;

	for (u2 i = 0; i < javaClass.m_constantPool.getSize(); i++)
		std::cout << javaClass.m_constantPool[i] << std::endl;
}
