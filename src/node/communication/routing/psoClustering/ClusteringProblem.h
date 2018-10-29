#ifndef _CLUSTERINGPROBLEM_H_
#define _CLUSTERINGPROBLEM_H_

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
#include "PSOClustering.h"

struct ID_Comparer
{
    bool operator() (const NeighborRecord& data1, const NeighborRecord& data2) const { return ( data1.id < data2.id );   }
};

struct ID_Comparer2
{
    bool operator() (const NodeControlInfo& data1, const NodeControlInfo& data2) const { return ( data1.id < data2.id );   }
};



class ClusteringProblem 
{
  	public:
  	size_t dimension;
  	size_t count;
  	int networkSize;
  	int e_networkSize;
	double clusteredNodes;
	vector<vector<double> > adjacencyMatrix;
  	static const int mode = 1; // Clustering Mode
  	static double *l_bounds;
  	static double *u_bounds;
  	static vector<NodeControlInfo> candidates;
  	static vector<NodeControlInfo> eligibleNodes;
  	static vector<NodeControlInfo> networkInf;
 	ofstream myfile;	
  	static int epoch;
  
 	ClusteringProblem(int dim,vector<NodeControlInfo> networkInfo, vector<NodeControlInfo> e_Nodes, vector<vector<double> > adjacency) 
 	{
		myfile.open("PSO-Trace.txt", std::ios::app);
    		myfile << "\n Problem init\n";

		count = 0;
		dimension = dim;
		clusteredNodes = 0;
		networkSize = networkInfo.size();
		e_networkSize = e_Nodes.size();
		
		candidates.clear();
		networkInf.clear();
		eligibleNodes.clear();
		
		for(int j = 0 ; j < networkSize ; j++ )
    		{
     			networkInf.push_back(networkInfo[j]);
		}

		for (int y = 0 ; y < e_networkSize ; y++)
		{
			eligibleNodes.push_back(e_Nodes[y]);
		}
		
		for (int i = 0 ; i < networkSize ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			adjacencyMatrix.push_back(r);
		}
		  
		for (int i = 0 ; i < networkSize; i++)
		{
			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = adjacency[i][j];
	//myfile << "Node "<< j << " is nbr with "<< i << " with RSSI = "<<  adjacency[i][j] <<"\n";
			}
		}
    				
		l_bounds = new double[dimension];
		u_bounds = new double[dimension];
		
