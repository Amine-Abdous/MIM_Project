#include <ilcplex/ilocplex.h>
#include <ilconcert/ilotupleset.h>
#include<tuple>
#include <list>
#include <time.h> 
#include <algorithm>
#include <ctime>
#include <vector>
#include <ilopl/iloopl.h>

ILOSTLBEGIN

void Solve_ALDP_Vib(int&d,int&rr) {
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
		// Read data file
		ifstream din(to_string(d) + "_" + to_string(rr) + ".dat");
		din >> T >> k >> n >> r;
		din >> t;
		din >> V;

		//Vl = 2.5*2.5 * 28800;
		Vl = 2.5*2.5* 28800;
		nn = floor(28800 / T);
	///////////// Precedence
		//vector <tuple<int, int>> pT(2 * n);
		int p, s;
		char c;
		din >> p;
		int compt = 0;
		vector <tuple<int, int>> pT;
		while (p != -1 && p != EOF) {

			din >> c;

			din >> s;
			pT.push_back(make_tuple(p, s));
			//pT[compt] = make_tuple(p, s);
			//cout << get<0>(pT[compt]) << "," << get<1>(pT[compt]) << endl;
			din >> p;
			//	compt++;
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
		IloNumVar M(env, 0, IloInfinity, ILOINT);
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
	// obj
		IloExpr Sum_BB(env);
		for (int m = 0; m < k; m++) {
			Sum_BB.clear();
			for (int i = 0; i < r; i++) {
				Sum_BB +=  y_ik[i][m];
			}
		mdl.add(Sum_BB <= M);
		}
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
			mdl.add(Sum_V <= Vl/nn);
		}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	IloObjective obj(env, M, IloObjective::Minimize);
		mdl.add(obj);

	// Solve and output solutions to a file
	IloCplex cplex(mdl);
	//cplex.setOut(env.getNullStream()); // free the console from compilation's journal
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cplex.setParam(IloCplex::TiLim, 300); // Time limit

	//cplex.setParam(IloCplex::ClockType, 1);
	clock_t Start_total_d = clock(); // start time
	if (cplex.solve()) {
	clock_t Fin_total_d = clock();

		// total time
	Time_Total_d = (Fin_total_d - Start_total_d) / (double)CLOCKS_PER_SEC;
		//////////////////////////////////////////////////////////////////////////
		vector<bool> X__k(k);
			vector<vector<bool>> X__jk(n, X__k);
			vector<vector<vector<bool>>> X_ijk(r, X__jk);
			cout << "x" << endl;
			for (int j = 0; j < n; j++) {
				for (int m = 0; m < k; m++) {
					for (int i = 0; i < r; i++) {
						X_ijk[i][j][m] = cplex.getValue(x_ijk[i][j][m]);
						cout << X_ijk[i][j][m] << "	";
					}
					cout << endl;
				}
				cout << endl;
			}
		////////////////////////////////////////////////////////////////////////////////////
			vector<bool> Y__k(k);
			vector<vector<bool>> Y_ik(r, Y__k);
			cout << "y" << endl;
	for (IloInt m = 0; m < k; m++) {
			for (IloInt i = 0; i < r; i++) {
				
					Y_ik[i][m] = cplex.getValue(y_ik[i][m]);
					cout << Y_ik[i][m]  << "	";
				}
				cout << endl;
			}
			
		//////////////////////////////////////////////////////////////////////////
	cout << endl;
	cout << d <<"	" <<cplex.getStatus() << "	" << cplex.getObjValue() << endl;

		}
		else {
			//////////////////////////////////////////////////////////////////////////
			// No solution
			cout << "No solution for	" << d << endl;
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