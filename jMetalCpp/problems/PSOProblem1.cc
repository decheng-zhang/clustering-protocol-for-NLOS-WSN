#include <PSOProblem1.h>
using namespace std;

	PSOProblem1 :: PSOProblem1(int networkSize_)
	{

	}
	PSOProblem1 :: PSOProblem1(vector< vector<double>> adjacencyM) 
	{
		//myfile.open("PSO_CH");
		networkSize = adjacencyM.size();
		numberOfObjectives_  = 1;
		numberOfConstraints_ = 0;
		problemName_         = "CHElection";
		
		numberOfCHs = ( networkSize * 5) / 100;
		numberOfVariables_   = numberOfCHs;
		
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
				adjacencyMatrix[i][j] = adjacencyM[i][j];
			}
		}

		lowerLimit_ = new double[numberOfVariables_];
		upperLimit_ = new double[numberOfVariables_];
		
		for (int var = 0; var < numberOfVariables_; var++)
		{
    			lowerLimit_[var] = 1;
    			upperLimit_[var] = networkSize - 1;
  		}
		
		solutionType_ = new RealSolutionType(this) ;

	}

	
	PSOProblem1 :: ~PSOProblem1() 
	{
		delete [] lowerLimit_ ;
  		delete [] upperLimit_ ;
  		delete solutionType_ ;
		network.clear();
		adjacencyMatrix.clear();
  	}

	void PSOProblem1 :: clusterTheNetwork(vector<int> candidates)
	{
		network.clear();

		totalRSSI_ = -100;
		sclb_ = 0;

		for (int i = 1 ; i < networkSize ; i++)
     		{
			double maxRSSI = -10000;
			int cHead = 0;

			for (int j = 0; j < numberOfCHs ; j++)
       			{
				int node2 = i;
				int node1 = candidates[j];
				double rssi = adjacencyMatrix[node1][node2];
				if (rssi != 0)
				{
					if ( (rssi > maxRSSI) && (i != candidates[j]) )
       					{
							maxRSSI = rssi;
       							cHead = candidates[j];
					}
				}
			}
                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(i,cHead));
			  double prssi = adjacencyMatrix[cHead][i]/-100;
			  if (prssi > totalRSSI_) totalRSSI_ = prssi;
			  sclb_ = sclb_ + 1;
			}
		}
	}

	string PSOProblem1 :: returnConfiguration(vector<int> candidates)
	{
		//myfile << "returnConfiguration \n";
		string configuration;		
		stringstream out;

		clusterTheNetwork(candidates);

		dijkstra(candidates);

		boost::char_separator<char> sep1(";");
    	
		boost::tokenizer<boost::char_separator<char>> tokens1(routes, sep1);

		multimap<int,int> routingTable;

		//myfile << "routes = "<< routes << "\n";

    		for (const auto& t : tokens1) 
		{
			boost::char_separator<char> sep2(" ");
			boost::tokenizer<boost::char_separator<char>> data(t, sep2);
			int i = 0;
			int relay;
			int nextHop;

			for (boost::tokenizer<boost::char_separator<char>>::iterator it = data.begin() ; it != data.end() ; ++it) 	
			{
				relay = stoi (*it);
				nextHop = stoi (*++it);

				for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
       				{
					if(iter->first == relay )
					{
						iter->second = nextHop;
					}
				}
			}
		}
             
		candidates.push_back(0);
		sort (candidates.begin(),candidates.end());
		candidates.erase (unique(candidates.begin(),candidates.end()),candidates.end());

		for (int i = 0 ; i < candidates.size() ; i++)
		{
		    out << candidates[i] << ",";

		    for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
       		    {
			if(iter->second == candidates[i] )
			{
				out << iter->first << " ";
			}
		    }
		    out << ";";
		}

		configuration = out.str();
		//myfile << "configuration" << configuration <<"\n";
		return configuration;
	}

	double PSOProblem1 :: dijkstra(vector<int> candidates)
        {
           double dCost = 0;
	   Dijkstra G;
           G.read(adjacencyMatrix , candidates);
    	   G.calculateDistance();
           dCost = G.output();
	   routes = G.returnRoutes();
	   //myfile << "Routes are " << routes << "\n";	
           //myfile << "IN dijkstra " << dCost << "\n";
	   return dCost;
   	}

	void PSOProblem1 :: printNetwork()
	{

	}

	double PSOProblem1 :: calculateEnergyRatio(vector<int> candidates)
	{
		double initialEnergy = 18720;
        	double totalCHEnergy = 0;
		double energyRatio = 0;
        
 		//Compute Total residual Energy of all Cluster Heads
        	for (int j = 0; j < candidates.size() ; j++)
		{
	   		totalCHEnergy += (initialEnergy / adjacencyMatrix[j][j]);
		}
	  
		return totalCHEnergy;
  	}
	
	double PSOProblem1 :: getCompatness(vector<int> candidates)
	{
		double maxRSSI = -100;
		double maxCompactness = -100;
		double maxAvgRSSI = 0;
		double totalClustered = 0;
		double totalRSSI = 0;
				
		for (int i = 0 ; i < candidates.size() ; i++)
		{
		    vector<int> members;

		    for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
		    {
			if (iter->second == candidates[i]) members.push_back(iter->first);
		    }

		    totalClustered+= members.size();
			
		    double clusterCompactness = 0;

		    for(int j = 0 ; j < members.size() ; j++)
		    {
			int node1 = candidates[i];
			int node2 = members[j];
			clusterCompactness += (adjacencyMatrix[node1][node2] / maxRSSI); 
			totalRSSI +=  adjacencyMatrix[node1][node2] / maxRSSI;
		    }

		    if (members.size() > 0)
		    {
			maxAvgRSSI = clusterCompactness / members.size();
		    }
		    else
		    {
			maxAvgRSSI = 0;
		    }

		    if (maxAvgRSSI > maxCompactness)
		    {
				maxCompactness = maxAvgRSSI;			
		    }

		}
		
		totalClusteredNodes = totalClustered;
		double rssi = totalRSSI;
	        //return rssi;
		return maxCompactness;
 	}

	double PSOProblem1 :: calculateNonclusteredRatio()
	{
		double clusteredNodes = 0;

		double nonClusteredNodes = networkSize - totalClusteredNodes;
		double ratio ;
               if (totalClusteredNodes > 0)
		{
			ratio = (nonClusteredNodes / totalClusteredNodes );
 		}
	
		return ratio;
	 }


	void PSOProblem1 :: evaluate(Solution *solution) 
	{
	     double fitness;
	     Variable ** decisionVariables  = solution->getDecisionVariables();
	     int m = numberOfVariables_;

	     vector<int> candidates(m);
             energy_ = 0;
             for (int i = 0 ; i < m; i++) 
	     {
    			candidates[i] = (int)decisionVariables[i]->getValue() ;
			energy_ += adjacencyMatrix[candidates[i]][candidates[i]];
 	     }
	     sort (candidates.begin(),candidates.end());
	     candidates.erase (unique(candidates.begin(),candidates.end()),candidates.end());

	     int n = candidates.size();

	     if (n == m ) 
	     {
		double maxEnergy = m * 18720; //best
	        double minEnergy = 0;			     //worst

		double maxLinkQ  =  0.97;
		double minLinkQ  =  0.50;

		double maxSclb	 = networkSize ;
		double minSclb   = 0;

		double maxTc	 = n * n * 0.97;
		double minTc     = n * 0.50;

		clusterTheNetwork(candidates);
                double tc = dijkstra(candidates);
		printNetwork();

		double r = getCompatness(candidates);

		double energyRat = (maxEnergy - energy_) / (maxEnergy - minEnergy);
	        double clusteredRatio = (maxSclb - sclb_) / (maxSclb - minSclb);
		double compactness = 1 - (maxLinkQ - r) / (maxLinkQ - minLinkQ);
	        double treeCost = 1 - (maxTc - tc) / (maxTc - minTc);

		fitness =  energyRat +  compactness + clusteredRatio; // + treeCost;
	        solution->setObjective(0,fitness);
	        //solution->setObjective(1,compactness);
	        //solution->setObjective(2,clusteredRatio);
              }
	      else
              {

		solution->setObjective(0,10000);
	        //solution->setObjective(1,10000);
	        //solution->setObjective(2,10000);
	      }
              network.clear();
             
	}

