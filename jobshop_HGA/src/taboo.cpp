#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../inc/Gantt.h"
#include "../inc/Oper.h"
#include "../inc/TabooTools.h"
#include "../inc/util/CircularArray.h"
#include "../inc/util/files.h"
#include "../inc/util/stringUtil.h"

using namespace std;

using RevOperIt = std::reverse_iterator<std::vector<Oper>::iterator>;

Gantt loadOperationsFromTaillardFile(fstream &file);
Gantt loadOperationsFromKacemFile(fstream &file);

void initPopGen(Gantt &ganttInfo);

RevOperIt whereCanIFit(const Oper & operation, RevOperIt rIter,
		const RevOperIt rEnd, unsigned int t0);

const string BENCHMARK_FILE_PATH = RESOURCE_FOLDER + "bench_js.txt";
const string BENCHMARK_FILE_PATH2 = RESOURCE_FOLDER + "kacem2002a-PFJS.txt";

const uint HTML_SCALE = 20;

const uint N_INSTANCES = 1;

const uint TABOO_MAX = 7;
const uint BACKTRACK_MAX = 5;

void executionTimeMs(const clock_t &start, const clock_t &end) {
	cout << "Executed in " << (double(end - start)) / CLOCKS_PER_SEC
			<< " seconds" << endl;
}

void generateLatexTables(std::string iID[], uint totNOper[], uint cMaxMIN[],
		uint cMaxMAX[], uint fOnCP[], double tAVG[]) {
	std::ofstream out("tabularx.txt");
	for (uint i = 0; i < 8; ++i) {
		out << "% " << i << " start" << endl
				<< "\\begin{tabularx}{\\textwidth}{|X|c|c|c|c|c|c|c|c|c|c|}"
				<< endl;
		for (uint j = 0; j < 6; ++j) {
			out << "\\hline" << endl;
			for (uint k = 0; k < 11; ++k) {
				uint index = i * 10 + k - 1;
				switch (j) {
				case 0:
					if (k == 0) {
						out << "$\\mathbf{iID}$";
					} else {
						out << " & \\textbf{" << iID[index] << "}";
					}
					break;
				case 1:
					if (k == 0) {
						out << "$\\mathbf{totNOper}$";
					} else {
						out << " & " << totNOper[index];
					}
					break;
				case 2:
					if (k == 0) {
						out << "$\\mathbf{t_{AVG}}$";
					} else {
						out << fixed;
						out << " & " << setprecision(2) << tAVG[index];
					}
					break;
				case 3:
					if (k == 0) {
						out << "$\\mathbf{\\underline{C_{max}}}$";
					} else {
						out << " & " << cMaxMIN[index];
					}
					break;
				case 4:
					if (k == 0) {
						out << "$\\mathbf{\\overline{C_{max}}}$";
					} else {
						out << " & " << cMaxMAX[index];
					}
					break;
				case 5:
					if (k == 0) {
						out << "$\\mathbf{F_{onCP}}$";
					} else {
						out << fixed;
						out << " & " << setprecision(2) << double(fOnCP[index]) / 5;
					}
					break;
				}

			}
			out << "\\\\" << endl;
		}
		out << "\\hline" << endl << "\\end{tabularx}" << endl << "% " << i << "end\n" << endl;
	}
	out.close();
}

void extractData() {
	std::ofstream out("results.txt");
	std::string iID[80];
	uint totNOper[80];
	uint cMaxMIN[80];
	uint cMaxMAX[80];
	uint fOnCP[80];
	double tAVG[80];
	for (uint j = 0; j < 80; ++j) {
		cMaxMIN[j] = numeric_limits<uint>::max();
		cMaxMAX[j] = 0;
		fOnCP[j] = 0;
		tAVG[j] = 0.0;
	}
	for (uint i = 0; i < 5; ++i) {
		std::ifstream in("log" + stringUtil::toString(i) + ".txt");
		std::string line;
		uint index = 0;
		bool updateIndex = false;
		while (getline(in, line, '\n').good() && index < 80) {
			cout << "****\ninput: " << line << endl;
			if (line[0] == '*') {
				iID[index] = "ta" + line.substr(line.size()-2);
			} else if (line.substr(0, 10).compare("initPopGen") == 0) {
				cout << line.substr(25) << endl;
				std::istringstream ss(line.substr(25));
				ss >> totNOper[index];
			} else if (line.substr(0, 4).compare("CMAX") == 0) {
				std::istringstream ss(line.substr(5));
				uint tmp;
				ss >> tmp;
				cMaxMIN[index] = min(tmp, cMaxMIN[index]);
				cMaxMAX[index] = max(tmp, cMaxMAX[index]);
			} else if (line.compare("breakdown on path") == 0) {
				++fOnCP[index];
			} else if (line.substr(0, 8).compare("Executed") == 0) {
				std::istringstream ss(line.substr(12));
				double tmp;
				ss >> skipws >> tmp;
				tAVG[index] += tmp;
				updateIndex = true;
			}

			cout << "\ndata:\n" << iID[index] << '\n' << totNOper[index] << '\n'
					<< tAVG[index] << '\n' << cMaxMIN[index] << '\n'
					<< cMaxMAX[index] << '\n' << fOnCP[index] << endl;
			if (updateIndex) {
				++index;
				updateIndex = false;
			}
		}
		in.close();
	}
	for (uint j = 0; j < 80; ++j) {
		tAVG[j] /= 5;
		out << iID[j] << setw(6) << totNOper[j];
		out << fixed;
		out << setprecision(2) << setw(6) << tAVG[j];
		out << setw(6) << cMaxMIN[j] << setw(6) << cMaxMAX[j] << setw(6)
				<< double(fOnCP[j]) / 5 << endl;
	}
	out.close();
	cout << "finito" << endl;
	generateLatexTables(iID, totNOper, cMaxMIN, cMaxMAX, fOnCP, tAVG);
}

