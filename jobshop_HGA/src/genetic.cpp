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

using namespace std;

void openFile(fstream &file, const string &fileName) throw (string);
Gantt loadOperationsFromFile(fstream &file) throw (string);
void initPopGen(Gantt &ganttInfo);

static const string BENCHMARK_FILE_PATH = "res/bench_js.txt";

int main() throw (string) {
	fstream file;
	try {
		openFile(file, BENCHMARK_FILE_PATH.c_str());
	} catch (const string &message) {
		cerr << message << endl;
		if (file.is_open()) {
			file.close();
		}
		return (1);
	}
	Gantt ganttInfo = loadOperationsFromFile(file);
	ganttInfo.printJobs();
	initPopGen(ganttInfo);

	file.close();
	return (0);
}

void openFile(fstream &file, const string &fileName) throw (string) {
	file.open(fileName.c_str(), fstream::in | fstream::out);
	if (file.bad()) {
		string message = "Nie udalo sie otworzyc pliku \"" + fileName + "\"";
		throw message;
	}
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

	for (int i = 0; i < numberOfJobs; ++i) {
		for (int j = 0; j < numberOfMachines; ++j) {
			Oper operation;
			int machineNumber, processingTime;
			file >> machineNumber >> processingTime;
			operation.setMachineNumber(machineNumber);
			operation.setProcessingTime(processingTime);
			operation.setPid(i);
			operation.setId(j);
			if (j > 0) {
				operation.setStartingTime(
						ganttInfo.prevOperationTo(i, j).getCompletitionTime());
			}
			ganttInfo.addOperation(i, operation);
		}
	}
	return (ganttInfo);
}

void initPopGen(Gantt &ganttInfo) {
	cout << "initPopGen(), tot_noper: " << ganttInfo.getTotNOper() << endl;
	// [2] losowanie kolejnosci zadan
	vector<int> sequence(ganttInfo.getNJobs());
	int n = 0;
	generate(sequence.begin(), sequence.end(), [&n] {return (n++);});
	srand(time(nullptr));
	random_shuffle(sequence.begin(), sequence.end());
	cout << "initial job order: ";
	copy(sequence.begin(), sequence.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	// [3]
	int operCount = 0, oper = 0;
	while (operCount < ganttInfo.getTotNOper()) { 	// [4]
		for (int i : sequence) {				// [5]
			bool prevFound = false;
			Oper prevOper;
			try {
				prevOper = ganttInfo.prevOperationTo(i, oper);
				prevFound = true;
			} catch (...) {
			}
			vector<Oper> M(ganttInfo.getNMachines());				// [6]
			// [7] == operInfo.completitionTime
			int t0, t1;							// [8]
			if (!prevFound) {		// [9]
				t0 = 0;
			} else {							// [10]
				t0 = prevOper.getCompletitionTime();
			}									// [11]
			for (unsigned int k = 0; k < M.size(); ++k) { // [12]
				vector<Oper> &machine = ganttInfo.getMachines()[k];
				Oper operInfo2(ganttInfo.getOperations()[i][oper]);
				operInfo2.setMachineNumber(k);
				// [13] == operInfo.p
				if (machine.empty()) {			// [14]
					t1 = 0;
				} else {						// [15]
					t1 = machine.back().getCompletitionTime();
				}								// [16]
				if (t1 <= t0) {					// [17]
					operInfo2.setStartingTime(t0);
				} else if ((machine.size() > 1)
						&& (operInfo2.getProcessingTime()
								<= (machine.back().getStartingTime()
										- machine[machine.size() - 2].getCompletitionTime()))) { // [18]
					if (machine.size() > 1) {
						operInfo2.setStartingTime(
								machine[machine.size() - 2].getCompletitionTime());
					}
				} else {						// [19]
					operInfo2.setStartingTime(t1);
				}								// [20]
				M[k] = operInfo2; 				// [17-20]
			}									// [21]
			// [22]
			vector<Oper>::iterator minC =
					min_element(M.begin(), M.end(),
							[](Oper a1, Oper a2) {return (a1.getCompletitionTime() < a2.getCompletitionTime());});
			int minCK = distance(M.begin(), minC);

			vector<Oper>::iterator iter =
					ganttInfo.getMachines()[minCK].begin();
			for (; iter != ganttInfo.getMachines()[minCK].end(); ++iter) {
				if (minC->getStartingTime() < iter->getCompletitionTime()) {
					break;
				}
			}
			ganttInfo.getMachines()[minCK].insert(iter, *minC);
			ganttInfo.getOperations()[i][oper] = *minC;
			// [23]
			Gene gene(minCK, i, oper);
			ganttInfo.getChromosome().push_back(gene);
			++operCount;
		}
		++oper;
	}
	ganttInfo.printMachines();
}
