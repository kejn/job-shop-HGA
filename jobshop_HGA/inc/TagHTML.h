/*
 * TagHTML.h
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#ifndef TAGHTML_H_
#define TAGHTML_H_

#include <vector>
#include <map>

class TagHTML {
protected:
	std::string name;
	std::map<std::string, std::string> params;
	std::vector<TagHTML> children;
public:
	TagHTML(const std::string &name);
	TagHTML(const std::string &name, std::map<std::string, std::string> params);
	TagHTML(const std::string &name, std::map<std::string, std::string> params,
			std::vector<TagHTML> children);
	void addParam(const std::string &param, const std::string &value);
	void addChild(const TagHTML &childElement);

	std::string toString();
};

#endif /* TAGHTML_H_ */
