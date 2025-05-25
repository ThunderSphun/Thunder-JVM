#include "javaParser.h"
#include "types.h"

#include <iostream>
#include <type_traits>

template<typename T>
T readBigEndian(std::ifstream& stream) {
	T t{};
	stream.read(reinterpret_cast<char*>(&t), sizeof(t));
	return std::byteswap(t);
}

template<>
u1 readBigEndian(std::ifstream& stream) {
	u1 val;
	stream.read(reinterpret_cast<char*>(&val), 1);
	return val;
}

template<>
tjvm::ConstantPool::Tag readBigEndian(std::ifstream& stream) {
	return tjvm::ConstantPool::Tag(readBigEndian<u1>(stream));
}

template<>
tjvm::ConstantPool::ClassInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::ClassInfo info{};

	info.name_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::FieldrefInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::FieldrefInfo info{};

	info.class_index = readBigEndian<u2>(stream);
	info.name_and_type_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::MethodrefInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::MethodrefInfo info{};

	info.class_index = readBigEndian<u2>(stream);
	info.name_and_type_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::InterfaceMethodrefInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::InterfaceMethodrefInfo info{};

	info.class_index = readBigEndian<u2>(stream);
	info.name_and_type_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::StringInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::StringInfo info{};

	info.string_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::IntegerInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::IntegerInfo info{};

	info.bytes = readBigEndian<u4>(stream);

	return info;
}

template<>
tjvm::ConstantPool::FloatInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::FloatInfo info{};

	info.bytes = readBigEndian<u4>(stream);

	return info;
}

template<>
tjvm::ConstantPool::LongInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::LongInfo info{};

	info.high_bytes = readBigEndian<u4>(stream);
	info.low_bytes = readBigEndian<u4>(stream);

	return info;
}

template<>
tjvm::ConstantPool::DoubleInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::DoubleInfo info{};

	info.high_bytes = readBigEndian<u4>(stream);
	info.low_bytes = readBigEndian<u4>(stream);

	return info;
}

template<>
tjvm::ConstantPool::NameAndTypeInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::NameAndTypeInfo info{};

	info.name_index = readBigEndian<u2>(stream);
	info.descriptor_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::Utf8Info readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::Utf8Info info{};

	info.bytes = new List<u1>(readBigEndian<u2>(stream));

	for (u2 j = 0; j < info.bytes->getSize(); j++)
		(*info.bytes)[j] = readBigEndian<u1>(stream);

	return info;
}

template<>
tjvm::ConstantPool::MethodHandleInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::MethodHandleInfo info{};

	info.reference_kind = readBigEndian<u1>(stream);
	info.reference_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::MethodTypeInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::MethodTypeInfo info{};

	info.descriptor_index = readBigEndian<u2>(stream);

	return info;
}

template<>
tjvm::ConstantPool::InvokeDynamicInfo readBigEndian(std::ifstream& stream) {
	tjvm::ConstantPool::InvokeDynamicInfo info{};

	info.bootstrap_method_attr_index = readBigEndian<u2>(stream);
	info.name_and_type_index = readBigEndian<u2>(stream);

	return info;
}

std::optional<tjvm::Class> tjvm::parseClass(std::ifstream& file) {
	Class java{};

	if (!file)
		return {};

	u4 magic = readBigEndian<u4>(file);
	if (magic != 0xCAFEBABE)
		return {};

	java.m_minor_version = readBigEndian<u2>(file);
	java.m_major_version = readBigEndian<u2>(file);

	java.m_constantPool = parseConstantPool(file, readBigEndian<u2>(file));

	return std::move(std::make_optional(std::move(java)));
}

List<tjvm::ConstantPool> tjvm::parseConstantPool(std::ifstream& file, u2 amount) {
	List<tjvm::ConstantPool> result(amount);

	for (u2 i = 0; i < amount; i++) {
		tjvm::ConstantPool& ref = result[i];
		ref.m_tag = readBigEndian<tjvm::ConstantPool::Tag>(file);

		switch (ref.m_tag) {
		case tjvm::ConstantPool::Tag::Class:				ref.m_class				= readBigEndian<tjvm::ConstantPool::ClassInfo>				(file); break;
		case tjvm::ConstantPool::Tag::Fieldref:				ref.m_fieldRef			= readBigEndian<tjvm::ConstantPool::FieldrefInfo>			(file); break;
		case tjvm::ConstantPool::Tag::Methodref:			ref.m_methodRef			= readBigEndian<tjvm::ConstantPool::MethodrefInfo>			(file); break;
		case tjvm::ConstantPool::Tag::InterfaceMethodref:	ref.m_interfaceMethodRef= readBigEndian<tjvm::ConstantPool::InterfaceMethodrefInfo>	(file); break;
		case tjvm::ConstantPool::Tag::String:				ref.m_string			= readBigEndian<tjvm::ConstantPool::StringInfo>				(file); break;
		case tjvm::ConstantPool::Tag::Integer:				ref.m_integer			= readBigEndian<tjvm::ConstantPool::IntegerInfo>			(file); break;
		case tjvm::ConstantPool::Tag::Float:				ref.m_float				= readBigEndian<tjvm::ConstantPool::FloatInfo>				(file); break;
		case tjvm::ConstantPool::Tag::Long:					ref.m_long				= readBigEndian<tjvm::ConstantPool::LongInfo>				(file); break;
		case tjvm::ConstantPool::Tag::Double:				ref.m_double			= readBigEndian<tjvm::ConstantPool::DoubleInfo>				(file); break;
		case tjvm::ConstantPool::Tag::NameAndType:			ref.m_nameAndType		= readBigEndian<tjvm::ConstantPool::NameAndTypeInfo>		(file); break;
		case tjvm::ConstantPool::Tag::Utf8:					ref.m_utf8				= readBigEndian<tjvm::ConstantPool::Utf8Info>				(file); break;
		case tjvm::ConstantPool::Tag::MethodHandle:			ref.m_methodHandle		= readBigEndian<tjvm::ConstantPool::MethodHandleInfo>		(file); break;
		case tjvm::ConstantPool::Tag::MethodType:			ref.m_methodType		= readBigEndian<tjvm::ConstantPool::MethodTypeInfo>			(file); break;
		case tjvm::ConstantPool::Tag::InvokeDynamic:		ref.m_invokeDynamic		= readBigEndian<tjvm::ConstantPool::InvokeDynamicInfo>		(file); break;
		default: {
			std::cerr << "invalid ref tag, continuing parsing the rest of the file" << std::endl;
			readBigEndian<u2>(file); // consume 2 bytes as that should be the minimum
			break;
		}}
	}

	return result;
}
