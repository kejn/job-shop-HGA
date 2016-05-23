/*
 * TabooTools.h
 *
 *  Created on: 21 maj 2016
 *      Author: Kamil
 */

#ifndef TABOOTOOLS_H_
#define TABOOTOOLS_H_

#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Oper.h"
#include "util/CircularArray.h"

using Move = std::pair<Oper,Oper>;
using MovesMap = std::map<Oper,Oper>;
using Permutation = std::vector<std::vector<Oper> >;
using BackTrackTriplet = std::pair<Permutation,std::pair<MovesMap,CircularArray<Move>>>;

class TabooTools {

	std::vector<std::vector<Oper> > bestPermutation;

	CircularArray<Move> tabooList;
	CircularArray<BackTrackTriplet> backJumpTrackList;
	MovesMap moves;
	std::vector<std::vector<Oper> > blocks;
public:
	TabooTools(uint tabooListCapacity, uint backJumpTrackListCapacity) :
			tabooList(tabooListCapacity), backJumpTrackList(
					backJumpTrackListCapacity) {
	}
	TabooTools(const TabooTools &other) :
			tabooList(other.tabooList.capacity()), backJumpTrackList(
					other.backJumpTrackList.capacity()) {
	}

	/**
	 * Identifies blocks with size > 1 in critical path.
	 */
	void setUpBlocks(std::vector<Oper> &criticalPath) {
		blocks.clear();
		uint machineIndex = criticalPath.front().getMachineNumber();

		std::vector<Oper>::iterator first = criticalPath.begin();
		std::vector<Oper>::iterator last = criticalPath.begin();
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

	void printBlocks() {
		for (uint i = 0; i < blocks.size(); ++i) {
			for (uint j = 0; j < blocks[i].size(); ++j) {
				std::cout << blocks[i][j].toString() << ' ';
			}
			std::cout << std::endl;
		}
	}

};

#endif /* TABOOTOOLS_H_ */
