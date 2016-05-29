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

#include "../Oper.h"

using uint = unsigned int;

class TagContentHTML: virtual public ContentHTML {
protected:
	std::string name;
	std::map<std::string, std::string> params;
	std::vector<ContentHTML*> children;
public:
	TagContentHTML(const std::string &name) :
			name(name) {
	}
	TagContentHTML(const std::string &name,
			std::map<std::string, std::string> params) :
			name(name), params(params) {
	}
	TagContentHTML(const std::string &name,
			std::map<std::string, std::string> params,
			std::vector<ContentHTML*> children) :
			name(name), params(params), children(children) {
	}

	static TagContentHTML * forTDEmptyOperation(uint width);
	static TagContentHTML * forTDOperation(Oper oper, std::string bgColor,
			uint htmlScale, bool inCriticalPath = false);
	static TagContentHTML * forTDJobLegend(uint jobNumber, std::string bgColor, uint cMax);

	void addParam(const std::string &param, const std::string &value);
	void addChild(ContentHTML * const & childElement);

	virtual std::string toString();
	virtual ~TagContentHTML();
};

#endif /* HTML_TAGCONTENTHTML_H_ */
