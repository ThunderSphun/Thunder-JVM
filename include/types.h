#pragma once

#include <cstdint>

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
	
	u2 getSize() const { return m_size; };
private:
	u2 m_size;
	T* m_data;
};
