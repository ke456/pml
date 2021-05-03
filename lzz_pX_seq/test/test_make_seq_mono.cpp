#include <iostream>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "lzz_pX_seq.h"

NTL_CLIENT
using namespace std;
int main(){
	bool verbose = false;
	long p = 9001;
	zz_p::init(p);
	
	int vec_size;
	if (verbose)
		cout << "enter vector size" << endl;
  cin >> vec_size;
	if (verbose)
		cout << "enter the y-degrees, order by x degrees" << endl;
	
	vector<int> degrees;
	string line;
	getline(cin,line);
	getline(cin,line);
	istringstream iss{line};
	int c;
	while (iss >> c){
		degrees.emplace_back(c);
	}
	int e = degrees[0];
	int d = degrees.size();
	
	vector<vector<vector<zz_p>>> coeffs;
	for (long t = 0; t < vec_size; t++){
	  coeffs.emplace_back(vector<vector<zz_p>>());
		for (long i = 0; i < 2*e; i++)
			coeffs[t].emplace_back(vector<zz_p>(d, zz_p(0)));
	}

	for (long i = 0; i < d; i++){
		for (long t = 0; t < vec_size; t++){
			for (long j = 0; j < degrees[i]; j++){
				coeffs[t][j][d-i-1] = random_zz_p();
			}
		}
	}

	cout << d << endl;
	cout << 2*e << endl;
	cout << vec_size << endl;

	for (long i = 0; i < 2*e; i++){
		for (long t = 0; t < vec_size; t++){
			cout << "[";
			for (long j = 0; j < d; j++){
				cout << " " << coeffs[t][i][j];
			}
			cout << "]" << endl;
		}
	}
}















