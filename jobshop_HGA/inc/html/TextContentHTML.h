/*
 * TextContentHTML.h
 *
 *  Created on: 22 kwi 2016
 *      Author: Kamil
 */

#ifndef HTML_TEXTCONTENTHTML_H_
#define HTML_TEXTCONTENTHTML_H_

#include <string>

#include "ContentHTML.h"

class TextContentHTML: public ContentHTML {
protected:
	std::string text;
public:
	TextContentHTML(const std::string& text) :
			text(text) {}
	virtual std::string toString();
	virtual ~TextContentHTML() {
	}
};

#endif /* HTML_TEXTCONTENTHTML_H_ */
