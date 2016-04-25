/*
 * TextContentHTML.cpp
 *
 *  Created on: 22 kwi 2016
 *      Author: Kamil
 */

#include "../../inc/html/TextContentHTML.h"

TextContentHTML::TextContentHTML(const std::string& text) {
	this->text = text;
}

std::string TextContentHTML::toString() {
	return (text);
}
