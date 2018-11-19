#include <NSGA_II_CCP_3D_CO.h>
#include <assert.h>
Define_Module(NSGA_II_CCP_3D_CO);

void NSGA_II_CCP_3D_CO::startup()
{	
	isSink = par("isSink");
	slotLength = par("slotLength");	
	percentage = par("percentage");
	roundLength = par("roundLength");
	advPacketSize = par("advPacketSize");
	joinPacketSize = par("joinPacketSize");
	tdmaPacketSize = par("tdmaPacketSize");
	dataPacketSize = par("dataPacketSize");
	numberOfRounds = par("numberOfRounds");
	sensingRange = par("sensingRange");
	isObstacleAware = par("isObstacleAware");
	applicationID = par("applicationID").stringValue(); 
		
	roundNumber=0;

	neighbours = "";
	candidates = "";
	networkInformation = " ";
	networkStatus = " ";
       	
	networkInfo.clear();	
	clusterMembers.clear();
	eligibleNodes.clear(); 
        CHs.clear();
	
	isCH = false;
	isCM = false;
	isActive = true;
	endFormClus = false;

	myCH = 0;
	myTDMATurn = 0;
	clusterLength = 0;

	noLiveNodes = 0;
	
        theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");
	width = getParentModule()->getParentModule()->getParentModule()->par("field_x");
	length = getParentModule()->getParentModule()->getParentModule()->par("field_y");
	height = getParentModule()->getParentModule()->getParentModule()->par("field_z");
	initialEnergy = getParentModule()->getParentModule()->getSubmodule("ResourceManager")->par("initialEnergy");

	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 
	
	cellWidth = 5;
	DEM.clear();
	visibilityMatrix.clear();
	Sensors.clear();
	Tin_Matrix.clear();
	coverageMatrix.clear();
	
	
	
     	if (isSink)
	{
		initializeMatrices();		
		loadDEMData();
		updateTinMatrix();
		updateSensorsElevation();
		updateVisibilityMatrix();
		generateLNSMPathLossMap();
		updateCoverageMatrix();
		updateAdjacencyMatrix();
		testingFun();
    	}
	  readXMLparams();
	
          declareOutput("PDR");
	  declareOutput("Average Path Loss");
	  // declareOutput("Convergence");
	  declareOutput("Number of unclustered nodes");
      	  declareOutput("Average number of unclustered nodes per round");
	  declareOutput("Average number of CHs per round");
	  declareOutput("Number of data packets received at BS");
	  declareOutput("Coverage Redundancy of CHs per round");
	 setTimer(START_ROUND,0.0);
}
// void NSGA_II_CCP_3D_CO :: updateCoverageMatrix()
// {
//   for (int i=0;i< networkSize; i++){
//     for (int j=0
//   }

// }
void NSGA_II_CCP_3D_CO :: updateCoverageMatrix()
{
  coveringMappingMatrix = vector<vector<int>> (Sensors.size(), vector<int>(0));
  for (int s_idx =Sensors.size()-1;s_idx>=0 ;s_idx--){
  //0 no checked, 1 : checked and valid , 2 : checked and invalid
    vector<vector<int> > sqMatrix (NO_TIN_D, vector<int> (NO_TIN_D, 0));
    // ostringstream os;
    // os<<"x: "<< Sensors[s_idx].x <<"y: "<<Sensors[s_idx].y<<"senrange: "<< Sensors[s_idx].sensorRadius;
    // trace()<< os.str();
  
    updateSingleCoverage(sqMatrix, Sensors[s_idx] );

    for(int i=0;i< sqMatrix.size();i++){
      for(int j = 0;j<sqMatrix[0].size();j++){
	//sqmatrix is row col
	if(sqMatrix[i][j]== 1){
	  auto p = findTinIdFromSqtCoor(j,i);
	  for(auto p_e : p){
	    //Tin is pure (x, y), sensor is pure (x,y)
	    if(coveringTin(Tin_Matrix[p_e], Sensors[s_idx])){
	      coverageMatrix[p_e] += 1;
	      coveringMappingMatrix[s_idx].push_back(p_e);
	      
	    }
  
	  }
	}
	
      }
    }
  }
}


void  NSGA_II_CCP_3D_CO ::  updateSingleCoverage(vector<vector<int>> &board, const SensorInfo sen) 
{
  //  vector<pair<int, int>> rst;
  double r = sen.sensorRadius;
  double s_x = sen.x;
  double s_y = sen.y;
  double s_z = sen.z;
  double cellwidth =(double) width / (double) NO_TIN_D; 
  int tem_j = int(s_x / cellwidth);
  int tem_i = int(s_y /cellwidth);
  bfs(board, tem_i, tem_j, s_x, s_y, sen.sensorRadius); 
   // for(auto row : board){
   //    trace() << "\n";
   //    ostringstream os;
   //    for(auto ele : row){
   // 	//	trace()<<" "<< ele <<" ";
	
   // 	os<< ele <<" ";
   //    }
   //    trace()<< os.str();
   // }

}

