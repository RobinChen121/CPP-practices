#pragma once
#include <map>
#include < unordered_map >

using namespace std;

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

	size_t operator()(State &s)
	{
		size_t h1 = hash<int>{}(s.period);
		size_t h2 = hash<double>{}(s.initialInventory);
		return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion)

	}

	// if inside a class, need be friend
	friend bool operator < (const State o1, const State o2) 
	{
		if (o1.period < o2.period)
			return true;
		else if (o1.period > o2.period)
			return false;
		else
		{
			if (o1.initialInventory < o2.initialInventory)
				return true;
			else
				return false;
		}
	}


	
};

