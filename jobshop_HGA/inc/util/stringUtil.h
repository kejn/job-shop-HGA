/*
 * stringUtil.h
 *
 *  Created on: 27 kwi 2016
 *      Author: Kamil
 */

#ifndef UTIL_STRINGUTIL_H_
#define UTIL_STRINGUTIL_H_

#include <sstream>
#include <string>

namespace stringUtil {

template<typename T>
std::string toString(const T & value){
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

}
#endif /* UTIL_STRINGUTIL_H_ */
