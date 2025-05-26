#include "types.h"

#include <iostream>

template<typename T>
List<T>::List() : List(0) {
}

template<typename T>
List<T>::List(u2 size) : m_size(size), m_data(m_size > 0 ? new T[m_size] : nullptr) {
	//std::cout << "creating list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
}

template<typename T>
List<T>::List(const List<T>& other): m_size(other.m_size), m_data(other.m_data) {
	//std::cout << "copying list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
}

template<typename T>
List<T>::List(List<T>&& other) noexcept: m_size(other.m_size), m_data(other.m_data) {
	//std::cout << "moving list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
	other.m_data = nullptr;
}

template<typename T>
List<T>::~List() {
	//if (m_data != nullptr)
		//std::cout << "removing list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
	delete[] m_data;
}

template<typename T>
List<T>& List<T>::operator=(List<T>& other) noexcept {
	if (this != &other) {
		m_size = 0;
		delete[] m_data;

		m_size = other.m_size;
		m_data = new T[m_size];
		memcpy(m_data, other.m_data, m_size);
	}
	return *this;
}

template<typename T>
List<T>& List<T>::operator=(List<T>&& other) noexcept {
	if (this != &other) {
		m_size = 0;
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

#include "javaData.h"

template class List<u1>;
template class List<u2>;
template class List<u4>;
template class List<u8>;
template class List<tjvm::ConstantPool>;

