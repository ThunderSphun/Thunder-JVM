#include "javaParser.h"
#include "types.h"

#include <iostream>

template<typename T>
T readBigEndian(std::ifstream& stream) {
	T t{};
	stream.read(reinterpret_cast<char*>(&t), sizeof(t));
	return std::byteswap(t);
}

std::optional<tjvm::Class> tjvm::parseClass(std::ifstream& file) {
	Class java{};

	if (!file)
		return {};

	u4 magic = readBigEndian<u4>(file);
	if (magic != 0xCAFEBABE)
		return {};

	java.minor_version = readBigEndian<u2>(file);
	java.major_version = readBigEndian<u2>(file);

	java.constantPool = parseConstantPool(file, readBigEndian<u2>(file));

	return std::move(std::make_optional(std::move(java)));
}

list<tjvm::cpInfo> tjvm::parseConstantPool(std::ifstream& file, u2 amount) {
	list<tjvm::cpInfo> result(amount);

	for (u2 i = 0; i < amount; i++) {
		tjvm::cpInfo& ref = result[i];
		ref.tag = readBigEndian<u1>(file);

		switch (ref.tag) {
		case 7: { // constant_class
			ref.info = list<u1>(2);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			break;
		}
		case 9: { // constant_fieldRef
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			break;
		}
		case 10: { // constant_methodRef
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			break;
		}
		case 11: { // constant_interfaceMethodRef
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			break;
		}
		case 8: { // constant_string
			ref.info = list<u1>(2);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			break;
		}
		case 3: { // constant_integer
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			break;
		}
		case 4: { // constant_float
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			break;
		}
		case 5: { // constant_long
			ref.info = list<u1>(8);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			ref.info[4] = readBigEndian<u1>(file);
			ref.info[5] = readBigEndian<u1>(file);
			ref.info[6] = readBigEndian<u1>(file);
			ref.info[7] = readBigEndian<u1>(file);
			break;
		}
		case 6: { // constant_double
			ref.info = list<u1>(8);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			ref.info[4] = readBigEndian<u1>(file);
			ref.info[5] = readBigEndian<u1>(file);
			ref.info[6] = readBigEndian<u1>(file);
			ref.info[7] = readBigEndian<u1>(file);
			break;
		}
		case 12: { // constant_nameAndType
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			ref.info[3] = readBigEndian<u1>(file);
			break;
		}
		case 1: { // constant_utf8
			// TODO: Handle utf8 correctly
			u2 length = readBigEndian<u2>(file);

			ref.info = list<u1>(length + 2);
			ref.info[0] = length & 0xFF;
			ref.info[1] = (length >> 8) & 0xFF;

			for (u2 j = 0; j < length; j++)
				ref.info[j + 2] = readBigEndian<u1>(file);

			break;
		}
		case 15: { // constant_methodHandle
			ref.info = list<u1>(3);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[2] = readBigEndian<u1>(file);
			break;
		}
		case 16: { // constant_methodType
			ref.info = list<u1>(2);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			break;
		}
		case 18: { // constant_invokeDynamic
			ref.info = list<u1>(4);
			ref.info[0] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			ref.info[1] = readBigEndian<u1>(file);
			break;
		}
		default: {
			std::cerr << "invalid ref tag, continuing parsing the rest of the file" << std::endl;
			readBigEndian<u2>(file); // consume 2 bytes as that should be the minimum
			break;
		}}
	}

	return result;
}
