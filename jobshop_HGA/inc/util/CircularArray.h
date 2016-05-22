/*
 * CircularArray.h
 *
 *  Created on: 21 maj 2016
 *      Author: Kamil
 */

#ifndef UTIL_CIRCULARARRAY_H_
#define UTIL_CIRCULARARRAY_H_

#include <stdexcept>
#include <string>
#include <vector>

#include "stringUtil.h"

using uint = unsigned int;

template<typename T>
class CircularArray {
	uint maxCapacity;
	uint beginIndex;
	uint size;
	std::vector<T> array;
public:
	CircularArray(uint capacity = 0) : //FIXME
			array(capacity) {
		this->maxCapacity = capacity;
		beginIndex = 0;
		size = 0;
	}

	CircularArray(const CircularArray<T> &other) : CircularArray(other.maxCapacity){
		(*this) = other;
	}

	const CircularArray<T> &operator =(const CircularArray<T> &other) {
		beginIndex = other.beginIndex;
		size = other.size;
		array = other.array;
		return (*this);
	}

	const T &operator[](uint index) const {
		if (index >= size) {
			std::string message = "index out of bounds. index["
					+ stringUtil::toString(index) + "], size["
					+ stringUtil::toString(size) + "]";
			throw std::out_of_range(message);
		}
		return array[(beginIndex + index) % maxCapacity];
	}

	void push_back(const T & elem) {
		array[(beginIndex+size) % maxCapacity] = elem;
		if (size < maxCapacity) {
			++size;
		} else {
			beginIndex = (beginIndex + 1) % maxCapacity;
		}
	}

	const T &first() const {
		return (*this)[beginIndex];
	}

	const T &last() const{
		return (*this)[size-1];
	}

	const uint capacity() const {
		return maxCapacity;
	}

};

#endif /* UTIL_CIRCULARARRAY_H_ */