void NSGA_II_CCP_3D_CO :: bfs (vector<vector<int>> &board, int row, int col, double coorx, double coory, double sen_range) {
  //row col
  typedef pair<int, int> state;

  queue<state> q;
  const int m =  board.size();
  const int n = board[0].size();
  auto state_is_valid = [&](const state &s) {
    // row and col
    const int x = s.first;
    const int y = s.second;
    if( x < 0 || x >= m || y< 0 || y>= n|| board[x][y] != 0){
      return false;
    }
    return true;
  };
  auto out_of_radius = [&](const state &s  ) {
    const double cellwidth =(double) width / (double) NO_TIN_D; 
    const double x_min_distance =   std::min(std::abs((s.second) * cellwidth - coorx) 
					     , std::abs((s.second+1) * cellwidth - coorx));
    const double y_min_distance =   std::min(std::abs((s.first) * cellwidth - coory) 
					     , std::abs((s.first+1) * cellwidth - coory));
    return (x_min_distance >= sen_range)||(y_min_distance >= sen_range);
  };


  auto state_extend = [&](const state &s ){
    vector<state> ext_result;
    ext_result.clear();
    const int x = s.first;
    const int y = s.second;
    const state new_states[4] = {
      {x-1, y}, {x+1, y}, {x, y-1}, {x, y+1}};
    for(int k=0;k<4;++k){
      if(state_is_valid(new_states[k])){
  //0: un-checked, 1 : checked and valid , 2 : checked and invalid
	if(!out_of_radius(new_states[k])){
	  board[new_states[k].first][new_states[k].second] = 1;
	  ext_result.push_back(new_states[k]);
	} else{
	  board[new_states[k].first][new_states[k].second] = 2;
	}
      } 
	
	
    }
    return ext_result;
   
  };

  state start ={row, col};
  if(state_is_valid (start)){
    board[row][col] = 1;
    q.push(start);
  }
  while(!q.empty()){
    auto cur = q.front();
    q.pop();
    auto new_states = state_extend(cur);
    for(auto s: new_states) q.push(s);
  }
}



void NSGA_II_CCP_3D_CO :: initializeMatrices()
{
		for (int i = 0 ; i < networkSize ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < networkSize ; j++)
			{
				r.push_back(0);
			}

			PLD.push_back(r);
			adjacencyMatrix.push_back(r);
			visibilityMatrix.push_back(r);
			SensorInfo tem_sen;
			Sensors.push_back(tem_sen);
        	}

		for (int i = 0 ; i < NO_TIN_D*NO_TIN_D *2 ; i++)
		{
			coverageMatrix.push_back(0);
         	}

		for (int i = 0 ; i < 20 ; i++)
		{
			vector<double> r;
			for (int j = 0 ; j < 20 ; j++)
			{
                            r.push_back(0);
			}
			DEM.push_back(r);
                }
}

bool NSGA_II_CCP_3D_CO :: areSameCells(double xt,double yt,double xr,double yr)
{

	bool sameCells = false;

	int t_row = (int) (yt / cellWidth);
	int t_column = (int) (xt / cellWidth);

	int r_row = (int) (yr / cellWidth);
	int r_column = (int) (xr / cellWidth);

	if ((r_row == t_row) && (r_column == t_column))
		sameCells = true;

	return sameCells;
}

bool NSGA_II_CCP_3D_CO :: areNeighoubringCells(double xt,double yt,double xr,double yr)
{

	bool neighoubringCells = false;

	int t_row = (int) (yt / cellWidth);
	int t_column = (int) (xt / cellWidth);

	int r_row = (int) (yr / cellWidth);
	int r_column = (int) (xr / cellWidth);

	if (((r_row == t_row) || (r_row = t_row + 1)|| (r_row = t_row - 1)) && ((r_column == t_column) || (r_column = t_column + 1)|| (r_column = t_column - 1)))
		neighoubringCells = true;

	return neighoubringCells;
}

//update the tin matrix utilize the loaded DEM info
void NSGA_II_CCP_3D_CO :: updateTinMatrix()
{
  // int noOfTilehori = 0, noOfTileVerti =0;
  static int tin_id = 0;
  double TIN_D = (double) width / (double) NO_TIN_D;

       for(int i =0 ;i < NO_TIN_D; i++){

	for( int j=0;j < NO_TIN_D; j++){
	  coor3d upperleft = givingElev({j*TIN_D, i*TIN_D, 0});
	  coor3d lowerleft = givingElev({j*TIN_D, (i+1)*TIN_D, 0});
	  coor3d upperright = givingElev({(j+1)*TIN_D, i*TIN_D, 0});
	  coor3d lowerright = givingElev({(j+1)*TIN_D, (i+1)*TIN_D, 0});
	  //the vertices in tin has (x,y)
	  Tin_Matrix.push_back( Tin(tin_id++, upperleft, lowerleft, lowerright));
	  //	  Tin temp = Tin_Matrix.back();
	  //	  trace() << "tin-" << temp.id << " : "<< temp.first.x << " - "<< temp.first.y <<"\n";
	  Tin_Matrix.push_back( Tin(tin_id++, upperleft, upperright, lowerright));
	}
      }
}

