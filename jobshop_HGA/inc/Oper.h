/*
 * Oper.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#ifndef OPER_H_
#define OPER_H_

class Oper {
	int startingTime;
	int processingTime;
	int machineNumber;

	int id;
	int pid;
public:
	Oper();
	Oper(const Oper &other);

	const Oper &operator=(const Oper &other);
	int getCompletitionTime();

	int getStartingTime() const;
	int getProcessingTime() const;
	int getMachineNumber() const;
	int getId() const;
	int getPid() const;

	void setStartingTime(int startingTime);
	void setProcessingTime(int processingTime);
	void setMachineNumber(int machineNumber);
	void setId(int id);
	void setPid(int pid);
};


#endif /* OPER_H_ */
