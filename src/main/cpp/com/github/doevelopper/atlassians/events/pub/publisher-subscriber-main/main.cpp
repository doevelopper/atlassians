#include "Publisher.hpp"
#include "Subscriber.hpp"
#include "Broker.hpp"

int main()
{
	Broker broker;
	Publisher publisher;
	broker.registerToPublisher(&publisher);
	
	Subscriber sub1, sub2;

	sub1.subscribeToBroker(&broker, "topicString");
	return 0;
}