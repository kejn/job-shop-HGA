/*
 * TagHTML.cpp
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#include "../../inc/html/TagContentHTML.h"

#include <iterator>
#include <sstream>
#include <utility>

#include "../../inc/html/TextContentHTML.h"
#include "../../inc/util/stringUtil.h"

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
	return ss.str();
}

TagContentHTML* TagContentHTML::forTDOperation(Oper oper, string bgColor,
		uint htmlScale, bool inCriticalPath) {
	TagContentHTML* tdOperationTag = new TagContentHTML("td");
	string style = "width: "
			+ stringUtil::toString(htmlScale * oper.getProcessingTime() - 1)
			+ "px;" + "background-color: " + bgColor + ";";

	if (inCriticalPath) {
		style += " border-top: 2px black ridge;";
		style += " border-bottom: 2px black ridge;";
	}

	tdOperationTag->addParam("style", style);
	tdOperationTag->addParam("title", oper.toString());

	string jobNumber = "job" + stringUtil::toString(oper.getPid());
	if (inCriticalPath) {
		tdOperationTag->addParam("class", jobNumber + " cpath");
		tdOperationTag->addParam("onclick", "cpath()");
	} else {
		tdOperationTag->addParam("class", jobNumber);
	}
	tdOperationTag->addParam("onmouseover", "mOver('" + jobNumber + "')");
	tdOperationTag->addParam("onmouseout", "mOut()");

	return tdOperationTag;
}

TagContentHTML::~TagContentHTML() {
	vector<ContentHTML*>::iterator iter = children.begin();
	for (; iter != children.end(); ++iter) {
		if (*iter != nullptr) {
			delete *iter;
		}
	}
}

TagContentHTML* TagContentHTML::forTDEmptyOperation(uint width) {
	TagContentHTML *tdTag = new TagContentHTML("td");

	string style("width: ");
	style += stringUtil::toString(width) + "px;";

	tdTag->addParam("style", style);
	return tdTag;
}

TagContentHTML* TagContentHTML::forTDJobLegend(uint jobNumber,
		std::string bgColor, uint cMax) {
	TagContentHTML *td = new TagContentHTML("td");
	string style = "text-align: center; width: 100px; background-color: "
			+ bgColor + ";";
	td->addParam("style", style);

	string jobNumberClass = "job" + stringUtil::toString(jobNumber);
	string cMaxString = "cMax: " + stringUtil::toString(cMax);
	td->addParam("title", cMaxString);
	td->addParam("class", jobNumberClass);
	td->addParam("onmouseover", "mOver('" + jobNumberClass + "')");
	td->addParam("onmouseout", "mOut()");
	td->addChild(new TextContentHTML("Job " + stringUtil::toString(jobNumber)));

	return td;
}
