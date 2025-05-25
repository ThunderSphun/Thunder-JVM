#include "types.h"

#include <iostream>

template<typename T>
list<T>::list() : list(0) {
}

template<typename T>
list<T>::list(u2 size) : size(size), data(size > 0 ? new T[size] : nullptr) {
	//std::cout << "creating list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
}

template<typename T>
list<T>::list(const list<T>& other): size(other.size), data(other.data) {
	//std::cout << "copying list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
}

template<typename T>
list<T>::list(list<T>&& other) noexcept: size(other.size), data(other.data) {
	//std::cout << "moving list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
	other.data = nullptr;
}

template<typename T>
list<T>::~list() {
	if (data != nullptr)
		//std::cout << "removing list<" << typeid(T).name() << "> (" << size << ")" << std::endl;
	delete[] data;
}

template<typename T>
list<T>& list<T>::operator=(list<T>&& other) noexcept {
	if (this != &other) {
		size = 0;
		delete[] data;

		size = other.size;
		data = other.data;
		
		other.size = 0;
		other.data = nullptr;
	}
	return *this;
}

template<typename T>
T& list<T>::operator[](u2 i) {
	if (i < size)
		return data[i];

	throw std::out_of_range("Index out of bounds");
}

template<typename T>
const T& list<T>::operator[](u2 i) const {
	if (i < size)
		return data[i];

	throw std::out_of_range("Index out of bounds");
}

#include "javaData.h"

template class list<u1>;
template class list<tjvm::cpInfo>;
