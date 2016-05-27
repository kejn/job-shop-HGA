/*
 * files.cpp
 *
 *  Created on: 25 kwi 2016
 *      Author: Kamil
 */

#include "../../inc/util/files.h"

#include <bits/basic_ios.tcc>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void openFile(fstream &file, const string &fileName,
		const std::ios_base::openmode openmode) throw (string) {
	file.open(fileName.c_str(), openmode);
	if (file.bad()) {
		if (file.is_open()) {
			file.close();
		}
		string message = "Nie udalo sie otworzyc pliku \"" + fileName + "\"";
		throw message;
	}
}

void copyFile(const string sourceFileName, const string destinationFileName)
		throw (string) {
	fstream sourceFile, destinationFile;
	openFile(sourceFile, sourceFileName, ios::in | ios::binary);
	openFile(destinationFile, destinationFileName, ios::out | ios::binary);

	destinationFile << sourceFile.rdbuf();
	destinationFile.close();
	sourceFile.close();
}

void createDirectory(std::string directoryName) {
	for (unsigned int i = 0; i < directoryName.size(); ++i) {
		if (directoryName[i] != '/') {
			continue;
		}
		directoryName[i] = '\\';
		string newDirectory = directoryName.substr(0, i).c_str();
		cout << "Creating directory '" << newDirectory << "'..." << endl;
		system(string("mkdir ").append(newDirectory).c_str());
	}
}
