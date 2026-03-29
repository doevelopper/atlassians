#pragma once
#include <string>

#include "Broker.hpp"
class Publisher : public IPublisher
{
public:
	Publisher();
	virtual void registerBroker(IBroker* broker);
	virtual void publishToBroker(std::string topic, int newData);
	virtual void update();
private:
	IBroker* broker;
	int dataTopicA;
	int dataTopicB;
	
};

