/*
 * Oper.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Oper.h"

#include <iostream>
#include <sstream>
#include <utility>

using namespace std;

Oper::Oper() :
		processingTime() {
	startingTime = 0;
	machineNumber = indexOnMachine = pid = id = -1;
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
	indexOnMachine = other.indexOnMachine;
	return *this;
}

uint Oper::getCompletitionTime() const {
	return (startingTime + processingTime.at(machineNumber));
}

uint Oper::getId() const {
	return id;
}

void Oper::setId(uint id) {
	this->id = id;
}

uint Oper::getMachineNumber() const {
	return machineNumber;
}

///**
// * Use after operation initialization when using Taillard examples
// * (flexible job shop with identical machines).
// */
//void Oper::changeMachineNumber(uint machineNumber) {
//	try {
//		processingTime.at(machineNumber);
//	} catch (const std::out_of_range & e) {
//		setProcessingTime(getProcessingTime(), machineNumber);
//	}
//	this->machineNumber = machineNumber;
//}

void Oper::setMachineNumber(uint machineNumber) {
	this->machineNumber = machineNumber;
}

uint Oper::getPid() const {
	return pid;
}

void Oper::setPid(uint pid) {
	this->pid = pid;
}

uint Oper::getProcessingTime() const {
	return processingTime.at(machineNumber);
}

void Oper::setProcessingTimes(map<uint, uint> processingTime) {
	this->processingTime = processingTime;
}

void Oper::setProcessingTime(uint processingTime, uint atMachine) {
	this->processingTime.insert(pair<uint, uint>(atMachine, processingTime));
}

uint Oper::getStartingTime() const {
	return startingTime;
}

void Oper::setStartingTime(uint startingTime) {
	this->startingTime = startingTime;
}

bool Oper::isFirstInJob() {
	return id == 0;
}

std::string Oper::toString() const {
	std::stringstream ss;
	ss << "[pid,id]=[" << pid << "," << id << "] (s,p,c)=(" << startingTime
			<< "," << getProcessingTime() << "," << getCompletitionTime()
			<< ")";
	return ss.str();
}

const std::map<uint, uint> &Oper::getProcessingTimes() const {
	return processingTime;
}

uint Oper::getIndexOnMachine() const {
	return indexOnMachine;
}

void Oper::setIndexOnMachine(uint indexOnMachine) {
	this->indexOnMachine = indexOnMachine;
}
