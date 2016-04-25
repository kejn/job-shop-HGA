/*
 * GanttHTML.h
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#ifndef PAGEHTML_H_
#define PAGEHTML_H_

#include <string>

#include "TagContentHTML.h"

class PageHTML: public ContentHTML {
	TagContentHTML _head;
	TagContentHTML _body;
public:
	PageHTML() :
			_head("head"), _body("body") {
	}

	enum Section {
		HEAD, BODY
	};
	void add(const PageHTML::Section& section,
			ContentHTML * const & childElement);

	void addContentType(std::string contentType);
	void addTitle(std::string title);
	void addStyle(std::string url);
	void addScript(std::string url);

	virtual std::string toString();
	virtual ~PageHTML() {
	}
};

#endif /* PAGEHTML_H_ */