void NSGA_II_CCP_3D_CO :: loadDEMData()
{
		ifstream elevationsFile;
 		elevationsFile.open("elevations.txt");
		double elevation = 0.0;

		for (int i = 19 ; i >= 0 ; i--)
		{
			for (int j = 19 ; j >= 0 ; j--)
			{
			     elevationsFile >> elevation;
			     DEM[i][j] =  elevation/10;
			     //trace() << "DEM[" << i <<"]["<<j<<"] = "<<  DEM[i][j] << "\n";
			}
                }

		 elevationsFile.close();
 }


void NSGA_II_CCP_3D_CO :: updateSensorsElevation()
{
		//trace() << "Update Sensors Elevation---\n";
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;

		myfile.open("Elevation.txt");

		Sensors[0].id = 0;
		Sensors[0].x = 0;
		Sensors[0].y = 0;
		Sensors[0].z = height;
		Sensors[0].sensorRadius = sensingRange;
		myfile << "SN.node[" << 0 << "].xCoor = " <<  0 << "\n";
		myfile << "SN.node[" << 0 << "].yCoor = " <<  0 << "\n";
		myfile << "SN.node[" << 0 << "].zCoor = " <<  0 << "\n";

		for (int j = 1 ; j < networkSize ; j++)
		{
		 	n = theSNModule->getSubmodule("node",j);
			r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
			l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
				
			double s_x = l->getLocation().x;
			double s_y = l->getLocation().y;

			int row = (int) (s_y / cellWidth);
			int column = (int) (s_x / cellWidth);

			Sensors[j].id = j;
			Sensors[j].x = s_x;
			Sensors[j].y = s_y;
			Sensors[j].z =  DEM[row][column];
			Sensors[j].sensorRadius = sensingRange;

			myfile << "SN.node[" << j << "].xCoor = " <<  s_x << "\n";
			myfile << "SN.node[" << j << "].yCoor = " <<  s_y << "\n";
			myfile << "SN.node[" << j << "].zCoor = " <<  DEM[row][column] << "\n";

			//trace() << "Sensor no " << j << " is in DEM[" << row << "][" << column << "] \n";

		 }
		 
		 myfile.close();
}

void NSGA_II_CCP_3D_CO :: updateAdjacencyMatrix()
{
	//trace() << "Update Adjacency Matrix---\n";

	float x1, x2, y1, y2, z1, z2, dist, rssi;

	for (int i = 0 ; i < networkSize ; i++)
	{
		x1 = Sensors[i].x;
		y1 = Sensors[i].y;
		z1 = Sensors[i].z;

		for (int j = 0 ; j < networkSize ; j++)
		{	
			x2 = Sensors[j].x;
			y2 = Sensors[j].y;
			z2 = Sensors[j].z;

			dist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
			rssi = maxPower - PLD[i][j];
			//trace() << "RSSI of the link from "<< j << " to " << i << " = " << rssi << "\n";
			adjacencyMatrix[i][j] = rssi;
		}
	}
}

void NSGA_II_CCP_3D_CO :: generateLNSMPathLossMap()
{

	myfile2.open("PathLossMap.txt");
	float x1, x2, y1, y2, z1, z2, dist;
	float PLd;		// path loss at distance dist, in dB
	float bidirectionalPathLossJitter; // variation of the pathloss in the two directions of a link, in dB

	
	float pathLossExponent = 2.4;
	float sigma = 4.0;
	float bidirectionalSigma = 1.0;
	float PLd0 = 55;
	float d0 = 1.0;
	float signalDeliveryThreshold = -100;
	float maxTxPower = 0.0;
	bool distFar;
	double numberOfLinks;

	double avgPathLoss = 0; 
	bool endOfLine = false;

	float distanceThreshold = d0 *	pow(10.0,(maxTxPower - signalDeliveryThreshold - PLd0 + 3 * sigma) / (10.0 * pathLossExponent));

	
	for (int i = 0 ; i < networkSize ; i++)
	{
		x1 = Sensors[i].x;
		y1 = Sensors[i].y;
		z1 = Sensors[i].z;

		distFar = false;

		myfile2 << i << ">";

		for (int j = 0 ; (j < networkSize) && (!distFar) ; j++)
		{
			x2 = Sensors[j].x;
			y2 = Sensors[j].y;
			z2 = Sensors[j].z;

			if ((i == (networkSize - 1)) && (j == (networkSize - 2))) endOfLine = true;

			dist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
			
			if ( i == j ) PLd = 0; // Pathloss to self = 0
			else
			{
				if (dist > distanceThreshold) distFar = true;
				numberOfLinks++;
				if (dist < d0/10.0) 
				{
					PLd = 0;
					bidirectionalPathLossJitter = 0;
				}
				else 
				{
					PLd = PLd0 + 10.0 * pathLossExponent * log10(dist / d0) + normal(0, sigma);
					bidirectionalPathLossJitter = normal(0, bidirectionalSigma) / 2;
				}

				if (maxTxPower - PLd - bidirectionalPathLossJitter >= signalDeliveryThreshold) 
				{
					PLd += bidirectionalPathLossJitter;
				}

				if (maxTxPower - PLd + bidirectionalPathLossJitter >= signalDeliveryThreshold) 
				{
					PLd -= bidirectionalPathLossJitter;
				}

				
			}
			if (isObstacleAware) PLd += visibilityMatrix[i][j]; // // add obstacles effect
			avgPathLoss += PLd;
			adjacencyMatrix[i][j] = maxPower - PLd;
			//trace() << "RSSI of the link from "<< j << " to " << i << " = " << adjacencyMatrix[i][j] << "\n";
			
				PLD[i][j] = PLd;

				myfile2 << j << ":" << PLd;  
				if ((!endOfLine) && (j < (networkSize - 1))) myfile2 <<",";  
				
				 
		}
		myfile2 << "\n";	
	}
	collectOutput("Average Path Loss","",avgPathLoss/numberOfLinks);
	myfile2.close();	
}

