#ifndef _CH_PROBLEM_H_
#define _CH_PROBLEM_H_

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
//#include "PSO_HC.h"

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
	bool isCH;
	bool isSubCH;
	bool clustered;
	vector <NeighborRecord> nbrs;
	vector <NodeControlInfo> members;
};

class CH_Problem 
{
  	public:
  	size_t dimension;
  	size_t count;
  	int networkSize;
  	int e_networkSize;
	double clusteredNodes;
	static vector<vector<double> > adjacencyMatrix;
  	static const int mode = 1; // Clustering Mode
  	static double *l_bounds;
  	static double *u_bounds;
  	static vector<NodeControlInfo> candidates;
  	static vector<NodeControlInfo> eligibleNodes;
  	static vector<NodeControlInfo> networkInf;
 	static ofstream myfile;	
  	static int epoch;
  
 	CH_Problem(int dim,vector<NodeControlInfo> networkInfo, vector<NodeControlInfo> e_Nodes, vector<vector<double> > adjacency) 
 	{
		//myfile.open("PSO-Trace.txt", std::ios::app);
    		
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

		//myfile << "Eligible node are : \n";

		for (int y = 0 ; y < e_networkSize ; y++)
		{
			eligibleNodes.push_back(e_Nodes[y]);
			//myfile << "  " << eligibleNodes[y].id ;
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
		for(int j = 0 ; j < networkSize ; j++ )
    		{
			networkInf[j].isCH = false;
			networkInf[j].isSubCH = false;
			networkInf[j].clustered = false;
		}

		//myfile << "Candidates : \n";
		for (int i=0 ; i < myVector.size() ; i++)
		{
			int index = myVector[i];
			NodeControlInfo n = eligibleNodes[index];
			n.isCH = true;
			int inn = n.id;
     			networkInf[inn].isCH = true;
			candidates.push_back(n);
		//	myfile << n.id << " ";
		}
		//myfile << "\n";

		candidates = clusterTheNetwork(candidates);

		//double std = getSTD();
		//double energyRat = calculateEnergyRatio();
		double compactness = getMaxCompatness();
		//double coverage = calculateCoverage();
		double chsRatio = calculateCHsRatio();

		//myfile << "Energy Ratio = " << energyRat << "\n";
		//myfile << "Compactness = " << compactness << "\n";
		//myfile << "CHs ratio = " << chsRatio << "\n";
		
		fitness = (3 * compactness) + (1 * chsRatio); //+ (0 * energyRat) + ( 0 * coverage);

		//if (myVector.size() < dimension ) fitness+= 100000;
			
     		////////myfile << "Fitness = " << fitness << "\n"; 
		return fitness;
  	}

	vector<NodeControlInfo> getTheWholeNetwork(vector<NodeControlInfo>& candidates)
	{
		for(int j = 0 ; j < networkSize ; j++ )
    		{
			networkInf[j].isCH = false;
			networkInf[j].isSubCH = false;
			networkInf[j].clustered = false;

			for(int i = 0 ; i < candidates.size() ; i++ )
    			{
				if (networkInf[j].id == candidates[i].id)
				networkInf[j].isCH = true;	
			}
		}

		candidates = clusterTheNetwork(candidates);
		return candidates;
	}

	vector<NodeControlInfo> clusterTheNetwork(vector<NodeControlInfo>& candidates)
	{
		//myfile << "\n PSO_HC : In Cluster the Network \n"; 
		//myfile << "Clustered nodes \n";

		int numOfCHs = candidates.size();
		int numOfnodes = networkInf.size();
		
		vector<int> firstClustered;
		vector<int> secondClustered;


		for (int i = 1 ; i < networkInf.size() ; i++)
     		{
			double maxRSSI = -10000;
			int ch = -1;

			for (int j = 0; j < numOfCHs ; j++)
       			{
				int node1 = networkInf[i].id;
				int node2 = candidates[j].id;
				double rssi = adjacencyMatrix[node2][node1];
      				//myfile << networkInf[i].id <<" and "<<candidates[j].id ;
				if (rssi != 0)
				{
					//myfile << " are nbrs \n";
					if ((rssi > maxRSSI) && (!networkInf[i].isCH))
       					{
							maxRSSI = rssi;
       							ch = candidates[j].id;
					}
				}
				//else //myfile << " are not nbrs \n";
         
       			}
			
			//if (networkInf[i].isCH == 1)
			////myfile <<"networkInf[i].id "<< networkInf[i].id <<" isCH "<<"\n";

			if (ch == -1 && !networkInf[i].isCH) //notClustered
			{
				secondClustered.push_back(networkInf[i].id);
				//myfile << " node " << networkInf[i].id << " is not clustered \n";
			}
       			else
			{	
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
						n.isCH = false;
						n.isSubCH = false;
						n.clustered = true;
            					candidates[x].members.push_back(n);
						//myfile << " " << n.id << "pushed in first tier \n";
						firstClustered.push_back(n.id);
						break;
         			}
				//myfile << "\n";
       			}
			}
			
     		}	
		
