/*
 * Gantt.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Gantt.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>

#include "../inc/html/PageHTML.h"
#include "../inc/html/TagContentHTML.h"
#include "../inc/html/TextContentHTML.h"
#include "../inc/util/stringUtil.h"

using namespace std;
using namespace stringUtil;

vector<string> Gantt::colors;

void Gantt::addOperation(int i, const Oper &operation) {
	operations[i].push_back(operation);
	++totNOper;
}

Oper Gantt::nextOperationTo(unsigned int job, unsigned int operation)
		throw (string) {
	if ((job >= nJobs) || (operation >= operations[job].size() - 1)) {
		throw string("next operation: operation not found.");
	}
	return (operations[job][operation + 1]);
}

Oper Gantt::prevOperationTo(unsigned int job, unsigned int referenceOperation)
		throw (string) {
	if ((job >= nJobs) || (referenceOperation == 0)) {
		throw string("prev operation: operation not found.");
	}
	return (operations[job][referenceOperation - 1]);
}

void Gantt::addOnMachine(int i, const Oper &operation) {
	machines[i].push_back(operation);
}

void Gantt::printJobs() {
	cout << "Gantt::printJobs()" << endl;
	for (unsigned int i = 0; i < operations.size(); ++i) {
		cout << "job" << i << '\t';
		for (unsigned int j = 0; j < operations[i].size(); ++j) {
			cout << setw(4) << 'M' << setw(2) << setfill('0')
					<< operations[i][j].getMachineNumber() << setw(3)
					<< setfill(' ') << operations[i][j].getProcessingTime()
					<< ' ' << flush;
		}
		cout << endl;
	}
}

void Gantt::printMachines() {
	for (unsigned int i = 0; i < machines.size(); ++i) {
		cout << "Machine" << i << '\t';
		for (unsigned int j = 0; j < machines[i].size(); ++j) {
			cout << setw(4) << 'J' << setw(2) << setfill('0')
					<< machines[i][j].getPid() << setfill(' ') << '('
					<< machines[i][j].getId() << ')' << flush;
		}
		cout << endl;
	}
}

std::vector<std::vector<Oper> >& Gantt::getMachines() {
	return (machines);
}

std::vector<std::vector<Oper> >& Gantt::getOperations() {
	return (operations);
}

unsigned int Gantt::getNJobs() const {
	return (nJobs);
}

unsigned int Gantt::getNMachines() const {
	return (nMachines);
}

void Gantt::insertOnMachine(int machineIndex,
		const std::vector<Oper>::iterator &iterator, const Oper &operation) {
	machines[machineIndex].insert(iterator, operation);
	operations[operation.getPid()][operation.getId()] = operation;
}

string Gantt::getRandomColor() {
	char letters[] = "56789ABCDE";
	string color = "#";
	for (int i = 0; i < 6; ++i) {
		color += letters[rand() % 10];
	}
	return (color);
}

string Gantt::getColor(unsigned int index) {
	if (index >= Gantt::colors.size()) {
		Gantt::colors.resize(index + 1);
	}
	if (Gantt::colors[index].empty()) {
		Gantt::colors[index] = getRandomColor();
	}
	return (Gantt::colors[index]);
}

void Gantt::printMachinesHTML(std::string fileName) {
	createDirectory(GENERATED_FOLDER);
	fstream outputFile;
	try {
		openFile(outputFile, GENERATED_FOLDER + fileName, ios::out);
	} catch (const string &message) {
		cerr << message << endl;
		return;
	}

	const string stylesFileName = "css/gantt_styles.css";
	const string scriptFileName = "js/hover_functions.js";

	PageHTML htmlPage;
	htmlPage.addContentType("text/html; charset=UTF-8");
	htmlPage.addTitle("Machines");
	htmlPage.addStyle(stylesFileName);
	htmlPage.addScript(scriptFileName);

	TagContentHTML* div = new TagContentHTML("div");
	for (unsigned int i = 0; i < machines.size(); ++i) {
		TagContentHTML* table = new TagContentHTML("table");
		TagContentHTML* tr = new TagContentHTML("tr");
		TagContentHTML* td = new TagContentHTML("td");

		string machineLabel = "Machine " + toString(i);
		TextContentHTML* machineNo = new TextContentHTML(machineLabel);
		td->addParam("style", "width: 100px;");
		td->addChild(machineNo);

		tr->addChild(td);
		for (unsigned int j = 0; j < machines[i].size(); ++j) {
			unsigned int sCurr = machines[i][j].getStartingTime();
			if (j > 0) {
				unsigned int cPrev = machines[i][j - 1].getCompletitionTime();
				if (cPrev < sCurr) {
					TagContentHTML* tdOperation = new TagContentHTML("td");
					string style("width: ");
					style += toString(sCurr - cPrev - 1) + "px;";
					tdOperation->addParam("style", style);
					tr->addChild(tdOperation);
				}
			} else if (sCurr > 0) {
				TagContentHTML* tdOperation = new TagContentHTML("td");
				string style("width: ");
				style += toString(sCurr - 1) + "px;";
				tdOperation->addParam("style", style);
				tr->addChild(tdOperation);
			}
			TagContentHTML* tdOperation = new TagContentHTML("td");
			string style = "width: "
					+ toString(machines[i][j].getProcessingTime() - 1) + "px;"
					+ "background-color: " + getColor(machines[i][j].getPid())
					+ ";";
			tdOperation->addParam("style", style);
			tdOperation->addParam("title", machines[i][j].toString());

			string jobNumber = "job" + toString(machines[i][j].getPid());
			tdOperation->addParam("class", jobNumber);
			tdOperation->addParam("onmouseover", "mOver('" + jobNumber + "')");
			tdOperation->addParam("onmouseout", "mOut()");
			tr->addChild(tdOperation);
		}
		table->addChild(tr);
		div->addChild(table);
	}
	htmlPage.add(PageHTML::Section::BODY, div);

	div = new TagContentHTML("div");
	TagContentHTML* table = new TagContentHTML("table");
	TagContentHTML* tr = new TagContentHTML("tr");

	for (unsigned int i = 0; i < colors.size(); ++i) {
		string color(getColor(i));

		TagContentHTML* td = new TagContentHTML("td");
		string style = "text-align: center; width: 50px; background-color: "
				+ getColor(i) + ";";
		td->addParam("style", style);

		string jobNumber = "job" + toString(i);
		td->addParam("class", jobNumber);
		td->addParam("onmouseover", "mOver('" + jobNumber + "')");
		td->addParam("onmouseout", "mOut()");
		td->addChild(new TextContentHTML("Job " + toString(i)));

		tr->addChild(td);
	}
	table->addChild(tr);
	div->addChild(table);
	htmlPage.add(PageHTML::Section::BODY, div);

	outputFile << htmlPage.toString();
	outputFile.close();

	try {
		createDirectory(GENERATED_FOLDER + "css/");
		createDirectory(GENERATED_FOLDER + "js/");
		copyFile(RESOURCE_FOLDER + stylesFileName,
				GENERATED_FOLDER + stylesFileName);
		copyFile(RESOURCE_FOLDER + scriptFileName,
				GENERATED_FOLDER + scriptFileName);
	} catch (const string &message) {
		cerr << message << endl;
		return;
	}
}

void Gantt::clearMachines() {
	for(auto & machine : machines) {
		machine.clear();
	}
}

unsigned int Gantt::getTotNOper() const {
	return (totNOper);
}

vector<unsigned int> Gantt::randomJobOrder() {
	vector<unsigned int> sequence(nJobs);
	int n = 0;
	generate(sequence.begin(), sequence.end(), [&n] {return (n++);});
	random_shuffle(sequence.begin(), sequence.end());
	return (sequence);
}

Gantt::Gantt(unsigned int nJobs, unsigned int nMachines) {
	this->nJobs = nJobs;
	this->nMachines = nMachines;
	totNOper = 0;
	operations = std::vector<std::vector<Oper>>(nJobs);
	machines = std::vector<std::vector<Oper>>(nMachines);
	Gantt::colors.reserve(nJobs);

	srand(time(nullptr));
	Gantt::colors = {
		"BlueViolet",
		"Crimson",
		"DarkGreen",
		"Tomato",
		"DarkOrange",
		"DodgerBlue",
		"Gold",
		"LightSalmon",
		"Lime",
		"MediumVioletRed",
		"DeepSkyBlue"
	};
}