int main() throw (string) {
//	extractData();
	srand(time(nullptr));

//	for (uint asdf = 0; asdf < 5; ++asdf) {
//		std::ofstream out("log" + stringUtil::toString(asdf) + ".txt");
//		std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
//		std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

		fstream file;
		try {
			openFile(file, BENCHMARK_FILE_PATH2, ios::in);
		} catch (const string &message) {
			cerr << message << endl;
			if (file.is_open()) {
				file.close();
			}
			return 1;
		}
		clock_t startAll = clock();
		for (uint i = 0; i < N_INSTANCES; ++i) {
			cerr << i << endl;
			cout << "\n******* TAIL " << setfill('0') << setw(2) << i << endl
					<< endl;
			clock_t start = clock();
//			Gantt ganttInfo = loadOperationsFromTaillardFile(file); // [1]
		Gantt ganttInfo = loadOperationsFromKacemFile(file); // [1]

			Oper breakdown = generateBreakdown(50, 100, 6);
			ganttInfo.setBreakdown(breakdown);

			initPopGen(ganttInfo);

//		ganttInfo.printMachinesHTML(
//				"machines" + stringUtil::toString(i) + ".html");

			TabooTools taboo = TabooTools::create(ganttInfo, TABOO_MAX,
					BACKTRACK_MAX);
			taboo.tsabAlgorithm();
			ganttInfo = taboo.getBestGantt();

			cout << "CMAX:" << cMax(ganttInfo.getMachines()).first << endl;

//		ganttInfo.printMachines();
			ganttInfo.printMachinesHTML(
					"machines" + stringUtil::toString(i) + ".html");

			cout << endl;
			executionTimeMs(start, clock());
		}
		cout << "\n****************************\n\n" << "Finished" << endl;
		executionTimeMs(startAll, clock());
		file.close();
//	system("generated\\machines0.html");

//		std::cout.rdbuf(coutbuf);
//	}
	return 0;
}

Gantt loadOperationsFromTaillardFile(fstream &file) {
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

			ganttInfo.addOperation(jobIndex, operation);
		}
	}
	return ganttInfo;
}

Gantt loadOperationsFromKacemFile(fstream &file) {
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
//	{3, 6, 4, 9, 1, 11, 13, 14, 12, 0, 5, 7, 2, 8, 10};
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

			for (const auto &entry : pTimes) {					// [14]
				uint key = entry.first;

				vector<Oper> & machineK = ganttInfo.getMachines()[key];
				Oper operInfo = ganttInfo.getOperations()[jobIndex][oper];
				operInfo.setMachineNumber(key);

				t1 = calculateT1(machineK, machineK.end());	// [16-20]

				bool result = applyBreakdown(max(t0, t1), operInfo,
						ganttInfo.getBreakdown());

//				cout << "result:" << operInfo.toString() << endl;

				if (!result) {
					if (t1 <= t0) {							// [21-27]
						operInfo.setStartingTime(t0);
					} else {
						RevOperIt iter = whereCanIFit(operInfo,
								machineK.rbegin(), machineK.rend(), t0);
						if (iter != machineK.rend()) {
							operInfo.setStartingTime(
									(*iter).getCompletitionTime());
						} else {
							operInfo.setStartingTime(t1);
						}
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

//			cout << "Chosen " << minCK << ' ' << M.at(minCK).toString() << endl << endl;
			ganttInfo.insertOnMachine(minCK, iter, M.at(minCK));

			++operCount;										// [31]
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