		/*myfile << "\nFirst tier \n";
		for (int i = 0 ; i < firstClustered.size() ; i++)
		{
			//myfile << " " << firstClustered[i];

		}

		//myfile << "\n";

		//myfile << "\nSecond tier \n";
		for (int i = 0 ; i < secondClustered.size() ; i++)
		{
			//myfile << " " << secondClustered[i];

		}*/
		

		if ( secondClustered.size() > 0)
		{
     			clusterAgain(secondClustered,firstClustered,candidates);
		}
		vector<NodeControlInfo> clustered(candidates);
		return clustered;
	}

	void clusterAgain(vector<int> tier2 , vector<int> tier1, vector<NodeControlInfo>& candidates)
	{

		//myfile << " nonclustered size  " << tier2.size() << "\n";
		//myfile << " Clustered size  " << tier1.size() << "\n";

		//myfile << "nonClustered \n";

		for (int i = 0 ; i < tier2.size() ; i++)
     		{
			double maxRSSI = -10000;
			int ch = -1;

			for (int j = 0; j < tier1.size() ; j++)
       			{
				
				int node1 = tier2[i];
				int node2 = tier1[j];

				double rssi = adjacencyMatrix[node2][node1];

					if (rssi != 0)
					{
			            //myfile << node1 <<" and "<< node2 << " are nbrs \n";

						if (rssi > maxRSSI)
       						{
							maxRSSI = rssi;
       							ch = node2;
						}
					}
			}
			
			
			//myfile << tier2[i] << " will be added to "<< ch << " \n";
			//myfile << " candidates.size() " << candidates.size() << " \n";

			for (int z = 0; z < candidates.size(); z++)
       			{ 
				
			////myfile << "  candidates[z].members.size() " <<  candidates[z].members.size() << " \n";
				for (int y = 0; y < candidates[z].members.size() ; y++)
				{	
					////myfile << "In for \n";
					////myfile << candidates[z].members[y].id << "\n";

          				if (candidates[z].members[y].id == ch)
         				{
						int n1 = tier2[i];
						int n2 = candidates[z].members[y].id;
						NodeControlInfo n;
						n.id = networkInf[n1].id;
						n.energy = networkInf[n1].energy;
						n.rssi  = adjacencyMatrix[n2][n1];
						n.clustered = true;
            					candidates[z].members[y].members.push_back(n);
						//myfile << n1 << " will be attached to "<< n2 << "\n";
						candidates[z].members[y].isSubCH = true;
						networkInf[candidates[z].members[y].id].isSubCH = true;
						break;
         				}
				}
       			}

		 }

	/*	myfile << "Whole NW is \n";

		for (int y = 0; y < candidates.size() ; y++)
		{	
			for (int z = 0; z < candidates[y].members.size() ; z++)
			{
				myfile << " first " << candidates[y].members[z].id << "\n";
				if (candidates[y].members[z].members.size() > 0)
				{
					for (int x = 0; x < candidates[y].members[z].members.size() ; x++)
					{
						myfile << " second  " << candidates[y].members[z].members[x].id<<"\n";
                	                }
					myfile << "\n";
				
				}
				
			}
			myfile << "\n";

		}*/

		//vector<NodeControlInfo> nw(tier1);*/
     	}

	
	~CH_Problem(void)
  	{
   		delete[] l_bounds;
   		delete[] u_bounds;
		adjacencyMatrix.clear();
		eligibleNodes.clear();
   		candidates.clear();
   		networkInf.clear();
   		//myfile.close();
	}

	double getSTD()
	{
		double noSubCH = 0;
		double totalSecond = 0;	

		for (int i = 0 ; i < candidates.size() ; i++)
		{
			for (int j = 0 ; j < candidates[i].members.size() ; j++)
			{
				if (candidates[i].members[j].isSubCH)
				{
					noSubCH++;
					totalSecond +=  candidates[i].members[j].members.size();

				}

			}
		}

		double mean = totalSecond / noSubCH;
		double difference = 0;

		for (int i = 0 ; i < candidates.size() ; i++)
		{
			for (int j = 0 ; j < candidates[i].members.size() ; j++)
			{
				if (candidates[i].members[j].isSubCH)
				{
					double no = candidates[i].members[j].members.size();
					difference += pow((no - mean),2);

				}

			}
		}

		double std = sqrt (difference / noSubCH);
		return std;
	}

	double getMaxCompatness()
	{
		double maxRSSI = -100;
		double worstRSSI1 = -100;
		double worstRSSI2 = -100;
		double totalRSSI = 0;
		double totalNodes = 0;
		
		for (int i = 0 ; i < candidates.size(); i++)
	     	{
			for (int j = 0; j < candidates[i].members.size() ; j++)
	        	{ 
				double rssi = (candidates[i].members[j].rssi / maxRSSI); 
				totalRSSI += rssi;
				totalNodes++;

				if ( rssi > worstRSSI1)
				{
					worstRSSI1 = rssi;
				}
						
				if (candidates[i].members[j].isSubCH)
				{
					for (int x = 0; x < candidates[i].members[j].members.size() ; x++)
					{
       					   double rssi2 = (candidates[i].members[j].members[x].rssi / maxRSSI);
					   totalRSSI += rssi;
					   totalNodes++;

					   if ( rssi2 > worstRSSI2)
					   {
						worstRSSI2 = rssi2;
					   } 
						
					}
				}

        		}

		}

		double avgRSSI;

		if ((worstRSSI1 > 0) && (worstRSSI2 > 0))
			avgRSSI = (worstRSSI1 + worstRSSI2) / 2;
		else if (worstRSSI1 > worstRSSI2)
			avgRSSI = worstRSSI1;
		else 	avgRSSI = worstRSSI2;
		
		return avgRSSI;
		
 	}
  

	double calculateEnergyRatio()
	{
		//myfile << "In calculateEnergyRatio \n";
		
		double initialEnergy = 18720;
		double totalSNEnergy = 0;
        	double totalConsumedEnergy = 0;
		double energyRatio = 0;
		        
		//Compute Total residual Energy of all Cluster Heads
        	for (int j = 0; j < candidates.size() ; j++)
		{
			double remainingEnergy = candidates[j].energy / initialEnergy;
			double consumedEnergy = 1 - remainingEnergy;
	   		totalConsumedEnergy += consumedEnergy;
		}
	  
		double avgConsumedEnergy = totalConsumedEnergy / candidates.size();
		return avgConsumedEnergy;
  	}

	
	double calculateCoverage()
	{
		double clusteredNodes = 0;
		
		
		for (int i = 0 ; i < candidates.size() ; i++)
		{
			clusteredNodes +=  candidates[i].members.size();

			for (int j = 0 ; j < candidates[i].members.size() ; j++)
			{

				if (candidates[i].members[j].isSubCH)
				{
					clusteredNodes += candidates[i].members[j].members.size();				
					
				}

			}
		}
		
		double unClusteredNodes = networkInf.size() - clusteredNodes;
		double ratio = unClusteredNodes / clusteredNodes;
		
		return ratio;
	}

	double calculateCHsRatio()
	{
		double noCHs = candidates.size();
		double ratio =  noCHs / dimension;
		
		return ratio;
	 }
  
};
vector<NodeControlInfo>  CH_Problem::eligibleNodes;
vector<NodeControlInfo>  CH_Problem::candidates;
vector<NodeControlInfo>  CH_Problem::networkInf;
vector<vector<double> > CH_Problem::adjacencyMatrix;
ofstream  CH_Problem::myfile;
double *CH_Problem::l_bounds;
double *CH_Problem::u_bounds;
int CH_Problem::epoch;


#endif
