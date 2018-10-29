#include <CCP_3D.h>

	CCP_3D :: CCP_3D(vector< vector<double>> coverageM, vector< vector<double>> adjacencyM, vector< vector<double>> visibilityM, vector<SensorInfo> sensors)
	{
		networkSize = sensors.size();
		numberOfCells = coverageM.size();
		numberOfObjectives_  = 6;
		numberOfConstraints_ = 0;
		problemName_         = "CCP_3D";
			
		numberOfVariables_   = networkSize;
				
		for (int i = 0 ; i < numberOfCells ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			coverageMatrix.push_back(r);
         	}

		for (int i = 0 ; i < numberOfCells ; i++)
		{
			for (int j = 0 ; j < networkSize ; j++)
			{
				coverageMatrix[i][j] = coverageM[i][j];
			}
		}	

		for (int i = 0 ; i < networkSize ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			sleepNodes.push_back(0);
			adjacencyMatrix.push_back(r);
			visibilityMatrix.push_back(r);
			Sensors.push_back(SensorInfo());
		}

		for (int i = 0 ; i < networkSize; i++)
		{
			Sensors[i].id = sensors[i].id;
			Sensors[i].x = sensors[i].x;
			Sensors[i].y = sensors[i].y;
			Sensors[i].z = sensors[i].z;
			Sensors[i].energy = sensors[i].energy;

			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = adjacencyM[i][j];
				visibilityMatrix[i][j] = visibilityM[i][j];
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

	int CCP_3D :: clusterTheNetwork()
	{
		network.clear();
		int numberOfClusteredNodes = 0;

		for (int i = 1 ; i < networkSize ; i++)
     		{
			double maxRSSI = -10000;
			int cHead = 0;

			for (int j = 0; j < clusterHeads.size() ; j++)
       			{
				int node2 = i;
				int node1 = clusterHeads[j];

				double rssi = adjacencyMatrix[node1][node2];

				if (rssi != 0)
				{
					if ( (rssi > maxRSSI) && (rssi >= -90) && (i != clusterHeads[j]) )
       					{
							maxRSSI = rssi;
       							cHead = clusterHeads[j];
					}
				}
			}

                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(i,cHead));
			  numberOfClusteredNodes = numberOfClusteredNodes + 1;
			}
		}

		return numberOfClusteredNodes;
	}

	double CCP_3D :: getCompactness()
	{
		double maxRSSI = -100;
		double maxCompactness = -100;
		double maxAvgRSSI = 0;
		double totalRSSI = 0;
				
		for (int i = 0 ; i < clusterHeads.size() ; i++)
		{
		    vector<int> members;

		    for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
		    {
			if (iter->second == clusterHeads[i]) members.push_back(iter->first);
		    }

		    double clusterCompactness = 0;

		    for(int j = 0 ; j < members.size() ; j++)
		    {
			int node1 = clusterHeads[i];
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
		
		return maxCompactness;
 	}

	double CCP_3D :: evaluateRedunduncy()
	{

                double totalEffect = 0; 
		double monitoredBy = 0;
		double monitoredByOld = 0;
		double oldTotalEffect = 0; 	
		double sensorEffect = 0; 
            	double nmCells = 0;

		vector<vector<double> > tmpCoverageMatrix(numberOfCells, vector<double>(networkSize,0));

                for (int i = 0 ; i < numberOfCells; i++)
		{
                        monitoredBy = 0;
			monitoredByOld = 0;
			for (int j = 1 ; j < networkSize ; j++)
			{
				tmpCoverageMatrix[i][j] = coverageMatrix[i][j];
				if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpCoverageMatrix[i][j] = 0;
				monitoredBy+=tmpCoverageMatrix[i][j];
				monitoredByOld+=coverageMatrix[i][j];
                         }
			  totalEffect+= monitoredBy;
		          oldTotalEffect+= monitoredByOld;
			  if ((monitoredBy == 0) && (monitoredByOld != 0)) uncover = 1; 
		}

              double avgRedunduncy = totalEffect / numberOfCells;
              return avgRedunduncy;    
 	}

	
	
	CCP_3D :: ~CCP_3D() 
	{
		delete [] lowerLimit_ ;
  		delete [] upperLimit_ ;
  		delete solutionType_ ;
                adjacencyMatrix.clear();
  	}

	
	void CCP_3D :: evaluate(Solution *solution) 
	{
	     double noOfCHs = 0;
	     double totalEnergy = 0;
	     double noActiveNodes = 0;

	     uncover = 0;
	     network.clear();
	     clusterHeads.clear();
	     sleepNodes.clear();
	     configuration.clear();

	     Variable ** decisionVariables  = solution->getDecisionVariables();

	     int m = numberOfVariables_;

	     vector<double> configuration(m);

             for (int i = 1 ; i < m; i++) 
	     {
    			configuration[i] = ceil ( decisionVariables[i]->getValue() * 2 ) ;

			if (configuration[i] == 2) 
			{
				noOfCHs++;
				totalEnergy+= 18720 / Sensors[i].energy;
				clusterHeads.push_back(i);
			}

			if (configuration[i] == 1) 
			{
				noActiveNodes++;
				totalEnergy+= 18720 / Sensors[i].energy;
				
			}

			else  sleepNodes.push_back(i);
 	     }

	      
	     int numberOfClusteredNodes = clusterTheNetwork();
	     int numberOfUnClusteredNodes = (networkSize - 1) - numberOfClusteredNodes;
	     double compactness = getCompactness();
	     double avgRedunduncy = evaluateRedunduncy();
	     double avgRemainingEnergy = totalEnergy;
	    
	     solution->setObjective(0,noActiveNodes);
	     solution->setObjective(1,compactness);
	     solution->setObjective(2,avgRedunduncy);
	     solution->setObjective(3,avgRemainingEnergy);
	     solution->setObjective(4,numberOfUnClusteredNodes);
	      solution->setObjective(5,noOfCHs);
	    
	     		 
	  }


