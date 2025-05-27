#pragma once

#include "types.h"

#include <string>

namespace tjvm {
	struct ConstantPool {
		enum struct Tag : u1 {
			Class				= 7,
			Fieldref			= 9,
			Methodref			= 10,
			InterfaceMethodref	= 11,
			String				= 8,
			Integer				= 3,
			Float				= 4,
			Long				= 5,
			Double				= 6,
			NameAndType			= 12,
			Utf8				= 1,
			MethodHandle		= 15,
			MethodType			= 16,
			InvokeDynamic		= 18,
		};

		struct ClassInfo {
			u2 name_index;
		};

		struct FieldrefInfo {
			u2 class_index;
			u2 name_and_type_index;
		};

		struct MethodrefInfo {
			u2 class_index;
			u2 name_and_type_index;
		};

		struct InterfaceMethodrefInfo {
			u2 class_index;
			u2 name_and_type_index;
		};

		struct StringInfo {
			u2 string_index;
		};

		struct IntegerInfo {
			u4 bytes;
		};

		struct FloatInfo {
			u4 bytes;
		};

		struct LongInfo {
			u4 high_bytes;
			u4 low_bytes;
		};

		struct DoubleInfo {
			u4 high_bytes;
			u4 low_bytes;
		};

		struct NameAndTypeInfo {
			u2 name_index;
			u2 descriptor_index;
		};

		struct Utf8Info {
			List<u1>* bytes;
		};

		struct MethodHandleInfo {
			u1 reference_kind;
			u2 reference_index;
		};

		struct MethodTypeInfo {
			u2 descriptor_index;
		};

		struct InvokeDynamicInfo {
			u2 bootstrap_method_attr_index;
			u2 name_and_type_index;
		};

		~ConstantPool();

		Tag m_tag;
		union {
			ClassInfo m_class;
			FieldrefInfo m_fieldRef;
			MethodrefInfo m_methodRef;
			InterfaceMethodrefInfo m_interfaceMethodRef;
			StringInfo m_string;
			IntegerInfo m_integer;
			FloatInfo m_float;
			LongInfo m_long;
			DoubleInfo m_double;
			NameAndTypeInfo m_nameAndType;
			Utf8Info m_utf8;
			MethodHandleInfo m_methodHandle;
			MethodTypeInfo m_methodType;
			InvokeDynamicInfo m_invokeDynamic;
		};
	};

	enum class AccessFlags : u2 {
		Public		= 0x0001,
		Final		= 0x0010,
		Super		= 0x0020,
		Interface	= 0x0200,
		Abstract	= 0x0400,
		Synthetic	= 0x1000,
		Annotation	= 0x2000,
		Enum		= 0x4000,
	};

	AccessFlags operator|(const AccessFlags& l, const AccessFlags& r);
	AccessFlags operator&(const AccessFlags& l, const AccessFlags& r);
	AccessFlags operator^(const AccessFlags& l, const AccessFlags& r);
	AccessFlags operator~(const AccessFlags& l);

	AccessFlags& operator|=(AccessFlags& l, const AccessFlags& r);
	AccessFlags& operator&=(AccessFlags& l, const AccessFlags& r);
	AccessFlags& operator^=(AccessFlags& l, const AccessFlags& r);

	struct Class {
		u2 m_minor_version;
		u2 m_major_version;

		List<ConstantPool> m_constantPool;

		AccessFlags m_accessFlags;
		
		u2 m_thisClass;
		u2 m_superClass;

		List<u2> m_interfaces;
	};

	void printInfo(const Class& javaClass);
	std::string getCPString(const List<ConstantPool>& cp, u2 index, ConstantPool::Tag tag);
}
