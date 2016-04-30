/*
 * TagHTML.h
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#ifndef HTML_TAGCONTENTHTML_H_
#define HTML_TAGCONTENTHTML_H_

#include <map>
#include <string>
#include <vector>

#include "ContentHTML.h"

class TagContentHTML: virtual public ContentHTML {
protected:
	std::string name;
	std::map<std::string, std::string> params;
	std::vector<ContentHTML*> children;
public:
	TagContentHTML(const std::string &name) :
			name(name) {}
	TagContentHTML(const std::string &name,
			std::map<std::string, std::string> params) :
			name(name), params(params) {}
	TagContentHTML(const std::string &name,
			std::map<std::string, std::string> params,
			std::vector<ContentHTML*> children) :
			name(name), params(params), children(children) {}

	void addParam(const std::string &param, const std::string &value);
	void addChild(ContentHTML * const & childElement);

	virtual std::string toString();
	virtual ~TagContentHTML();
};

#endif /* HTML_TAGCONTENTHTML_H_ */
