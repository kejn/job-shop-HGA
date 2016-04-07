/*
 * Gantt.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Gantt.h"

#include <iomanip>
#include <iostream>

using namespace std;

void Gantt::addOperation(int i, const Oper &operation) {
	operations[i].push_back(operation);
	++totNOper;
}

Oper Gantt::nextOperationTo(unsigned int job, unsigned int operation)
		throw (string) {
	if ((job < operations.size()) && (operation < operations[job].size() - 1)) {
		return (operations[job][operation + 1]);
	}
	throw string("next operation: operation not found.");
}

Oper Gantt::prevOperationTo(unsigned int job, unsigned int referenceOperation)
		throw (string) {
	if ((job < operations.size()) && (referenceOperation > 0)) {
		return (operations[job][referenceOperation - 1]);
	}
	throw string("prev operation: operation not found.");
}

void Gantt::addOnMachine(int i, const Oper &operation) {
	machines[i].push_back(operation);
}

void Gantt::printJobs() {
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

int Gantt::getNJobs() const {
	return (nJobs);
}

int Gantt::getNMachines() const {
	return (nMachines);
}


int Gantt::getTotNOper() const {
	return (totNOper);
}
