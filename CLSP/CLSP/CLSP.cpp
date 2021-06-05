#include <iostream>
#include <boost/math/distributions/poisson.hpp>
#include <vector>
#include <algorithm>
#include "State.h"
#include <map>
#include < unordered_map >
#include <ctime>


using namespace std;
using boost::math::poisson;

// global variables
double truncationQuantile = 0.9999;  // 置信度稍微改一下，泊松分布特殊
double stepSize = 1;
double minState = -150;
double maxState = 300;  
double discountFactor = 1.0;

double fixedOrderingCost = 100;
double proportionalOrderingCost = 0;
double holdingCost = 1;
double penaltyCost = 10;
int maxOrderQuantity = 100;

double initialInventory = 0;
double meanDemand[] = {50, 50, 50};

const int T = size(meanDemand);
poisson dists[T];

//class MyHash
//{
//public:
//	size_t operator()(State &s)
//	{
//		size_t h1 = hash<int>{}(s.period);
//		size_t h2 = hash<double>{}(s.initialInventory);
//		return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion)
//	}
//};
//
//class MyComparator
//{
//public:
//	bool operator( )(State o1, State o2) const {
//		if (o1.period < o2.period)
//			return true;
//		else
//		{
//			if (o1.initialInventory < o2.initialInventory)
//				return true;
//			else
//			{
//				return false;
//			}
//		}
//		//return o1.period > o2.period ? true : o1.initialInventory > o2.initialInventory ? true :false;
//		return false;
//		//return o1.period < o2.period || (
//		//	!(o1.period < o1.period) && o1.initialInventory < o2.initialInventory);
//	}
//};

map <State, double> cacheValues;
map <State, double> cacheActions;

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
		vector<double> vect{demand, prob};
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
	map<State, double>::iterator ii = cacheValues.find(state);
	if (ii != cacheValues.end())
		return ii->second;
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
		cacheValues.insert({state, val});
		//cacheValues[state] =  val;
		cacheActions.insert({state, bestOrderQty});
		return val;
	}	
}

int main()
{
	for (int i = 0; i < T; i++)
	{
		dists[i] = poisson(meanDemand[i]);
	}
	
	int period = 1;
	State initialState = State(period, initialInventory);
	
	const clock_t begin_time = clock();
	double finalValue = recursionFunction(initialState);
	float time = float(clock() - begin_time) / 1000;
	cout << "final optimal expected value is: " << finalValue << endl;
	double optQ = cacheActions.find(initialState)->second;
	cout << "optimal order quantity in the first priod is : " << optQ << endl;

	cout << "running time is " << time << " s" << endl;
}



//Poisson dists[T];
//for (int i = 0; i < T; i++) {
//	dists[i] = Poisson(i*10);
//}
//vector<poisson> dists;
//for (int i = 0; i < T; i++) {
//	dists.push_back(poisson(i));
//}

//double mean = 10.0;
//poisson s(mean);
//cout << "mean for this poisson distribution is: " << s.mean() << endl;
//cout << "quantile for mean in  this poisson distribution is: " << initialState.initialInventory << endl;

	//double supportLB[T];
	//double supportUB[T];
	//for (int i = 0; i < T; i++)
	//{
	//	supportLB[i] = quantile(dists[i], 0);
	//	supportUB[i] = quantile(dists[i], 1 - truncationQuantile);
	//}
	//double *** pmf = new double**[T];
	//for (int i = 0; i < T; i++)
	//{
	//	int demandLength = static_cast<int>((supportUB[i] - supportLB[i] + 1) / stepSize);
	//	pmf[i] = new double*[demandLength];
	//	for (int j = 0; j < demandLength; j++)
	//	{
	//		pmf[i][j] = new double[2];
	//		pmf[i][j][0] = supportLB[i] + j * stepSize;
	//		double probilitySum = cdf(dists[i], supportUB[i]) - cdf(dists[i], max(0.0, supportLB[i] - 1));
	//		pmf[i][j][1] = pdf(dists[i], j) / probilitySum;
	//	}
	//}
	//
	//cout << "mean for this poisson distribution is: " << dAndP(1, dists[1])[15][0] << endl;


	//for (int i = 0; i < T; i++)
	//{
	//	int demandLength = (int)((supportUB[i] - supportLB[i] + 1) / stepSize);
	//	for (int j = 0; j < demandLength; j++)
	//	{
	//		delete[] pmf[i][j];
	//	}
	//	delete[] pmf[i];
	//}
	//delete[] pmf;





