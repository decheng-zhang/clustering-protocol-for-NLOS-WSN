#ifndef _GAPROBLEM_H_
#define _GAPROBLEM_H_

#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <list>
#include <Problem.h>
#include <RealSolutionType.h>
#include <BinaryRealSolutionType.h>
#include <Solution.h>
#include <XReal.h>

using namespace std;

struct nodeInfo
{
	int id;
	double x;
	double y;
	double energy;
	vector<nodeInfo> members;
};

struct ID_Comparer
{
    bool operator() (const nodeInfo& data1, const nodeInfo& data2) const { return ( data1.id < data2.id );   }
};

class GAProblem : public Problem
{
	int networkSize;
	int e_networkSize;
	vector<nodeInfo> candidates;
	vector<nodeInfo> networkInf;
	vector <nodeInfo> eligibleNodes; 
	ofstream myfile;
	
	public: 
 	GAProblem();
	GAProblem(double numOfCHs,vector<nodeInfo> networkInfo,vector<nodeInfo> e_Nodes);
	~GAProblem();
	vector<nodeInfo> clusterTheNetwork(vector<nodeInfo> candidates);
	double getTotalDistance();
  	void evaluate(Solution *solution);
};

#endif
