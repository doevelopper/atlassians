#pragma once
#include "IBroker.hpp"
class IPublisher {
public:
	virtual void registerBroker(IBroker* broker) = 0;
	virtual void publishToBroker(std::string topic, int newData) = 0;
	virtual void update() = 0;
};