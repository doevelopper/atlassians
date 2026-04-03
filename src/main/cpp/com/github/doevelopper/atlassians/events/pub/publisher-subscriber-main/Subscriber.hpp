#pragma once
#include <string>

#include "Broker.hpp"

class Subscriber : public ISubscriber
{
public:
	Subscriber();
	virtual void subscribeToBroker(IBroker* broker, std::string topic);
	virtual void updateCallback(int newData);
private:
	int data;
	std::string topic;
};

