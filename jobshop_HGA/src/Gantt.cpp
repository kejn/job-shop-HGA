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
#include <iomanip>
#include <iostream>

using namespace std;

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
	for(const Gene &gene : chromosome) {
		gene.print();
		cout << "-";
	}
	cout << endl;
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

