#pragma once

#include <cstdint>
#include <string>

using u1 = uint8_t;
using u2 = uint16_t;
using u4 = uint32_t;
using u8 = uint64_t;

template<typename T>
class List {
public:
	List();
	explicit List(u2 size);
	List(const List<T>& other);
	List(List<T>&& other) noexcept;
	~List();

	List<T>& operator=(List<T>&& other) noexcept;

	T& operator[](u2 i);
	const T& operator[](u2 i) const;
	
	u2 getSize() const;
private:
	u2 m_size;
	T* m_data;
};

namespace tjvm {
	class Utf8;
}

namespace std {
	std::string to_string(const tjvm::Utf8& utf8);
	std::string to_string(const List<tjvm::Utf8>& utf8);
}

namespace tjvm {
	class Utf8 {
	public:
		Utf8();
		explicit Utf8(u8 data);
		Utf8(const Utf8& other);
		Utf8(Utf8&& other) noexcept;
		~Utf8();

		Utf8& operator=(Utf8&& other) noexcept;
		Utf8& operator=(const Utf8& other) noexcept;

		u1 getCodePoints() const;

		friend std::string std::to_string(const tjvm::Utf8& utf8);

	private:
		u1 m_codePoints;
		u1* m_bytes;
	};
}

