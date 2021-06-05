#include "Poisson.h"

Poisson::Poisson()
{
	lambda = 1;
}

Poisson::Poisson(double lambda)
{
	this->lambda = lambda;
}

double Poisson::mean()
{
	return lambda;
}