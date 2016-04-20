/*
 * Oper.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Oper.h"
#include <sstream>

Oper::Oper() {
	startingTime = processingTime = 0;
	machineNumber = pid = id = -1;
}
Oper::Oper(const Oper &other) {
	*this = other;
}

const Oper &Oper::operator=(const Oper &other) {
	startingTime = other.startingTime;
	processingTime = other.processingTime;
	machineNumber = other.machineNumber;
	id = other.id;
	pid = other.pid;
	return (*this);
}

unsigned int Oper::getCompletitionTime() const {
	return (startingTime + processingTime);
}

int Oper::getId() const {
	return (id);
}

void Oper::setId(int id) {
	this->id = id;
}

unsigned int Oper::getMachineNumber() const {
	return (machineNumber);
}

void Oper::setMachineNumber(unsigned int machineNumber) {
	this->machineNumber = machineNumber;
}

int Oper::getPid() const {
	return (pid);
}

void Oper::setPid(int pid) {
	this->pid = pid;
}

unsigned int Oper::getProcessingTime() const {
	return (processingTime);
}

void Oper::setProcessingTime(unsigned int processingTime) {
	this->processingTime = processingTime;
}

unsigned int Oper::getStartingTime() const {
	return (startingTime);
}

void Oper::setStartingTime(unsigned int startingTime) {
	this->startingTime = startingTime;
}

bool Oper::isFirstInJob() {
	return (id == 0);
}

std::string Oper::toString() {
	std::stringstream ss;
	ss << "[pid,id]=[" << pid << "," << id << "], (s,p,c)=(" << startingTime
			<< "," << processingTime << "," << getCompletitionTime() << ")";
	return (ss.str());
}
