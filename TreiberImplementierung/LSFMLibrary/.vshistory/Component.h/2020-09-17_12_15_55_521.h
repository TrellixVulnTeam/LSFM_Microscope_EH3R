#pragma once
class Component
{
protected:
	int controllerStage; // Variable to define which stage  belongs to each controller. 
public:

	virtual void add(Component* component) = 0;      // add a leaf or a Composite to a Component 

	virtual void remove(Component* component) = 0;    // remove a leaf or a copmosite to  a Component 

	virtual void Connect() = 0;

	virtual void Disconnect() = 0;

	virtual bool MoveRelativXYZ(int stage, double position) = 0;   // Relativ move of a Controller to a specified Position

	virtual bool MoveAbsolutXYZ(int stage, double position) = 0; // Absolut move of a COntroller to a specified Position

	int GetStage() { return controllerStage; }
};

