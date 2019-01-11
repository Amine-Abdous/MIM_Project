
#include <ilcplex/ilocplex.h>
#include <ilconcert/ilotupleset.h>
#include <ilopl/iloopl.h>
#include <math.h>
#include<tuple>
#include <time.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <ctime>
#include <list>
#include <vector>
#include <math.h>
#include <iostream>
#include <algorithm>

ILOSTLBEGIN
void Solve_ALDP_Vib(int&d, int&rr, int&pp);
void dat(int &d, int&r);

int main() {
	clock_t Start_total = clock(); // total time
	int d = 1003;
	int rr = 10;
	//int pp = 1; // without vibration cst
	int pp = 0; // with vibration cst
//******************************************************************************
//dat(d, rr);
//******************************************************************************
	Solve_ALDP_Vib(d,rr,pp);

	double Time_Total_d;
	clock_t Fin_total_d = clock();
	// total time
	Time_Total_d = (Fin_total_d - Start_total) / (double)CLOCKS_PER_SEC;

	cout <<endl<<"Time:	"<< Time_Total_d <<" s"<<endl;
	system("pause");
	//system("shutdown/l");
	return 0;
}