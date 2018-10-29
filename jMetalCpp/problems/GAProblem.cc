#include <GAProblem.h>

GAProblem :: GAProblem()
{



}


GAProblem :: GAProblem(double numOfCHs,vector<nodeInfo> networkInfo,vector<nodeInfo> e_Nodes) 
{
	   numberOfVariables_   = (int) numOfCHs;
	numberOfObjectives_  = 1;
	numberOfConstraints_ = 0;
			
	networkSize = networkInfo.size();
	e_networkSize = e_Nodes.size();
		
		lowerLimit_ = new double[numberOfVariables_];
		upperLimit_ = new double[numberOfVariables_];
		
		for (int var = 0; var < numberOfVariables_; var++)
		{
    			lowerLimit_[var] = 0.0;
    			upperLimit_[var] = e_networkSize - 1;
  		} 
		solutionType_ = new RealSolutionType(this) ;

		for(int x = 0 ; x < networkSize ; x++)
    		{
			networkInf.push_back(networkInfo[x]);
		}

		for (int y = 0 ; y < e_networkSize ; y++)
		{
			eligibleNodes.push_back(e_Nodes[y]);
		}
		
	}

	GAProblem :: ~GAProblem() 
	{
		delete [] lowerLimit_ ;
  		delete [] upperLimit_ ;
  		delete solutionType_ ;
  	}

	vector<nodeInfo> GAProblem :: clusterTheNetwork(vector<nodeInfo> candidates)
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
		}	
		
		vector<nodeInfo> clustered(candidates);
     		return clustered;
	}

	double GAProblem :: getTotalDistance()
  	{
     		double distToBS = 0;
		double totalDistance = 0;

		for (int i = 0; i < candidates.size() ; i++)
     		{
      			double distSum = 0;
			if (candidates[i].members.size() != 0)
       			{
       				for (int j = 0 ; j < candidates[i].members.size() ; j++)
       				{
         double dist = sqrt (pow ((candidates[i].x - candidates[i].members[j].x),2) + pow ((candidates[i].y - candidates[i].members[j].y),2) );
               			 
				distSum += pow(dist,2);
	  			}
       
				
     			}
			else
			{
				distSum = 0;

			}

		  distToBS = sqrt (pow ((candidates[i].x - networkInf[0].x),2) + pow ((candidates[i].y - networkInf[0].y),2) );
		  totalDistance += distSum + pow(distToBS,2);
		}
		return totalDistance;
	}

	void GAProblem ::  evaluate(Solution *solution) 
	{
		XReal * vars = new XReal(solution);
                int m = numberOfVariables_;

		candidates.clear();
		for (int i = 0; i < numberOfVariables_; i++)
		{
    			int index = (int) vars->getValue(i);
			nodeInfo ch = eligibleNodes[index];
			candidates.push_back(ch);
		}
		 int n = candidates.size();

	          candidates = clusterTheNetwork(candidates);
		  double fitness = getTotalDistance();

		  solution->setObjective(0,fitness);
  	}


