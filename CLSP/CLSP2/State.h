#pragma once

class State
{
private:

public:
	double initialInventory;
	int period;
	State(int period, double initialInventory)
	{
		this->period = period;
		this->initialInventory = initialInventory;
	}
};


