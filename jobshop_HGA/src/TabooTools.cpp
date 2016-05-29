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
			if (first.getPid() != second.getPid()) {
				moves.insert(make_pair(first, second));
			}
		}
		if ((iter + 1) != blocks.end()) {
			Oper secondToLast = iter->rbegin()[1];
			Oper last = iter->back();
			if (secondToLast.getPid() != last.getPid()) {
				moves.insert(make_pair(secondToLast, last));
			}
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
	withoutNonProfitableTaboo.insert(fProfitable.begin(), fProfitable.end());

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
		while (moves.find(tabooList.first().first) == moves.end()) {
			tabooList.push_back(tabooList.last());
		}
		bestMove = tabooList.first();
	}

	return bestMove;
}

MovesMap TabooTools::withoutTabooMoves() {
	MovesMap withoutTabooMoves(moves);

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
				forbiddenMoves.erase(iter++);
			} else {
				++iter;
			}
		} catch (const string & message) {
			++iter;
		}
	}
}

uint TabooTools::makeMove(Gantt &gantt, const Move &move) throw (string) {
	if (move.first.getMachineNumber() != move.second.getMachineNumber()) {
		throw string("You can't move operations of different machines");
	}
	uint machineIndex = move.first.getMachineNumber();

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

	repairPermutation(gantt);
	uint cmax = cMax(gantt.getMachines()).first;

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
		for (uint l, k = 0; k < machines.size(); ++k) {
			l = indexUpdated[k];
			if (l < machines[k].size()) {

				uint jobNumber = machines[k][l].getPid();
				uint operationId = machines[k][l].getId();

				if (!operationsUpdated[jobNumber][operationId]) {
					continue;
				}
				operationsUpdated[jobNumber][operationId + 1] = 1;
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
	uint maxiter = MAXITER;
	bool save = true;
	bool skip = false;

	uint bestCmax = cMax(bestGantt.getMachines()).first;

	do {
		if (!skip) {
			++iter;
			setUpBlocks(criticalPath(currentGantt));
			setMovesBasedOnBlocks();

			if (moves.empty()) {
				cout << "\nSTOP. bestGantt is OPTIMAL." << endl;
				return;
			}
		}
		skip = false;

		Move bestMove = nspAlgorithm();
		moves.erase(bestMove.first);
		if (save && (moves.size() > 1)) {
			BackTrackTriplet btt;
			btt.gantt = currentGantt;
			btt.moves = moves;
			btt.tabooList = tabooList;
			backJumpTrackList.push_back(btt);
		}
		moves.insert(bestMove);
		save = false;

		makeMove(currentGantt, bestMove);
		tabooList.push_back(make_pair(bestMove.second, bestMove.first));

		uint currentCmax = cMax(currentGantt.getMachines()).first;
		if (currentCmax < bestCmax) {
			cout << "I(" << currentCmax << ")" << flush;
			bestGantt = currentGantt;
			bestCmax = currentCmax;
			save = true;
			iter = 0;
		} else {
			cout << "." << flush;
		}

		if (iter <= maxiter) {
			continue;
		}

		if (!backJumpTrackList.empty()) {
			currentGantt = Gantt(backJumpTrackList.first().gantt);

			moves = MovesMap(backJumpTrackList.first().moves);
			tabooList = CircularArray<Move>(
					backJumpTrackList.first().tabooList);
			backJumpTrackList.pop_front();

			iter = 1;
			save = true;
			skip = true;
			maxiter = MAXITER
					- (backJumpTrackList.capacity() - backJumpTrackList.size()
							- 1);
		}
	} while (iter <= maxiter);
	cout << "\nSTOP. iter > MAXITER." << endl;
}
