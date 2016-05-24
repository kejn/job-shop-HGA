/*
 * TabooTools.h
 *
 *  Created on: 21 maj 2016
 *      Author: Kamil
 */

#ifndef TABOOTOOLS_H_
#define TABOOTOOLS_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Gantt.h"
#include "Oper.h"
#include "util/CircularArray.h"

using Move = std::pair<Oper,Oper>;
using MovesMap = std::map<Oper,Oper>;
//using BackTrackTriplet = std::pair<Gantt,std::pair<MovesMap,CircularArray<Move>>>;

class TabooTools {
	struct BackTrackTriplet {
		Gantt gantt;
		MovesMap moves;
		CircularArray<Move> tabooList;
	};

	Gantt bestGantt;

	CircularArray<Move> tabooList;
	CircularArray<BackTrackTriplet> backJumpTrackList;
	MovesMap moves;
	Permutation blocks;
public:
	static TabooTools create(const Gantt &ganttInfo, uint tabooListCapacity,
			uint backJumpTrackListCapacity);

	void nspAlgorithm();

	void printBlocks();
	void printMoves();

	Gantt & getBestGantt() {
		return bestGantt;
	}
private:
	TabooTools(const Gantt &ganttInfo, uint tabooListCapacity,
			uint backJumpTrackListCapacity) :
			tabooList(tabooListCapacity), backJumpTrackList(
					backJumpTrackListCapacity) {
		setUpBlocks(criticalPath(ganttInfo));
		setMovesBasedOnBlocks();
		setBestGantt(ganttInfo);
	}

	/**
	 * Identifies blocks with size > 1 in critical path.
	 */
	void setUpBlocks(std::vector<Oper> criticalPath);
	void setMovesBasedOnBlocks() throw (std::string);
	void setBestGantt(const Gantt &bestGantt);

	MovesMap forbiddenMoves();
	void forbiddenProfitable(MovesMap & forbiddenMoves);
	uint makeMove(Gantt &permutation, const Move &move) throw (std::string);
	void repairPermutation(Gantt &gantt, std::vector<Oper>::iterator iter);

	inline uint calculateT0(const Gantt & gantt, uint jNum, uint oNum);

	inline uint calculateT1(const std::vector<Oper>& machine,
			std::vector<Oper>::iterator iter);

};

#endif /* TABOOTOOLS_H_ */
