#include "javaParser.h"

#include "types.h"

#include <iosfwd>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <bit>
#include <format>

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
	u2 bytesWritten = 0;

	while (bytesWritten < info.bytes->getSize()) {
		u1 byte = readBigEndian<u1>(stream);

		if (byte == 0)
			throw std::format_error("utf8 byte cannot be 0");
		if (byte >= 0xf0)
			throw std::format_error("utf8 byte cannot be in the range 0xf0 - 0xff");

		if (byte == 0b11101101) {								// utf16
			u1 u = byte;
			u1 v = readBigEndian<u1>(stream);
			u1 w = readBigEndian<u1>(stream);
			u1 x = readBigEndian<u1>(stream);
			u1 y = readBigEndian<u1>(stream);
			u1 z = readBigEndian<u1>(stream);

			if ((v & (0b1010 << 4))	!= (0b1010 << 4))	throw std::format_error("utf16 byte[1] should start with '0b1010'");
			if ((w & (0b10   << 6))	!= (0b10   << 6))	throw std::format_error("utf16 byte[2] should start with '0b10'");
			if ( x					!=  0b11101101  )	throw std::format_error("utf16 byte[3] should be '0b11101101'");
			if ((y & (0b1011 << 4))	!= (0b1011 << 4))	throw std::format_error("utf16 byte[4] should start with '0b1011'");
			if ((z & (0b10   << 6))	!= (0b10   << 6))	throw std::format_error("utf16 byte[5] should start with '0b10'");

			(*info.bytes)[bytesWritten++] = u;
			(*info.bytes)[bytesWritten++] = v;
			(*info.bytes)[bytesWritten++] = w;
			(*info.bytes)[bytesWritten++] = x;
			(*info.bytes)[bytesWritten++] = y;
			(*info.bytes)[bytesWritten++] = z;
		} else if ((byte & (0b1110 << 4)) == (0b1110 << 4)) {	// 0x0800 - 0xFFFF
			u1 x = byte;
			u1 y = readBigEndian<u1>(stream);
			u1 z = readBigEndian<u1>(stream);

			if ((y & (0b10 << 6)) != (0b10 << 6))	throw std::format_error("utf8 byte[1] should start with '0b10'");
			if ((z & (0b10 << 6)) != (0b10 << 6))	throw std::format_error("utf8 byte[2] should start with '0b10'");

			(*info.bytes)[bytesWritten++] = x;
			(*info.bytes)[bytesWritten++] = y;
			(*info.bytes)[bytesWritten++] = z;
		} else if ((byte & (0b110 << 5)) == (0b110 << 5)) {		// 0x0080 - 0x07FF
			u1 x = byte;
			u1 y = readBigEndian<u1>(stream);

			if ((y & (0b10 << 6)) != (0b10 << 6))	throw std::format_error("utf8 byte[1] should start with '0b10'");

			(*info.bytes)[bytesWritten++] = x;
			(*info.bytes)[bytesWritten++] = y;
		} else if ((byte & (0b0 << 7)) == (0b0 << 7)) {			// 0x0001 - 0x007F
			(*info.bytes)[bytesWritten++] = byte;
		} else {
			throw std::format_error(std::format("{} {}", "unknown utf8 byte", byte));
		}
	}

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

std::optional<List<tjvm::ConstantPool>> tjvm::parseConstantPool(std::ifstream& file, u2 size) {
	using cp = tjvm::ConstantPool;
	using tag = cp::Tag;

	List<cp> list(size);
	bool isValid = true;

	for (u2 i = 0; i < size; i++) {
		cp& ref = list[i];
		ref.m_tag = readBigEndian<tag>(file);
		try {
			switch (ref.m_tag) {
			case tag::Class:				ref.m_class				= readBigEndian<cp::ClassInfo>(file);				break;
			case tag::Fieldref:				ref.m_fieldRef			= readBigEndian<cp::FieldrefInfo>(file);			break;
			case tag::Methodref:			ref.m_methodRef			= readBigEndian<cp::MethodrefInfo>(file);			break;
			case tag::InterfaceMethodref:	ref.m_interfaceMethodRef= readBigEndian<cp::InterfaceMethodrefInfo>(file);	break;
			case tag::String:				ref.m_string			= readBigEndian<cp::StringInfo>(file);				break;
			case tag::Integer:				ref.m_integer			= readBigEndian<cp::IntegerInfo>(file);				break;
			case tag::Float:				ref.m_float				= readBigEndian<cp::FloatInfo>(file);				break;
			case tag::Long:					ref.m_long				= readBigEndian<cp::LongInfo>(file);				break;
			case tag::Double:				ref.m_double			= readBigEndian<cp::DoubleInfo>(file);				break;
			case tag::NameAndType:			ref.m_nameAndType		= readBigEndian<cp::NameAndTypeInfo>(file);			break;
			case tag::Utf8:					ref.m_utf8				= readBigEndian<cp::Utf8Info>(file);				break;
			case tag::MethodHandle:			ref.m_methodHandle		= readBigEndian<cp::MethodHandleInfo>(file);		break;
			case tag::MethodType:			ref.m_methodType		= readBigEndian<cp::MethodTypeInfo>(file);			break;
			case tag::InvokeDynamic:		ref.m_invokeDynamic		= readBigEndian<cp::InvokeDynamicInfo>(file);		break;
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
			ref.m_utf8.bytes = new List<u1>();
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
