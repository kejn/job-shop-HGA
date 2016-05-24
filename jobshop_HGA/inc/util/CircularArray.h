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
#include <algorithm>

#include "stringUtil.h"

using uint = unsigned int;

template<typename T>
class CircularArray {
	uint maxCapacity;
	uint beginIndex;
	uint arraySize;
	std::vector<T> array;
public:
	CircularArray(uint capacity = 0) : //FIXME
			array(capacity) {
		this->maxCapacity = capacity;
		beginIndex = 0;
		arraySize = 0;
	}

	CircularArray(const CircularArray<T> &other) : CircularArray(other.maxCapacity){
		(*this) = other;
	}

	const CircularArray<T> &operator =(const CircularArray<T> &other) {
		beginIndex = other.beginIndex;
		arraySize = other.arraySize;
		array = other.array;
		return (*this);
	}

	const T &operator[](uint index) const {
		if (index >= arraySize) {
			std::string message = "index out of bounds. index["
					+ stringUtil::toString(index) + "], size["
					+ stringUtil::toString(arraySize) + "]";
			throw std::out_of_range(message);
		}
		return array[(beginIndex + index) % maxCapacity];
	}

	void push_back(const T & elem) {
		array[(beginIndex+arraySize) % maxCapacity] = elem;
		if (arraySize < maxCapacity) {
			++arraySize;
		} else {
			beginIndex = (beginIndex + 1) % maxCapacity;
		}
	}

	const T &first() const {
		return (*this)[beginIndex];
	}

	const T &last() const{
		return (*this)[arraySize-1];
	}

	uint capacity() {
		return maxCapacity;
	}

	uint size() {
		return arraySize;
	}

};

#endif /* UTIL_CIRCULARARRAY_H_ */
