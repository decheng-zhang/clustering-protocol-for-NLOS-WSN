#ifndef __COVERAGEPROBLEM_H__
#define __COVERAGEPROBLEM_H__

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
#include <BinarySolutionType.h>
#include <XReal.h>

class CoverageProblem : public Problem
{
	double networkSize;
	double numOfCells;
	int uncover;
	double maxRedunduncy;
	ofstream myfile;
	vector<int> configuration;
	vector<double> energyVector;
        vector<int> sleepNodes;
	vector <vector<double> > coverageMatrix;
      	double totalRSSI_, sclb_, energy_;
	string routes;

        public:

	CoverageProblem (int networkSize, int numberOfCells, vector< vector<double>> coverageMatrix, vector< double> energyValues);
		
	~CoverageProblem();
	
	string returnConfiguration(vector<int> candidates);
     
	double computeCoverageEffect();

	void printNetwork();

	double calculateNumberofActiveNodes();
	
	double calculateCoverageEffect();

	void evaluate(Solution *solution);

};

#endif
