#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

#include "../inc/Gantt.h"
#include "../inc/Gene.h"
#include "../inc/Oper.h"
#include "../inc/util/files.h"

using namespace std;
using RevOperIt = std::reverse_iterator<std::vector<Oper>::iterator>;

Gantt loadOperationsFromFile(fstream &file) throw (string);

vector<Gene> initPopGen(Gantt &ganttInfo);
void decode(const vector<Gene> &chromosome, Gantt &ganttInfo);

unsigned int calculateT0(Gantt& ganttInfo, unsigned int jNum,
		unsigned int oNum);
unsigned int calculateT1(const vector<Oper> & machine);
unsigned int compareT0withT1(unsigned int t0, unsigned int t1);
RevOperIt whereCanIFit(const Oper & operation, RevOperIt rIter,
		const RevOperIt rEnd, unsigned int t0);
void printChromosome(const vector<Gene> &chromosome);

static const string BENCHMARK_FILE_PATH = RESOURCE_FOLDER + "bench_js.txt";

int main() throw (string) {
	srand(time(nullptr));

	fstream file;
	try {
		openFile(file, BENCHMARK_FILE_PATH, ios::in);
	} catch (const string &message) {
		cerr << message << endl;
		if (file.is_open()) {
			file.close();
		}
		return (1);
	}
	Gantt ganttInfo = loadOperationsFromFile(file); // [1]
	file.close();

	ganttInfo.getMachines() = ganttInfo.getOperations();
	ganttInfo.printMachinesHTML("jobs.html");

	ganttInfo.clearMachines();

	vector<Gene> chromosome = initPopGen(ganttInfo);
	ganttInfo.printMachinesHTML();

	system("generated\\machines.html");

	return (0);
}

Gantt loadOperationsFromFile(fstream &file) throw (string) {
	int numberOfJobs;
	int numberOfMachines;

	file.ignore(numeric_limits<streamsize>::max(), 't');
	file.ignore(numeric_limits<streamsize>::max(), '\n');

	if (file.bad()) {
		throw "Can't read from file anymore.";
	}

	file >> numberOfJobs >> numberOfMachines;
	Gantt ganttInfo(numberOfJobs, numberOfMachines);

	for (int jobIndex = 0; jobIndex < numberOfJobs; ++jobIndex) {
		for (int machineIndex = 0; machineIndex < numberOfMachines;
				++machineIndex) {
			Oper operation;
			int machineNumber, processingTime;

			file >> machineNumber >> processingTime;

			operation.setMachineNumber(machineNumber);
			operation.setProcessingTime(processingTime);
			operation.setPid(jobIndex);
			operation.setId(machineIndex);

			if (!operation.isFirstInJob()) {
				int startingTime = ganttInfo.prevOperationTo(jobIndex,
						machineIndex).getCompletitionTime();
				operation.setStartingTime(startingTime);
			}

			ganttInfo.addOperation(jobIndex, operation);
		}
	}
	return (ganttInfo);
}

