/*
 * TabooTools.cpp
 *
 *  Created on: 23 maj 2016
 *      Author: Kamil
 */

#include "../inc/TabooTools.h"

#include <iostream>
#include <iterator>
#include <string>

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
	Permutation::iterator iter = blocks.begin();
	for (; iter != blocks.end(); ++iter) {
		if (iter->size() < 2) {
			throw string("error! block size is < 2");
		}
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

void TabooTools::setBestGantt(const Gantt &bestGantt) {
	this->bestGantt = bestGantt;
}

TabooTools TabooTools::create(const Gantt &ganttInfo, uint tabooListCapacity,
		uint backJumpTrackListCapacity) {
	return TabooTools(ganttInfo, tabooListCapacity, backJumpTrackListCapacity);
}

void TabooTools::nspAlgorithm() {
	MovesMap fMoves = forbiddenMoves();
	MovesMap fProfitable;
	makeMove(bestGantt, *(moves.begin()));

	/**
	 * tu skoñczy³em: odrzuæ ruchy FN i jeœli {U,FP} nie jest pusty wybierz
	 * 				  najlepszy z {U,FP}.
	 * 				  jeœli jest tylko jeden ruch to go wybierz.
	 * 				  przeciwnie wybierz najstarszy...itp
	 */
}

MovesMap TabooTools::forbiddenMoves() {
	MovesMap forbiddenMoves;
	for (const auto & m : moves) {
		for (uint i = 0; i < tabooList.size(); ++i) {
			if ((tabooList[i].first == m.first)
					&& (tabooList[i].second == m.second)) {
				forbiddenMoves.insert(m);
				break;
			}
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
		uint cMax = makeMove(bestCopy, *iter);
		if (cMax < bestCmax) {
			++iter;
		} else {
			forbiddenMoves.erase(iter++);
		}
	}
}

uint TabooTools::makeMove(Gantt &gantt, const Move &move) throw (string) {
	if (move.first.getMachineNumber() != move.second.getMachineNumber()) {
		throw string("You can't move operations of different machines");
	}
	uint machineIndex = move.first.getMachineNumber();

	cout << "Moving " << move.first.toString() << " and "
			<< move.second.toString() << endl;

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

	repairPermutation(gantt, firstIter);

	return cMax(gantt.getMachines()).first;
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

