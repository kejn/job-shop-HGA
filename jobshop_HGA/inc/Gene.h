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

	void setMachineNumber(int machineNumber);
	void setJobNumber(int jobNumber);
	void setOperationNumber(int operationNumber);
};

#endif /* INC_GENE_H_ */
