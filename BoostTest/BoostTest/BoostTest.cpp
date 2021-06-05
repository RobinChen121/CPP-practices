#include <boost/math/distributions/normal.hpp> // for normal_distribution
using boost::math::normal; // typedef provides default type is double.

#include <iostream>
using std::cout; using std::endl; using std::left; using std::showpoint; using std::noshowpoint;
#include <iomanip>
using std::setw; using std::setprecision;
#include <limits>
using std::numeric_limits;

int main()
{
	cout << "Example: Normal distribution, Miscellaneous Applications.\n";

	double step = 1.; // in z 
	double range = 4; // min and max z = -range to +range.
	int precision = 17; // traditional tables are only computed to much lower precision.

	// Construct a standard normal distribution s
	normal s; // (default mean = zero, and standard deviation = unity)
	cout << "Standard normal distribution, mean = " << s.mean()
		<< ", standard deviation = " << s.standard_deviation() << endl;

	cout << "cdf is " << cdf(s, 0) << endl;

}