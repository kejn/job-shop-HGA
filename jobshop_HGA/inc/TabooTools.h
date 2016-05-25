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
	Gantt currentGantt;

	CircularArray<Move> tabooList;
	CircularArray<BackTrackTriplet> backJumpTrackList;
	MovesMap moves;
	Permutation blocks;
public:
	static TabooTools create(const Gantt &ganttInfo, uint tabooListCapacity,
			uint backJumpTrackListCapacity);

	void tsAlgorithm();
	void tsabAlgorithm();

	void printBlocks();
	void printMoves();

	Gantt & getBestGantt() {
		return bestGantt;
	}
	void setBestGantt(Gantt & gantt) {
		this->bestGantt = gantt;
	}

	Gantt & getCurrentGantt() {
		return currentGantt;
	}
	void setCurrentGantt(Gantt & gantt) {
		this->currentGantt = gantt;
	}
private:
	TabooTools(const Gantt &ganttInfo, uint tabooListCapacity,
			uint backJumpTrackListCapacity) :
			tabooList(tabooListCapacity), backJumpTrackList(
					backJumpTrackListCapacity) {
		bestGantt = ganttInfo;
		currentGantt = ganttInfo;
	}

	Move nspAlgorithm();

	/**
	 * Identifies blocks with size > 1 in critical path.
	 */
	void setUpBlocks(std::vector<Oper> criticalPath);
	void setMovesBasedOnBlocks() throw (std::string);

	MovesMap withoutTabooMoves();
	MovesMap forbiddenMoves();
	void forbiddenProfitable(MovesMap & forbiddenMoves);
	uint makeMove(Gantt &permutation, const Move &move) throw (std::string);
	void repairPermutation(Gantt &gantt, std::vector<Oper>::iterator iter);

	inline uint calculateT0(const Gantt & gantt, uint jNum, uint oNum);

	inline uint calculateT1(const std::vector<Oper>& machine,
			std::vector<Oper>::iterator iter);

};

const uint MAXITER = 5;

#endif /* TABOOTOOLS_H_ */
