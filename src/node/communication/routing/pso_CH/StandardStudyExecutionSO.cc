#include <StandardStudyExecutionSO.h>

StandardStudyExecutionSO::StandardStudyExecutionSO(vector<vector<double> > adjacencyM) 
{
		networkSize = adjacencyM.size();

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

}

Algorithm * StandardStudyExecutionSO::algorithmSettings(string problemName,int algorithmId, int experimentIndividualId) 
{
  Algorithm * alg;

  switch (algorithmId) 
  {
  case 0:
     algorithmSettingsList_[experimentIndividualId] =  new PSO_Settings(problemName,networkSize,adjacencyMatrix);
    alg = (algorithmSettingsList_[experimentIndividualId])->configure();
    break;
  case 1:
    algorithmSettingsList_[experimentIndividualId] =  new ssGA_Settings(problemName,networkSize,adjacencyMatrix);
    alg = (algorithmSettingsList_[experimentIndividualId])->configure();
    break;
  case 2:
    algorithmSettingsList_[experimentIndividualId] =  new DE_Settings(problemName,networkSize,adjacencyMatrix);
    alg = (algorithmSettingsList_[experimentIndividualId])->configure();
    break;
   }

  return alg;
} // algorithmSettings