void NSGA_II_CCP_3D_CO :: updateVisibilityMatrix()
{
	for (int i = 0 ; i < networkSize ; i++)
	{
		SensorInfo s1 = Sensors[i];
		double x1 = s1.x;
		double y1 = s1.y;
		double z1 = s1.z;

		for (int j = 0 ; j < networkSize ; j++)
		{
			//trace() << "Calculating LOS from sensor " << i << " to sensor " << j <<  "\n";
			SensorInfo s2 = Sensors[j];
                        double x2 = s2.x;
		        double y2 = s2.y;
			double z2 = s2.z;
			double nObstacles = LOS(x1,y1,z1,x2,y2,z2);
			visibilityMatrix[i][j] = nObstacles;
			
		}	
	}
}
 
double NSGA_II_CCP_3D_CO :: LOS(double xt,double yt,double zt, double xr,double yr, double zr)
{
	double v; // Diffraction Parameter
	double totalObstPL = 0;

	//trace() << " Sender coord " << xt << "," << yt << " receiver coord " << xr << "," << yr << "\n";

	bool inSameCell = areSameCells(xt, yt, xr, yr);
	if (inSameCell) { totalObstPL = 0; }
	else
	{
		 //trace() << " Sender and receiever are NOOT in same cell \n";
		totalObstPL = findNextObstacle(xt,yt,zt,xr,yr, zr);
	}
        return totalObstPL;
}

double NSGA_II_CCP_3D_CO :: findNextObstacle(double xt,double yt, double zt , double xr,double yr, double zr) //DDA3 algorithm
{
    int n;
    int A[] = { xt,         yt,         zt         };
    int B[] = { xr,         yr,         zr         };
    int error[] = { 0,         0,         0         };

    int coord[] = { xt,      yt,      zt      };
    int delta[] = { B[0]-A[0], B[1]-A[1], B[2]-A[2] };
    int step[]  = {
        delta[0] >= 0 ? 1 : -1,
        delta[1] >= 0 ? 1 : -1,
        delta[2] >= 0 ? 1 : -1,
    };
    
    delta[0] = delta[0] < 0 ? -delta[0] : delta[0];
    delta[1] = delta[1] < 0 ? -delta[1] : delta[1];
    delta[2] = delta[2] < 0 ? -delta[2] : delta[2]; 
    
    n = delta[0] > delta[1] ? delta[0] : delta[1];
    n = n > delta[2] ? n : delta[2];
    
    int noOfObstacles = 0;
    double PL_Obstacles = 0;
    double maxElevation = zt;
    double waveLength = 0.33;

    vector<Obstacle> Obstacles;
    Obstacles.clear();

    Obstacle first;
    first.id = 0;
    first.x = xt;
    first.y = yt;
    first.z = zt;
    Obstacles.push_back(first);
	
    

    for( int i = 0; i < n; i++ )
    {
        //trace() << "Next Point " << coord[0] << "," << coord[1] << "," << coord[2] <<"\n";
	
	int x = coord[0];
	int y = coord[1];

	bool inSameSenderCell = areSameCells(xt, yt, x, y);
	bool inSameReceiverCell = areSameCells(xr, yr, x, y);

	int row = (int) (y / cellWidth);
	int column = (int) (x / cellWidth);
	double e =  DEM[row][column];

        //trace() << "Next Point " << x << "," << y << " has height =  " << e << "\n";

	
	if ((e > maxElevation) && (!inSameSenderCell) && (!inSameReceiverCell))
	{
		maxElevation = e;
		
		noOfObstacles++;

		Obstacle o;
    		o.id = noOfObstacles;
    		o.x = x;
    		o.y = y;
    		o.z = e;
		Obstacles.push_back(o);

        }     

        // Bresenham error updates
        error[0] += delta[0];
        error[1] += delta[1];
        error[2] += delta[2];    
        
        // check if error in x exceeds threshold, if so update coord and error
        if( (error[0]<<1) >= n ){
            coord[0] += step[0];
            error[0] -= n;
        }
        
        // check if error in y exceeds threshold, if so update coord and error
        if( (error[1]<<1) >= n )
        {
            coord[1] += step[1];
            error[1] -= n;
        }
        
        if( (error[2]<<1) >= n )
	{
            coord[2] += step[2];
            error[2] -= n;
        }
        
    }

    Obstacle last;
    last.id = noOfObstacles + 1;
    last.x = xr;
    last.y = yr;
    last.z = zr;
    Obstacles.push_back(last);

    double PLDObstacles = calculatePLDObstacles(Obstacles);

    //if (noOfObstacles != 0 ) PLDObstacles = PLDObstacles / noOfObstacles;
    //else PLDObstacles = 0;

    // trace() << "Total number of obstacles  = " << noOfObstacles << " PLDObstacles  = " << PLDObstacles <<"\n";
    assert( coord[0] == B[0] && coord[1] == B[1] && coord[2] == B[2] );
    return PLDObstacles;
 }

