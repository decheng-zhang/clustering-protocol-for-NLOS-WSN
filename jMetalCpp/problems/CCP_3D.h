#ifndef _CCP_3D_H_
#define _CCP_3D_H_

#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <Problem.h>
#include <RealSolutionType.h>
#include <BinaryRealSolutionType.h>
#include <Solution.h>
#include <queue>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <fstream>
#include <Problem.h>
#include <Solution.h>
#include <random>
#include <RealSolutionType.h>
#include <boost/tokenizer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <string>

struct SensorInfo
{
	int id;
	double x;
	double y;
	double z;
	double energy;
	
};

class CCP_3D : public Problem
{
	int networkSize;
	int numberOfCells;
		
	ofstream myfile;

	int uncover;
	vector<int> sleepNodes;
	vector<int> clusterHeads;
	vector<int> configuration;

	multimap<int,int> network;

	vector <SensorInfo> Sensors;
	vector<vector<double> > coverageMatrix;
	vector <vector<double> > adjacencyMatrix;
	vector <vector<double> > visibilityMatrix;
	
        public:
	

	CCP_3D(vector< vector<double>> coverageM, vector< vector<double>> adjacencyM, vector< vector<double>> visibility, vector<SensorInfo> sensors);
		
	~CCP_3D();

	int clusterTheNetwork();

	double getCompactness();
	
	double evaluateRedunduncy();

	void evaluate(Solution *solution);

};

#endif
