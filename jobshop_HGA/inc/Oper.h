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

	uint buffer;

	uint id;
	uint pid;
public:
	Oper();
	Oper(const Oper &other);

	bool isFirstInJob();
	Oper &operator=(const Oper &other);
	uint getCompletitionTime() const;

	uint getStartingTime() const;
	uint getProcessingTime() const;
	const std::map<uint, uint> &getProcessingTimes() const;
	uint getMachineNumber() const;
	uint getId() const;
	uint getPid() const;

	void setStartingTime(uint startingTime);
	void setProcessingTime(uint processingTime, uint atMachine);
	void setProcessingTimes(std::map<uint, uint> processingTime);
	void setMachineNumber(uint machineNumber);
	void setId(uint id);
	void setPid(uint pid);

	void setBuffer(uint buffer);

	std::string toString() const;
};

bool operator<(const Oper &_1, const Oper &_2);
bool operator==(const Oper& _1, const Oper& _2);

#endif /* OPER_H_ */
