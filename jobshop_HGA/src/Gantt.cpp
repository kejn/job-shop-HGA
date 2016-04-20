/*
 * Gantt.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Gantt.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;

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

std::vector<Gene>& Gantt::getChromosome() {
	return (chromosome);
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

void Gantt::printChromosome() {
	for (const Gene &gene : chromosome) {
		gene.print();
		cout << "-";
	}
	cout << endl;
}

string Gantt::getRandomColor() {
    char letters[] = "0123456789ABCDEF";
    string color = "#";
    for (int i = 0; i < 6; ++i) {
        color += letters[rand() % 16];
    }
    return (color);
}

void Gantt::printJobsHTML(std::string fileName) {
	ofstream outputFile(fileName.c_str());
	if (!outputFile.good()) {
		throw string("Cannot open file: " + fileName);
	}
	outputFile << "<!doctype html>\n"
			<< "<html>\n"
			<< "<head>\n"
			<< "<meta "
			<< 		"http-equiv=\"Content-Type\""
			<<		"content=\"text/html; charset=UTF-8\"/>\n"
			<< "<title>Jobs</title>"
			<< "<style>\n"
			<< 	"body,div,table,tr,td {\n"
			<< 		"margin: 0;\n"
			<<		"padding: 0;\n"
			<< 	"}\n"
			<<	"div {\n"
			<<		"width: 2000px;"
			<<	"}\n"
			<< 	"table {\n"
			<<		"border-spacing: 0px;\n"
			<<		"padding: 2px;"
			<< 	"}\n"
			<< 	"td {\n"
			<<		"border-right: 1px black solid;\n"
			<< 	"}\n"
			<< "</style>\n"
			<< "</head>\n"
			<< "<body>\n";

	for (unsigned int i = 0; i < operations.size(); ++i) {
		outputFile << "<div><table>\n<tr>\n<td style=\"width: 70px;\">Job "
				<< i << "</td>\n";
		for (unsigned int j = 0; j < operations[i].size(); ++j) {
			outputFile << "<td style=\"width: "
					<< operations[i][j].getProcessingTime()
					<< "px; background-color: "
					<< getColor(i)
					<< ";\"></td>\n";
		}
		outputFile << "</tr>\n</table></div>";
	}
	outputFile << "</body>\n</html>";
}

string Gantt::getColor(unsigned int index) {
	if(index >= Gantt::colors.size()) {
		Gantt::colors.resize(index);
		Gantt::colors.push_back(getRandomColor());
	}
	return (Gantt::colors[index]);
}

void Gantt::printMachinesHTML(std::string fileName) {
	ofstream outputFile(fileName.c_str());
	if (!outputFile.good()) {
		throw string("Cannot open file: " + fileName);
	}
	outputFile << "<!doctype html>\n"
			<< "<html>\n"
			<< "<head>\n"
			<< "<meta "
			<< 		"http-equiv=\"Content-Type\""
			<<		"content=\"text/html; charset=UTF-8\"/>\n"
			<< "<title>Machines</title>"
			<< "<style>\n"
			<< 	"body,div,table,tr,td {\n"
			<< 		"margin: 0;\n"
			<<		"padding: 0;\n"
			<< 	"}\n"
			<<	"div {\n"
			<<		"width: 2000px;"
			<<	"}\n"
			<< 	"table {\n"
			<<		"border-spacing: 0px;\n"
			<<		"padding: 2px;"
			<< 	"}\n"
			<< 	"td {\n"
			<<		"border-right: 1px black solid;\n"
			<< 	"}\n"
			<< "</style>\n"
			<< "</head>\n"
			<< "<body>\n";

	for (unsigned int i = 0; i < machines.size(); ++i) {
		outputFile << "<div><table>\n<tr>\n<td style=\"width: 100px;\">Machine "
				<< i << "</td>\n";
		for (unsigned int j = 0; j < machines[i].size(); ++j) {
			unsigned int sCurr = machines[i][j].getStartingTime();
			if(j > 0) {
				unsigned int cPrev = machines[i][j-1].getCompletitionTime();
				if(cPrev < sCurr) {
					outputFile << "<td style=\"width: " << sCurr - cPrev
							<< "px;\"></td>";
				}
			} else if(sCurr > 0){
				outputFile << "<td style=\"width: " << sCurr << "px;\"></td>";
			}
			outputFile << "<td style=\"width: "
					<< machines[i][j].getProcessingTime()
					<< "px; background-color: "
					<< colors[machines[i][j].getPid()]
					<< ";\" title=\"" << machines[i][j].toString()
					<< "\"></td>\n";
		}
		outputFile << "</tr>\n</table></div>";
	}
	outputFile << "</body>\n</html>";
}

unsigned int Gantt::getTotNOper() const {
	return (totNOper);
}

vector<unsigned int> Gantt::randomJobOrder() {
	vector<unsigned int> sequence(nJobs);
	int n = 0;
	generate(sequence.begin(), sequence.end(), [&n] {return (n++);});
	srand(time(nullptr));
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
	Gantt::colors  = {
			"BlueViolet",
			"Crimson",
			"DarkGreen",
			"Tomato",
			"DarkOrange",
			"DodgerBlue",
			"Gold",
			"Indigo",
			"LightSalmon",
			"Lime",
			"MediumVioletRed",
			"DeepSkyBlue"
	};
}
