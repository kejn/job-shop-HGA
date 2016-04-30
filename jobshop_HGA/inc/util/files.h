/*
 * files.h
 *
 *  Created on: 25 kwi 2016
 *      Author: Kamil
 */

#ifndef UTIL_FILES_H_
#define UTIL_FILES_H_

#include <ios>
#include <fstream>
#include <string>

static const std::string GENERATED_FOLDER = "generated/";
static const std::string RESOURCE_FOLDER = "res/";

void openFile(std::fstream &file, const std::string &fileName,
		const std::ios_base::openmode openmode) throw (std::string);

void copyFile(const std::string sourceFileName,
		const std::string destinationFileName) throw (std::string);

void createDirectory(std::string directoryName);

#endif /* UTIL_FILES_H_ */
