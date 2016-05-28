/*
 * TabooTools.cpp
 *
 *  Created on: 23 maj 2016
 *      Author: Kamil
 */

#include "../inc/TabooTools.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace std;

void TabooTools::setUpBlocks(vector<Oper> criticalPath) {
	blocks.clear();
	uint machineIndex = criticalPath.front().getMachineNumber();

	vector<Oper>::iterator first = criticalPath.begin();
	vector<Oper>::iterator last = criticalPath.begin();
	for (; last != criticalPath.end(); ++last) {
		uint currentMachineIndex = last->getMachineNumber();
		if (currentMachineIndex != machineIndex) {
			if (distance(first, last) > 1) {
				blocks.push_back(std::vector<Oper>(first, last));
			}
			first = last;
			machineIndex = currentMachineIndex;
		}
	}
	if (distance(first, last) > 1) {
		blocks.push_back(std::vector<Oper>(first, last));
	}
}

void TabooTools::printBlocks() {
	cout << "BLOCKS" << endl;
	for (uint i = 0; i < blocks.size(); ++i) {
		for (uint j = 0; j < blocks[i].size(); ++j) {
			std::cout << blocks[i][j].toString() << ' ';
		}
		cout << endl;
	}
}

void TabooTools::setMovesBasedOnBlocks() throw (std::string) {
	moves.clear();
	Permutation::iterator iter = blocks.begin();
	for (; iter != blocks.end(); ++iter) {
		Oper first = iter->front();
		if (first.getStartingTime() > 0) {
			Oper second = iter->begin()[1];
			moves.insert(make_pair(first, second));
		}
		if ((iter + 1) != blocks.end()) {
			Oper secondToLast = iter->rbegin()[1];
			Oper last = iter->back();
			moves.insert(make_pair(secondToLast, last));
		}
	}
}

void TabooTools::printMoves() {
	cout << "MOVES" << endl;
	for (const pair<Oper, Oper> & move : moves) {
		cout << "<<" << move.first.toString() << " | " << move.second.toString()
				<< ">>" << endl;
	}
}

TabooTools TabooTools::create(const Gantt &ganttInfo, uint tabooListCapacity,
		uint backJumpTrackListCapacity) {
	return TabooTools(ganttInfo, tabooListCapacity, backJumpTrackListCapacity);
}

Move TabooTools::nspAlgorithm() {
	MovesMap fProfitable = forbiddenMoves();
	forbiddenProfitable(fProfitable);

	MovesMap withoutNonProfitableTaboo(withoutTabooMoves());
//	cout << "out" << flush;
	withoutNonProfitableTaboo.insert(fProfitable.begin(), fProfitable.end());

//	cout << "Nsp" << flush;
	Move bestMove;
	if (!withoutNonProfitableTaboo.empty()) {
		// *10 to make sure that at least one is chosen
		uint bestCmax = cMax(currentGantt.getMachines()).first * 10;

		MovesMap::iterator iter = withoutNonProfitableTaboo.begin();
		const MovesMap::iterator endIter = withoutNonProfitableTaboo.end();
		for (; iter != endIter; ++iter) {
			Gantt currentCopy(currentGantt);
			try {
				uint cMax = makeMove(currentCopy, *iter);
				if (cMax < bestCmax) {
					bestCmax = cMax;
					bestMove = *iter;
				}
			} catch (const string & message) {
				cerr << message << endl;
				getchar();
			}
		}
	} else if (moves.size() == 1) {
		bestMove = *(moves.begin());
	} else {
//		cout << "s(" << moves.size() << ')' << flush;
		while (moves.find(tabooList.first().first) == moves.end()) {
			tabooList.push_back(tabooList.last());
		}
		bestMove = tabooList.first();
	}

	return bestMove;
}

MovesMap TabooTools::withoutTabooMoves() {
//	cout << "Wt" << flush;
	MovesMap withoutTabooMoves(moves);
//	cout << ";" << flush;

	for (uint i = 0; i < tabooList.size(); ++i) {
		try {
			MovesMap::iterator iter = moves.find(tabooList[i].first);
			if (iter != moves.end()) {
				withoutTabooMoves.erase(tabooList[i].first);
			}
		} catch (const out_of_range & oor) {
		}
	}
	return withoutTabooMoves;
}

