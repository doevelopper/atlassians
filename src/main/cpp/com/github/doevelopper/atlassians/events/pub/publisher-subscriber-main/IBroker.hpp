#pragma once
#include "ISubscriber.hpp"
#include "IPublisher.hpp"
class IBroker {
public:
	virtual void addSubscriber(ISubscriber* subscriber, std::string topic) = 0;
	virtual void registerToPublisher(IPublisher* publisher) = 0;
	virtual void onPublish(std::string topic, int newData) = 0;
};