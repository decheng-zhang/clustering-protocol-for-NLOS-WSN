#include <MOPSOProblem.h>

	using namespace std;

	MOPSOProblem :: MOPSOProblem(vector< vector<double>> adjacencyM) 
	{
		//myfile.open("MOPSO.txt");
		
		//myfile << "Init \n";
		networkSize = adjacencyM.size();
		numberOfObjectives_  = 5;
		numberOfConstraints_ = 0;
		problemName_         = "MO-CHElection";
			
		numberOfVariables_   = networkSize - 1;
		totalRSSI = 0;
	      	totalUnclustered = 0;
		
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
				//myfile << "Link from "<< j << " to " << i << " has RSSI = " << adjacencyMatrix[i][j] <<"\n";
			}
		}

		lowerLimit_ = new double[numberOfVariables_];
		upperLimit_ = new double[numberOfVariables_];
		
		for (int var = 0; var < numberOfVariables_; var++)
		{
    			lowerLimit_[var] = 0;
    			upperLimit_[var] = 1;
  		}
		
		solutionType_ = new RealSolutionType(this) ;

	}

	vector<double> MOPSOProblem :: getInitialSolution() 
	{
		vector <double> configuration;
		//myfile << "getInitialSolution \n";
		boost::uniform_int<> dist(0, networkSize - 1);
    		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
	
		int numberOfCHs = ((5 * networkSize) / 100);
		
		//myfile << "Cluster heads are : ";

    		for (int x = 0; x < numberOfCHs; x++)
		{
			int n = die();
			//myfile << n << " ";
			clusterHeads.push_back(n);
		}
		
		//myfile << "\n";

		sort (clusterHeads.begin(),clusterHeads.end());
		clusterHeads.erase (unique(clusterHeads.begin(),clusterHeads.end()),clusterHeads.end());
		
		for (int i = 1; i < networkSize; i++)
		{
			double maxRSSI = -10000;
			double numOfNbrs = 0;
			double chIndex = -1;
			int node = i;
						
			//myfile << "Node " << node << " nbrs are : \n";

			for (int j = 0; j < adjacencyMatrix[i].size(); j++)
			{
				int node1 = node;
				int node2 = j;
				double rssi = adjacencyMatrix[node2][node1];
      				
				if (rssi < 0)
				{
					numOfNbrs++;
					//myfile <<  j << ", rssi = " << rssi <<"\n";
					if ((rssi > maxRSSI) && (find(clusterHeads.begin(), clusterHeads.end(), j) != clusterHeads.end()))
       					{
							maxRSSI = rssi;
       							chIndex = numOfNbrs - 1;
					}
				}
			}
		
			double code = chIndex / numOfNbrs;
			//code = floor (code * 1000) / 1000;
			configuration.push_back(code);

		//myfile << "Node "<< node << " Num of nbrs = "<< numOfNbrs <<" CH Index = " <<  chIndex << " code = " << code << "\n"; 
		}
		//string s = "maya";
                //return s;

		return configuration;
	}
	
	
	MOPSOProblem :: ~MOPSOProblem() 
	{
		delete [] lowerLimit_ ;
  		delete [] upperLimit_ ;
  		delete solutionType_ ;
                //myfile.close();
		network.clear();
		adjacencyMatrix.clear();
  	}

	void MOPSOProblem :: clusterTheNetwork(vector<double> configurtation)
	{
		//myfile << "clusterTheNetwork \n";
		network.clear();
		clusterHeads.clear();
		totalUnclustered = 0;
		totalRSSI = 0;
		
		int nLinks = 0;

		for (int i = 0 ; i < configurtation.size() ; i++)
     		{
		    int node = i + 1 ;
			
		    vector<int> nbrs;

		    //myfile << node << " nbrs are :  \n";
		    for (int j = 0 ; j < adjacencyMatrix.size() ; j++)
		    {
			if (adjacencyMatrix[j][node] < 0)
			{
                                //myfile << j << "  rssi = " << adjacencyMatrix[j][node] << "\n";
				nbrs.push_back(j);			
			}

		    }
		    //myfile << "\n";
		    //nbrs.push_back(-1);
		    int ch_Index = -1;
		    int ch = -1;
		     
                    if (nbrs.size() != 0 && configurtation[i]>=0)
		    {
                       ch_Index = ceil (configurtation[i] * (nbrs.size() - 1));
         	       ch = nbrs[ch_Index];
		       totalRSSI+= (adjacencyMatrix[ch][node])/-100;
		       nLinks++;
		       clusterHeads.push_back(ch);
       		    } 
		    else
		    {
			totalUnclustered++;
		    }
  	            network.insert(pair<int,int>(node,ch));
		    //myfile << node << " will send to " << ch << "\n";
		}
		//clusterHeads.push_back(0);
		sort (clusterHeads.begin(),clusterHeads.end());
		clusterHeads.erase (unique(clusterHeads.begin(),clusterHeads.end()),clusterHeads.end());
		totalRSSI /= nLinks;
		//myfile << "Cluster Heads are : "<< "\n";
		//for (int i = 0 ; i < clusterHeads.size(); i++) myfile << " " <<  clusterHeads[i];
		//myfile << "\n";
	}

	string MOPSOProblem :: returnConfiguration(vector<double> config)
	{
		//myfile << "returnConfiguration \n";
		string configuration;		
		stringstream out;

		clusterTheNetwork(config);

		dijkstra();

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
             
		clusterHeads.push_back(0);
		sort (clusterHeads.begin(),clusterHeads.end());
		clusterHeads.erase (unique(clusterHeads.begin(),clusterHeads.end()),clusterHeads.end());

		for (int i = 0 ; i < clusterHeads.size() ; i++)
		{
		    out << clusterHeads[i] << ",";

		    for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
       		    {
			if(iter->second == clusterHeads[i] )
			{
				out << iter->first << " ";
			}
		    }
		    out << ";";
		}
		configuration = out.str();
			
		return configuration;
	}

	
	double MOPSOProblem :: calculateEnergyRatio()
	{
		double totalEnergy = 0;

		for (int i = 0 ; i < clusterHeads.size() ; i++)
		{
		    	int ch = clusterHeads[i];
			totalEnergy+= (18720/adjacencyMatrix[ch][ch]);
		}

		return clusterHeads.size()/totalEnergy; 
	}
	
	
	double MOPSOProblem :: dijkstra()
        {
           double dCost = 0;
	   Dijkstra G;
           G.read(adjacencyMatrix , clusterHeads);
    	   G.calculateDistance();
           dCost = G.output();
	   routes = G.returnRoutes();
	   //myfile << "Routes are " << routes << "\n";	
           //myfile << "IN dijkstra " << dCost << "\n";
	   return dCost;
   	}

	
	void MOPSOProblem :: evaluate(Solution *solution) 
	{
	    // myfile << "Evaluate solution \n";
	     double fitness;
	     Variable ** decisionVariables  = solution->getDecisionVariables();

	     int m = numberOfVariables_;

	     vector<double> configuration(m);

             for (int i = 0 ; i < m; i++) 
	     {
			
    			configuration[i] = decisionVariables[i]->getValue();

 	     }

	      clusterTheNetwork(configuration);

	       //myfile << "Cluster heads are: ";
	    //   for (int i = 0 ; i < clusterHeads.size(); i++) 
	     //{
    			//myfile << clusterHeads[i] << " ";
 	    // }
	    // myfile << "\n";
	      
	      double ch = clusterHeads.size();
	      double co = totalUnclustered;
	      double lq = totalRSSI;//getCompatness();
	      double en = calculateEnergyRatio();
	      double tc = dijkstra();
	      	
	    //  myfile << "dijkstra = " << tc << "\n";
              solution->setObjective(0,ch);
	      solution->setObjective(1,co);
	      solution->setObjective(2,lq);
	      solution->setObjective(3,en);
	      solution->setObjective(4,tc);
              

	      totalRSSI = 0;
	      totalUnclustered = 0;
              network.clear();
	      clusterHeads.clear();	
	  }


