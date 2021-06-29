
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

	 bool  Composite::MoveRelativXYZ(int stage, double position)
	{
		for (int i = 0; i < children.size(); i++)
		{
			Component* p1 = children.at(i);

			if (p1->GetStage() == stage)
			{
				p1->MoveRelativXYZ(stage, position);
				break;
			}
		}

		return true;

	}


	 bool Composite::MoveAbsolutXYZ(int stage, double position)
	{
		if (stage != 0 && stage != 1 && stage != 2 && stage != 3)
		{
			printf("The selected stage doesn't Exist. Please select a stage between 0, 1 ,2 and 3\n ");

			return false;
		}
		else
		{
			for (int i = 0; i < children.size(); i++)
			{
				Component* p1 = children.at(i);

				if (p1->GetStage() == stage)
				{
					p1->MoveAbsolutXYZ(stage, position);
					break;
				}
			}

			return true;
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
