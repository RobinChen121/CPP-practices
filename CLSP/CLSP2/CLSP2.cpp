#include <iostream>
#include <boost/math/distributions/poisson.hpp>
#include <vector>
#include <algorithm>
#include "State.h"
#include <ctime>


using namespace std;
using boost::math::poisson;

// global variables
double truncationQuantile = 0.9999;  // 置信度稍微改一下，泊松分布特殊
double stepSize = 1;
const double minState = -150;
const double maxState = 300;
double discountFactor = 1.0;

double fixedOrderingCost = 100;
double proportionalOrderingCost = 0;
double holdingCost = 1;
double penaltyCost = 10;
int maxOrderQuantity = 100;

double initialInventory = 0;
double meanDemand[] = { 50, 50, 50 };

const int T = size(meanDemand);
poisson dists[T];
double M = 10000.0;


const int periodStateLength = static_cast<int>(maxState - minState + 1);
double cacheValues[T][451];
double cacheActions[T][451];


vector<double> getFeasibleActions(State &s)
{
	vector<double> arr;
	for (double i = 0; i <= maxOrderQuantity; i = i + stepSize)
	{
		arr.push_back(i);
	}
	return arr;
}

vector<vector<double>> dAndP(int period, poisson &dist)
{
	double supportLB = quantile(dist, 0);
	double supportUB = quantile(dist, truncationQuantile);
	int demandLength = static_cast<int>((supportUB - supportLB + 1) / stepSize);
	vector<vector<double>> arr(demandLength);
	double probilitySum = cdf(dist, supportUB) - cdf(dist, max(0.0, supportLB - 1));
	for (int j = 0; j < demandLength; j++)
	{
		double demand = supportLB + j * stepSize;
		double prob = pdf(dist, j) / probilitySum;
		vector<double> vect{ demand, prob };
		arr[j] = vect;
	}
	return arr;
}

State stateTransition(State &s, double action, double randomDemand)
{
	double nextInventory = s.initialInventory + action - randomDemand;
	nextInventory = nextInventory > maxState ? maxState : nextInventory;
	nextInventory = nextInventory < minState ? minState : nextInventory;
	return State(s.period + 1, nextInventory);
}

double immediateValue(State &s, double action, double randomDemand)
{
	double fixedCost = action > 0 ? fixedOrderingCost : 0;
	double variableCost = proportionalOrderingCost * action;
	double inventoryLevel = s.initialInventory + action - randomDemand;
	double holdingCosts = holdingCost * max(inventoryLevel, 0.0);
	double penaltyCosts = penaltyCost * max(-inventoryLevel, 0.0);
	double totalCosts = fixedCost + variableCost + holdingCosts + penaltyCosts;
	return totalCosts;
}

double recursionFunction(State &state)
{
	int tIndex = state.period - 1;
	int inventoryIndex = static_cast<int>(state.initialInventory - minState);
	if (cacheValues[tIndex][inventoryIndex] < M - 1)
		return cacheValues[tIndex][inventoryIndex];
	else
	{
		vector<double> feasibleActions = getFeasibleActions(state);
		int t = state.period - 1;
		vector<vector<double>> thisDandP = dAndP(t, dists[t]);
		vector<double> QValues(feasibleActions.size());

		double val = DBL_MAX;
		double bestOrderQty = 0;
		for (unsigned int i = 0; i < feasibleActions.size(); i++)
		{
			double orderQty = feasibleActions[i];
			double thisQValue = 0;
			for (unsigned int j = 0; j < thisDandP.size(); j++)
			{
				double randomDemand = thisDandP[j][0];
				double thisDValue = immediateValue(state, orderQty, randomDemand);
				double dProb = thisDandP[j][1];
				thisQValue += dProb * thisDValue;
				if (state.period < T)
				{
					State newState = stateTransition(state, orderQty, thisDandP[j][0]);
					double newValue = recursionFunction(newState);
					thisQValue += thisDandP[j][1] * discountFactor * newValue;
				}
			}

			QValues[i] = thisQValue;
			if (QValues[i] < val)
			{
				val = QValues[i];
				bestOrderQty = orderQty;
			}
		}
		cacheValues[tIndex][inventoryIndex] = val;
		cacheActions[tIndex][inventoryIndex] =  bestOrderQty;
		return val;
	}
}

int main()
{
	for (int i = 0; i < T; i++)
	{
		dists[i] = poisson(meanDemand[i]);
	}

	for (int t = 0; t < T; t++)
	{
		for (int j = 0; j < periodStateLength; j++)
		{
			cacheValues[t][j] = M;
			cacheActions[t][j] = M;
		}
	}

	int period = 1;
	State initialState = State(period, initialInventory);

	const clock_t begin_time = clock();
	double finalValue = recursionFunction(initialState);
	float time = float(clock() - begin_time) / 1000;
	cout << "final optimal expected value is: " << finalValue << endl;
	int tIndex = initialState.period - 1;
	int inventoryIndex = static_cast<int>(initialState.initialInventory - minState);
	double optQ = cacheActions[tIndex][inventoryIndex];
	cout << "optimal order quantity in the first priod is : " << optQ << endl;

	cout << "running time is " << time << " s" << endl;
}