double NSGA_II_CCP_3D_CO :: calculatePLDObstacles(vector<Obstacle> Obstacles)
{
	double waveLength = 0.33;
	double PL_Obstacles = 0;

 	for (int i = 1 ; i <  Obstacles.size() - 1 ; i++)
    	{

	   double x = Obstacles[i].x;
	   double y = Obstacles[i].y;
	   double z = Obstacles[i].z;

	   double xt = Obstacles[i-1].x;
	   double yt = Obstacles[i-1].y;
	   double zt = Obstacles[i-1].z;


	   double xr = Obstacles[i + 1].x;
	   double yr = Obstacles[i + 1].y;
	   double zr = Obstacles[i + 1].z;

           double d1 = sqrt((xt - x) * (xt - x) + (yt - y) * (yt - y) + (zt - z) * (zt - z));
	   double d2 = sqrt((xr - x) * (xr - x) + (yr - y) * (yr - y) + (zr - z) * (zr - z));

	   //trace() << "d1 " << d1 << " d2 =  " << d2 << "\n";

		//Find v1 = x0-xt
		double xd =  x - xt;
		double yd =  y - yt;
		double zd =  z - zt;

		//Find v2 = x0-xr
		double xd2 =  x - xr;
		double yd2 =  y - yr;
		double zd2 =  z - zr;

		//Find v3 = xr-xt
		double xd3 =  xr - xt;
		double yd3 =  yr - yt;
		double zd3 =  zr - zt;

		//Find v1 x v2
		double cp_x = (yd * zd2) - (zd * yd2);
		double cp_y = (zd * xd2) - (xd * zd2);
		double cp_z = (xd * yd2) - (yd * xd2);

		//Find mag of cp
		double mag1 = sqrt ( pow(cp_x,2) + pow(cp_y,2) + pow(cp_z,2) );
		double mag2 = sqrt ( pow(xd3,2) + pow(yd3,2) + pow(zd3,2) );
		double PDistance =  (mag1 / mag2); 

		//trace() << "PDistance = " << PDistance << "\n";
		double fv, pl;

		if (PDistance != 0)
		{
			//Fresnel-Kirchhoff diffraction parameter
			double v = PDistance * sqrt ((2 * (d1 + d2))/(waveLength * d1 * d2));
			//trace() << "Fresnel-Kirchhoff diffraction parameter = " << v << "\n";

			
 
			if (v > 0.78)
			{
			  	fv = sqrt(pow(v-0.1,2)+1) + v - 0.1;
				pl = 6.9 + 20 * log(fv);
			}
			else pl = 0;
			
			
	
			PL_Obstacles+= pl;
		}
		//trace() << "PL due to obstacle " <<  i  << " = " << pl << "\n";
	}

	return PL_Obstacles;

}


void NSGA_II_CCP_3D_CO :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		NSGA_II_CCP_3D_CO_Packet *netPacket = new NSGA_II_CCP_3D_CO_Packet("NSGA_II_CCP_3D_CO routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setNSGA_II_CCP_3D_CO_PacketKind(NSGA_II_CCP_3D_CO_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		// If cluster formation ended and you r not CH, then set dest. to CH and buffer packet
		if (!isCH && endFormClus)
		{
			stringstream buffer;
			buffer << myCH;
			string dst = buffer.str();
			netPacket->setDestination(dst.c_str());	
			bufferPacket(netPacket);
		}	
		else if (!isCH && !endFormClus) 
		{
			tempTXBuffer.push(netPacket);
		}
		else if (isCH) 
		{
			bufferAggregate.push_back(*netPacket);
		}
	}

	
}

void NSGA_II_CCP_3D_CO :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	NSGA_II_CCP_3D_CO_Packet *netPacket = dynamic_cast <NSGA_II_CCP_3D_CO_Packet*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getNSGA_II_CCP_3D_CO_PacketKind()) 
	{

		case NSGA_II_CCP_3D_CO_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				//trace() << "CH, Aggregate Data Frame from "<< source << "\n";
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
				bufferAggregate.push_back(*netPacket);	
			}
			else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink) 
			{
		//trace() << "SINK, Processing Data Packet from "<< atoi(netPacket->getSource()) <<"\n";
				int n = netPacket->getNumOfPackets();
				//trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
			}
			break;	
		}
	}
}


