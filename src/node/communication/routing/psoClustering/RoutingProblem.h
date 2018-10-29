#ifndef _ROUTINGPROBLEM_H_
#define _ROUTINGPROBLEM_H_

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
//#include "PSOClustering.h"

struct NeighborRecord 
{
	int id;
	double rssi;
	double remainingEnergy;
};
//typedef neighborRecord NeighborRecord;

struct NodeControlInfo 
{
	int id;	//the ID of the Node
	double rssi;
	double energy;
	int numOfNbrs;
	vector <NeighborRecord> nbrs;
	vector <NodeControlInfo> members;
};
//typedef NodeControlInfo NodeControlInfo;


struct Path
{
  bool valid;
  vector<int> route;
};

struct PeriorityData
{
	int id;
	double periority;
};

struct periorityComparer
{
    bool operator() (const PeriorityData& data1, const PeriorityData& data2) const { return ( data1.periority > data2.periority );   }
};

struct isInRoute
{
	int id;
	isInRoute(int i) : id(i) {};

	bool operator() (const PeriorityData data)
	{
		return (data.id == id);
	}

};

class RoutingProblem 
{
  	public:
  	size_t dimension;
  	size_t count;
	int networkSize;
  	static const int mode = 2; //Routing Mode
  	static double *l_bounds;
  	static double *u_bounds;
  	vector<int> CH_Candidates;
	vector <int> relayNodes;
  	vector<vector<double> > adjacencyMatrix;
  	static vector<NodeControlInfo> networkInf;
  	//ofstream //rfile;	
  
