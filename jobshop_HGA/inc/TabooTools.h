/*
 * TabooTools.h
 *
 *  Created on: 21 maj 2016
 *      Author: Kamil
 */

#ifndef TABOOTOOLS_H_
#define TABOOTOOLS_H_

#include <map>
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
public:
	TabooTools(uint tabooListCapacity, uint backJumpTrackListCapacity) :
			tabooList(tabooListCapacity), backJumpTrackList(
					backJumpTrackListCapacity) {
	}
	TabooTools(const TabooTools &other) :
			tabooList(other.tabooList.capacity()), backJumpTrackList(
					other.backJumpTrackList.capacity()) {
	}
};

#endif /* TABOOTOOLS_H_ */