		for(int i = 0 ; i < dimension ; i++)
		{
			l_bounds[i] = 0;
			u_bounds[i] = (e_networkSize-1);
			//myfile << "e_networkSize =  " << e_networkSize << "\n";
		}
 	}

	double get_lbound(size_t index)
	{  
		return 0;
	}

	double get_ubound(size_t index)
	{    
		return (e_networkSize-1);
	}

 	bool stop(double fitness, int epoch)
  	{
    		return (fitness <= 1e-4) || (epoch >= 200);
  	}

  	double evaluate(double * params)
  	{
    		double fitness = 0.0;
		vector<int> myVector(params,params+dimension);
		sort (myVector.begin(),myVector.end());
		myVector.erase (unique(myVector.begin(),myVector.end()),myVector.end());

		candidates.clear();

		//myfile << "Candidates : \n";
		for (int i=0 ; i < dimension ; i++)
		{
			int index = (int) params[i];
			NodeControlInfo n = eligibleNodes[index];
			candidates.push_back(n);
			//myfile << n.id << " ";
		}
		//myfile << "\n";

		candidates = clusterTheNetwork(candidates);

		double energyRat = calculateEnergyRatio();
		double compactness = getMaxCompatness();
		double clusteredRatio = calculateNonclusteredRatio();
		    
		//myfile << "Energy Ratio = " << energyRat << "\n";
		//myfile << "Compactness = " << compactness << "\n";
		//myfile << "clusteredRatio = " << clusteredRatio << "\n";
		
		fitness = (0.33 * energyRat) + (0.33 * compactness) + (0.33 * clusteredRatio);

		if (myVector.size() < dimension ) fitness+= 100000;
			
     		//myfile << "Fitness = " << fitness << "\n"; 
		return fitness;
  	}

	vector<NodeControlInfo> clusterTheNetwork(vector<NodeControlInfo> candidates)
	{
		//myfile << "PSOClustering : In Cluster the Network \n"; 
		int numOfCHs = candidates.size();
		int numOfnodes = networkInf.size();
		int clusteredNodes = 0;
		int nonClusteredNodes = 0; 
		//double totalRSSI = 0;

		for (int i = 1 ; i < networkInf.size() ; i++)
     		{
			double maxRSSI = -10000;
			int ch = 0;

			for (int j = 0; j < numOfCHs ; j++)
       			{
				int node1 = networkInf[i].id;
				int node2 = candidates[j].id;
				double rssi = adjacencyMatrix[node2][node1];
      				//myfile << networkInf[i].id <<" and "<<candidates[j].id ;
				if (rssi != 0)
				{
					//myfile << " are nbrs \n";
					if ((rssi > maxRSSI) && (networkInf[i].id != candidates[j].id))
       					{
							maxRSSI = rssi;
       							ch = candidates[j].id;
					}
				}
				//else myfile << " are not nbrs \n";
         
       			}
       				
       			for (int x = 0; x < candidates.size(); x++)
       			{ 
          			if (ch == candidates[x].id)
         			{
						int n1 = networkInf[i].id;
						int n2 = candidates[x].id;
						NodeControlInfo n;
						n.id = networkInf[i].id;
						n.energy = networkInf[i].energy;
						n.rssi  = adjacencyMatrix[n2][n1];
            					candidates[x].members.push_back(n);
						//totalRSSI += n.rssi / -100;
						//clusteredNodes = clusteredNodes + 1.0;
			//myfile << networkInf[i].id << " belongs to "<< ch <<" RSSI = "<< n.rssi <<" \n";
						break;
         			}

       			}
			
     		}	
		
		vector<NodeControlInfo> clustered(candidates);
     		return clustered;
	}

	
	~ClusteringProblem(void)
  	{
   		delete[] l_bounds;
   		delete[] u_bounds;
		eligibleNodes.clear();
   		candidates.clear();
   		networkInf.clear();
   		//myfile.close();
	}


	double getMaxCompatness()
	{
		double maxRSSI = -100;
		double maxCompactness = -100;
		double maxAvgRSSI = 0;
		double totalRSSI = 0;

		//myfile << "In calculateCompatness \n";
		for (int i = 0 ; i < candidates.size(); i++)
	     	{
			//myfile << "CH : " << candidates[i].id << "  \n";
	     		//myfile << "Members :  \n";
	                double clusterCompactness = 0;

	       	       	for (int j = 0; j < candidates[i].members.size() ; j++)
	        	{ 
				if (candidates[i].members[j].energy > 5)//Threshold to live nodes
				{
       					clusterCompactness += (candidates[i].members[j].rssi / maxRSSI); //* (18720/candidates[i].members[j].energy);
					totalRSSI += (maxRSSI / candidates[i].members[j].rssi );
          	   		}

        		}

			if (candidates[i].members.size() > 0)
			{
				maxAvgRSSI = clusterCompactness / candidates[i].members.size();
			}
			else
			{
				maxAvgRSSI = 0;
			}

			if (maxAvgRSSI > maxCompactness)
			{
				maxCompactness = maxAvgRSSI;			
			}
                	//myfile << " cluster Compactness " << clusterCompactness << "\n";
		}
     
		return maxCompactness;
		//return totalRSSI;
 	}
  

	double calculateEnergyRatio()
	{
		double initialEnergy = 18720;
        	double totalCHEnergy = 0;
		double energyRatio = 0;
        
 		//Compute Total residual Energy of all Cluster Heads
        	for (int j = 0; j < candidates.size() ; j++)
		{
	   		totalCHEnergy += (initialEnergy / candidates[j].energy);
		}
	  
		return totalCHEnergy;
  	}

	

	double calculateNonclusteredRatio()
	{
		double clusteredNodes = 0;

		for (int i = 0 ; i < candidates.size() ; i++)
		{
			clusteredNodes +=  candidates[i].members.size();
		}

		double nonClusteredNodes = networkInf.size() - clusteredNodes;
		double ratio = (nonClusteredNodes / clusteredNodes );
	
		return ratio;
	 }
  
};
vector<NodeControlInfo>  ClusteringProblem::eligibleNodes;
vector<NodeControlInfo>  ClusteringProblem::candidates;
vector<NodeControlInfo>  ClusteringProblem::networkInf;
//ofstream  ClusteringProblem::myfile;
double *ClusteringProblem::l_bounds;
double *ClusteringProblem::u_bounds;
int ClusteringProblem::epoch;


#endif