void NSGA_II_CCP_3D_CO :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		if (getTimer(START_SLOT) != 0) 
		{ 
			cancelTimer(START_SLOT);
		}

		setPowerLevel(maxPower);

		par("energy") = initialEnergy - resMgrModule->getSpentEnergy();

		candidates = "";
		networkInformation = " ";
		networkInfo.clear();
		eligibleNodes.clear();
		clusterMembers.clear();
		clusterHeads.clear();
		actualMembers.clear();
		neighborTable.clear();
		neighbors.clear();

		isCH = false;
		isCM = false;
		endFormClus = false;

		myCH = 0;
		myTDMATurn = 0;
		clusterLength = 0;

		noLiveNodes = 0;

		if (isSink) 
		{ 
	   		setTimer(RUN_PSO,0.0);	 

		}
		else
		{
			setTimer(RUN_CDDP,5.0);
			setTimer(ENTER_STEADY_PHASE,7.0);
        	}

	       	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);
		break;
	}

	case RUN_PSO:
	{
		updateSensorInfo();

		Problem   * problem   ; 
  		Algorithm * algorithm ; 
  		Operator  * crossover ; 
  		Operator  * mutation  ; 
  		Operator  * selection ; 
	

		  //CH_3D_CO * p = // new CH_3D_CO(adjacencyMatrix,Sensors);

	CH_3D_CO * p =  new CH_3D_CO(adjacencyMatrix, Sensors, coverageMatrix,coveringMappingMatrix);
		problem = p;
			
		algorithm = new NSGAII(problem);


		int populationSize = 100;
  		int maxEvaluations = 40000;
		algorithm->setInputParameter("populationSize",&populationSize);
  		algorithm->setInputParameter("maxEvaluations",&maxEvaluations);  
		
		map<string, void *> parameters;

		parameters.clear();
		double mutationProbability = 1.0/problem->getNumberOfBits();
		parameters["probability"] = &mutationProbability;
		mutation = new BitFlipMutation(parameters);

		parameters.clear();
		double crossoverProbability = 0.9;
		parameters["probability"] = &crossoverProbability;
		crossover = new SinglePointCrossover(parameters);

		parameters.clear();
		selection = new BinaryTournament2(parameters) ;

		algorithm->addOperator("crossover",crossover);
		algorithm->addOperator("mutation",mutation);
		algorithm->addOperator("selection",selection);

		SolutionSet * population = algorithm->execute();

		Solution * bestSolution = findBestCompromiseSolution(population);
		Variable ** decisionVariables  = bestSolution->getDecisionVariables();

		double numberRounds = numberOfRounds;
		double nOfActiveSensors=0;

		stringstream out;
		vector<int> sleepNodes;
		vector<int> activeNodes;

		Binary * variable ;
	     	for (int i = 1; i < networkSize ; i++)
	     	{
     		 	variable = (Binary *)(bestSolution->getDecisionVariables()[i]) ;
                	if (variable->getIth(0)){ clusterHeads.push_back(i);}
		}
		
	    	double numberOfCHs = clusterHeads.size();
		trace()<< "Number of CHs per round" << numberOfCHs << "\n";
		
		collectOutput("Average number of CHs per round","",numberOfCHs/numberRounds);
		double redundancyOfCHsInThisRound = evaluateCoverageRedundancy();
		trace()<< "Coverage Redundancy of CHs per round" <<redundancyOfCHsInThisRound << "\n" ;
		collectOutput("Coverage Redundancy of CHs per round", "", redundancyOfCHsInThisRound );
		string networkStatus = returnConfiguration();		
		par("networkInformation") = networkStatus;


		//population->printVariablesToFile("VAR");
		//population->printObjectivesToFile("FUN");

		delete selection;
 		delete mutation;
  		delete crossover;
  		delete population;
  		delete algorithm;
		break;
	}
	
	case RUN_CDDP:
	{
		runCDDP();
		
                break;
	}

	case ENTER_STEADY_PHASE:
	{
		if (!isCH && !isCM ) setStateSleep(); // I am not clustered
	
		if (isCH)
		{
			if (clusterMembers.size()!=0)
			{
				clusterLength = clusterMembers.size();
				setTimer(START_SLOT, clusterLength * slotLength);
			}
			else
			{
				setTimer(START_SLOT, slotLength);	
			}
		}
		else if (isCM)
		{
			setStateSleep();
			setTimer(START_SLOT, myTDMATurn * slotLength);
		}
		break;
	}
    	
	case START_SLOT:
	{
		if (!isCH && !isCM) setStateSleep(); // I am not clustered
		if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
		else setTimer(START_SLOT, clusterLength * slotLength);
				
		if (isCH) 
		{
			sendAggregate(); 
			processBufferedPacket();
		}

		else 
		{
			setPowerLevel(0);
			processBufferedPacket();
			setTimer(END_SLOT, slotLength);
		}
		break;
	}

	case END_SLOT:
	{
		if (!isSink && !isCH) setStateSleep();
		break;
	}
	
	

	
   }
}
double NSGA_II_CCP_3D_CO::  evaluateCoverageRedundancy(){

  int totalCoverage = 0;
  int sizeOfCH  = clusterHeads.size();
  
  for(const auto & ele : coverageMatrix){
    if (ele)   totalCoverage++;
  }
  double totalCoverageRatio =(double) totalCoverage  / (double)coverageMatrix.size();

  
  double coverageRedun=0.0 ;
  if(sizeOfCH != 0){

    for(const auto &ele : clusterHeads){
      if(ele < coveringMappingMatrix.size()){
	auto coveredTinSet = coveringMappingMatrix[ele];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0;
	  double numerator  = 0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 ){
	       throw "Calculaton error, at least covered by self";
	     } else {
	       denominator += 1.0 / (double)coveredTimes ;
	     }
	   }
	  coverageRedun += numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
      }else {
	throw "EA setting problem: sensor id exceed coveragemappingmatrix length!";
      }
    }
    assert( coverageRedun != 0.0);
  } else{
    //everyone is CH
    for(int idx_ch =0;idx_ch < coverageMatrix.size();idx_ch ++){
    auto coveredTinSet = coveringMappingMatrix[idx_ch];
	if(coveredTinSet.size() > 0 ){
	  double denominator = 0.0;
	  double numerator  = 0.0;
	  for( auto & coveredTin:  coveredTinSet ) {
	    numerator ++;
	     int coveredTimes  = coverageMatrix[coveredTin];
	     if(coveredTimes < 1 ){
	       throw "Calculaton error, at least covered by self";
	     } else {
	       denominator += 1.0 /(double) coveredTimes ;
	     }
	   }
	  coverageRedun += numerator/ denominator;
	}else{
	  //does nothing,  no covertin, no coverage redundancy;
	}
    }

}
  int outsideNumerator = (sizeOfCH ==0)? coverageMatrix.size() : sizeOfCH;
    return coverageRedun;

}
string NSGA_II_CCP_3D_CO :: returnConfiguration()
{
		multimap<int,int> network;

		for (int i = 1 ; i < networkSize ; i++)
     		{
			double maxRSSI = -10000;
			int cHead = 0;

			for (int j = 0; j < clusterHeads.size() ; j++)
       			{
				int node2 = i;
				int node1 = clusterHeads[j];

				double rssi = adjacencyMatrix[node1][node2];
				 //trace() << "rssi " << rssi << "\n";
				if (rssi != 0)
				{
					if ( (rssi > maxRSSI) && (node2 != node1 ))
       					{
							maxRSSI = rssi;
       							cHead = clusterHeads[j];
					}
				}
			}

                        if (maxRSSI != -10000)
			{
			  network.insert(pair<int,int>(i,cHead));
			
			}
		}


		stringstream out;
		string configuration;
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
		//trace() << "configuration" << configuration <<"\n";
		return configuration;
	}


