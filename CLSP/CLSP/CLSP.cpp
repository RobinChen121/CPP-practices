#include <iostream>
using namespace std;

int main()
{
	double initialInventory = 0;
	double meanDemand[] = {50, 50};

	double truncationQuantile = 0.9999;  // 置信度稍微改一下，泊松分布特殊
	double stepSize = 1;
	double minState = -150;
	double maxState = 300;
	int T = size(meanDemand);

	double fixedOrderingCost = 100;
	double proportionalOrderingCost = 0;
	double holdingCost = 1;
	double penaltyCost = 10;

	int maxOrderQuantity = 200;

	printf("%d", T);
}
