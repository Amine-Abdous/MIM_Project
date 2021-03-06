#include <ilcplex/ilocplex.h>
#include <ilconcert/ilotupleset.h>
#include<tuple>
#include <list>
#include <time.h> 
#include <algorithm>
#include <ctime>
#include <vector>
#include <ilopl/iloopl.h>
#include <math.h>

ILOSTLBEGIN

void Solve_ALDP_Vib(int&d,int&rr, int&pp) {
	ofstream c_out(to_string(d) +"_"+to_string(pp)+"_Solution_Vibration.sol");
	IloEnv env;
	try {
		double Time_Total_d;

		IloArray<IloNumArray> t(env);
		IloNum T; // takt time
		IloNum r; // nb of equipment
		IloNum k; // number of workstation
		IloNum n; // number of tasks
		IloArray<IloNumArray> V(env); // tasks Vibration
		IloNum Vl; // Vibration limit
		IloNum nn; // nb de cycle
		IloNumArray C(env); // cost of equipment
		// Read data file
		ifstream din(to_string(d) + "_" + to_string(rr) + ".dat");
		din >> T >> k >> n >> r;
		din >> t;
		din >> V;
		din >> C;

		if(pp==0){Vl = 2.5*2.5 * 28800;}
		if (pp == 1) { Vl = 30*30 * 28800; }

		//nn = floor(28800 / T);
		nn = floor(28800 / T)+1;
		cout << "nn=" << nn << endl;
		Vl = Vl/nn;
		//cout <<"Limit vib:	"<< Vl << endl;
	///////////// Precedence
		int p, s;
		char c;
		din >> p;
		int compt = 0;
		vector <tuple<int, int>> pT;
		while (p != -1 && p != EOF) {
			din >> c;
			din >> s;
			pT.push_back(make_tuple(p, s));
			din >> p;
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Create model
		IloModel mdl(env);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Define variables
		// x_ijk
		IloArray<IloArray<IloBoolVarArray>> x_ijk(env, r);
		for (int i = 0; i < r; i++) {
			x_ijk[i] = IloArray<IloBoolVarArray>(env, n);
			for (int j = 0; j < n; j++) {
				x_ijk[i][j] = IloBoolVarArray(env, k);
			}
		}
		// y_ik
		IloArray<IloBoolVarArray> y_ik(env, r);
		for (int i = 0; i < r; i++)
		{
			y_ik[i] = IloBoolVarArray(env, k);
		}

		// Obj
		IloExpr objf(env);
		for (int m = 0; m < k; m++) {
			for (int i = 0; i < r; i++) {
				objf += C[i]*y_ik[i][m];
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  all tasks should be assigned
		IloExpr Sum_jj(env);
		//Sum.setName("c1");
		for (int j = 0; j < n; j++) {
			Sum_jj.clear(); // clear the current
			for (int i = 0; i < r; i++) {
				for (int m = 0; m < k; m++) {
					Sum_jj += x_ijk[i][j][m];
				}
			}
			mdl.add(Sum_jj == 1);
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// One equipment per wk
/*		IloExpr Sum_eq(env);
		//Sum.setName("c1");
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < n; j++) {
			Sum_eq.clear(); // clear the current	
		for (int m = 0; m < k; m++) {	
				Sum_eq += x_ijk[i][j][m];
			}
				mdl.add(Sum_eq == 1);
			}
		}*/
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// link between x and y
		//IloExpr Sum_takt(env);
		//Sum_takt.clear();
		for (int m = 0; m < k; m++) {
			for (int i = 0; i < r; i++) {
				for (int j = 0; j < n; j++) {
					mdl.add(x_ijk[i][j][m] <= y_ik[i][m]);
				}	
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// respect of takt time 
		IloExpr Sum_takt(env);
		for (int m = 0; m < k; m++) {
			Sum_takt.clear();
			for (int i = 0; i < r; i++) {	
				for (int j = 0; j < n; j++) {
					Sum_takt += (x_ijk[i][j][m] * t[i][j]);
				}
			}
			mdl.add(Sum_takt <= T);
		}
		/////////////// Precedences constraints (g,h) g is the predecessor of h ////////////////////////////////////////////////////
		int g, h;
		IloExpr Sum_g(env);
		IloExpr Sum_h(env);

		for (int p = 0; p < pT.size(); p++) {
			Sum_g.clear();
			Sum_h.clear();
			g = get<0>(pT[p]);
			h = get<1>(pT[p]);
			for (int m = 0; m < k; m++) {
				for (int i = 0; i < r; i++) {
					Sum_h += ((m + 1)*x_ijk[i][h - 1][m]);
					Sum_g += ((m + 1)*x_ijk[i][g - 1][m]);
				}
			}
			mdl.add(Sum_g <= Sum_h);
		}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	// obj
		IloExpr Sum_BB(env);
		for (int m = 0; m < k; m++) {
			Sum_BB.clear();
			for (int i = 0; i < r; i++) {
				Sum_BB +=  y_ik[i][m];
			}
		mdl.add(Sum_BB <= M);
		}*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// respect of viration
		IloExpr Sum_V(env);
		for (int m = 0; m < k; m++) {
			Sum_V.clear();
			for (int i = 0; i < r; i++) {
				for (int j = 0; j < n; j++) {
					Sum_V += (x_ijk[i][j][m] * V[i][j]* t[i][j]);
				}
			}
			mdl.add(Sum_V <= Vl);
		}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	IloObjective obj(env, objf, IloObjective::Minimize);
	mdl.add(obj);

	// Solve and output solutions to a file
	IloCplex cplex(mdl);
	//cplex.setOut(env.getNullStream()); // free the console from compilation's journal
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//cplex.setParam(IloCplex::TiLim, 300); // Time limit
	cplex.setParam(IloCplex::Param::ClockType,2); // Time param

	//cplex.setParam(IloCplex::ClockType, 1);
	clock_t Start_total_d = clock(); // start time
if (cplex.solve()) {
	clock_t Fin_total_d = clock();

		// total time
	Time_Total_d = (Fin_total_d - Start_total_d) / (double)CLOCKS_PER_SEC;
		//////////////////////////////////////////////////////////////////////////
/*	vector<vector<double>> Vib;
	vector<double> V1;
	int min_pos;
		for (int i = 0; i < V.getSize(); i++) {
		for (int m = 0; m < V[i].getSize(); m++) {
			V1.push_back(V[i][m]);
			c_out << V[i][m] << "	";

		}
		min_pos = distance(V1.begin(), min_element(V1.begin(), V1.end()));
		Vib.push_back(V1);
		c_out << endl;
	}
	//int min_pos = distance(Vib.begin(), min_element(Vib.begin(), Vib.end()));
	cout << "Min Vib	" << Vib[min_pos]<<endl;
	*/
	
	vector<bool> X__k(k);
			vector<vector<bool>> X__jk(n, X__k);
			vector<vector<vector<bool>>> X_ijk(r, X__jk);
			cout << "x" << endl;
			c_out << "x" << endl;
			for (int j = 0; j < n; j++) {
				for (int m = 0; m < k; m++) {
					for (int i = 0; i < r; i++) {
						X_ijk[i][j][m] = cplex.getValue(x_ijk[i][j][m]);
						cout << X_ijk[i][j][m] << "	";
						c_out << X_ijk[i][j][m] << "	";
					}
					cout << endl;
					c_out << endl;
				}
				cout << endl;
				c_out << endl;
			}
		////////////////////////////////////////////////////////////////////////////////////
			vector<bool> Y__k(k);
			vector<vector<bool>> Y_ik(r, Y__k);
			cout << "y" << endl;
			c_out << "y" << endl;
	for (IloInt m = 0; m < k; m++) {
			for (IloInt i = 0; i < r; i++) {
				
					Y_ik[i][m] = cplex.getValue(y_ik[i][m]);
					cout << Y_ik[i][m]  << "	";
					c_out << Y_ik[i][m] << "	";
				}
				cout << endl;
				c_out << endl;
			}
			
		//////////////////////////////////////////////////////////////////////////
	cout << endl;
	cout << d <<"	" <<cplex.getStatus() << "Total Cost(obj): " << cplex.getObjValue() << endl;
	
	c_out << endl;
	c_out << d << "	" << cplex.getStatus() << "Total Cost(obj): " << cplex.getObjValue() << endl;

	vector<double> Vib_m;
	vector<tuple<int,int>> Tool_m;
	double sum_Vi = 0;
	double cte;
	cte = (1.0/28800.0) *nn;
	//cout << "cte:	"<< cte<< endl;
	for (int m=0;m<k;m++){
		for (int i=0;i<r;i++){
			if (Y_ik[i][m] == 1) { Tool_m.push_back(make_tuple(m + 1, i + 1)); }
		}
	}
	double sum_f;
	for (int m = 0; m < k; m++) {
		sum_Vi = 0;
		sum_f = 0;
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < n; j++) {
				sum_Vi += X_ijk[i][j][m] * V[i][j] * t[i][j];
			}
		}
		sum_Vi = sum_Vi*cte;
		sum_f = sqrtl(sum_Vi);
		//cout << sum_Vi <<"	"<< sum_f << endl;
		Vib_m.push_back(sum_f);
	}
	// time in each workstation
	vector<int>time_k;
	int sum_time;
	for (int m = 0; m < k; m++) {
		sum_time = 0;
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < n; j++) {
				sum_time += X_ijk[i][j][m] * t[i][j];
			}
		}
		time_k.push_back(sum_time);
	}
	//******************************************************************************************
	cout << "vibration in each wk: " << endl;
	c_out << "vibration in each wk: " << endl;

	for (int m = 0; m < k; m++) {
		cout << m + 1 << "	" << Vib_m[m] << endl;
		c_out << m + 1 << "	" << Vib_m[m] << endl;
	}
	//******************************************************************************************
	cout << "Tools in each wk: " << endl;
	c_out << "Tools in each wk: " << endl;
	for (int m = 0; m < Tool_m.size(); m++) {
		for (int i = 0; i < k; i++) {
			if(get<0>(Tool_m[m])== i){
			cout << i+1 << "," << get<1>(Tool_m[m]) << endl;
			}
		}

	}
	//******************************************************************************************
	cout << "Time: " << endl;
	c_out << "Time: " << endl;
			for (int m = 0; m < k; m++) {
				cout << m + 1 << "	" << time_k[m] <<  endl;
				c_out << m + 1 << "	" << time_k[m] << endl;
			}
}
		else {
			//////////////////////////////////////////////////////////////////////////
			// No solution
			cout << "No solution for	" << d << endl;
			c_out << "No solution for	" << d << endl;
		}
	}

	// Error handling
	catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}
	catch (...) {
		cerr << "Error" << endl;
	}

	// End environment
	env.end();

}