/*
 * Oper.cpp
 *
 *  Created on: 6 kwi 2016
 *      Author: Kamil
 */

#include "../inc/Oper.h"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

using namespace std;

Oper::Oper() :
		processingTime() {
	startingTime = buffer = 0;
	machineNumber = pid = id = -1;
}

Oper::Oper(const Oper &other) {
	*this = other;
}

Oper &Oper::operator=(const Oper &other) {
	startingTime = other.startingTime;
	processingTime = other.processingTime;
	machineNumber = other.machineNumber;
	id = other.id;
	pid = other.pid;
	buffer = other.buffer;
	return *this;
}

uint Oper::getCompletitionTime() const {
	uint result;
	try {
		result = startingTime + getProcessingTime();
	} catch (const out_of_range & oor) {
		cerr << "ERROR, tried to read " << machineNumber << " machine cTime" << endl;
		result = -1;
		getchar();
	}
	return result;
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
	return processingTime.at(machineNumber) + buffer;
}

void Oper::setProcessingTimes(map<uint, uint> processingTime) {
	this->processingTime = processingTime;
}

void Oper::setProcessingTime(uint processingTime, uint atMachine) {
//	this->processingTime.insert(pair<uint, uint>(atMachine, processingTime));
	this->processingTime[atMachine] = processingTime;
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

bool operator <(const Oper& _1, const Oper& _2) {
	return (_1.getPid() * 1000 + _1.getId()) < (_2.getPid() * 1000 + _2.getId());
}

bool operator==(const Oper& _1, const Oper& _2) {
	return (_1.getPid() == _2.getPid()) && (_1.getId() == _2.getId());
}

void Oper::setBuffer(uint buffer) {
	this->buffer = buffer;
}

uint Oper::getBuffer() const {
	return buffer;
}
