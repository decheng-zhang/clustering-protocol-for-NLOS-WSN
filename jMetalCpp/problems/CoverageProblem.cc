#include <CoverageProblem.h>
using namespace std;

	CoverageProblem :: CoverageProblem(int nnetworkSize, int numberOfCells,  vector< vector<double>> coverageM, vector< double> energyValues) 
	{
		//myfile.open("PSO_CCP");
		networkSize = nnetworkSize;
		numOfCells = numberOfCells;
		numberOfObjectives_  = 1;
		numberOfConstraints_ = 0;
		problemName_         = "COV_Optimization";
		numberOfVariables_   = networkSize;
		maxRedunduncy = 0;
		uncover = 0;
		double total = 0 ;

                 	
		

		for (int j = 0 ; j < numOfCells ; j++)
		{	
				vector<double> c;
				
				for (int i = 0 ; i < networkSize  ; i++)
				{
					c.push_back(0);
                                        
				}
				coverageMatrix.push_back(c);
		}
		double sum = 0;
	        double   monitors = 0;
		for (int i = 0 ; i < numOfCells; i++)
		{
			monitors = 0;
			for (int j = 0 ; j < networkSize ; j++)
			{
				coverageMatrix[i][j] = coverageM[i][j];
				monitors+= coverageMatrix[i][j];
				
			}
			if (monitors > maxRedunduncy) maxRedunduncy = monitors;
			sum+= monitors;
		}
		double Avg = sum / numOfCells;
		//myfile << "Before Optimization, Avg # of sensor monitoring a cell = " << Avg << "\n";
		//myfile << "maxRedunduncy " << maxRedunduncy << "\n";

                for (int j = 0 ; j < networkSize ; j++)
		{	
			 energyVector.push_back(0);

			energyVector[j] = energyValues[j];
		}

		solutionType_ = new BinarySolutionType(this);
		length_       = new int[numberOfVariables_];
  		for (int i = 0; i < numberOfVariables_; i++) length_  [i] = 1 ;
	
			
		

	}

	double CoverageProblem :: computeCoverageEffect()
	{

                double totalEffect = 0; 
		double monitoredBy = 0;
		double monitoredByOld = 0;
		double oldTotalEffect = 0; 	
		double sensorEffect = 0; 
            
	        double nmCells = 0;

		 vector<vector<double> > tmpCoverageMatrix(numOfCells, vector<double>(networkSize,0));

                for (int i = 0 ; i < numOfCells; i++)
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
			  if ((monitoredBy == 0) && (monitoredByOld != 0)) uncover = 1; //myfile<< "was covered, now not \n"; //totalEffect+=100000000;  //This cell was monitored before but now it is not.
                          //myfile<< "totalEffect " << totalEffect << "\n";
		}

	      
              double avgRedunduncy = totalEffect / numOfCells;
              
	     
              return avgRedunduncy;    
                         

	}



	CoverageProblem :: ~CoverageProblem() 
	{
		delete []length_ ;
  		delete solutionType_ ;
               	coverageMatrix.clear();
		sleepNodes.clear();
		energyVector.clear();
		configuration.clear();
                //myfile.close();
  	}

     
	void CoverageProblem :: evaluate(Solution *solution) 
	{
            Binary * variable ;
  	    double numOfActiveSensors  = 0;
	    double fitness = 0;
	    double totalEnergy = 0;
            double avgEnergy = 0;
	    uncover = 0;
	    sleepNodes.clear();
	    configuration.clear();

   	    for (int i = 1; i < numberOfVariables_ ; i++)
	    {
     		 variable = (Binary *)(solution->getDecisionVariables()[i]) ;
                 if (variable->getIth(0))
		 {
		   configuration.push_back(1);
                   totalEnergy+= energyVector[i];
		   numOfActiveSensors++;

                }
		else  
		{
			configuration.push_back(0);
		          sleepNodes.push_back(i);
			  //myfile << "value is 0 ";
		}

  	    }
               
	  

            double activeNodesRatio = numOfActiveSensors  ;
            double coverageEffect = computeCoverageEffect();
            if (numOfActiveSensors != 0) avgEnergy = totalEnergy / numOfActiveSensors;
	    double scaledAvgEnergy = 1 - avgEnergy;
            double scaledCoverageEffect = coverageEffect/maxRedunduncy;
	    //myfile << "After optimization, coverageEffect = " << coverageEffect << " \n";
            //myfile << "scaledAvgEnergy = " << scaledAvgEnergy << " \n";
	    //myfile << "scaledCoverageEffect = " << scaledCoverageEffect << " \n";
            
            fitness = scaledCoverageEffect + scaledAvgEnergy;
	
	    if (uncover == 1) fitness = 1000000;

	   solution->setObjective(0, fitness);
             
	}

	

	
