//codey by Chen Zhen, 2014/10/17, Beihang University
//使用了二维动态数组

#include<iostream>
#include<math.h>
#include<iomanip>
using namespace std;

void Gauss(double **a, double *b, int);//顺序Gauss消去法
void lie_Gauss(double **a, double *b, int);//列主元素Gauss消去法
void Doolittle(double **a, double *b, int);//Doolittle分解法
void print(double **a, double *b, int);//打印方程

void main()
{
	//cout.setf(ios::fixed);
	cout.precision(4);//设置四位有效数字

	//输入矩阵A和b
	int n;
	cout << "请输入系数矩阵的行数(或列数，列数等于行数):\n";
	cin >> n;
	cin.clear();//这3行代码重置输入
	while (cin.get() != '\n')
		continue;
	cout << "请输入系数矩阵A:\n";

	//要用到二维动态数组
	double **a;
	a = new double *[n];  //申请行的空间
	for (int i = 0; i < n; i++)
		a[i] = new double[n];
	for (int i = 0; i < n; i++)
	{
		cout << "第 " << i + 1 << " 行:";
		for (int j = 0; j < n; j++)
			while (!(cin >> a[i][j]))
			{
				cin.clear();
				while (cin.get() != '\n')
					continue;
				cout << "请输入数字";
			}
		cin.clear();
		while (cin.get() != '\n')
			continue;
	}
	cout << "请输入矩阵b:\n";
	double *b = new  double[n];
	for (int j = 0; j < n; j++)
		cin >> b[j];
	cin.clear();
	while (cin.get() != '\n')
		continue;

	//将系数矩阵A与矩阵b存储在临时矩阵中，临时矩阵中的元素参与行列变换
	double **temp_a = new double *[n];
	double *temp_b = new double[n];
	for (int i = 0; i < n; i++)
		temp_a[i] = new double[n];
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			temp_a[i][j] = a[i][j];
	for (int i = 0; i < n; i++)
		temp_b[i] = b[i];

	//打印方程
	print((double **)temp_a, temp_b, n);

	//选择解方程方式
	cout << "请输入你选择的解方程方式(数字):\n";
	cout << "*************************\n";
	cout << "1.顺序Gauss法\n2.列主元素高斯法\n";
	cout << "3.Doolittle分解法\n";
	cout << "*************************\n";
	int choice;
	while (cin >> choice)
	{
		switch (choice)
		{
		case 1: Gauss((double **)temp_a, temp_b, n);
			break;
		case 2: lie_Gauss((double **)temp_a, temp_b, n);
			break;
		case 3:	Doolittle((double **)temp_a, temp_b, n);
			break;
		case 0: return;
		}
		cout << "请输入你选择的解方程方式(1-3的数),输入0退出:\n";
	}

	for (int i = 0; i < n; i++)
		delete[]temp_a[i];
	delete[]temp_a;
	delete[]temp_b;
	for (int i = 0; i < n; i++)
		delete[]a[i];
	delete[]a;
	delete[]b;
}

void Gauss(double **a, double *b, int n)
{
	double mm;
	double *jie = new double[n];
	for (int k = 1; k < n; k++)
	{
		if (a[k - 1][k - 1] == 0)
		{
			cout << "算法失效" << endl;
			return;
		}
		for (int i = k + 1; i < n + 1; i++)
		{
			mm = a[i - 1][k - 1] / a[k - 1][k - 1];
			for (int j = k + 1; j < n + 1; j++)
			{
				a[i - 1][j - 1] = a[i - 1][j - 1] - mm * a[k - 1][j - 1];

			}
			b[i - 1] = b[i - 1] - mm * b[k - 1];
			for (int j = 1; j < k + 1; j++)
				a[i - 1][j - 1] = 0;
		}
		//print((double**)a,b,n);
	}
	if (a[n - 1][n - 1] == 0)
	{
		cout << "算法失效" << endl;
		return;
	}
	cout << "使用顺序Gauss消去法的解为：\n";
	jie[n - 1] = b[n - 1] / a[n - 1][n - 1];
	for (int k = n - 1; k > 0; k--)
	{
		double temp = 0;
		for (int j = k + 1; j < n + 1; j++)
			temp += a[k - 1][j - 1] * jie[j - 1];
		jie[k - 1] = (b[k - 1] - temp) / a[k - 1][k - 1];
	}
	for (int i = 0; i < n - 1; i++)
		cout << "x" << i + 1 << " = " << jie[i] << endl;
	cout << "x" << n << " = " << jie[n - 1] << endl;
	cout << endl;
	delete[]jie;
}

