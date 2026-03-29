#pragma once
#include "IBroker.hpp"
#include <string>
class IBroker;
class ISubscriber {
public:
	virtual void subscribeToBroker(IBroker* broker, std::string topic) = 0;
	virtual void updateCallback(int newData) = 0;
};