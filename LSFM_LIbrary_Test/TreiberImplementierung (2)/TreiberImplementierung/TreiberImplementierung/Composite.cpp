
#include "Composite.h"


void  Composite::add(Component* component)
{

	ChildIterator itr = find(children.begin(), children.end(), component);
	if (itr == children.end())
	{
		children.push_back(component);
	}
}


	void Composite::remove(Component* component) 
	{

		ChildIterator itr = find(children.begin(), children.end(), component);
		if (itr != children.end())
		{

			children.erase(itr);
			printf("Component successfull Remove");
		}
	}



	void Composite::Connect()
	{
		for (int i = 0; i < children.size(); i++)


			children[i]->Connect();
	}


	void Composite::Disconnect()
	{
		for (int i = 0; i < children.size(); i++)

			children[i]->Disconnect();
	}
