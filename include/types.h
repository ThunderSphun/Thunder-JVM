#pragma once

#include <cstdint>

using u1 = uint8_t;
using u2 = uint16_t;
using u4 = uint32_t;
using u8 = uint64_t;

template<typename T>
class list {
public:
	list();
	explicit list(u2 size);
	list(const list<T>& other);
	list(list<T>&& other) noexcept;
	~list();

	list<T>& operator=(list<T>&& other) noexcept;

	T& operator[](u2 i);
	const T& operator[](u2 i) const;
	
	u2 getSize() const { return size; };
private:
	u2 size;
	T* data;
};
