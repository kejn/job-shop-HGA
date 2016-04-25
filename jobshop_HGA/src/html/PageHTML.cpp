/*
 * GanttHTML.cpp
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#include "../../inc/html/PageHTML.h"
#include "../../inc/html/TextContentHTML.h"

#include <sstream>

using namespace std;

void PageHTML::add(const PageHTML::Section& section,
		ContentHTML * const & childElement) {
	if (section == Section::HEAD) {
		_head.addChild(childElement);
	} else if (section == Section::BODY) {
		_body.addChild(childElement);
	}
}

void PageHTML::addContentType(string contentType) {
	TagContentHTML* meta = new TagContentHTML("meta");
	meta->addParam("http-equiv", "Content-Type");
	meta->addParam("content", contentType);
	add(Section::HEAD, meta);
}

void PageHTML::addTitle(string title) {
	TagContentHTML* titleTag = new TagContentHTML("title");
	titleTag->addChild(new TextContentHTML(title));
	add(Section::HEAD, titleTag);
}

void PageHTML::addStyle(string url) {
	TagContentHTML* linkTag = new TagContentHTML("link");
	linkTag->addParam("rel", "stylesheet");
	linkTag->addParam("type", "text/css");
	linkTag->addParam("href", url);
	add(Section::HEAD, linkTag);
}

void PageHTML::addScript(string url) {
	TagContentHTML* scriptTag = new TagContentHTML("script");
	scriptTag->addChild(new TextContentHTML(""));
	scriptTag->addParam("src", url);
	add(Section::HEAD, scriptTag);
}

std::string PageHTML::toString() {
	stringstream ss;
	ss << "<!doctype html>\n";
	ss << "<html>\n";
	ss << _head.toString() << endl;
	ss << _body.toString() << endl;
	ss << "</html>" << endl;

	return (ss.str());
}
