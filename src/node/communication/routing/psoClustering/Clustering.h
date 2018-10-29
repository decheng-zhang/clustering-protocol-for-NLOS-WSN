#ifndef __CLUSTERING__
#define __CLUSTERING__

#include <Problem.h>
#include <cmath>
#include <RealSolutionType.h>
#include <BinaryRealSolutionType.h>
#include <Solution.h>
#include <XReal.h>
#include <stdio.h>
#include <string.h>

using namespace std;

struct NeighborRecord 
{
	int id;
	double rssi;
	double remainingEnergy;
};

struct NodeControlInfo 
{
	int id;	//the ID of the Node
	double rssi;
	double energy;
	int numOfNbrs;
	vector <NeighborRecord> nbrs;
	vector <NodeControlInfo> members;
};

/**
  * @class Clustering
  * @brief Class representing the Clustering Problem
 **/
class Clustering : public Problem 
{
	private:
    	double * x_;
	double * fx_ ;
     	int networkSize;
  	int e_networkSize;
	double clusteredNodes;
	vector<vector<double> > adjacencyMatrix;
  	vector<NodeControlInfo> candidates;
  	vector<NodeControlInfo> eligibleNodes;
  	vector<NodeControlInfo> networkInf;
 	//ofstream //myfile;	
  	
	public:
	Clustering(string solutionType, int numOfCHs, vector<NodeControlInfo> networkInfo, vector<NodeControlInfo> e_Nodes, vector<vector<double> > adjacency);
	~Clustering();
	void evaluate(Solution *solution);
  	void evaluateConstraints(Solution * solution);
  
};

#endif /* __CLUSTERING__ */
