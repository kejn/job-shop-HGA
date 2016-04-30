/*
 * TagHTML.cpp
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#include "../../inc/html/TagContentHTML.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>

using namespace std;

void TagContentHTML::addParam(const string& param, const string& value) {
	params.insert(pair<string, string>(param, value));
}

void TagContentHTML::addChild(ContentHTML * const & childElement) {
	children.push_back(childElement);
}

std::string TagContentHTML::toString() {
	stringstream ss;
	ss << "<" << name;
	for (const auto & param : params) {
		ss << " " << param.first << "=\"" << param.second << "\"";
	}
	if (children.empty() && name.compare("body") && name.compare("head")) {
		ss << " />\n";
	} else {
		ss << ">";
		for (const auto & childElement : children) {
			ss << '\n' << childElement->toString();
		}
		ss << "	</" << name << ">";
	}
	return (ss.str());
}

TagContentHTML::~TagContentHTML() {
	vector<ContentHTML*>::iterator iter = children.begin();
	for (; iter != children.end(); ++iter) {
		if (*iter != nullptr) {
			delete *iter;
		}
	}
}
