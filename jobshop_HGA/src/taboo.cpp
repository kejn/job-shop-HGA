#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../inc/Gantt.h"
#include "../inc/Oper.h"
#include "../inc/util/files.h"

using namespace std;

using RevOperIt = std::reverse_iterator<std::vector<Oper>::iterator>;

Gantt loadOperationsFromTaillardFile(fstream &file) throw (string);
Gantt loadOperationsFromKacemFile(fstream &file) throw (string);

void initPopGen(Gantt &ganttInfo);

unsigned int calculateT0(Gantt& ganttInfo, unsigned int jNum,
		unsigned int oNum);
unsigned int calculateT1(const vector<Oper> & machine);
RevOperIt whereCanIFit(const Oper & operation, RevOperIt rIter,
		const RevOperIt rEnd, unsigned int t0);

const string BENCHMARK_FILE_PATH = RESOURCE_FOLDER + "bench_js.txt";
const string BENCHMARK_FILE_PATH2 = RESOURCE_FOLDER + "kacem2002a-PFJS.txt";

const uint HTML_SCALE = 1;

const uint TABOO_MAX = 7;
const uint BACKTRACK_MAX = 5;

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
		return 1;
	}
//	Gantt ganttInfo = loadOperationsFromKacemFile(file); // [1]
	Gantt ganttInfo = loadOperationsFromTaillardFile(file); // [1]
	file.close();
	TabooTools taboo(TABOO_MAX, BACKTRACK_MAX);

	initPopGen(ganttInfo);

	vector<Oper> cPath = criticalPath(ganttInfo);
	taboo.setUpBlocks(cPath);
	taboo.printBlocks();

	ganttInfo.printMachinesHTML();

	system("generated\\machines.html");

	return 0;
}

Gantt loadOperationsFromTaillardFile(fstream &file) throw (string) {
	int numberOfJobs;
	int numberOfMachines;

	file.ignore(numeric_limits<streamsize>::max(), 't');
	file.ignore(numeric_limits<streamsize>::max(), '\n');

	if (file.bad()) {
		throw "Can't read from file anymore.";
	}

	file >> numberOfJobs >> numberOfMachines;
	Gantt ganttInfo(numberOfJobs, numberOfMachines, HTML_SCALE);

	for (int jobIndex = 0; jobIndex < numberOfJobs; ++jobIndex) {
		for (int machineIndex = 0; machineIndex < numberOfMachines;
				++machineIndex) {
			Oper operation;
			int machineNumber, processingTime;

			file >> machineNumber >> processingTime;

			operation.setMachineNumber(machineNumber - 1);
			operation.setProcessingTime(processingTime, machineNumber - 1);
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
	return ganttInfo;
}

Gantt loadOperationsFromKacemFile(fstream &file) throw (string) {
	int numberOfJobs;
	int numberOfMachines;

	file.ignore(numeric_limits<streamsize>::max(), 'k');
	file.ignore(numeric_limits<streamsize>::max(), '\n');

	if (file.bad()) {
		throw "Can't read from file anymore.";
	}

	file >> numberOfJobs >> numberOfMachines;
	Gantt ganttInfo(numberOfJobs, numberOfMachines, HTML_SCALE);

	cout << "numberOfJobs: " << numberOfJobs << ", numberOfMachines: "
			<< numberOfMachines << endl;
	for (int jobIndex = 0; jobIndex < numberOfJobs; ++jobIndex) {
		int jobNumber;
		int numberOfOperations;
		file >> jobNumber >> numberOfOperations;
		for (int operationIndex = 0; operationIndex < numberOfOperations;
				++operationIndex) {
			Oper operation;
			for (int machineIndex = 0; machineIndex < numberOfMachines;
					++machineIndex) {
				uint processingTime;
				string next;

				file >> skipws >> next;
				if (next == "X") {
					continue;
				}
				processingTime = (uint) atoi(next.c_str());

				operation.setMachineNumber(machineIndex);
				operation.setProcessingTime(processingTime, machineIndex);
				operation.setPid(jobNumber - 1);
				operation.setId(operationIndex);
			}
			ganttInfo.addOperation(jobIndex, operation);
		}
	}
	return ganttInfo;
}

void initPopGen(Gantt &ganttInfo) {
	cout << "initPopGen(), tot_noper: " << ganttInfo.getTotNOper() << endl;

	// [2] losowanie kolejnosci zadan
	vector<uint> sequence = ganttInfo.randomJobOrder();

	cout << "initial job order: ";
	copy(sequence.begin(), sequence.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	uint operCount = 0, oper = 0;								// [3]
	while (operCount < ganttInfo.getTotNOper()) { 				// [4]
		for (uint jobIndex : sequence) {						// [5]
			if (oper >= ganttInfo.getOperations()[jobIndex].size()) {
				continue;
			}
			const map<uint, uint> & pTimes =
					ganttInfo.getOperations()[jobIndex][oper].getProcessingTimes();
			map<uint, Oper> M;					// [6]
			map<uint, uint> T;					// [7]
			uint t0, t1;										// [8]

			t0 = calculateT0(ganttInfo, jobIndex, oper);

			for (const auto &entry : pTimes) {							// [14]
				uint key = entry.first;

				vector<Oper> & machineK = ganttInfo.getMachines()[key];
				Oper & operInfo = ganttInfo.getOperations()[jobIndex][oper];
				operInfo.setMachineNumber(key);
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

				T.insert(make_pair(key, operInfo.getCompletitionTime()));
				M.insert(make_pair(key, operInfo));
			} // end for

			// [29]
			pair<uint, uint> minC = *min_element(T.begin(), T.end(),
					[](pair<uint,uint> l, pair<uint,uint> r) {
						return l.second < r.second;
					});
			uint minCK = minC.first;
			vector<Oper>::iterator iter =
					ganttInfo.getMachines()[minCK].begin();
			for (; iter != ganttInfo.getMachines()[minCK].end(); ++iter) {
				if (M.at(minCK).getStartingTime()
						< iter->getCompletitionTime()) {
					break;
				}
			}

			ganttInfo.insertOnMachine(minCK, iter, M.at(minCK));

			++operCount;												// [31]
		} // end for (unsigned int i : sequence)
		++oper;
	}
	return;
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

	return canFit.empty() ? rIter : canFit.back();
}

/*
 * t0 calculation rule (hGA).
 */
inline uint calculateT0(Gantt& ganttInfo, uint jNum, uint oNum) {
	uint t0;
	try {
		Oper prevOper = ganttInfo.prevOperationTo(jNum, oNum);
		t0 = prevOper.getCompletitionTime();
	} catch (const string &message) {
		t0 = 0;
	}
	return t0;
}

inline uint calculateT1(const vector<Oper>& machine) {
	uint t1;
	if (machine.empty()) {
		t1 = 0;
	} else {
		t1 = machine.back().getCompletitionTime();
	}
	return t1;
}
