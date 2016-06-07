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
using Permutation = std::vector<std::vector<Oper> >;

/**
 * Gantt chart of nJobs on nMachines with altogether totNOper number of operations.
 */
class Gantt {
	uint nJobs;
	uint totNOper;
	Permutation operations;

	uint nMachines;
	Permutation machines;

	Oper breakdown;

	static std::vector<std::string> colors;

	std::string getRandomColor();
	std::string getColor(uint index);
	const uint HTML_SCALE;
public:
	Gantt(uint htmlScale = 1) : HTML_SCALE(htmlScale){
		nJobs = 0; totNOper = 0; nMachines = 0;
	}
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

	Permutation& getMachines();
	const Permutation& getMachines() const;
	Permutation& getOperations();
	const Permutation& getOperations() const;

	const Oper & getBreakdown() const throw(std::string);
	void setBreakdown(Oper breakdown);

	uint getNJobs() const;
	uint getNMachines() const;
	uint getTotNOper() const;

	Gantt& operator=(const Gantt& other) {
		nJobs = other.nJobs;
		totNOper = other.totNOper;
		operations = other.operations;

		nMachines = other.nMachines;
		machines = other.machines;

		breakdown = other.breakdown;

		return *this;
	}
};

std::vector<Oper> criticalPath(const Gantt & gantt,
		int machineIndex = -1, int opIndex = -1);

/**
 * Calculates cMax of a feasible Permutation.
 * First: cMax value, Second: machineIndex
 */
std::pair<uint, uint> cMax(const Permutation &permutation);

Oper generateBreakdown(uint start, uint duration, uint machine);

#endif /* GANTT_H_ */
