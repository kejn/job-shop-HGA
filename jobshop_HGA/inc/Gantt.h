/*
 * Gantt.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include <string>
#include <vector>

#include "Oper.h"
#include "TabooTools.h"

#ifndef GANTT_H_
#define GANTT_H_

using uint = unsigned int;

/**
 * Gantt chart of nJobs on nMachines with altogether totNOper number of operations.
 */
class Gantt {
	uint nJobs;
	uint nMachines;
	uint totNOper;
	std::vector<std::vector<Oper> > operations;
	std::vector<std::vector<Oper> > machines;

	static std::vector<std::string> colors;

	std::string getRandomColor();
	std::string getColor(uint index);
	const uint HTML_SCALE;
public:
	Gantt(uint nJobs, uint nMachines, uint htmlScale = 1);

	void addOperation(uint i, const Oper &operation);
	Oper nextOperationTo(uint job, uint operation) const throw (std::string);
	Oper prevOperationTo(uint job, uint referenceOperation) const throw (std::string);

	void addOnMachine(uint i, const Oper &operation);
	void insertOnMachine(uint machineIndex,
			const std::vector<Oper>::iterator &iterator,
			Oper operation);
	void clearMachines();

	void printJobs();
	void printMachines();
	void printMachinesHTML(std::string fileName = "machines.html");

	std::vector<uint> randomJobOrder();

	std::vector<std::vector<Oper> >& getMachines();
	const std::vector<std::vector<Oper> >& getMachines() const;
	std::vector<std::vector<Oper> >& getOperations();
	const std::vector<std::vector<Oper> >& getOperations() const;

	uint getNJobs() const;
	uint getNMachines() const;
	uint getTotNOper() const;
};

std::vector<Oper> criticalPath(const Gantt & gantt,
		int machineIndex = -1, int opIndex = -1);

/**
 * First: cMax value, Second: machineIndex
 */
std::pair<uint, uint> cMax(const Gantt & gantt);

#endif /* GANTT_H_ */
