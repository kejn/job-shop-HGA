/*
 * GanttHTML.cpp
 *
 *  Created on: 20 kwi 2016
 *      Author: Kamil
 */

#include "../inc/GanttHTML.h"

#include <sstream>

using namespace std;

std::string GanttHTML::toString() {
	stringstream ss;
	ss << "<!doctype html>\n";
	ss << "<html>";
	ss << "<head>" << head.toString() << "</head>";
	ss << "<body>" << body.toString() << "</body>";
	ss << "</html>";

	return (ss.str());
}
