#include <Clustering.h>

/**
 * Constructor.
 * Creates a new instance of the Clustering problem.
 * @param solutionType The solution type must "Real" or "BinaryReal
 */
Clustering::Clustering(string solutionType, int numOfCHs, vector<NodeControlInfo> networkInfo, vector<NodeControlInfo> e_Nodes, vector<vector<double> > adjacency) 
{
	numberOfVariables_   = 5;
	numberOfObjectives_  = 2;
	numberOfConstraints_ = 2;
	problemName_ 	     = "CLUSTERING";

	x_ = new double[numberOfVariables_];
	fx_ = new double[numberOfObjectives_] ;

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
	////myfile << "Node "<< j << " is nbr with "<< i << " with RSSI = "<<  adjacency[i][j] <<"\n";
		}
	}
    			

	lowerLimit_ = new double[numberOfVariables_];
	if (lowerLimit_ == NULL) 
	{
		cout << "Clustering. Error reserving memory for storing the array of lower limits" << endl;
		exit(-1) ;
	}	
	
	upperLimit_ = new double[numberOfVariables_];
	if (upperLimit_ == NULL) 
	{
		cout << "Clustering. Error reserving memory for storing the array of upper limits" << endl;
		exit(-1) ;
	}
	
  	for (int var = 0; var < numberOfVariables_; var++)
	{
    		lowerLimit_[var] = 0;
   		upperLimit_[var] = (e_networkSize-1);
  	} // for
  
  	if (solutionType.compare("BinaryReal")==0) 
	{
   	 	solutionType_ = new BinaryRealSolutionType(this);
  	} 
	else if (solutionType.compare("Real")==0) 
	{
    		solutionType_ = new RealSolutionType(this);
  	} 
	else 
	{
    		cout << "Error: solution type " << solutionType << " invalid" << endl;
    		exit(-1);
  	}
} // Clustering


/**
 * Destructor
 */
Clustering::~Clustering() {
  delete [] lowerLimit_ ;
  delete [] upperLimit_ ;
  delete solutionType_ ;
} // ~Clustering


/**
 * Evaluates a solution
 * @param solution The solution to evaluate
 */
void Clustering::evaluate(Solution *solution) 
{
	XReal * vars = new XReal(solution);

	for (int i = 0; i < numberOfVariables_; i++)
   	{
	    	x_[i] = (int) vars->getValue(i);
	}


  
} // evaluate

/**
 * Evaluates the constraint overhead of a solution
 * @param solution The solution
 */
void Clustering::evaluateConstraints(Solution *solution) {
  
  
} // evaluateConstraints

