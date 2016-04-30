/*
 * Oper.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#ifndef OPER_H_
#define OPER_H_

#include <string>

class Oper {
	unsigned int startingTime;
	unsigned int processingTime;
	unsigned int machineNumber;

	unsigned int id;
	unsigned int pid;
public:
	Oper();
	Oper(const Oper &other);

	bool isFirstInJob();
	const Oper &operator=(const Oper &other);
	unsigned int getCompletitionTime() const;

	unsigned int getStartingTime() const;
	unsigned int getProcessingTime() const;
	unsigned int getMachineNumber() const;
	unsigned int getId() const;
	unsigned int getPid() const;

	void setStartingTime(unsigned int startingTime);
	void setProcessingTime(unsigned int processingTime);
	void setMachineNumber(unsigned int machineNumber);
	void setId(unsigned int id);
	void setPid(unsigned int pid);

	std::string toString();
};


#endif /* OPER_H_ */
