/*
 * ContentHTML.h
 *
 *  Created on: 22 kwi 2016
 *      Author: Kamil
 */

#ifndef HTML_CONTENTHTML_H_
#define HTML_CONTENTHTML_H_

#include <string>

class ContentHTML {
protected:
	ContentHTML(){}
public:
	virtual std::string toString() = 0;
	virtual ~ContentHTML(){}
};

#endif /* CONTENTHTML_H_ */
