#include "types.h"

#include <iostream>
#include <stdexcept>
#include <bit>
#include <sstream>
#include <cstring>
#include <format>

//#define LOGGING

template<typename T>
List<T>::List() : List(0) {
}

template<typename T>
List<T>::List(u2 size) : m_size(size), m_data(m_size > 0 ? new T[m_size] : nullptr) {
#ifdef LOGGING
	if (m_data != nullptr)
		std::cout << "creating list<" << typeid(T).name() << "> (" << m_size << ")" << std::endl;
#endif
}

template<typename T>
List<T>::List(const List<T>& other): m_size(other.m_size), m_data(m_size > 0 ? new T[m_size] : nullptr) {
#ifdef LOGGING
	std::cout << "copying list<" << typeid(T).name() << "> (" << m_size << ")" << std::endl;
#endif
	for (u2 i = 0; i < m_size; i++)
		(*this)[i] = other[i];
}

template<typename T>
List<T>::List(List<T>&& other) noexcept: m_size(other.m_size), m_data(other.m_data) {
#ifdef LOGGING
	std::cout << "moving list<" << typeid(T).name() << "> (" << m_size << ")" << std::endl;
#endif
	other.m_data = nullptr;
}

template<typename T>
List<T>::~List() {
#ifdef LOGGING
	if (m_data != nullptr)
		std::cout << "removing list<" << typeid(T).name() << "> (" << m_size << ")" << std::endl;
#endif
	delete[] m_data;
}

template<typename T>
List<T>& List<T>::operator=(List<T>&& other) noexcept {
	if (this != &other) {
		delete[] m_data;

		m_size = other.m_size;
		m_data = other.m_data;

		other.m_size = 0;
		other.m_data = nullptr;
	}
	return *this;
}

template<typename T>
T& List<T>::operator[](u2 i) {
	return const_cast<T&>(static_cast<const List&>(*this)[i]);
}

template<typename T>
const T& List<T>::operator[](u2 i) const {
	if (i < m_size)
		return m_data[i];

	throw std::out_of_range("Index out of bounds");
}

template<typename T>
u2 List<T>::getSize() const {
	return m_size;
}

tjvm::Utf8::Utf8() : m_codePoints(0), m_bytes(nullptr) {}

tjvm::Utf8::Utf8(u8 data) {
	data = std::byteswap(data);
	u1* bytes = static_cast<u1*>(static_cast<void*>(&data));

	u1 lead = bytes[0];

	if (lead == 0b11101101) {								// utf16
		u1 u = bytes[0];
		u1 v = bytes[1];
		u1 w = bytes[2];
		u1 x = bytes[3];
		u1 y = bytes[4];
		u1 z = bytes[5];

		if ((u & (0b11101101 )) != (0b11101101 )) throw std::format_error("utf16 byte[1] should be '0b11101101'");
		if ((v & (0b1010 << 4)) != (0b1010 << 4)) throw std::format_error("utf16 byte[1] should start with '0b1010'");
		if ((w & (0b10   << 6)) != (0b10   << 6)) throw std::format_error("utf16 byte[2] should start with '0b10'");
		if ((x & (0b11101101 )) != (0b11101101 )) throw std::format_error("utf16 byte[3] should be '0b11101101'");
		if ((y & (0b1011 << 4)) != (0b1011 << 4)) throw std::format_error("utf16 byte[4] should start with '0b1011'");
		if ((z & (0b10   << 6)) != (0b10   << 6)) throw std::format_error("utf16 byte[5] should start with '0b10'");

		m_codePoints = 6;
		m_bytes = new u1[m_codePoints];

		m_bytes[0] = u;
		m_bytes[1] = v;
		m_bytes[2] = w;
		m_bytes[3] = x;
		m_bytes[4] = y;
		m_bytes[5] = z;
	} else if ((lead & (0b1110 << 4)) == (0b1110 << 4)) {	// 0x0800 - 0xFFFF
		u1 x = bytes[0];
		u1 y = bytes[1];
		u1 z = bytes[2];

		if ((x & (0b1110 << 4)) != (0b1110 << 4)) throw std::format_error("utf8 byte[0] should start with '0b1110'");
		if ((y & (0b10   << 6)) != (0b10   << 6)) throw std::format_error("utf8 byte[1] should start with '0b10'");
		if ((z & (0b10   << 6)) != (0b10   << 6)) throw std::format_error("utf8 byte[2] should start with '0b10'");

		m_codePoints = 3;
		m_bytes = new u1[m_codePoints];

		m_bytes[0] = x;
		m_bytes[1] = y;
		m_bytes[2] = z;
	} else if ((lead & (0b110 << 5)) == (0b110 << 5)) {		// 0x0080 - 0x07FF
		u1 x = bytes[0];
		u1 y = bytes[1];

		m_codePoints = 2;
		m_bytes = new u1[m_codePoints];

		if ((x & (0b110 << 5)) != (0b110 << 5))	throw std::format_error("utf8 byte[0] should start with '0b110'");
		if ((y & (0b10  << 6)) != (0b10  << 6))	throw std::format_error("utf8 byte[1] should start with '0b10'");

		m_bytes[0] = x;
		m_bytes[1] = y;
	} else if ((lead & (0b0 << 7)) == (0b0 << 7)) {			// 0x0001 - 0x007F
		u1 byte = bytes[0];

		if ((byte & (0b0 << 7)) != (0b0 << 7))	throw std::format_error("utf8 byte[0] should start with '0b0'");

		m_codePoints = 1;
		m_bytes = new u1[m_codePoints];

		m_bytes[0] = byte;
	} else {
		throw std::format_error(std::format("unknown utf8 byte {}", lead));
	}

	for (u1 i = 0; i < m_codePoints; i++) {
		if (m_bytes[i] == 0)
			throw std::format_error("utf8 byte cannot be 0");
		if (m_bytes[i] >= 0xf0)
			throw std::format_error("utf8 byte cannot be in the range 0xf0 - 0xff");
	}
}