Solution * NSGA_II_CCP_3D_CO :: findBestCompromiseSolution(SolutionSet *set)
{

	vector<double> minObjectives(5,1000);
	vector<double> maxObjectives(5,-1000);

	for (int i = 0 ; i < set->size() ; i++)
	{
		Solution * solution = set->get(i);

		for (int j = 0 ; j < solution->getNumberOfObjectives(); j++)
		{
			double objective =  solution->getObjective(j);
			if (objective < minObjectives[j] ) minObjectives[j] = objective;
			if (objective > maxObjectives[j] ) maxObjectives[j] = objective;
		}
	}

	double totalMembership;
	double totalAchievement;
	vector<double> achievement(set->size());

	for (int i = 0 ; i < set->size() ; i++)
	{
		totalMembership = 0;
		Solution * solution = set->get(i);

		for (int j = 0 ; j < solution->getNumberOfObjectives(); j++)
		{
			double membership;

			double objective =  solution->getObjective(j);
			if(std::isinf(objective)){
			    //GDB
			  }
			if(minObjectives[j] == maxObjectives[j]) membership = 0;
			else if (std::isinf(minObjectives[j]) || std::isinf(maxObjectives[j])) membership = 0;
			else if (objective <= minObjectives[j] ) membership = 1;
			else if (objective >= maxObjectives[j] ) membership = 0;
			else membership = (maxObjectives[j] - objective) / (maxObjectives[j] - minObjectives[j]);
			if(std::isnan(membership) ){
			  //NG

			};
			totalMembership+= membership;
						
		}

		achievement[i] = totalMembership;
		totalAchievement += totalMembership;
	}
	

	int index=-1;
	double maxA = -1000;

	for (int i = 0 ; i < set->size() ; i++)
	{
		achievement[i] = achievement[i] / totalAchievement;

		if (achievement[i] - maxA > 0.0)
		{
			maxA = achievement[i];
			index = i;
		}
	
	}
	  //gdb jump in;
	if(index == -1){
	  //gd;
	};


	Solution *best = set->get(index);
	return best;
}

