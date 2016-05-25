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
		Oper second = iter->begin()[1];
		Oper secondToLast = iter->rbegin()[1];
		Oper last = iter->back();
		moves.insert(make_pair(first, second));
		moves.insert(make_pair(secondToLast, last));
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

	MovesMap allProfitable(withoutTabooMoves());
	allProfitable.insert(fProfitable.begin(), fProfitable.end());

//	cout << "NSP" << endl;
	Move bestMove;
	if (!allProfitable.empty()) {
		// *10 to make sure that at least one is chosen
		uint bestCmax = cMax(currentGantt.getMachines()).first * 10;

		MovesMap::iterator iter = allProfitable.begin();
		const MovesMap::iterator endIter = allProfitable.end();

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
			}
		}
	} else if (moves.size() == 1) {
		bestMove = *(moves.begin());
	} else {
		uint i = 0;
		while (withoutTabooMoves().size() > 1) {
			tabooList.push_back(tabooList.last());
			if (++i % 100000) {
				cerr << "possible ERROR NSP line 112" << endl;
			}
		}
		bestMove = *(withoutTabooMoves().begin());
	}
	return bestMove;
}

MovesMap TabooTools::withoutTabooMoves() {
	MovesMap withoutTabooMoves = moves;

	for (uint i = 0; i < tabooList.size(); ++i) {
		try {
			moves.at(tabooList[i].first);
			withoutTabooMoves.erase(tabooList[i].first);
		} catch (const out_of_range & oor) {
		}
	}
	return withoutTabooMoves;
}

MovesMap TabooTools::forbiddenMoves() {
	MovesMap forbiddenMoves;
	for (uint i = 0; i < tabooList.size(); ++i) {
		try {
			moves.at(tabooList[i].first);
			forbiddenMoves.insert(tabooList[i]);
		} catch (const out_of_range & oor) {
		}
	}
	return forbiddenMoves;
}

void TabooTools::forbiddenProfitable(MovesMap & forbiddenMoves) {
	uint bestCmax = cMax(bestGantt.getMachines()).first;
	MovesMap::iterator iter = forbiddenMoves.begin();
	const MovesMap::iterator endIter = forbiddenMoves.end();

	while (iter != endIter) {
		Gantt bestCopy(bestGantt);
		try {
			uint cMax = makeMove(bestCopy, *iter);
			if (cMax < bestCmax) {
				++iter;
			} else {
//				cout << "NOT" << endl;
				forbiddenMoves.erase(iter++);
			}
		} catch (const string & message) {
			cerr << message << endl;
		}
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

	repairPermutation(gantt, firstIter);

	uint cmax = cMax(gantt.getMachines()).first;

//	cout << "...repaired. cmax: " << cmax << endl;

	return cmax;
}

void TabooTools::repairPermutation(Gantt &gantt, vector<Oper>::iterator iter) {

	uint machineIndex = iter->getMachineNumber();
	uint jobNumber = iter->getPid();
	uint operationId = iter->getId();

	uint startingTime = max(calculateT0(gantt, jobNumber, operationId),
			calculateT1(gantt.getMachines()[machineIndex], iter));
	iter->setStartingTime(startingTime);
	gantt.getOperations()[jobNumber][operationId].setStartingTime(startingTime);

	if ((iter + 1) != gantt.getMachines()[machineIndex].end()) {
		repairPermutation(gantt, iter + 1);
	}
	try {
		Oper operInfo = gantt.nextOperationTo(jobNumber, operationId);
		vector<Oper> & machine =
				gantt.getMachines()[operInfo.getMachineNumber()];
		vector<Oper>::iterator iter = find(machine.begin(), machine.end(),
				operInfo);
		repairPermutation(gantt, iter);
	} catch (const string & message) {
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
	bool save = true;
	bool skip = false;

	do {
		if (!skip) {
			cout << "STEP 1." << endl;
			++iter;
			setUpBlocks(criticalPath(currentGantt));
			setMovesBasedOnBlocks();

			if (moves.empty()) {
				cout << "STOP. bestGantt is OPTIMAL." << endl;
				return;
			}
		}

		cout << "STEP 2." << endl;
		Move bestMove = nspAlgorithm();
		cout << "NSP done" << endl;
		if (save && (moves.size() > 1) && !skip) {
			cout << "++bt " << flush;
			BackTrackTriplet btt;
			btt.gantt = currentGantt;
			btt.moves = moves;
			MovesMap::iterator it =
					find_if(moves.begin(), moves.end(),
							[&](const Move & m) {
								return (m.first == bestMove.first) && (m.second == bestMove.second);
							});
			btt.moves.erase(it);
			btt.tabooList = tabooList;
			backJumpTrackList.push_back(btt);

			cout << backJumpTrackList.size() << ' '
					<< backJumpTrackList.capacity() << endl;
		}
		cout << "Best move:" << bestMove.first.toString() << ", "
				<< bestMove.second.toString() << endl;
//		cout << "." << flush;
		makeMove(currentGantt, bestMove);
		tabooList.push_back(make_pair(bestMove.second, bestMove.first));
		save = false;
		skip = false;

		cout << "STEP 3." << endl;
		if (cMax(currentGantt.getMachines()).first
				< cMax(bestGantt.getMachines()).first) {
			cout << "IMPROVEMENT" << endl;
			bestGantt = currentGantt;
			save = true;
			iter = 0;
		}

		cout << "STEP 4." << endl;
		if (iter <= MAXITER) {
			continue;
		}
		cout << "STEP 5." << endl;
		if (!backJumpTrackList.empty()) {
			cout << "--bt " << flush;
			currentGantt = backJumpTrackList.first().gantt;
			moves = backJumpTrackList.first().moves;
			tabooList = backJumpTrackList.first().tabooList;
			backJumpTrackList.pop_front();

			cout << backJumpTrackList.size() << ' '
					<< backJumpTrackList.capacity() << endl;

			iter = 1;
			save = true;
			skip = true;
		}
	} while (iter <= MAXITER);
	cout << "STOP. iter > MAXITER." << endl;

}
