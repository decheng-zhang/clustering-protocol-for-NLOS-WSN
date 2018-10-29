#ifndef _CCPROBLEM_H_
#define _CCPROBLEM_H_

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


class CCProblem : public Problem
{
	int networkSize;
	double totalUnclustered;
	double tCost;
	double totalRSSI;
	int numberOfCHs;
	ofstream myfile;
	int totalClusteredNodes;
	multimap<int,int> network;
	vector <int> clusterHeads;
        string configuration;
	string routes;
        stringstream r_out;
	
	

        public:
	vector <vector<double> > adjacencyMatrix;

	CCProblem(vector< vector<double>> adjacencyM);
		
	~CCProblem();

	void clusterTheNetwork(vector<double> configurtation);
	
	string returnConfiguration(vector<double> configurtation);

	void printNetwork();

	double calculateEnergyRatio();

	double getCompactness();

	double getTreeCost();

	double getPathCost(int CH);

	double dijkstra();
	
	double getCoverageRatio();

	double getNumberOfActiveNodes();

	vector<double> getInitialSolution();

	void evaluate(Solution *solution);

};

#endif
