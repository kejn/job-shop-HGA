/*
 * Gene.h
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#ifndef INC_GENE_H_
#define INC_GENE_H_

class Gene {
	int machineNumber; // machine no
	int jobNumber; // job no
	int operationNumber; // operation no
public:
	Gene();
	Gene(int machineNo, int jobNo, int operationNo);
	Gene & operator=(const Gene & other);

	int getMachineNumber() const;
	int getJobNumber() const;
	int getOperationNumber() const;

	void setMachineNumber(int machineNumber);
	void setJobNumber(int jobNumber);
	void setOperationNumber(int operationNumber);
};

#endif /* INC_GENE_H_ */
