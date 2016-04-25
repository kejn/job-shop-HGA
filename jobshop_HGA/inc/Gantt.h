/*
 * Gantt.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include <string>
#include <vector>

#include "files/files.h"
#include "Gene.h"
#include "Oper.h"

#ifndef GANTT_H_
#define GANTT_H_

/**
 * Gantt chart of nJobs on nMachines with altogether totNOper number of operations.
 */
class Gantt {
	unsigned int nJobs;
	unsigned int nMachines;
	unsigned int totNOper;
	std::vector<std::vector<Oper> > operations;
	std::vector<std::vector<Oper> > machines;
	static std::vector<std::string> colors;

	std::vector<Gene> chromosome;

	std::string getRandomColor();
	std::string getColor(unsigned int index);
public:
	Gantt(unsigned int nJobs, unsigned int nMachines);

	void addOperation(int i, const Oper &operation);

	Oper nextOperationTo(unsigned int job, unsigned int operation)
			throw (std::string);
	Oper prevOperationTo(unsigned int job, unsigned int referenceOperation)
			throw (std::string);

	void addOnMachine(int i, const Oper &operation);
	void insertOnMachine(int machineIndex,
			const std::vector<Oper>::iterator &iterator,
			const Oper & operation);

	void printJobs();
	void printMachines();
	void printChromosome();
	void printJobsHTML(std::string fileName = GENERATED_FOLDER + "jobs.html");
	void printMachinesHTML(
			std::string fileName = GENERATED_FOLDER + "machines.html");

	std::vector<unsigned int> randomJobOrder();

	std::vector<Gene>& getChromosome();
	std::vector<std::vector<Oper> >& getMachines();
	std::vector<std::vector<Oper> >& getOperations();

	unsigned int getNJobs() const;
	unsigned int getNMachines() const;
	unsigned int getTotNOper() const;
};

#endif /* GANTT_H_ */
