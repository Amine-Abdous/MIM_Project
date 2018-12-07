#include <ilcplex/ilocplex.h>
#include <ilconcert/ilotupleset.h>
#include<tuple>
#include <list>
#include <vector>
#include <ilopl/iloopl.h>
#include <cstdlib> 
ILOSTLBEGIN


void dat(int &d, int&r) {
	srand(time(0)); // tunes the rand() function with the current time 
	ifstream instIN2(to_string(d) + ".IN2");
	ifstream instF(to_string(d) + ".txt");

	// checking that the file exists
	if (!instIN2.is_open()) {
		cerr << "the instance IN2 file does not exists" << endl;
	}

	if (!instF.is_open()) {
		cerr << "the txt file (FORCE) does not exists" << endl;
	}

	int T, k, n;

	instIN2 >> T;
	instIN2 >> k;
	instIN2 >> n;

	vector<int> T_i(n);
	vector<vector<int>> T_ij(r, T_i);

	vector<int> t(n);

	vector<double> F_i(n);
	vector<vector<double>> F_ij(r, F_i);
	vector<double> F(n);


	for (int i = 0; i < n; i++)
	{
		instIN2 >> t[i];
	}
	int p, s;
	char c;
	instIN2 >> p;
	int compt = 0;
	vector <tuple<int, int>> pT;
	while (p != -1 && p != EOF) {

		instIN2 >> c;

		instIN2 >> s;
		pT.push_back(make_tuple(p, s));
		//pT[compt] = make_tuple(p, s);
		//cout << get<0>(pT[compt]) << "," << get<1>(pT[compt]) << endl;
		instIN2 >> p;
		//	compt++;
	}
	//pT.resize(compt);

	for (int i = 0; i < n; i++)
	{
		instF >> F[i];
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int j = 0; j < n; j++) {
		T_ij[0][j] = t[j];
		F_ij[0][j] = F[j];
		cout << F_ij[0][j] << "	"<<endl;
	}

	int bias = 1;
	double random;
	double random2;
	double random3;
	for (int i = 1; i < r; i++) {
		random = (rand() % 20);	
		random2 = random / 100;
		//cout << "RANDOM	"<< random << endl;
		for (int j = 0; j < n; j++) {
			random3 = (rand() % 10);
			//cout << "RANDOM3	" << random3 << endl;
			//if (T_ij[0][j] + bias != 0) { T_ij[i][j] = T_ij[0][j] + bias; }
			//else if ((T_ij[0][j] + bias) == 0) { T_ij[i][j] = T_ij[0][j]; }
			T_ij[i][j] = T_ij[0][j] + bias;
		//	if ((bias-5)==(rand()%10)){ F_ij[i][j] = 200000; }
			if (random3 == 7) { F_ij[i][j] = 200000;}
			else if(random3 == 5){ F_ij[0][j] = 200000;}
			else { F_ij[i][j] = F_ij[0][j] + random2; }
	
		}


	}
	
for (int j = 0; j < n; j++) {
	for (int i = 0; i < r; i++) {

			cout  << F_ij[i][j] << "	";

		}
	cout << endl;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// Write .dat file 

	ofstream dout(to_string(d) + "_" + to_string(r) + ".dat");

	dout << T << endl;
	dout << k << endl;
	dout << n << endl;
	dout << r << endl;
	//////////// t
	dout << "[";
	for (int i = 0; i < r; i++) {
		dout << "[";
		for (int j = 0; j < n; j++) {
			dout << T_ij[i][j];
			if (j != n - 1) { dout << ","; }
		}
		if (i == r - 1) { dout << "]]" << endl; }
		else { dout << "]," << endl; }
	}

	//////////////////////////////
	//////////// F
	dout << "[";
	for (int i = 0; i < r; i++) {
		dout << "[";
		for (int j = 0; j < n; j++) {
			dout << F_ij[i][j]* F_ij[i][j];
			if (j != n - 1) { dout << ","; }
		}
		if (i == r - 1) { dout << "]]" << endl; }
		else { dout << "]," << endl; }
	}
/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/////////////// Precedences
	for (int i = 0; i < pT.size(); i++)
	{
		dout << get<0>(pT[i]) << "," << get<1>(pT[i]) << endl;
	}
	dout << -1 << "," << -1;

}