void NSGA_II_CCP_3D_CO :: updateSensorInfo()
{
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
              	n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
				
		double energy = c->par("energy").doubleValue(); 
		Sensors[i].energy = energy;
		//assert(sensingRange ==20);
		//		Sensors[i].sensorRadius= sensingRange; 
	}
}

void NSGA_II_CCP_3D_CO :: runCDDP()
{
	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string info = tmpModule->par("networkInformation").stringValue();
	//trace()<<"NW Info "<< info <<"\n";
	char_separator<char> sep(";");
    	
	tokenizer<char_separator<char>> tokens(info, sep);
    	for (const auto& t : tokens) 
	{
		char_separator<char> sep2(",");
		tokenizer<char_separator<char>> data(t, sep2);
		int i = 0;
		int ch;
		for (const auto& d : data) 
		{
			if (i==0)
			{
				ch = stoi (d);
				if (self == ch)
				{
					isCH = true;
				}
				i++;
			}
			else
			{
				//trace() << "NBRS are " << d;
				char_separator<char> sep3(" ");
				tokenizer<char_separator<char>> mmbr(d, sep3);
				int j = 0;
				for (const auto& m : mmbr) 
				{
					if (self == ch)
					{
						clusterMembers.push_back(stoi(m));
						//trace() << "CH, will add " <<stoi(m) <<" to members \n";
					}
					if ( self == stoi(m) )
					{
						
							myCH = ch;
							isCM = true;
							myTDMATurn = j;
							//trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
						
						
					}
					j++;
					if (myCH == ch ) clusterLength = j;
				}
				
			}

		}
	}
	if (!isCH && !isCM )
		{
			double numberRounds = numberOfRounds;
			collectOutput("Average number of unclustered nodes per round","",1/numberRounds);
			setStateSleep(); // I am not clustered
		}
	endFormClus = true;
}


void NSGA_II_CCP_3D_CO :: sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		NSGA_II_CCP_3D_CO_Packet *aggrPacket = new NSGA_II_CCP_3D_CO_Packet("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setNSGA_II_CCP_3D_CO_PacketKind(NSGA_II_CCP_3D_CO_DATA_PACKET);
		aggrPacket->setSource(SELF_NETWORK_ADDRESS);
		aggrPacket->setDestination(SINK_NETWORK_ADDRESS);
		aggrPacket->setNumOfPackets(bufferAggregate.size());

		ApplicationPacket *newPacket = new ApplicationPacket("App gen pkt", APPLICATION_PACKET);
		newPacket->setData(0);
		newPacket->getAppNetInfoExchange().destination = string("0");
		newPacket->getAppNetInfoExchange().source = selfAddress;
		newPacket->getAppNetInfoExchange().timestamp = simTime();
		newPacket->setApplicationID(applicationID.c_str());
		encapsulatePacket(aggrPacket, newPacket);
		//trace() << "CH, will buffer aggregated data \n";

		bufferPacket(aggrPacket);
		bufferAggregate.clear();
	}
}

void NSGA_II_CCP_3D_CO :: processBufferedPacket()
{
	string dst;
	if (isCH)
	{
		ostringstream convert;   
  		convert << 0;      
  		dst = convert.str();
	}
	else if (isCM)
	{
		ostringstream convert;   
  		convert << myCH;      
  		dst = convert.str();
	}
	while (!tempTXBuffer.empty())
	{
		cPacket *pkt = tempTXBuffer.front();	
		NSGA_II_CCP_3D_CO_Packet *netPacket = dynamic_cast <NSGA_II_CCP_3D_CO_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
	
}

void NSGA_II_CCP_3D_CO :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void NSGA_II_CCP_3D_CO :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void NSGA_II_CCP_3D_CO :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void NSGA_II_CCP_3D_CO :: levelTxPower(int linkBudget)
{
	vector<int>::iterator constIterator;
	for (constIterator = powers.begin();
		constIterator != powers.end();
	        constIterator++ ) {
		if(*constIterator > (linkBudget))
		{
			setPowerLevel(*constIterator);
			break;
		}
	}
}

void NSGA_II_CCP_3D_CO::finishSpecific()
{
	DEM.clear();
	Sensors.clear();
	myfile.close();
	visibilityMatrix.clear();
}


void NSGA_II_CCP_3D_CO :: readXMLparams()
{
	cXMLElement *rootelement = par("powersConfig").xmlValue();
	if (!rootelement) endSimulation();
	cXMLElement* data = rootelement->getFirstChildWithTag("maxPower");
	maxPower = atoi(data->getNodeValue());
	data = rootelement->getFirstChildWithTag("sensibility");
	sensibility = atoi(data->getNodeValue());
	data = rootelement->getFirstChildWithTag("aggrConsumption");
	aggrConsumption = atoi(data->getNodeValue());
	cXMLElementList sources = rootelement->getChildrenByTagName("power");
	for (int s = 0; s < sources.size(); s++)
		powers.push_back(atoi(sources[s]->getNodeValue()));
	if (powers.size() < 1) endSimulation();
	sort(powers.begin(), powers.end());
}



