/*
 * GanttHTML.h
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#ifndef GANTTHTML_H_
#define GANTTHTML_H_

#include "TagHTML.h"

#include <string>

class GanttHTML {
	TagHTML head;
	TagHTML body;
public:
	GanttHTML() : head("head"), body("body") {}
	std::string toString();
};

#endif /* GANTTHTML_H_ */
