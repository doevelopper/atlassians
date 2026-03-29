#pragma once
#include <string>
#include <vector>

#include "IBroker.hpp"

class Subscriber;
class Publisher;
class Broker : public IBroker
{
public:
	Broker();
	virtual void addSubscriber(ISubscriber* subscriber, std::string topic);
	virtual void registerToPublisher(IPublisher* publisher);
	virtual void onPublish(std::string topic, int newData);
private:
	//Publisher* publisher;
	std::vector<Subscriber*> subscriberTopicA;
	std::vector<Subscriber*> subscriberTopicB;

};

