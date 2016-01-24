#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Gene {
	int k; // machine no
	int i; // job no
	int j; // operation no
	Gene() {
		k = i = j = -1;
	}
	Gene(int k, int i, int j) {
		this->k = k;
		this->i = i;
		this->j = j;
	}
	Gene &operator=(const Gene & other) {
		k = other.k;
		i = other.i;
		j = other.j;
		return (*this);
	}
};

struct Oper {
	int s;
	int p;
	int c() {
		return s + p;
	}
	int m;
	int id;
	int pid;
	Oper() {
		s = p = 0;
		m = pid = id = -1;
	}
	Oper(const Oper &other) {
		*this = other;
	}
	const Oper &operator=(const Oper &other);
};

const Oper &Oper::operator=(const Oper &other) {
	this->s = other.s;
	this->p = other.p;
	this->m = other.m;
	this->id = other.id;
	this->pid = other.pid;
	return (*this);
}

struct Gantt {
	int nJobs;
	int nMachines;
	int totNOper;
	vector<vector<Oper> > operations;
	vector<vector<Oper> > machines;

	vector<Gene> chromosome;

	Gantt(int nJobs, int nMachines) {
		this->nJobs = nJobs;
		this->nMachines = nMachines;
		totNOper = 0;
		operations = vector<vector<Oper>>(nJobs);
		machines = vector<vector<Oper>>(nMachines);
	}

	void addOperation(int i, Oper operation);

	Oper nextOperationTo(unsigned int job, unsigned int operation)
			throw (string);
	Oper prevOperationTo(unsigned int job, unsigned int referenceOperation)
			throw (string);

	void addOnMachine(int i, Oper operation);

	void printJobs();
	void printMachines();
};

void Gantt::addOperation(int i, Oper operation) {
	operations[i].push_back(operation);
	++totNOper;
}

Oper Gantt::nextOperationTo(unsigned int job, unsigned int operation)
		throw (string) {
	if ((job < operations.size()) && (operation < operations[job].size() - 1)) {
		return operations[job][operation + 1];
	}
	throw string("next operation: operation not found.");
}

Oper Gantt::prevOperationTo(unsigned int job, unsigned int referenceOperation)
		throw (string) {
	if ((job < operations.size()) && (referenceOperation > 0)) {
		return operations[job][referenceOperation - 1];
	}
	throw string("prev operation: operation not found.");
}

void Gantt::addOnMachine(int i, Oper operation) {
	machines[i].push_back(operation);
}

void Gantt::printJobs() {
	for (unsigned int i = 0; i < operations.size(); ++i) {
		cout << "job" << i << '\t';
		for (unsigned int j = 0; j < operations[i].size(); ++j) {
			cout << setw(4) << 'M' << setw(2) << setfill('0')
					<< operations[i][j].m << setw(3) << setfill(' ')
					<< operations[i][j].p << ' ' << flush;
		}
		cout << endl;
	}
}

void Gantt::printMachines() {
	for (unsigned int i = 0; i < machines.size(); ++i) {
		cout << "Machine" << i << '\t';
		for (unsigned int j = 0; j < machines[i].size(); ++j) {
			cout << setw(4) << 'J' << setw(2) << setfill('0')
					<< machines[i][j].pid << setfill(' ') << '('
					<< machines[i][j].id << ')' << flush;
		}
		cout << endl;
	}
}

void openFile(fstream &file, const string &fileName) throw (string);
Gantt loadOperationsFromFile(fstream &file) throw (string);
void initPopGen(Gantt &ganttInfo);

int main() throw (string) {
	fstream file;
	try {
		openFile(file, "bench_js.txt");
	} catch (const string &message) {
		cerr << message << endl;
		if (file.is_open()) {
			file.close();
		}
		return 1;
	}
	Gantt ganttInfo = loadOperationsFromFile(file);
	ganttInfo.printJobs();
	initPopGen(ganttInfo);

	file.close();
	return 0;
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
			file >> operation.m >> operation.p;
			operation.pid = i;
			operation.id = j;
			if (j > 0) {
				operation.s = ganttInfo.prevOperationTo(i, j).c();
			}
			ganttInfo.addOperation(i, operation);
		}
	}
	return ganttInfo;
}

void initPopGen(Gantt &ganttInfo) {
	cout << "initPopGen(), tot_noper: " << ganttInfo.totNOper << endl;
	// [2] losowanie kolejnosci zadan
	vector<int> sequence(ganttInfo.nJobs);
	int n = 0;
	generate(sequence.begin(), sequence.end(), [&n] {return n++;});
	srand(time(nullptr));
	random_shuffle(sequence.begin(), sequence.end());
	cout << "initial job order: ";
	copy(sequence.begin(), sequence.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	// [3]
	int operCount = 0, oper = 0;
	while (operCount < ganttInfo.totNOper) { 	// [4]
		for (int i : sequence) {				// [5]
			bool prevFound = false;
			Oper prevOper;
			try {
				prevOper = ganttInfo.prevOperationTo(i, oper);
				prevFound = true;
			} catch (...) {
			}
			vector<Oper> M(ganttInfo.nMachines);				// [6]
			// [7] == operInfo.c
			int t0, t1;							// [8]
			if (!prevFound) {		// [9]
				t0 = 0;
			} else {							// [10]
				t0 = prevOper.c();
			}									// [11]
			for (unsigned int k = 0; k < M.size(); ++k) { // [12]
				vector<Oper> &machine = ganttInfo.machines[k];
				Oper operInfo2(ganttInfo.operations[i][oper]);
				operInfo2.m = k;
				// [13] == operInfo.p
				if (machine.empty()) {			// [14]
					t1 = 0;
				} else {						// [15]
					t1 = machine.back().c();
				}								// [16]
				if (t1 <= t0) {					// [17]
					operInfo2.s = t0;
				} else if ((machine.size() > 1)
						&& (operInfo2.p
								<= (machine.back().s
										- machine[machine.size() - 2].c()))) { // [18]
					if (machine.size() > 1) {
						operInfo2.s = machine[machine.size() - 2].c();
					}
				} else {						// [19]
					operInfo2.s = t1;
				}								// [20]
				M[k] = operInfo2; 				// [17-20]
			}									// [21]
			// [22]
			vector<Oper>::iterator minC = min_element(M.begin(), M.end(),
					[](Oper a1, Oper a2) {return a1.c() < a2.c();});
			int minCK = distance(M.begin(), minC);

			vector<Oper>::iterator iter = ganttInfo.machines[minCK].begin();
			for (; iter != ganttInfo.machines[minCK].end(); ++iter) {
				if (minC->s < iter->c()) {
					break;
				}
			}
			ganttInfo.machines[minCK].insert(iter, *minC);
			ganttInfo.operations[i][oper] = *minC;
			// [23]
			Gene gene(minCK, i, oper);
			ganttInfo.chromosome.push_back(gene);
			++operCount;
		}
		++oper;
	}
	ganttInfo.printMachines();
}