MovesMap TabooTools::forbiddenMoves() {
//	cout << "Fm" << flush;
	MovesMap forbiddenMoves;
	for (uint i = 0; i < tabooList.size(); ++i) {
		try {
			MovesMap::iterator iter = moves.find(tabooList[i].first);
			if (iter != moves.end()) {
				forbiddenMoves.insert(tabooList[i]);
			}
		} catch (const out_of_range & oor) {
		}
	}
	return forbiddenMoves;
}

void TabooTools::forbiddenProfitable(MovesMap & forbiddenMoves) {
//	cout << "Fp" << flush;
	MovesMap::iterator iter = forbiddenMoves.begin();
	const MovesMap::iterator endIter = forbiddenMoves.end();

	while (iter != endIter) {

		Oper first = iter->first;
		Oper second = iter->second;

		try {
			uint jobIx = second.getPid();
			uint operIx = second.getId();
			Oper prevToSecond = currentGantt.prevOperationTo(jobIx, operIx);

			if (prevToSecond.getCompletitionTime() >= first.getStartingTime()) {
				forbiddenMoves.erase(iter);
			}
		} catch (const string & message) {
		}
		++iter;
	}
}

uint TabooTools::makeMove(Gantt &gantt, const Move &move) throw (string) {
	if (move.first.getMachineNumber() != move.second.getMachineNumber()) {
		throw string("You can't move operations of different machines");
	}
	uint machineIndex = move.first.getMachineNumber();

//	cout << "Moving " << move.first.toString() << " and "
//			<< move.second.toString() << endl;

	vector<Oper>::iterator firstIter = find(
			gantt.getMachines()[machineIndex].begin(),
			gantt.getMachines()[machineIndex].end(), move.first);
	if (firstIter == gantt.getMachines()[machineIndex].end()) {
		throw string("firstIter not found");
	}

	vector<Oper>::iterator secondIter = find(
			gantt.getMachines()[machineIndex].begin(),
			gantt.getMachines()[machineIndex].end(), move.second);
	if (secondIter == gantt.getMachines()[machineIndex].end()) {
		throw string("secondIter not found");
	}

	iter_swap(firstIter, secondIter);

//	cout << "Moved. Repairing..." << endl;

	repairPermutation(gantt);
	uint cmax = cMax(gantt.getMachines()).first;

//	cout << "...repaired. cmax: " << cmax << endl;

	return cmax;
}

void TabooTools::repairPermutation(Gantt &gantt) {
	Permutation & machines = gantt.getMachines();

	vector<vector<bool> > operationsUpdated(gantt.getNJobs());
	for (uint i = 0; i < operationsUpdated.size(); ++i) {
		operationsUpdated[i].resize(gantt.getOperations()[i].size() + 1, false);
		operationsUpdated[i][0] = true;
	}

	vector<uint> indexUpdated(gantt.getNMachines(), 0);

	uint operationsScheduled = 0;

	while (operationsScheduled < gantt.getTotNOper()) {
		for (uint k = 0; k < machines.size(); ++k) {
			uint l = indexUpdated[k];
			if (l < machines[k].size()) {

				uint jobNumber = machines[k][l].getPid();
				uint operationId = machines[k][l].getId();

				if (!operationsUpdated[jobNumber][operationId]) {
					continue;
				}
				operationsUpdated[jobNumber][operationId + 1] = true;
				++indexUpdated[k];
				++operationsScheduled;

				uint startingTime = max(
						calculateT0(gantt, jobNumber, operationId),
						calculateT1(machines[k], machines[k].begin() + l));

				machines[k][l].setStartingTime(startingTime);
				gantt.getOperations()[jobNumber][operationId].setStartingTime(
						startingTime);
			}
		}
	}
}

uint TabooTools::calculateT0(const Gantt & gantt, uint jNum, uint oNum) {
	uint t0;
	try {
		Oper prevOper = gantt.prevOperationTo(jNum, oNum);
		t0 = prevOper.getCompletitionTime();
	} catch (const string &message) {
		t0 = 0;
	}
	return t0;
}

