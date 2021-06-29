#pragma once
class Component
{
public:

	virtual void add(Component* component) = 0;      // add a leaf or a Composite to a Component 

	virtual void remove(Component* component) = 0;    // remove a leaf or a copmosite to  a Component 

	virtual void Connect() = 0;

	virtual void Disconnect() = 0;

	



};

