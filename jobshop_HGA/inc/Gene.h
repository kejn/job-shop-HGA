/*
 * Gene.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#ifndef INC_GENE_H_
#define INC_GENE_H_

class Gene {
	unsigned int machineNumber; // machine no
	unsigned int jobNumber; // job no
	unsigned int operationNumber; // operation no
public:
	Gene();
	Gene(int machineNo, int jobNo, int operationNo);
	Gene & operator=(const Gene & other);

	unsigned int getMachineNumber() const;
	unsigned int getJobNumber() const;
	unsigned int getOperationNumber() const;

	void print() const;

	void setMachineNumber(unsigned int machineNumber);
	void setJobNumber(unsigned int jobNumber);
	void setOperationNumber(unsigned int operationNumber);
};

#endif /* INC_GENE_H_ */
