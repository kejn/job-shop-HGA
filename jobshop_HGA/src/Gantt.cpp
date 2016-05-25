/*
 * Gantt.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Gantt.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>

#include "../inc/html/PageHTML.h"
#include "../inc/html/TagContentHTML.h"
#include "../inc/html/TextContentHTML.h"
#include "../inc/util/files.h"
#include "../inc/util/stringUtil.h"

using namespace std;
using namespace stringUtil;

vector<string> Gantt::colors;

void Gantt::addOperation(uint i, const Oper &operation) {
	operations[i].push_back(operation);
	++totNOper;
}

Oper Gantt::nextOperationTo(uint job, uint operation) const throw (string) {
	if ((job >= nJobs) || (operation >= operations[job].size() - 2)) {
		throw string("next operation: operation not found.");
	}
	return operations[job][operation + 1];
}

Oper Gantt::prevOperationTo(uint job, uint referenceOperation) const
		throw (string) {
	if ((job >= nJobs) || (referenceOperation == 0)) {
		throw string("prev operation: operation not found.");
	}
	return operations[job][referenceOperation - 1];
}

void Gantt::addOnMachine(uint i, const Oper &operation) {

	machines[i].push_back(operation);
}

void Gantt::printJobs() {
	cout << "Gantt::printJobs()" << endl;
	for (uint i = 0; i < operations.size(); ++i) {
		for (uint j = 0; j < operations[i].size(); ++j) {
			if (j == 0) {
				cout << "job" << operations[i][0].getPid() << endl;
			}
			cout << "operation" << operations[i][j].getId() << endl;
			for (const auto & entry : operations[i][j].getProcessingTimes()) {
				cout << setw(4) << 'M' << setw(2) << setfill('0') << entry.first
						<< setw(3) << setfill(' ') << entry.second << ' '
						<< flush;
			}
			cout << endl;
		}
		cout << endl;
	}
}

void Gantt::printMachines() {
	for (uint i = 0; i < machines.size(); ++i) {
		cout << "Machine" << i << '\t';
		for (uint j = 0; j < machines[i].size(); ++j) {
			cout << setw(4) << 'J' << setw(2) << setfill('0')
					<< machines[i][j].getPid() << setfill(' ') << '('
					<< machines[i][j].getId() << ')' << flush;
		}
		cout << endl;
	}
}

Permutation& Gantt::getMachines() {
	return machines;
}

const Permutation& Gantt::getMachines() const {
	return machines;
}

Permutation& Gantt::getOperations() {
	return operations;
}

const Permutation& Gantt::getOperations() const {
	return operations;
}

uint Gantt::getNJobs() const {
	return nJobs;
}

uint Gantt::getNMachines() const {
	return nMachines;
}

void Gantt::insertOnMachine(uint machineIndex,
		const std::vector<Oper>::iterator &iterator, Oper operation) {
	machines[machineIndex].insert(iterator, operation);
	operations[operation.getPid()][operation.getId()] = operation;
}

string Gantt::getRandomColor() {
	char letters[] = "56789ABCDE";
	string color = "#";
	for (int i = 0; i < 6; ++i) {
		color += letters[rand() % 10];
	}
	return color;
}

string Gantt::getColor(uint index) {
	if (index >= Gantt::colors.size()) {
		Gantt::colors.resize(index + 1);
	}
	if (Gantt::colors[index].empty()) {
		Gantt::colors[index] = getRandomColor();
	}
	return Gantt::colors[index];
}

void Gantt::printMachinesHTML(string fileName) {
	createDirectory(GENERATED_FOLDER);
	fstream outputFile;
	try {
		openFile(outputFile, GENERATED_FOLDER + fileName, ios::out);
	} catch (const string &message) {
		cerr << message << endl;
		return;
	}

	vector<Oper> path = criticalPath(*this);
//	cout << "PATH:" << endl;
//	for(const auto & o : path) {
//		cout << o.toString() << endl;
//	}

	const string stylesFileName = "css/gantt_styles.css";
	const string scriptFileName = "js/hover_functions.js";

	PageHTML htmlPage;
	htmlPage.addContentType("text/html; charset=UTF-8");
	htmlPage.addTitle("Machines");
	htmlPage.addStyle(stylesFileName);
	htmlPage.addScript(scriptFileName);

	TagContentHTML* div = new TagContentHTML("div");
	for (uint i = 0; i < machines.size(); ++i) {
		TagContentHTML* table = new TagContentHTML("table");
		TagContentHTML* tr = new TagContentHTML("tr");
		TagContentHTML* td = new TagContentHTML("td");

		string machineLabel = "Machine " + toString(i);
		TextContentHTML* machineNo = new TextContentHTML(machineLabel);
		td->addParam("style", "width: 100px;");
		td->addChild(machineNo);

		tr->addChild(td);
		for (uint j = 0; j < machines[i].size(); ++j) {
			uint sCurr = machines[i][j].getStartingTime();
			if (j > 0) {
				uint cPrev = machines[i][j - 1].getCompletitionTime();
				if (cPrev < sCurr) {
					TagContentHTML* tdOperation = new TagContentHTML("td");
					string style("width: ");
					style += toString(HTML_SCALE * (sCurr - cPrev) - 1) + "px;";

					tdOperation->addParam("style", style);
					tr->addChild(tdOperation);
				}
			} else if (sCurr > 0) {
				TagContentHTML* tdOperation = new TagContentHTML("td");
				string style("width: ");
				style += toString(HTML_SCALE * sCurr - 1) + "px;";

				tdOperation->addParam("style", style);
				tr->addChild(tdOperation);
			}
			TagContentHTML* tdOperation = new TagContentHTML("td");
			string style = "width: "
					+ toString(
							HTML_SCALE * machines[i][j].getProcessingTime() - 1)
					+ "px;" + "background-color: "
					+ getColor(machines[i][j].getPid()) + ";";

			vector<Oper>::iterator found = find_if(path.begin(), path.end(),
					[=](const Oper &o) {
						return o.toString() == machines[i][j].toString();
					});
			if (found != path.end()) {
				style += " border-top: 2px black ridge;";
				style += " border-bottom: 2px black ridge;";
			}

			tdOperation->addParam("style", style);
			tdOperation->addParam("title", machines[i][j].toString());

			string jobNumber = "job" + toString(machines[i][j].getPid());
			if (found != path.end()) {
				tdOperation->addParam("class", jobNumber + " cpath");
				tdOperation->addParam("onclick", "cpath()");
			} else {
				tdOperation->addParam("class", jobNumber);
			}
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

	for (uint i = 0; i < nJobs; ++i) {
		string color(getColor(i));

		TagContentHTML* td = new TagContentHTML("td");
		string style = "text-align: center; width: 50px; background-color: "
				+ getColor(i) + ";";
		td->addParam("style", style);

		string jobNumber = "job" + toString(i);
		string cMax = "cMax: "
				+ toString(operations[i].back().getCompletitionTime());
		td->addParam("title", cMax);
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
	for (auto & machine : machines) {
		machine.clear();
	}
}

uint Gantt::getTotNOper() const {
	return totNOper;
}

vector<uint> Gantt::randomJobOrder() {
	vector<uint> sequence(nJobs);
	int n = 0;
	generate(sequence.begin(), sequence.end(), [&n] {return n++;});
	random_shuffle(sequence.begin(), sequence.end());
	return sequence;
}

Gantt::Gantt(uint nJobs, uint nMachines, uint htmlScale) :
		HTML_SCALE(htmlScale) {
	this->nJobs = nJobs;
	this->nMachines = nMachines;
	totNOper = 0;
	operations = Permutation(nJobs);
	machines = Permutation(nMachines);
	Gantt::colors.reserve(nJobs);

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

vector<Oper> criticalPath(const Gantt & gantt, int machineIndex, int opIndex) {

	vector<Oper> path;

	if (machineIndex == -1 || opIndex == -1) {
		machineIndex = cMax(gantt.getMachines()).second;
		opIndex = gantt.getMachines()[machineIndex].size() - 1;
		return criticalPath(gantt, machineIndex, opIndex);
	}

	if (opIndex > 0) {
		Oper operation = gantt.getMachines()[machineIndex][opIndex];
		Oper prevOnMachine = gantt.getMachines()[machineIndex][opIndex - 1];
		uint starting = operation.getStartingTime();
		uint completitionMachine = prevOnMachine.getCompletitionTime();

		if (starting == completitionMachine) {
			path = criticalPath(gantt, machineIndex, opIndex - 1);
		} else {
			try {
				uint job = operation.getPid();
				uint referenceOperation = operation.getId();
				Oper prevInJob = gantt.prevOperationTo(job, referenceOperation);

				uint newMachineIndex = prevInJob.getMachineNumber();
				vector<Oper> & newMachine =
						const_cast<vector<Oper>&>(gantt.getMachines()[newMachineIndex]);
				vector<Oper>::iterator iter = find_if(newMachine.begin(),
						newMachine.end(), [&](const Oper &o) {
							return o == prevInJob;
						});
				uint indexOnNewMachine = distance(newMachine.begin(), iter);

				path = criticalPath(gantt, newMachineIndex, indexOnNewMachine);
			} catch (const string & message) {
				cerr << message << endl;
			}
		}
	}
	path.push_back(gantt.getMachines()[machineIndex][opIndex]);
	return path;
}

pair<uint, uint> cMax(const Permutation& permutation) {
	uint cMax = 0;
	uint machineIndex = 0;

	for (uint i = 0; i < permutation.size(); ++i) {
		uint c = permutation[i].back().getCompletitionTime();
		if (c > cMax) {
			cMax = c;
			machineIndex = i;
		}
	}
	return make_pair(cMax, machineIndex);
}
