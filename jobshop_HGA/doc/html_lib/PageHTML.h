/*
 * GanttHTML.h
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#ifndef HTML_PAGEHTML_H_
#define HTML_PAGEHTML_H_

#include <string>

#include "TagContentHTML.h"

class PageHTML: public ContentHTML {
	TagContentHTML head;
	TagContentHTML body;
public:
	PageHTML() :
			head("head"), body("body") {}

	enum Section {
		HEAD,
		BODY
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

#endif /* HTML_PAGEHTML_H_ */
