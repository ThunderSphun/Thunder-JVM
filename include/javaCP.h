#pragma once

#include "types.h"

namespace tjvm {
	struct ConstantPool {
		enum struct Tag : u1 {
			Class = 7,
			Fieldref = 9,
			Methodref = 10,
			InterfaceMethodref = 11,
			String = 8,
			Integer = 3,
			Float = 4,
			Long = 5,
			Double = 6,
			NameAndType = 12,
			Utf8 = 1,
			MethodHandle = 15,
			MethodType = 16,
			InvokeDynamic = 18,
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
			List<tjvm::Utf8>* bytes;
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

		tjvm::ConstantPool::Tag m_tag;
		union {
			tjvm::ConstantPool::ClassInfo m_class;
			tjvm::ConstantPool::FieldrefInfo m_fieldRef;
			tjvm::ConstantPool::MethodrefInfo m_methodRef;
			tjvm::ConstantPool::InterfaceMethodrefInfo m_interfaceMethodRef;
			tjvm::ConstantPool::StringInfo m_string;
			tjvm::ConstantPool::IntegerInfo m_integer;
			tjvm::ConstantPool::FloatInfo m_float;
			tjvm::ConstantPool::LongInfo m_long;
			tjvm::ConstantPool::DoubleInfo m_double;
			tjvm::ConstantPool::NameAndTypeInfo m_nameAndType;
			tjvm::ConstantPool::Utf8Info m_utf8;
			tjvm::ConstantPool::MethodHandleInfo m_methodHandle;
			tjvm::ConstantPool::MethodTypeInfo m_methodType;
			tjvm::ConstantPool::InvokeDynamicInfo m_invokeDynamic;
		};
	};
}
