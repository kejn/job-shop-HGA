/*
 * TagHTML.cpp
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#include "../inc/TagHTML.h"

#include <sstream>

using namespace std;

TagHTML::TagHTML(const string& name) {
	this->name = name;
}

TagHTML::TagHTML(const string& name, map<string, string> params) {
	this->name = name;
	this->params = params;
}

TagHTML::TagHTML(const string& name, map<string, string> params,
		vector<TagHTML> children) {
	this->name = name;
	this->params = params;
	this->children = children;
}

void TagHTML::addParam(const string& param, const string& value) {
	params.insert(pair<string,string>(param,value));
}

void TagHTML::addChild(const TagHTML& childElement) {
	children.push_back(childElement);
}

std::string TagHTML::toString() {
	stringstream ss;
	ss << "<" << name;
	for(const auto & param : params) {
		ss << " " << param.first << "=\"" << param.second << "\"";
	}
	if(children.empty()) {
		ss << " />";
	} else {
		ss << ">";
		for(auto & childElement : children) {
			ss << childElement.toString();
		}
		ss << "</" << name << ">";
	}
	return (ss.str());
}
