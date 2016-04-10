/*
 * Gene.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Gene.h"

#include <iostream>

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

unsigned int Gene::getMachineNumber() const {
	return (machineNumber);
}

unsigned int Gene::getJobNumber() const {
	return (jobNumber);
}

unsigned int Gene::getOperationNumber() const {
	return (operationNumber);
}

void Gene::setMachineNumber(unsigned int machineNumber) {
	this->machineNumber = machineNumber;
}

void Gene::setJobNumber(unsigned int jobNumber) {
	this->jobNumber = jobNumber;
}

void Gene::setOperationNumber(unsigned int operationNumber) {
	this->operationNumber = operationNumber;
}

void Gene::print() const {
	std::cout << "(" << machineNumber << "," << jobNumber << ","
			<< operationNumber << ")";
}