void lie_Gauss(double **a, double *b, int n)
{
	for (int k = 1; k < n; k++)
	{
		double max = abs(a[k - 1][k - 1]);
		int lie_hao = k;
		for (int i = k; i < n + 1; i++)
			if (abs(a[i - 1][k - 1]) > max)
			{
				max = abs(a[i - 1][k - 1]);
				lie_hao = i;
			}
		for (int j = k; j < n + 1; j++)
		{
			double temp = a[k - 1][j - 1];
			a[k - 1][j - 1] = a[lie_hao - 1][j - 1];
			a[lie_hao - 1][j - 1] = temp;
		}
		for (int i = k + 1; i < n + 1; i++)
		{
			double mm = a[i - 1][k - 1] / a[k - 1][k - 1];
			for (int j = k + 1; j < n + 1; j++)
			{
				a[i - 1][j - 1] = a[i - 1][j - 1] - mm * a[k - 1][j - 1];

			}
			b[i - 1] = b[i - 1] - mm * b[k - 1];
			for (int j = 1; j < k + 1; j++)
				a[i - 1][j - 1] = 0;
		}
	}
	cout << "使用列主元素Gauss法的解为：\n";
	double *jie = new double[n];
	jie[n - 1] = b[n - 1] / a[n - 1][n - 1];
	for (int k = n - 1; k > 0; k--)
	{
		double temp = 0;
		for (int j = k + 1; j < n + 1; j++)
			temp += a[k - 1][j - 1] * jie[j - 1];
		jie[k - 1] = (b[k - 1] - temp) / a[k - 1][k - 1];
	}
	for (int i = 0; i < n - 1; i++)
		cout << "x" << i + 1 << " = " << jie[i] << endl;
	cout << "x" << n << " = " << jie[n - 1] << endl;
	cout << endl;
	delete[]jie;
}

void Doolittle(double **a, double *b, int n)
{
	double **u = new double *[n];
	for (int i = 0; i < n; i++)
		u[i] = new double[n];
	double **l = new double *[n];
	for (int i = 0; i < n; i++)
		l[i] = new double[n];
	double *jie_x = new double[n];
	double *jie_y = new double[n];
	for (int k = 1; k < n + 1; k++)
	{
		for (int j = k, i = k + 1; j < n + 1, i < n + 1; j++, i++)
		{
			double sum_1 = 0;
			for (int t = 1; t < k; t++)
			{
				sum_1 += l[k - 1][t - 1] * u[t - 1][j - 1];
			}
			u[k - 1][j - 1] = a[k - 1][j - 1] - sum_1;
			double sum_2 = 0;
			for (int t = 1; t < k; t++)
			{
				sum_2 += l[i - 1][t - 1] * u[t - 1][k - 1];
			}
			l[i - 1][k - 1] = (a[i - 1][k - 1] - sum_2) / u[k - 1][k - 1];
		}
		double sum_1 = 0;
		for (int t = 1; t < k; t++)
		{
			sum_1 += l[k - 1][t - 1] * u[t - 1][n - 1];
		}
		u[k - 1][n - 1] = a[k - 1][n - 1] - sum_1;
	}
	jie_y[0] = b[0];
	for (int i = 2; i < n + 1; i++)
	{
		double sum = 0;
		for (int t = 1; t < i; t++)
			sum += l[i - 1][t - 1] * jie_y[t - 1];
		jie_y[i - 1] = b[i - 1] - sum;
	}
	jie_x[n - 1] = jie_y[n - 1] / u[n - 1][n - 1];
	for (int i = n - 1; i > 0; i--)
	{
		double sum = 0;
		for (int t = i + 1; t < n + 1; t++)
			sum += u[i - 1][t - 1] * jie_x[t - 1];
		jie_x[i - 1] = (jie_y[i - 1] - sum) / u[i - 1][i - 1];
	}
	cout << "使用Doolittle分解法的解为\n";
	for (int i = 0; i < n - 1; i++)
		cout << "x" << i + 1 << " = " << jie_x[i] << endl;
	cout << "x" << n << " = " << jie_x[n - 1] << endl;
	cout << endl;

	delete[]jie_x;
	delete[]jie_y;
	for (int i = 0; i < n; i++)
		delete[]u[i];
	delete[]u;
	for (int i = 0; i < n; i++)
		delete[]l[i];
	delete[]l;
}

void print(double **a, double *b, int n)
{
	cout << "方程为:\n";
	cout << "====================================================\n";
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n - 1; j++)
		{
			cout << a[i][j] << " x" << j + 1 << " + ";
		}
		cout << a[i][n - 1] << " x" << n << " = " << b[i] << endl;
	}
	cout << "====================================================\n";
	cout << endl;
}