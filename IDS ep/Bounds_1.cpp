/*
#include <ilcplex/ilocplex.h>
#include <ilconcert/ilotupleset.h>
#include<tuple>
#include <list>
#include <vector>
#include <math.h>
#include <algorithm>
#include <ilopl/iloopl.h>
ILOSTLBEGIN

// tuple(LB,UB,Ln)

tuple<double,double,IloNumArray, vector<double>> LBound1(vector<vector<int>> &Xs, double &UB, IloNumArray &I, IloNum &T, IloNumArray &t, IloNum &n, IloNum &k, int &d, int &comp){
	ofstream fb_out("B_" + to_string(d) + ".txt");
	fb_out << endl << "----------------------------------------- Instance " << d << "---------------------------------------------------------" << endl << endl;
	fb_out << comp << "	";
		IloEnv env;
		double R = 0.016;
		double C = 1;
		vector<double> B(k);
		vector<double> Lnn(T + 1);
	
		tuple<double, double, IloNumArray, vector<double>> tpBS;

		ofstream f_out("test_fr.sol");
		///////////////////////////////////////////////// (exp(a)+1)*exp(b)
		for (int m = 0; m < k; m++)
		{
			int Sum_I = 0;
			int Sum_tr = 0;
			for(int j=0;j<n;j++){

				Sum_I += Xs[j][m] * I[j];
				Sum_tr +=(Xs[j][m] * t[j]);
			}

			//B[m] = (exp(-(C*Sum_I))-1)*exp(-R*(T- Sum_tr));
			B[m] = (exp(-(C*Sum_I)) - 1)*exp(-R*(T - Sum_tr));
			fb_out << 100*(1+B[m]) << "  ";
			//cout << endl << "--------------------- SOLUTION  ---------------------------" << endl;
			//cout <<"      "<< 100 * (1 + B[m]) <<endl;
			//cout << endl << "------------------------------------------------" << endl;
		}

		//cout << endl;

		//min_element(B.begin(), B.end());
		int min_pos = distance(B.begin(), min_element(B.begin(), B.end()));
		double LB;
		double LB_target;
		double delta;

		LB = B[min_pos];
		//cout << endl << "--------------------- SOLUTION  ---------------------------" << endl;
		//cout <<"      "<< 100 * (1 + B[min_pos]) <<endl;
		//f_out << "      " << 100 * (1 + B[min_pos]) << endl;
		//cout << endl << "------------------------------------------------" << endl;
		delta = (UB - LB) / 2;
		LB_target = LB + delta;



		///////////////////////////// return LB
		//prBR.first= LB;
		get<0>(tpBS) = LB;
		get<1>(tpBS) = UB;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		double expr = 0;
		for (int j = 0; j < n; j++)
		{
			expr += C*I[j];
			//expr += I[j];
		}
		IloNumArray Ln(env, T + 1);
		for (int l = 0; l <= T; l++)
		{
			if(0<(LB_target*exp(R*l)+1)){
			Ln[l] = -log(LB_target*exp(R*l) + 1);
			}
			else if(0>=(LB_target*exp(R*l) + 1)){
				Ln[l] = expr;
			}
		}

		///////////////////////////// return Ln
		get<2>(tpBS) = Ln;
		get<3>(tpBS) = B;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		return(tpBS);

	env.end();
	fb_out << endl << "----------------------------- END of instance " << d << "------------------------------------------------------------------" << endl;

}
*/