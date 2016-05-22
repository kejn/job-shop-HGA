/*
 * Oper.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#ifndef OPER_H_
#define OPER_H_

#include <map>
#include <string>

using uint = unsigned int;

class Oper {
	uint startingTime;
	std::map<uint, uint> processingTime;
	uint machineNumber;
	uint indexOnMachine;

	uint id;
	uint pid;
public:
	Oper();
	Oper(const Oper &other);

	bool isFirstInJob();
	const Oper &operator=(const Oper &other);
	uint getCompletitionTime() const;

	uint getStartingTime() const;
	uint getProcessingTime() const;
	const std::map<uint, uint> &getProcessingTimes() const;
	uint getMachineNumber() const;
	uint getId() const;
	uint getPid() const;
	uint getIndexOnMachine() const;

	void setStartingTime(uint startingTime);
	void setProcessingTime(uint processingTime, uint atMachine);
	void setProcessingTimes(std::map<uint, uint> processingTime);
//	void changeMachineNumber(uint machineNumber);
	void setMachineNumber(uint machineNumber);
	void setId(uint id);
	void setPid(uint pid);
	void setIndexOnMachine(uint positionOnMachine);

	std::string toString() const;
};

#endif /* OPER_H_ */
