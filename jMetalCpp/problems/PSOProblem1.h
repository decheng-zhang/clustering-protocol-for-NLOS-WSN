#ifndef __PSOPROBLEM1_H__
#define __PSOPROBLEM1_H__

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
#include <RealSolutionType.h>

class PSOProblem1 : public Problem
{
	int networkSize;
	int numberOfCHs;
	int totalClusteredNodes;
	ofstream myfile;
	multimap<int,int> network;
	vector <int> clusterHeads;
	vector <vector<double> > adjacencyMatrix;
	double totalRSSI_, sclb_, energy_;
	string routes;

        public:

	PSOProblem1(int networkSize_);
	PSOProblem1(vector< vector<double>> adjacencyM);
		
	~PSOProblem1();
	
	void clusterTheNetwork(vector<int> candidates);
	
	string returnConfiguration(vector<int> candidates);

	void printNetwork();

	double dijkstra(vector<int> candidates);

	double calculateEnergyRatio(vector<int> candidates);
	
	double getCompatness(vector<int> candidates);

	double calculateNonclusteredRatio();

	void evaluate(Solution *solution);

};

#endif
