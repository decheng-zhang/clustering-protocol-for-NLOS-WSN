#ifndef _CHSELECTIONPROBLEM_H_
#define _CHSELECTIONPROBLEM_H_

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


class CHSelectionProblem 
{
	public:
	size_t dimension;
	size_t count;
	int networkSize;
	int e_networkSize;
	static const int mode = 1; // Clustering Mode
	static double *l_bounds;
	static double *u_bounds;
	static vector<nodeInfo> candidates;
	static vector<nodeInfo> networkInf;
	static vector <nodeInfo> eligibleNodes; 
	ofstream myfile;	
	  
	CHSelectionProblem(int dim,vector<nodeInfo> networkInfo,vector<nodeInfo> e_Nodes) 
	{
		//myfile.open("PSO-Trace.txt", std::ios::app);
   		//myfile << "\n Problem init\n";

		count = 0;
		dimension = dim;
		networkSize = networkInfo.size();
		e_networkSize = e_Nodes.size();

		candidates.clear();
		networkInf.clear();
		eligibleNodes.clear();
	
	
		for(int x = 0 ; x < networkSize ; x++)
    		{
			networkInf.push_back(networkInfo[x]);
		}

		for (int y = 0 ; y < e_networkSize ; y++)
		{
			eligibleNodes.push_back(e_Nodes[y]);
		}

		l_bounds = new double[dimension];
		u_bounds = new double[dimension];
		
		for(int i = 0 ; i < dimension ; i++)
		{
			l_bounds[i] = 0;
			u_bounds[i] = (e_networkSize - 1);
		}
		//myfile << "e_networkSize =  " << e_networkSize << "\n";
   	
	}

	double get_lbound(size_t index)
	{  
		return 0;
	}

	double get_ubound(size_t index)
	{    
		return (e_networkSize - 1);
	}

 	bool stop(double fitness, int epoch)
  	{
    		return (fitness <= 1e-4) || (epoch >= 200);
  	}

  	double getMaxAvgDistance()
  	{
     		double maxAvgDist = -100;

		for (int i = 0; i < candidates.size() ; i++)
     		{
			double distSum = 0;
       			double avgDist = 0;
			if (candidates[i].members.size() != 0)
       			{
       				for (int j = 0 ; j < candidates[i].members.size() ; j++)
       				{
         				double dist = sqrt (pow ((candidates[i].x - candidates[i].members[j].x),2) + pow ((candidates[i].y - candidates[i].members[j].y),2) );
          
         			distSum += dist;
	 
       				}
       
				avgDist = (distSum / candidates[i].members.size());

				if (avgDist > maxAvgDist)
       				{
          				maxAvgDist = avgDist;
       				} 
     			}
			else
			{
				maxAvgDist = 0;

			}
		}

		return maxAvgDist;
	}

	double getEnergyRatio()
	{
		double totalNWEnergy = 0;
	        double totalCHEnergy = 0;
		double energyRatio = 0;

		//Compute Total Energy of all nodes
		for (int i = 0; i < networkInf.size(); i++)        
		{
			totalNWEnergy = totalNWEnergy + networkInf[i].energy;
	        }

        	//Compute Total Energy of all Cluster Heads
        	for (int j = 0; j < candidates.size() ; j++)
		{
	   		totalCHEnergy = totalCHEnergy + candidates[j].energy;
		}
  
		energyRatio = totalNWEnergy / totalCHEnergy;
		return energyRatio;

	}

	double evaluate(double * params)
	{
		//myfile << "\n In evaluate \n";

		double fitness = 0.0;

		vector<int> myVector(params,params+dimension);
		sort (myVector.begin(),myVector.end());
		myVector.erase (unique(myVector.begin(),myVector.end()),myVector.end());
  

		candidates.clear();
		//myfile << "Candidates : \n";
		for (int i=0 ; i < dimension ; i++)
		{
			int index = (int) params[i];
			//myfile << "INdex = "<< index <<"\n";
			nodeInfo ch = eligibleNodes[index];
			//myfile << ch.id << "  ";
			candidates.push_back(ch);
		}
		//myfile << "\n";
     
		candidates = clusterTheNetwork(candidates);
		
		double maxAvgDist = getMaxAvgDistance();
		double energyRat = getEnergyRatio();

     		fitness = (0.5 * energyRat) + (0.5 * maxAvgDist) ;

     		if (myVector.size() < dimension) fitness+= 100000;

     		//myfile << "Fitness = " << fitness; 
       		return fitness;
	}

	vector<nodeInfo> clusterTheNetwork(vector<nodeInfo> candidates)
	{
		int numOfCHs = candidates.size();
		int numOfnodes = networkInf.size();

		sort(networkInf.begin(), networkInf.end(),ID_Comparer());
    		sort(candidates.begin(), candidates.end(),ID_Comparer());
    		vector<nodeInfo> network;
		set_difference(networkInf.begin(), networkInf.end(), candidates.begin(), candidates.end(), back_inserter(network),ID_Comparer());
		   
     		for (int i = 1 ; i < network.size() ; i++)
     		{
			double minDistance = 10000;
			int ch = 0;
			for (int j = 0; j < numOfCHs ; j++)
       			{
         				double dist = sqrt (pow ((network[i].x - candidates[j].x),2) + pow ((network[i].y - candidates[j].y),2) );
         
        				if ((dist < minDistance) && (network[i].id != candidates[j].id))
         				{
           					minDistance = dist;
           					ch = candidates[j].id;
	 				}
         
       				}
       				
       				for (int x = 0; x < candidates.size(); x++)
       				{ 
          				if (ch == candidates[x].id)
         				{
            					candidates[x].members.push_back(network[i]);
         				}

       				}
			//}
     		}	
		
		vector<nodeInfo> clustered(candidates);
     		return clustered;
	}

	~CHSelectionProblem(void)
  	{
   		delete[] l_bounds;
   		delete[] u_bounds;
   		candidates.clear();
   		networkInf.clear();
		eligibleNodes.clear();
   		myfile.close();
	}

};

vector<nodeInfo>  CHSelectionProblem::candidates;
vector<nodeInfo>  CHSelectionProblem::networkInf;
vector<nodeInfo>  CHSelectionProblem::eligibleNodes;
double *CHSelectionProblem::l_bounds;
double *CHSelectionProblem::u_bounds;


#endif
