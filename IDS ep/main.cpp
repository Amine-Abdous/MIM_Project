
#include <ilcplex/ilocplex.h>
#include <ilconcert/ilotupleset.h>
#include <ilopl/iloopl.h>

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
void Solve_ALDP_Vib(int& d, int& rr);

int main() {

	int d = 12;
	int rr = 2;

	Solve_ALDP_Vib(d,rr);

	system("pause");
	//system("shutdown/l");
	return 0;
}