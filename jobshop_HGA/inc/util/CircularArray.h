/*
 * CircularArray.h
 *
 *  Created on: 21 maj 2016
 *      Author: Kamil
 */

#ifndef UTIL_CIRCULARARRAY_H_
#define UTIL_CIRCULARARRAY_H_

#include <list>
#include <stdexcept>
#include <string>

#include "stringUtil.h"

using uint = unsigned int;

template<typename T>
class CircularArray {
	uint maxCapacity;
	std::list<T> array;
public:
	CircularArray(uint capacity = 0) {
		this->maxCapacity = capacity;
	}

	CircularArray(const CircularArray<T> &other) :
			CircularArray(other.maxCapacity) {
		(*this) = other;
	}

	const CircularArray<T> &operator =(const CircularArray<T> &other) {
		array = other.array;
		return (*this);
	}

	const T &operator[](uint index) const throw (std::out_of_range) {
		if ((index >= array.size()) || (index < 0)) {
			std::string message = "index out of bounds. index["
					+ stringUtil::toString(index) + "], size["
					+ stringUtil::toString(array.size()) + "]";
			throw std::out_of_range(message);
		}
		auto iter = array.begin();
		for (; (index > 0) && (iter != array.end()); ++iter, --index);
		return *iter;
	}

	void push_back(const T & elem) {
		array.push_back(elem);
		if (array.size() > maxCapacity) {
			array.pop_front();
		}
	}

	void pop_front() {
		array.pop_front();
	}

	const T &first() const {
		return array.front();
	}

	const T &last() const {
		return array.back();
	}

	uint capacity() {
		return maxCapacity;
	}

	uint size() {
		return array.size();
	}

	bool empty() {
		return array.empty();
	}

};

#endif /* UTIL_CIRCULARARRAY_H_ */
