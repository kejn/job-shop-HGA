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
	for (uint i = 1; i < path.size(); ++i) {
		if (path[i].getStartingTime() < path[i - 1].getCompletitionTime()) {
			cout << path[i - 1].toString() << endl << path[i].toString()
					<< endl;
		}
	}

	const string stylesFileName = "css/gantt_styles.css";
	const string scriptFileName = "js/hover_functions.js";

	PageHTML htmlPage;
	htmlPage.addContentType("text/html; charset=UTF-8");
	htmlPage.addTitle("Machines");
	htmlPage.addStyle(stylesFileName);
	htmlPage.addScript(scriptFileName);

	Oper breakdown = this->breakdown;

	TagContentHTML* div = new TagContentHTML("div");
	for (uint i = 0; i < machines.size(); ++i) {
		TagContentHTML* table = new TagContentHTML("table");
		TagContentHTML* tr = new TagContentHTML("tr");
		TagContentHTML* td = TagContentHTML::forTDEmptyOperation(100);

		string machineLabel = "Machine " + toString(i);
		TextContentHTML* machineNo = new TextContentHTML(machineLabel);
		td->addChild(machineNo);

		tr->addChild(td);
		for (uint j = 0; j < machines[i].size(); ++j) {
			uint sCurr = machines[i][j].getStartingTime();

			uint bStart = breakdown.getStartingTime();
			uint bEnd = breakdown.getCompletitionTime();
			uint compTime = machines[i][j].getCompletitionTime()
					- machines[i][j].getBuffer();

			if (j > 0) {
				uint cPrev = machines[i][j - 1].getCompletitionTime();
				if (cPrev < sCurr) {
					uint width = HTML_SCALE * (sCurr - cPrev) - 1;
					if ((bEnd == sCurr) && (i == breakdown.getMachineNumber())) {
						width -= (bEnd - bStart);
					}
					TagContentHTML* tdOperation =
							TagContentHTML::forTDEmptyOperation(width);
					tdOperation->addParam("name", "1");
					tr->addChild(tdOperation);
				}
			} else if (sCurr > 0) {
				uint width = HTML_SCALE * sCurr - 1;
				if ((bEnd == sCurr)  && (i == breakdown.getMachineNumber())) {
					width -= (bEnd - bStart);
				}
				TagContentHTML* tdOperation =
						TagContentHTML::forTDEmptyOperation(width);
				tdOperation->addParam("name", "2");
				tr->addChild(tdOperation);
			}
			vector<Oper>::iterator foundInPath = find_if(path.begin(),
					path.end(), [=](const Oper &o) {
						return o.toString() == machines[i][j].toString();
					});

			TagContentHTML* tdOperation = nullptr;
			TagContentHTML* tdPrevOp = nullptr;
			TagContentHTML* tdNextOp = nullptr;

			tdOperation = TagContentHTML::forTDOperation(machines[i][j],
					getColor(machines[i][j].getPid()), HTML_SCALE,
					(foundInPath != path.end()));

			if (i == breakdown.getMachineNumber()) {
				if (whileProcessing(bStart, sCurr, compTime)) {
					string opColor = getColor(machines[i][j].getPid());

					Oper op1 = machines[i][j];
					op1.setBuffer(0);
					op1.setProcessingTime(bStart - sCurr, i);

					Oper op2 = machines[i][j];
					op2.setProcessingTime(
							compTime + machines[i][j].getBuffer() - bEnd, i);
					op2.setStartingTime(bEnd);
					op2.setBuffer(0);

					tdPrevOp = TagContentHTML::forTDOperation(op1, opColor,
							HTML_SCALE, (foundInPath != path.end()));
					tdNextOp = TagContentHTML::forTDOperation(op2, opColor,
							HTML_SCALE, (foundInPath != path.end()));

					delete tdOperation;
					tdOperation = TagContentHTML::forTDOperation(breakdown,
							"red", HTML_SCALE, (foundInPath != path.end()));
				} else if (bEnd == sCurr) {
					tdPrevOp = TagContentHTML::forTDOperation(breakdown, "red",
							HTML_SCALE, (foundInPath != path.end()));
				}
			}
			tr->addChild(tdPrevOp);
			tr->addChild(tdOperation);
			tr->addChild(tdNextOp);
		}
		table->addChild(tr);
		div->addChild(table);
	}
	string bodyWidth = "width: "
			+ toString(HTML_SCALE * cMax(machines).first + 150) + "px;";
	div->addParam("style", bodyWidth);
	htmlPage.add(PageHTML::Section::BODY, div);

	div = new TagContentHTML("div");
	TagContentHTML* table = new TagContentHTML("table");
	TagContentHTML* tr = new TagContentHTML("tr");

	for (uint i = 0; i < nJobs; ++i) {
		string color(getColor(i));
		uint cMax = operations[i].back().getCompletitionTime();
		TagContentHTML* td = TagContentHTML::forTDJobLegend(i, color, cMax);
		tr->addChild(td);
	}
	TagContentHTML* td = TagContentHTML::forTDJobLegend(-1, "red", -1,
			"Breakdown");
	tr->addChild(td);
	table->addChild(tr);
	div->addChild(table);
	div->addParam("style", bodyWidth);
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

const Oper& Gantt::getBreakdown() const throw (string) {
	if (breakdown.getId() + 1 == 0) {
		throw string("breakdown was not set!");
	}
	return breakdown;
}

void Gantt::setBreakdown(Oper breakdown) {
	this->breakdown = breakdown;
}

void Gantt::resetBuffers() {
	for (uint i = 0; i < nMachines; ++i) {
		for (uint j = 0; j < machines[i].size(); ++j) {
			machines[i][j].setBuffer(0);
		}
	}
	for (uint i = 0; i < nJobs; ++i) {
		for (uint j = 0; j < operations[i].size(); ++j) {
			operations[i][j].setBuffer(0);
		}
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

	Oper operation = gantt.getMachines()[machineIndex][opIndex];
	uint starting = operation.getStartingTime();
	if (opIndex > 0) {
		Oper prevOnMachine = gantt.getMachines()[machineIndex][opIndex - 1];
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
	} else if (opIndex == 0) {
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
//			cerr << message << endl;
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

Oper generateBreakdown(uint start, uint duration, uint machine) {
	Oper breakdown;
	breakdown.setId(-2);
	breakdown.setMachineNumber(machine);
	breakdown.setStartingTime(start);
	breakdown.setProcessingTime(duration, machine);

	return breakdown;
}
