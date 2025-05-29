#include "javaParser.h"

#include "types.h"

#include <iosfwd>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <bit>
#include <format>
#include <vector>

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

	u2 size = readBigEndian<u2>(stream);
	std::vector<tjvm::Utf8> storage;
	// always at most this size, if everything is single byte encoded
	storage.reserve(size);
	u2 bytesRead = 0;

	while (bytesRead < size) {
		u8 data = readBigEndian<u8>(stream);
		tjvm::Utf8& utf8 = storage.emplace_back(data);
		stream.seekg(-sizeof(u8) + utf8.getCodePoints(), std::ios_base::cur);
		bytesRead += utf8.getCodePoints();
	}

	info.bytes = new List<tjvm::Utf8>(storage.size());
	for (u2 i = 0; i < info.bytes->getSize(); i++)
		(*info.bytes)[i] = std::move(storage[i]);

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

	std::optional<List<tjvm::ConstantPool>> cp = parseConstantPool(file, readBigEndian<u2>(file) - 1);
	if (!cp)
		return {};
	java.m_constantPool = std::move(cp.value());

	java.m_accessFlags = tjvm::ClassFlags(readBigEndian<u2>(file));

	java.m_thisClass = readBigEndian<u2>(file);
	java.m_superClass = readBigEndian<u2>(file);

	java.m_interfaces = List<u2>(readBigEndian<u2>(file));
	for (u2 i = 0; i < java.m_interfaces.getSize(); i++)
		java.m_interfaces[i] = readBigEndian<u2>(file);

	return std::move(java);
}

std::optional<List<tjvm::ConstantPool>> tjvm::parseConstantPool(std::ifstream& file, const u2 size) {
	List<tjvm::ConstantPool> list(size);
	bool isValid = true;

	for (u2 i = 0; i < size; i++) {
		tjvm::ConstantPool& ref = list[i];
		ref.m_tag = readBigEndian<tjvm::ConstantPool::Tag>(file);
		try {
			switch (ref.m_tag) {
			case tjvm::ConstantPool::Tag::Class:				ref.m_class				= readBigEndian<tjvm::ConstantPool::ClassInfo>(file);				break;
			case tjvm::ConstantPool::Tag::Fieldref:				ref.m_fieldRef			= readBigEndian<tjvm::ConstantPool::FieldrefInfo>(file);			break;
			case tjvm::ConstantPool::Tag::Methodref:			ref.m_methodRef			= readBigEndian<tjvm::ConstantPool::MethodrefInfo>(file);			break;
			case tjvm::ConstantPool::Tag::InterfaceMethodref:	ref.m_interfaceMethodRef= readBigEndian<tjvm::ConstantPool::InterfaceMethodrefInfo>(file);	break;
			case tjvm::ConstantPool::Tag::String:				ref.m_string			= readBigEndian<tjvm::ConstantPool::StringInfo>(file);				break;
			case tjvm::ConstantPool::Tag::Integer:				ref.m_integer			= readBigEndian<tjvm::ConstantPool::IntegerInfo>(file);				break;
			case tjvm::ConstantPool::Tag::Float:				ref.m_float				= readBigEndian<tjvm::ConstantPool::FloatInfo>(file);				break;
			case tjvm::ConstantPool::Tag::Long:					ref.m_long				= readBigEndian<tjvm::ConstantPool::LongInfo>(file);				break;
			case tjvm::ConstantPool::Tag::Double:				ref.m_double			= readBigEndian<tjvm::ConstantPool::DoubleInfo>(file);				break;
			case tjvm::ConstantPool::Tag::NameAndType:			ref.m_nameAndType		= readBigEndian<tjvm::ConstantPool::NameAndTypeInfo>(file);			break;
			case tjvm::ConstantPool::Tag::Utf8:					ref.m_utf8				= readBigEndian<tjvm::ConstantPool::Utf8Info>(file);				break;
			case tjvm::ConstantPool::Tag::MethodHandle:			ref.m_methodHandle		= readBigEndian<tjvm::ConstantPool::MethodHandleInfo>(file);		break;
			case tjvm::ConstantPool::Tag::MethodType:			ref.m_methodType		= readBigEndian<tjvm::ConstantPool::MethodTypeInfo>(file);			break;
			case tjvm::ConstantPool::Tag::InvokeDynamic:		ref.m_invokeDynamic		= readBigEndian<tjvm::ConstantPool::InvokeDynamicInfo>(file);		break;
			default:
			{
				std::cerr << "invalid ref tag, continuing parsing the rest of the file" << std::endl;
				readBigEndian<u2>(file); // consume 2 bytes as that should be the minimum
				break;
			}
			}
		} catch (std::format_error err) {
			std::cerr << err.what() << std::endl;
			ref.m_utf8 = {};
			ref.m_utf8.bytes = new List<tjvm::Utf8>();
			isValid = false;
#ifdef _MSC_VER
			__debugbreak();
#endif
		}
	}

	if (isValid)
		return std::move(list);
	else
		return {};
}
