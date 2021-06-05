#pragma once
class Poisson
{
private:
	double lambda;

public:
	Poisson();
	Poisson(double lambda);
	double mean();
	void setMean(Poisson dist);
	void pdf(Poisson dist, double k);
};

