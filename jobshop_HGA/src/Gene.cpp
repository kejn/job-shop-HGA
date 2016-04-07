/*
 * Gene.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Gene.h"

Gene::Gene() {
	machineNumber = jobNumber = operationNumber = -1;
}

Gene::Gene(int machineNo, int jobNo, int operationNo) {
	machineNumber = machineNo;
	jobNumber = jobNo;
	operationNumber = operationNo;
}

Gene & Gene::operator =(const Gene & other) {
	machineNumber = other.machineNumber;
	jobNumber = other.jobNumber;
	operationNumber = other.operationNumber;
	return (*this);
}

int Gene::getMachineNumber() const {
	return (machineNumber);
}

int Gene::getJobNumber() const {
	return (jobNumber);
}

int Gene::getOperationNumber() const {
	return (operationNumber);
}

void Gene::setMachineNumber(int machineNumber) {
	this->machineNumber = machineNumber;
}

void Gene::setJobNumber(int jobNumber) {
	this->jobNumber = jobNumber;
}

void Gene::setOperationNumber(int operationNumber) {
	this->operationNumber = operationNumber;
}
