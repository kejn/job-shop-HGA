#include <algorithm>
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
	Gantt ganttInfo = loadOperationsFromFile(file); // [1]
	ganttInfo.printJobs();
	initPopGen(ganttInfo);

	file.close();
//	system("schedule.jpg");
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

void initPopGen(Gantt &ganttInfo) {
	cout << "initPopGen(), tot_noper: " << ganttInfo.getTotNOper() << endl;

	// [2] losowanie kolejnosci zadan
	vector<unsigned int> sequence = ganttInfo.randomJobOrder();

	cout << "initial job order: ";
	copy(sequence.begin(), sequence.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	unsigned int operCount = 0, oper = 0;								// [3]
	while (operCount < ganttInfo.getTotNOper()) { 						// [4]
		for (unsigned int i : sequence) {								// [5]
			vector<Oper> M(ganttInfo.getNMachines());					// [6]
			vector<unsigned int> T(ganttInfo.getNMachines());			// [7]
			int t0, t1;													// [8]

			try {														//[9-13]
				Oper prevOper = ganttInfo.prevOperationTo(i, oper);
				t0 = prevOper.getCompletitionTime();
			} catch (const string &message) {
				cerr << message << "(job: " << i << ", oper: " << oper << ")\n";
				t0 = 0;
			}

			for (unsigned int k = 0; k < ganttInfo.getNMachines(); ++k) {// [14]
				vector<Oper> & machineK = ganttInfo.getMachines()[k];
				Oper operInfo(ganttInfo.getOperations()[i][oper]);
				operInfo.setMachineNumber(k);
				// [15] Oper::getProcessingTime()

				if (machineK.empty()) {								 // [16-20]
					t1 = 0;
				} else {
					t1 = machineK.back().getCompletitionTime();
				}

				if (t1 <= t0) {										// [21-27]
					operInfo.setStartingTime(t0);
				} else if ((machineK.size() > 1)
						&& (operInfo.getProcessingTime()
								<= (machineK.back().getStartingTime()
										- machineK[machineK.size() - 2].getCompletitionTime()))) {
					if (machineK.size() > 1) {
						operInfo.setStartingTime(
								machineK[machineK.size() - 2].getCompletitionTime());
					}
				} else {
					operInfo.setStartingTime(t1);
				}
				T[k] = operInfo.getCompletitionTime();
				M[k] = operInfo;
			}															// [28]
			// [29]
			vector<unsigned int>::iterator minC = min_element(T.begin(), T.end());
			unsigned int minCK = distance(T.begin(), minC);
			vector<Oper>::iterator iter =
					ganttInfo.getMachines()[minCK].begin();
			for (; iter != ganttInfo.getMachines()[minCK].end(); ++iter) {
				if (M[minCK].getStartingTime() < iter->getCompletitionTime()) {
					break;
				}
			}
			ganttInfo.getMachines()[minCK].insert(iter, M[minCK]);
			ganttInfo.getOperations()[i][oper] = M[minCK]; // update!!!
			// [23]
			Gene gene(minCK, i, oper);
			ganttInfo.getChromosome().push_back(gene);
			++operCount;
		}
		++oper;
	}
	ganttInfo.printMachines();
}
