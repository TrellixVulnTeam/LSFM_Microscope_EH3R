#pragma once
#include "Component.h"
#include <vector>

using namespace std;
class Composite : public Component
{

	vector < Component* > children;

public:
	typedef	typename vector<Component*>::iterator ChildIterator;

	void add(Component* component) override;


	void remove(Component* component) override;

	
	 void Connect() override;


	 void Disconnect() override;

};