uint TabooTools::calculateT1(const vector<Oper>& machine,
		vector<Oper>::iterator iter) {
	uint t1;

	if (iter == machine.begin()) {
		t1 = 0;
	} else {
		t1 = (iter - 1)->getCompletitionTime();
	}
	return t1;
}

void TabooTools::tsAlgorithm() {
	uint iter = 0;

	do {
		++iter;
		setUpBlocks(criticalPath(currentGantt));
		setMovesBasedOnBlocks();

		if (moves.empty()) {
			cout << "STOP. bestGantt is OPTIMAL." << endl;
			return;
		}
//		cout << "N" << flush;
		Move bestMove = nspAlgorithm();
		cout << "." << flush;
		makeMove(currentGantt, bestMove);
		tabooList.push_back(make_pair(bestMove.second, bestMove.first));

		if (cMax(currentGantt.getMachines()).first
				< cMax(bestGantt.getMachines()).first) {
			cout << "I" << flush;
			bestGantt = currentGantt;
			iter = 0;
		}
	} while (iter <= MAXITER);
	cout << "\nSTOP. iter > MAXITER." << endl;
}

void TabooTools::tsabAlgorithm() {
	uint iter = 0;
	bool save = true;
	bool skip = false;

	do {
		if (!skip) {
//			cout << "STEP 1." << endl;
			++iter;
			setUpBlocks(criticalPath(currentGantt));
			setMovesBasedOnBlocks();

			if (moves.empty()) {
				cout << "STOP. bestGantt is OPTIMAL." << endl;
				return;
			}
		}

//		cout << "STEP 2." << endl;
		Move bestMove = nspAlgorithm();
//		cout << "NSP done" << endl;
//		cout << "Best move:" << flush;
//		cout << bestMove.first.toString() << ", " << flush;
//		cout << bestMove.second.toString() << endl;
//		printMoves();
		moves.erase(bestMove.first);
		if (save && (moves.size() > 1)) {
//			cout << "++bt " << flush;
			BackTrackTriplet btt;
			btt.gantt = currentGantt;
			btt.moves = moves;
//			MovesMap::iterator it =
//					find_if(btt.moves.begin(), btt.moves.end(),
//							[&](const Move & m) {
//								return (m.first == bestMove.first) && (m.second == bestMove.second);
//							});
//			printMoves();
//			if (it != btt.moves.end()) {
//				cout << "erasing.." << flush;
//				btt.moves.erase(it);
//				cout << "erased!" << endl;
//			}
			btt.tabooList = tabooList;
			backJumpTrackList.push_back(btt);

//			cout << backJumpTrackList.size() << ' '
//					<< backJumpTrackList.capacity() << endl;
		}
		moves.insert(bestMove);
		cout << "." << flush;
		makeMove(currentGantt, bestMove);
		tabooList.push_back(make_pair(bestMove.second, bestMove.first));
		save = false;
		skip = false;

//		cout << "STEP 3." << endl;
		if (cMax(currentGantt.getMachines()).first
				< cMax(bestGantt.getMachines()).first) {
			cout << "I" << flush;
			bestGantt = currentGantt;
			save = true;
			iter = 0;
		}

//		cout << "STEP 4." << endl;
		if (iter <= MAXITER) {
			continue;
		}
//		cout << "STEP 5." << endl;
		if (!backJumpTrackList.empty()) {
//			cout << "--bt " << flush;
			currentGantt = Gantt(backJumpTrackList.first().gantt);

			moves = MovesMap(backJumpTrackList.first().moves);
			tabooList = CircularArray<Move>(
					backJumpTrackList.first().tabooList);
			backJumpTrackList.pop_front();

//			cout << backJumpTrackList.size() << ' '
//					<< backJumpTrackList.capacity() << endl;

			iter = 1;
			save = true;
			skip = true;
		}
	} while (iter <= MAXITER);
	cout << "STOP. iter > MAXITER." << endl;

}