  	RoutingProblem(int dim, vector<vector<double> > adjacency, vector<int> candidates, vector<NodeControlInfo> networkInfo)
  	{
		////rfile.open("Routing-Trace.txt", std::ios::app);
		////rfile << "\n Initialize inner problem \n";
 
		dimension = dim;
		networkSize = networkInfo.size();
    
		l_bounds = new double[dimension];
		u_bounds = new double[dimension];
     
		for(int x=0; x < networkInfo.size(); x++)
		{
			networkInf.push_back(networkInfo[x]);
		}
   
		for(int i = 0 ; i < dimension ; ++i)
		{
			l_bounds[i] =  -1.0;
			u_bounds[i] =  1.0;
		}
     
		for(int j=0; j < candidates.size(); j++)
		{
			CH_Candidates.push_back(candidates[j]);
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
		//////rfile << "adjacencyMatrix["<< i << "]["<<j<<"] = "<< adjacencyMatrix[i][j] <<"\n";
			}
		}
    ////////rfile.close();
   	}

	double get_lbound(size_t index)
	{  
		return -1.0;
	}

	double get_ubound(size_t index)
	{    
		return 1.0;
	}

 	bool stop(double fitness, int epoch)
  	{
    		return (fitness <= 1e-4) || (epoch >= 200);
  	}

	~RoutingProblem(void)
  	{
   		delete[] l_bounds;
		delete[] u_bounds;
	        CH_Candidates.clear();
   	        adjacencyMatrix.clear();
  	        //rfile.close();
  	}
 
  	int getNumOfHops(vector<Path> paths) 
  	{
		int numOfHops = 0;
		int numOfCHs = 0;

		numOfHops = relayNodes.size();

		for (int i = 0 ; i < numOfHops ; i++)
		{
			if (find(CH_Candidates.begin(),CH_Candidates.end(),relayNodes[i]) != CH_Candidates.end())
			{
				numOfCHs++;
			}
		}
		
		//////rfile << "Number of relay nodes = "<< numOfHops << "\n";
		//////rfile << "Number of CHS-Relay = "<< numOfCHs << "\n";
		//////rfile << "Ratio = "<< numOfHops / numOfCHs << "\n";
		double ratio = numOfHops / numOfCHs;

		return ratio;
 	}
  
  
  	double evaluate(double * params)
  	{
		double fitness = 0;
    		int m = CH_Candidates.size();

		relayNodes.clear();

    		//rfile << "\n In evaluate : Routing\n";
		//rfile << "\n Periorities are \n";
                double particle[dimension];
		for (int i = 0; i < dimension ; i++)
		{
			particle[i] = params[i];
			//rfile << particle[i] << " "<< "\n";
		}
    		vector<Path> routes= decodeRoutingParticle (particle); 
    		//rfile << " Paths are: \n";
    
    		for (int x = 0 ; x < m ; x++)
    		{

			vector <int> p = routes[x].route;
		  	copy (p.begin(),p.end(),back_inserter(relayNodes));

     			for (int y = 0 ; y < routes[x].route.size() ; y++)
     			{
     				//rfile << routes[x].route[y] << " ";
     			}
     			//rfile << " \n"; 
     	
			if (!routes[x].valid)
     			{
				//rfile << "PATH IS NOT VALID \n";
				fitness +=100000;
     			}
     		}

		sort(relayNodes.begin(),relayNodes.end());
		relayNodes.erase( unique( relayNodes.begin(), relayNodes.end() ), relayNodes.end() );

		//rfile << "RelayNodes \n";
		for(int i = 0 ; i < relayNodes.size() ; i++)
		{
			//rfile << relayNodes[i] << " ";
		}
	
     		int numOfHops = getNumOfHops(routes);
		double energyRatio = calculateEnergyRatio(routes);
		double linkQuality = calculateLinkQuality(routes);

 		////rfile<< "numOfHops " << numOfHops << "\n";
		////rfile<< "energyRatio " << energyRatio << "\n";
		////rfile<< "linkQuality " << linkQuality << "\n";
		
     		fitness = (1.0 * numOfHops) + (1.0 * energyRatio) + (1.0 * linkQuality);

     		////rfile << " FITNESS = "<< fitness;

     		return fitness;
  	}


  	bool inRoute(const PeriorityData &nbr , int node)
  	{
    		return (nbr.id == node) ;
  	}

	double getRemainingEnergy(int id)
	{
		double remEnergy; 

		for (int i = 0 ; i < networkInf.size() ; i++ )
		{
			if ( id == networkInf[i].id )  
			{
				remEnergy = networkInf[i].energy;
				break;
			}
		}

		return remEnergy;
	}	

	double getRSSI (int src,int dst)
 	{
    		double node_rssi; 

     		for (int i = 0 ; i < adjacencyMatrix.size() ; i++ )
     		{
			for (int j = 0 ; j < adjacencyMatrix[i].size() ; j++)
        		if ( src == i && dst == j )  
        		{
          			node_rssi = adjacencyMatrix[i][j];
          			break;
        		}
     		}
     		return node_rssi;
 	}

	double calculateLinkQuality(vector<Path> paths)
	{
		double maxRSSIRange = -100;
		double maxtotalRSSI = -1000;
		double totalLength = 0;

		////rfile << "\n In calculateLinkQuality \n";

		for (int x = 0 ; x < paths.size() ; x++)
		{
			double avg = 0;	
			double pathQuality = 0; 

			for(int y = 0 ; y < paths[x].route.size() - 1 ; y++)
			{
				int node = paths[x].route[y];
				int nextNode = paths[x].route[y+1];
				//////rfile << "Node "<< node <<" Next Node "<< nextNode << "\n";
				double rssi = adjacencyMatrix[nextNode][node];
				//////rfile << "adjacencyMatrix["<<nextNode<<"]["<<node<<"]\n";
				//////rfile << "nextNodeRSSI " << rssi << "\n";
				pathQuality += (rssi/maxRSSIRange);
			}

			totalLength += pathQuality;

			if (pathQuality > maxtotalRSSI)
			{
				maxtotalRSSI = pathQuality;
			}

			////rfile << "Path Quality = "<< pathQuality << "\n";
			////rfile << "TotalLength  = "<< totalLength << "\n";
		}

		return maxtotalRSSI;
		//return totalLength;
	}	

  	vector<Path> decodeRoutingParticle (double * params)
  	{
		////////rfile << "In decode \n";

		vector<Path> routes;
		int m = CH_Candidates.size();
		vector<double> periorites;
		copy ( params, params + dimension, std::back_inserter ( periorites ) );

		Path path[m];
		for (int i = 0 ; i < m ; i++)
		{
			int ch = CH_Candidates[i];
    			path[i].valid = true;
			path[i].route.push_back(ch);	//start from source : DST is always 0
        		periorites[ch] = -100000;
        		findNextNode(ch , path[i] , periorites);
        		routes.push_back(path[i]);
    		}
     
 		return routes;
	}

   double calculateEnergyRatio(vector<Path> paths)
  {
	//Compute Total Energy of all nodes
	double totalNWEnergy = 0;
	double initialEnergy = 18720;
	double totalRelayNodesEnergy = 0;
	double energyRatio = 0;

	for (int i = 0 ; i < networkInf.size() ; i++)
	{
		totalNWEnergy += networkInf[i].energy;
	}   

	for (int x = 0 ; x < relayNodes.size() ; x++)
	{
		int relay = relayNodes[x];
		totalRelayNodesEnergy += (initialEnergy / getRemainingEnergy(relay));
	}
		
        energyRatio = (totalNWEnergy / totalRelayNodesEnergy);

	return totalRelayNodesEnergy;
  }


   void findNextNode(int source , Path &path , vector<double> periorites)
  {
       int nextHop;
       vector<PeriorityData> nbrs;
      
       for (int x = 0 ; x < networkSize ; x++)
       {
	   if (adjacencyMatrix[x][source] < 0)
	   {
		PeriorityData node;
		node.id = x;
		node.periority = periorites[x];
		nbrs.push_back(node);
           	//////rfile << x << " is nbr of " << source << " RSSI " << adjacencyMatrix[x][source] <<" with periority = "<< periorites[x] <<" \n ";
	   }
	}

	if ( nbrs.size () != 0 )
        {
		sort(nbrs.begin(),nbrs.end(),periorityComparer());   
  
       		if (nbrs[0].periority == -100000) path.valid = false;
		//This mean the next hop is already in path, return Non-Valid Path (non-exit path)

       		nextHop = nbrs[0].id;
        	periorites[nextHop] = -100000;
       		//rfile << "Next node to add " << nextHop <<"\n";
		path.route.push_back(nextHop);
	}
	else
	{
		path.valid = false;
	}

	if (nextHop != 0  && path.valid) // as long as I didn't reach sink and the path is still valid
	{
        	findNextNode(nextHop,path,periorites);
	}
        
     }

  //Dynamic Matrix version
  void findNextNode2(int source, Path &path, double * params)
  {
       //put check if sink has 0 nbrs, network is down

      // //////rfile << "In the recursive fn Print - Dijakstra\n";
       int nextHop;
       vector<PeriorityData> nbrs;
       vector<double> neighbours = adjacencyMatrix[source];

       for (int x = 0 ; x < neighbours.size() ; x++)
       {
	   if (neighbours[x] < 0)
	   {
		PeriorityData node;
		node.id = x;
		node.periority = params[x];
		nbrs.push_back(node);
           	////////rfile << x << " is nbr of " << source << " with periority = "<< params[x] <<" \n ";
	   }
	}

        for (int i = 0 ; i < path.route.size() ; i++)
	{
		int in = path.route[i];
		nbrs.erase(remove_if(nbrs.begin(),nbrs.end(),isInRoute(in)),nbrs.end());
    	
	}   

	sort(nbrs.begin(),nbrs.end(),periorityComparer());   
  
        nextHop = nbrs[0].id;
       // //////rfile << "Next node to add " << nextHop <<"\n";
	path.route.push_back(nextHop);
	if ( (nextHop != 0) && (path.route.size() < dimension) ) 
	{
        	findNextNode2(nextHop,path,params);
	}
	
	if (path.route.size() >= dimension) 
	{
        	path.valid = false;
	}
	
	//else break;
	//return route;
  }

};

double *RoutingProblem::l_bounds;
double *RoutingProblem::u_bounds;
vector<NodeControlInfo>  RoutingProblem::networkInf;
//typedef RoutingProblem RoutingProblem;

#endif