vector<Gene> initPopGen(Gantt &ganttInfo) {
	cout << "initPopGen(), tot_noper: " << ganttInfo.getTotNOper() << endl;

	// [2] losowanie kolejnosci zadan
	vector<unsigned int> sequence = ganttInfo.randomJobOrder();

	cout << "initial job order: ";
	copy(sequence.begin(), sequence.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	vector<Gene> chromosome(ganttInfo.getTotNOper());

	unsigned int operCount = 0, oper = 0;								// [3]
	while (operCount < ganttInfo.getTotNOper()) { 						// [4]
		for (unsigned int jobIndex : sequence) {						// [5]
			vector<Oper> M(ganttInfo.getNMachines());					// [6]
			vector<unsigned int> T(ganttInfo.getNMachines());			// [7]
			unsigned int t0, t1;										// [8]

			t0 = calculateT0(ganttInfo, jobIndex, oper);

			for (unsigned int k = 0; k < ganttInfo.getNMachines(); ++k) {// [14]
				vector<Oper> & machineK = ganttInfo.getMachines()[k];
				Oper operInfo(ganttInfo.getOperations()[jobIndex][oper]);
				operInfo.setMachineNumber(k);
				// [15] Oper::getProcessingTime()

				t1 = calculateT1(machineK);							 // [16-20]

				if (t1 <= t0) {										 // [21-27]
					operInfo.setStartingTime(t0);
				} else {
					RevOperIt iter = whereCanIFit(operInfo, machineK.rbegin(),
							machineK.rend(), t0);
					if (iter != machineK.rend()) {
						operInfo.setStartingTime((*iter).getCompletitionTime());
					} else {
						operInfo.setStartingTime(t1);
					}
				}

				T[k] = operInfo.getCompletitionTime();
				M[k] = operInfo;
			} // end for

			// [29]
			vector<unsigned int>::iterator minC = min_element(T.begin(),
					T.end());
			unsigned int minCK = distance(T.begin(), minC);
			vector<Oper>::iterator iter =
					ganttInfo.getMachines()[minCK].begin();
			for (; iter != ganttInfo.getMachines()[minCK].end(); ++iter) {
				if (M[minCK].getStartingTime() < iter->getCompletitionTime()) {
					break;
				}
			}

			ganttInfo.insertOnMachine(minCK, iter, M[minCK]);

			chromosome.push_back(Gene(minCK, jobIndex, oper));			// [30]
			++operCount;												// [31]
		} // end for (unsigned int i : sequence)
		++oper;
	}
	return (chromosome);
}

void decode(const vector<Gene> &chromosome, Gantt& ganttInfo) {
	ganttInfo.clearMachines();
	for (const Gene & gene : chromosome) {
		unsigned int jNum = gene.getJobNumber();
		unsigned int oNum = gene.getOperationNumber();
		unsigned int mNum = gene.getMachineNumber();

		vector<Oper> & machine = ganttInfo.getMachines()[mNum];

		Oper operInfo = ganttInfo.getOperations()[jNum][oNum];
		operInfo.setMachineNumber(mNum);

		unsigned int t0, t1;

		t0 = calculateT0(ganttInfo, jNum, oNum);
		t1 = calculateT1(machine);

		if (t1 <= t0) {
			operInfo.setStartingTime(t0);
		} else {
			RevOperIt iter = whereCanIFit(operInfo, machine.rbegin(),
					machine.rend(), t0);
			if (iter != machine.rend()) {
				operInfo.setStartingTime((*iter).getCompletitionTime());
			} else {
				operInfo.setStartingTime(t1);
			}
		}
		machine.push_back(operInfo);
	}
}

/**
 * Starting from the last operation check if operation, with minimum starting
 * time = t0, can fit between two consecutive operations on machineK.
 *
 * Parameters:
 * operation - operation to fit
 * machineK - machine on which we check
 * t0 - completition time of operation's technological predecessor
 *
 * Return:
 * iterator to the operation after which the operation can fit
 * (as early as possible after t0).
 */
RevOperIt whereCanIFit(const Oper & operation, RevOperIt rIter,
		const RevOperIt rEnd, unsigned int t0) {

	unsigned int p = operation.getProcessingTime();
	vector<RevOperIt> canFit;

	for (++rIter; rIter != rEnd; ++rIter) {
		unsigned int currentOperEnds = (*(rIter)).getCompletitionTime();
		unsigned int nextOperStarts = (*(rIter - 1)).getStartingTime();
		if (currentOperEnds < t0) {
			rIter = rEnd;
			break;
		} else if (p <= (nextOperStarts - currentOperEnds)) {
			canFit.push_back(rIter);
		}
	}

	return (canFit.empty() ? rIter : canFit.back());
}

/*
 * t0 calculation rule (hGA).
 */
inline unsigned int calculateT0(Gantt& ganttInfo, unsigned int jNum,
		unsigned int oNum) {
	unsigned int t0;
	try {
		Oper prevOper = ganttInfo.prevOperationTo(jNum, oNum);
		t0 = prevOper.getCompletitionTime();
	} catch (const string &message) {
		t0 = 0;
	}
	return (t0);
}

inline unsigned int calculateT1(const vector<Oper>& machine) {
	unsigned int t1;
	if (machine.empty()) {
		t1 = 0;
	} else {
		t1 = machine.back().getCompletitionTime();
	}
	return (t1);
}

void printChromosome(const vector<Gene> &chromosome) {
	for (const Gene &gene : chromosome) {
		gene.print();
		cout << "-";
	}
	cout << endl;
}

