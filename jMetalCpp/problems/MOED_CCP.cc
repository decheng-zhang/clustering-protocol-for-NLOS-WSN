#include <MOED_CCP.h>

	MOED_CCP :: MOED_CCP(vector< vector<double>> coverageM, vector< vector<double>> adjacencyM, vector<SensorInfo> sensors)
	{
		networkSize = sensors.size();
		numberOfObjectives_  = 2;
		numberOfConstraints_ = 0;
		numOfCubes = coverageM.size();

		problemName_         = "MOED_CCP";
			
		numberOfVariables_   = networkSize;

		for (int i = 0 ; i < numOfCubes ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}
			coverageMatrix.push_back(r);
		}

		for (int i = 0 ; i < numOfCubes ; i++)
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
			status.push_back(0);
			sleepNodes.push_back(0);
			activeNodes.push_back(0);
			clusterHeads.push_back(0);
			adjacencyMatrix.push_back(r);
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

	void MOED_CCP :: clusterTheNetwork()
	{
		network.clear();
				
		for (int i = 1 ; i < networkSize ; i++)
     		{
			double maxDistance = 10000;
			int cHead = 0;

			for (int j = 0; j < clusterHeads.size() ; j++)
       			{
				int node2 = i;
				int node1 = clusterHeads[j];

				double dist = adjacencyMatrix[node1][node2];

				if ( dist < maxDistance)
       				{
					maxDistance = dist;
       					cHead = clusterHeads[j];
				}
			}

                          network.insert(pair<int,int>(i,cHead));
		}

	}

	double MOED_CCP :: getUncovered()
	{
	 	vector<vector<double> > tmpCoverageMatrix(numOfCubes, vector<double>(networkSize,0));
		double numberOfCoveredCubes=0;

                for (int i = 0 ; i < numOfCubes; i++)
		{
			bool isCovered = false;

			for (int j = 1 ; j < networkSize ; j++)
			{
				tmpCoverageMatrix[i][j] = coverageMatrix[i][j];
				if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpCoverageMatrix[i][j] = 0;
				if (tmpCoverageMatrix[i][j] == 1) 
				{
					isCovered = true;
				}
                         }
			 if (isCovered) numberOfCoveredCubes++;
		}

              double numberOfUncoveredCubes =  numOfCubes - numberOfCoveredCubes;
	      return numberOfUncoveredCubes;    
 	}


	double MOED_CCP :: getTotalEnergy()
	{
		double totalEnergy;
		double d0 = 5;
		double AE;
		double packetLength = 9;
		double E_elec = 20;
		double E_fs = 10;
		double E_mp = 0.0013;
		double E_da = 5;
		double E_rx = packetLength *  E_elec ;
		double E_activation = 5.0 * 0.000000001;

		for (int i = 0; i < clusterHeads.size() ; i++)
       		{
			int node1 = clusterHeads[i];
			for (int j = 1 ; j < networkSize ; j++)
     			{
				int node2 = j;
				
				
				 for (multimap<int,int>::iterator iter = network.begin(); iter != network.end(); iter++)
		   		 {
					if ((iter->second == clusterHeads[i]) && ( iter->first == j))
					{
						double dist = adjacencyMatrix[node1][node2];
						if (dist < d0)	totalEnergy+= (packetLength *  E_elec) + E_da +  (E_fs * packetLength *  pow(dist,2));
						else if (dist >= d0) totalEnergy+=(packetLength *  E_elec) + E_da + (E_mp * packetLength *  pow(dist,4));
						if (status[j]==1) AE+= E_activation;
					}
					
		    		 }
			}
			double distToBS = adjacencyMatrix[0][node1];
			if (distToBS < d0)	totalEnergy+= (packetLength *  E_elec) +  (E_fs * packetLength *  pow(distToBS,2));
			else if (distToBS >= d0) totalEnergy+=(packetLength *  E_elec) +  (E_fs * packetLength *  pow(distToBS,4));
			
		}
		totalEnergy+=  E_rx;
		if (clusterHeads.size() == 0) totalEnergy = 10000000000;
		return totalEnergy;
	
 	}


	
	
	MOED_CCP :: ~MOED_CCP() 
	{
		delete [] lowerLimit_ ;
  		delete [] upperLimit_ ;
  		delete solutionType_ ;
		network.clear();
		adjacencyMatrix.clear();
  	}

	
	void MOED_CCP :: evaluate(Solution *solution) 
	{
	     double totalEnergy = 0;
	    
	     uncover = 0;
	     status.clear();
	     network.clear();
	     clusterHeads.clear();
	     sleepNodes.clear();
	     activeNodes.clear();
	     configuration.clear();

	     Variable ** decisionVariables  = solution->getDecisionVariables();

	     for (int i = 1; i < numberOfVariables_ ; i++)
	     {
     		 double sts =  ceil ( decisionVariables[i]->getValue() * 3 ) ;
		 status[i] = sts;

                 if (sts == 2)
		 {
		   	totalEnergy+= 18720 / Sensors[i].energy;
			clusterHeads.push_back(i);
	         }

		 if (sts == 1)
		 {
		   	activeNodes.push_back(i);
			totalEnergy+= 18720 / Sensors[i].energy;
	         }

		 if (sts == 0)
		 {
		   	 sleepNodes.push_back(i);
	         }
	    }


	    clusterTheNetwork();
            double energyValue = getTotalEnergy();
	    double uncovered = getUncovered();

	    solution->setObjective(0,energyValue);
	    solution->setObjective(1,uncovered);
	   
	      
	  }