tjvm::Utf8::Utf8(const Utf8& other) : m_codePoints(other.m_codePoints), m_bytes(m_codePoints > 0 ? new u1[m_codePoints] : nullptr) {
	for (u1 i = 0; i < m_codePoints; i++)
		m_bytes[i] = other.m_bytes[i];
}

tjvm::Utf8::Utf8(Utf8&& other) noexcept : m_codePoints(other.m_codePoints), m_bytes(other.m_bytes) {
	other.m_bytes = nullptr;
}

tjvm::Utf8::~Utf8() {
	delete[] m_bytes;
}

tjvm::Utf8& tjvm::Utf8::operator=(Utf8&& other) noexcept {
	if (this != &other) {
		delete[] m_bytes;

		m_codePoints = other.m_codePoints;
		m_bytes = other.m_bytes;

		other.m_codePoints = 0;
		other.m_bytes = nullptr;
	}
	return *this;
}

tjvm::Utf8& tjvm::Utf8::operator=(const Utf8& other) noexcept {
	if (this != &other) {
		delete[] m_bytes;

		m_codePoints = other.m_codePoints;
		m_bytes = new u1[m_codePoints];

		memcpy(m_bytes, other.m_bytes, m_codePoints);
	}
	return *this;
}

u1 tjvm::Utf8::getCodePoints() const {
	return m_codePoints;
}

#include "javaCP.h"

template class List<u1>;
template class List<u2>;
template class List<u4>;
template class List<u8>;
template class List<tjvm::Utf8>;
template class List<tjvm::ConstantPool>;

std::string std::to_string(const tjvm::Utf8& utf8) {
	switch (utf8.m_codePoints) {
	case 1: {
		u8 byte = utf8.m_bytes[0];

		byte &= 0b01111111;

		u8 str = byte;
		return std::string((char*) &str, 1);
	}
	case 2: {
		u8 x = utf8.m_bytes[1];
		u8 y = utf8.m_bytes[0];

		x &= 0b00011111;
		y &= 0b00111111;

		u8 str = ((x & 0x1f) << 6) + (y & 0x3f);
		return std::string((char*) &str, 1);
	}
	case 3: {
		u8 x = utf8.m_bytes[2];
		u8 y = utf8.m_bytes[1];
		u8 z = utf8.m_bytes[0];

		x &= 0b00001111;
		y &= 0b00111111;
		y &= 0b00111111;

		u8 str = ((x & 0xf) << 12) + ((y & 0x3f) << 6) + (z & 0x3f);
		return std::string((char*) &str, 1);
	}
	case 6: {
		u8 u = utf8.m_bytes[5];
		u8 v = utf8.m_bytes[4];
		u8 w = utf8.m_bytes[3];
		u8 x = utf8.m_bytes[2];
		u8 y = utf8.m_bytes[1];
		u8 z = utf8.m_bytes[0];

		u &= 0b00000000;
		v &= 0b00001111;
		w &= 0b00111111;
		x &= 0b00000000;
		y &= 0b00001111;
		y &= 0b00111111;

		u8 str = 0x10000 + ((v & 0x0f) << 16) + ((w & 0x3f) << 10) + ((y & 0x0f) << 6) + (z & 0x3f);
		return std::string((char*) &str, 1);
	}
	}
	return reinterpret_cast<const char*>(u8"invalid");
}

std::string std::to_string(const List<tjvm::Utf8>& utf8) {
	std::ostringstream ss{};

	for (u2 i = 0; i < utf8.getSize(); i++)
		ss << std::to_string(utf8[i]);

	return ss.str();
}
