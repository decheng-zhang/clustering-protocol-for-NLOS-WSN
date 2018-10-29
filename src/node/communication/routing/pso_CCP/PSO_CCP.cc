#include "PSO_CCP.h"

Define_Module(PSO_CCP);

void PSO_CCP::startup()
{	
       
        //--- The .ned file's parameters ---
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
	applicationID = par("applicationID").stringValue(); 
		
	//--- Class parameters ---
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
	
        
	
	//srand (time(NULL));

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");
	width = getParentModule()->getParentModule()->getParentModule()->par("field_x");
	height = getParentModule()->getParentModule()->getParentModule()->par("field_y");
	initialEnergy = getParentModule()->getParentModule()->getSubmodule("ResourceManager")->par("initialEnergy");

	
	cellWidth = 5 ; //sensingRange / sqrt(5);
	distError = ((cellWidth * sqrt(2)) / 2);	
       // trace() << "Distance Error = " << distError << "\n"; 
	numOfCells = pow ((width / cellWidth), 2);
	//trace() << "Number of cells in this network = " << numOfCells << "\n";
	
	 double numberRounds = numberOfRounds;
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;
		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
		}
		adjacencyMatrix.push_back(r);
                energyVector.push_back(0);
        }

	if (isSink) //Initialize Coverage Matrix
	{
		for (int j = 0 ; j < numOfCells ; j++)
		{	
				vector<double> c;
				
				for (int i = 0 ; i < networkSize  ; i++)
				{
					c.push_back(0);
				}
				coverageMatrix.push_back(c);
		}

        	
	}	

	 declareOutput("Before Coverage Control: Average number nodes covering a cell"); //measures redunduncy  
	declareOutput("Before: Coverage Ratio");  
	declareOutput("Before: Maximum Number of Nodes covering a cell");
	declareOutput("Before: Minimum Number of Nodes covering a cell");
	if (isSink)
	{
		double x = cellWidth / 2;
		double y = cellWidth / 2;
		
		double c1_x = 0;
		double c1_y = 0;

		double c2_x = cellWidth;
		double c2_y = 0;

		double c3_x = 0;
		double c3_y = cellWidth;

		double c4_x = cellWidth;
		double c4_y = cellWidth;
		
                cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;

		double total = 0;
		double counter = 0;
		double minNodes = 1000000;
		double maxNodes = -1000000;

               
           for (int i = 0 ; i < numOfCells ; i++)
		{
			 bool isCovered = false;	
			//trace() << "Cell no " << i << " coordinates x = " << x << " y = " << y << "\n";
			counter = 0;
			
			trace() << "Cell  " << i << " center coord is ( " << x << " , " << y <<" ) \n"; 
				trace() << "Cell  " << i << " c1 coord is ( " << c1_x << " , " << c1_y <<" ) \n"; 
				trace() << "Cell  " << i << " c2 coord is ( " << c2_x << " , " << c2_y <<" ) \n"; 
				trace() << "Cell  " << i << " c3 coord is ( " << c3_x << " , " << c3_y <<" ) \n"; 
				trace() << "Cell  " << i << " c4 coord is ( " << c4_x << " , " << c4_y <<" ) \n"; 
				

			for (int j = 0 ; j < networkSize ; j++)
			{
                                coverageMatrix[i][j] = 0;
				double farthest = -1;
				n = theSNModule->getSubmodule("node",j);
				r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
				l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
				
				double s_x = l->getLocation().x;
				double s_y = l->getLocation().y;
				
				trace()<<"sensor " << j << " coord ( " << s_x << " , " << s_y << " ) \n";
                                double dist = sqrt (pow ((x - s_x),2) + pow ((y - s_y),2) ) + distError;
				double dist1 = sqrt (pow ((c1_x - s_x),2) + pow ((c1_y - s_y),2));
				if ( dist1 > farthest) farthest = dist1;
				double dist2 = sqrt (pow ((c2_x - s_x),2) + pow ((c2_y - s_y),2));
				if ( dist2 > farthest) farthest = dist2;
				double dist3 = sqrt (pow ((c3_x - s_x),2) + pow ((c3_y - s_y),2));
				if ( dist3 > farthest) farthest = dist3;
				double dist4 = sqrt (pow ((c4_x - s_x),2) + pow ((c4_y - s_y),2));
				if ( dist4 > farthest) farthest = dist4;

				trace() << "Estimated distance between sensor no " << j <<  " and Cell no  = " << i << " is " <<  dist << "\n";
				trace() << "Farthest distance between sensor no " << j <<  " and Cell no  = " << i << " is " <<  farthest << "\n";
				if (dist != farthest) trace() << "MISMATCH \n";
				//trace() << "Sensing Range = " << sensingRange << "\n";
				if (farthest < sensingRange) 
				{ 
					coverageMatrix[i][j] = 1;
					isCovered = true;
					trace() << "Sensor no " << j << " covers Cell no " << i << "\n";
				}
                                counter+= coverageMatrix[i][j];
				
			}
			
                        if (isCovered) collectOutput("Before: Coverage Ratio","",1/numOfCells);
			if (counter < minNodes) minNodes = counter;
					if (counter > maxNodes) maxNodes = counter;
                        total+= counter;

			x+= cellWidth;
			c1_x+= cellWidth;
			c2_x+= cellWidth;
			c3_x+= cellWidth;
			c4_x+= cellWidth;
 
			if ( x >= width) 
			{
				y+= cellWidth;
				x= cellWidth/2;

			}

			if ( c1_x >= width) 
			{
				c1_y+= cellWidth;
				c1_x= 0;

			}
			
			if ( c2_x > width) 
			{
				c2_y+= cellWidth;
				c2_x= cellWidth;

			}

			if ( c3_x >= width) 
			{
				c3_y+= cellWidth;
				c3_x= 0;

			}

			if ( c4_x > width) 
			{
				c4_y+= cellWidth;
				c4_x= cellWidth;

			}
		}
                double avgRedunduncy = total/numOfCells;
		collectOutput("Before Coverage Control: Average number nodes covering a cell","",avgRedunduncy);
		collectOutput("Before: Minimum Number of Nodes covering a cell","",minNodes/numberRounds);
				collectOutput("Before: Maximum Number of Nodes covering a cell","",maxNodes/numberRounds);
            }

		

	declareOutput("PDR");
	declareOutput("Coverage Ratio");
	declareOutput("Maximum Number of Nodes covering a cell");
	declareOutput("Minimum Number of Nodes covering a cell");
	
	declareOutput("Total Received Packets");
        declareOutput("Total Generated Packets");
	declareOutput("Convergence");
	declareOutput("Number of unclustered nodes");
	declareOutput("Average Number of Cluster Heads per Round");
      	declareOutput("Average number of unclustered nodes");
	declareOutput("Number of data packets received at BS");
	declareOutput("Average number of active nodes per round");
	
	declareOutput("After Coverage Control: Average number nodes covering a cell"); //measures redunduncy
	
	declareOutput("Average number nodes covering a cell per round");

	 readXMLparams();

	setTimer(START_ROUND,0.0);
}


void PSO_CCP :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		PSO_CCP_Packet *netPacket = new PSO_CCP_Packet("PSO_CCP routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setPSO_CCP_PacketKind(PSO_CCP_DATA_PACKET);
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

void PSO_CCP :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	PSO_CCP_Packet *netPacket = dynamic_cast <PSO_CCP_Packet*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getPSO_CCP_PacketKind()) 
	{

		case PSO_CCP_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData info = netPacket->getInfo();
                        int n_id = info.id;
			double n_rssi = rssi;
			double n_remainingEnergy = info.energy;
			updateNeighborTable(n_id,n_rssi,n_remainingEnergy);

			string nbrs = par("neighbours").stringValue();

			stringstream out;
 			string nbrr;
			out <<  nbrs << n_id << "," << n_rssi << "," << n_remainingEnergy << ";";
			nbrr = out.str();
 			par("neighbours") = nbrr;
			break;	
     		}

		case PSO_CCP_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
			double tp = tmpModule->par("totalPackets").doubleValue();
			trace() << "DATA: Round no " << roundNumber << " totalPackets " << tp <<"\n";

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				//trace() << "CH, Aggregate Data Frame from "<< source << "\n";
				double p = 1 / tp;
				collectOutput("PDR","",p);
				collectOutput("Total Received Packets","",1);
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


void PSO_CCP :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		par("neighbours") = "";
		par("numOfNbrs") = 0;
		par("energy") = initialEnergy - resMgrModule->getSpentEnergy();
	
		//trace() << "PSO_CCP - START ROUND NO. " << roundNumber << "\n";
		setStateRx();

		if (getTimer(START_SLOT) != 0) 
		{ 
			cancelTimer(START_SLOT);
		}
			
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
		isActive = true;
		endFormClus = false;

		myCH = 0;
		myTDMATurn = 0;
		clusterLength = 0;

		noLiveNodes = 0;

		for (int i = 0 ; i < networkSize ; i++)
		{
			for (int j = 0 ; j < networkSize ; j++)
			{
				adjacencyMatrix[i][j] = 0;
			}
		}

		double timer = uniform (0.0,5.0);
     
		setTimer(FIND_NEIGHBORS,timer);

		if (isSink) 
		{ 
	   		setTimer(RUN_PSO,6.0);	
			setTimer(OPT_COVERAGE,7.0);	 
		}

		else
		{
			setTimer(RUN_CDDP,8.0);
			setTimer(ENTER_STEADY_PHASE,9.0);
        	}

		//setTimer (TEST, 99.0);*/

        	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);
		break;
	}

	case FIND_NEIGHBORS:
     	{  
		setPowerLevel(0);
		nodeData info;
		info.id = self;
		info.energy = par("energy").doubleValue();
		PSO_CCP_Packet *crtlPkt = new PSO_CCP_Packet("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setPSO_CCP_PacketKind(PSO_CCP_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_PSO:
	{
		  initProtocol();
		  clock_t t_ini, t_fin;

		  Problem   * problem   ; // The problem to solve
		  Algorithm * algorithm ; // The algorithm to use
		  Operator  * mutation  ; // Mutation operator

		  PSOProblem * p = new PSOProblem(adjacencyMatrix);
		  problem = p;
		
		  algorithm = new StandardPSO2011(problem);

		  // Algorithm parameters
		  int swarmSize = 50;
		  int maxIterations = 500;
		  int numberOfParticlesToInform = 3;
		  algorithm->setInputParameter("swarmSize",&swarmSize);
		  algorithm->setInputParameter("maxIterations",&maxIterations);
		  algorithm->setInputParameter("numberOfParticlesToInform", &numberOfParticlesToInform);

		  // Execute the Algorithm
		  SolutionSet * population = algorithm->execute();
		  Solution * candidateSolution = population->get(0);
		  
		  int numberOfCHs = ( 5 * networkSize ) / 100;
		  vector <int> ClusterHeads(numberOfCHs); 
		 
		  Variable ** decisionVariables  = candidateSolution->getDecisionVariables();

		  trace() << "Cluster heads are  ";
	          for (int i = 0 ; i < numberOfCHs ; i++) 
		  {
			ClusterHeads[i] = (int)decisionVariables[i]->getValue() ;
			CHs.push_back(ClusterHeads[i]);
			trace() << ClusterHeads[i] << " "; 
		  }
 	   	  //trace() << "\n";
		 double numberRounds = numberOfRounds;
		  collectOutput("Average Number of Cluster Heads per Round","",numberOfCHs/numberRounds);
		  string configuration = p->returnConfiguration(ClusterHeads);
		  par("networkInformation") = configuration;

		  //trace() << " Network Info " << configuration << "\n";

		  delete mutation;
		  delete population;
		  delete algorithm;
		 
		  break;
	}

	case OPT_COVERAGE:
     	{  

		  clock_t t_ini, t_fin;

		  Problem   * problem   ; // The problem to solve
		  Algorithm * algorithm ; // The algorithm to use
		  Operator  * crossover ; // Crossover operator
		  Operator  * mutation  ; // Mutation operator
		  Operator  * selection ; // Selection operator
		
                
                  CoverageProblem * p = new CoverageProblem(networkSize, numOfCells, coverageMatrix, energyVector);
		  problem = p;

		 algorithm = new gGA(problem);

		// Algorithm parameters
		int populationSize = 100;
		int maxEvaluations = 25000;
		algorithm->setInputParameter("populationSize",&populationSize);
		algorithm->setInputParameter("maxEvaluations",&maxEvaluations);

		map<string, void *> parameters;

		// Mutation operator
		parameters.clear();
		double mutationProbability = 1.0/problem->getNumberOfBits();
		parameters["probability"] = &mutationProbability;
		mutation = new BitFlipMutation(parameters);

		// Crossover operator
		parameters.clear();
		double crossoverProbability = 0.9;
		parameters["probability"] = &crossoverProbability;
		crossover = new SinglePointCrossover(parameters);

		// Selection Operator
		parameters.clear();
		selection = new BinaryTournament2(parameters) ;

		// Add the operators to the algorithm
		algorithm->addOperator("crossover",crossover);
		algorithm->addOperator("mutation",mutation);
		algorithm->addOperator("selection",selection);

		// Execute the Algorithm
		t_ini = clock();
		SolutionSet * population = algorithm->execute();
		t_fin = clock();
		double secs = (double) (t_fin - t_ini);
		secs = secs / CLOCKS_PER_SEC;
        
        	Solution * candidateSolution = population->get(0);

		vector<int> sleepNodes;
		vector<int> status;

                 Binary * variable;
                 stringstream out;
		  double numberOfActiveNodes = 0;
		double number = 0;
        	for (int i = 1 ; i < networkSize ; ++i)
  		{ 
 		   variable = (Binary *)(candidateSolution->getDecisionVariables()[i]) ;
                   if (variable->getIth(0))
		  {
		      status.push_back(1);
		      out <<  1 << ";";
                      number++;
		      if (find(CHs.begin(),CHs.end(),i) == CHs.end())
		      {

				 numberOfActiveNodes++;
			         trace() << " Node " << i << " is active and it is not a CH \n";
		      }
             	  }
		  else  
		  {
			status.push_back(0);
		        sleepNodes.push_back(i);
			out <<  0 << ";";
		  }

                  

  	        }
		 double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
		double tp = numberOfActiveNodes * roundLength * 1; //to Calculate PDR
               	par("totalPackets") = tp;
		collectOutput("Total Generated Packets","",tp);

		double numberRounds = numberOfRounds;
		
				vector<vector<double> > tmpCoverageMatrix(numOfCells, vector<double>(networkSize,0));
	        
               
				double total = 0;
				double minNodes = 1000000;
				double maxNodes = -1000000;

				
			 	for (int i = 0 ; i < numOfCells; i++)
				{
                        		int counter = 0;
					bool isCovered = false;
					
                       			for (int j = 1 ; j < networkSize ; j++)
					{
						tmpCoverageMatrix[i][j] = coverageMatrix[i][j];
						if(std::find(sleepNodes.begin(),sleepNodes.end(),j)!=sleepNodes.end()) tmpCoverageMatrix[i][j] = 0;
						if (tmpCoverageMatrix[i][j] == 1) 
						{
							counter++;
							isCovered = true;
						}
				
					}
					if (counter < minNodes) minNodes = counter;
					if (counter > maxNodes) maxNodes = counter;
					total+= counter; 

					if (isCovered) collectOutput("Coverage Ratio","",1/numOfCells);
 			
				}

                		double avgRedunduncy = total/numOfCells;
				collectOutput("After Coverage Control: Average number nodes covering a cell","",avgRedunduncy/numberRounds);
				collectOutput("Minimum Number of Nodes covering a cell","",minNodes/numberRounds);
				collectOutput("Maximum Number of Nodes covering a cell","",maxNodes/numberRounds);
		
		//collectOutput("Average number of active nodes per round","",numberOfActiveNodes);
                string networkStatus = out.str();
		par("networkStatus") = networkStatus;

		
		//collectOutput("Average number nodes covering a cell per round",avgRedunduncy/numberOfRounds);

       
	// Result messages
	//trace() << "Total execution time: " << secs << "s" << endl;
	//()  << "Variables values have been written to file VAR" << endl;
	//population->printVariablesToFile("VAR");
	//trace()  << "Objectives values have been written to file FUN" << endl;
	//population->printObjectivesToFile("FUN");

		
		  delete mutation;
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
		if ((!isCH && !isCM) || (!isActive))
		{
 			setStateSleep(); // I am not clustered or Redundunt
			trace() << "I am not clustered or redundunt \n";
		}
	
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
		if ((!isCH && !isCM) || (!isActive))
		{
 			setStateSleep(); // I am not clustered or Redundunt
			trace() << "I am not clustered \n";
		}
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

void PSO_CCP :: initProtocol()
{
	double totalEnergy;
	networkInfo.clear();
	eligibleNodes.clear();
	energyVector.clear();

	noLiveNodes = 0;
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
               // trace() << "Node " << i << " \n";
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));

		
		double energy = c->par("energy").doubleValue(); 
		adjacencyMatrix[i][i] = energy;
                double RemainingEnergyRatio = energy/initialEnergy;
		energyVector.push_back(RemainingEnergyRatio);


		const char* nbrs = c->par("neighbours").stringValue();
		//trace()<< "neighbors are: " << nbrs << "\n";
                char* neighbor = const_cast <char*> (nbrs);
		char* rest;
		char* token;
		char* nbr;
		char* ptr = neighbor;
		
		while (token = strtok_r(ptr,";",&rest))// Find each nbr
		{
			nbr = token;
			ptr = rest;
			
			char* tok = strtok(nbr,",");
			double id;
			double rssi;
			double remainingEnergy;

                        for (int j = 0 ; j < 3 ; j++)
			{
	  			if (j==0)	id = atoi (tok);
				else if (j==1) 	rssi = atof (tok);
				else if (j==2)  remainingEnergy = atof (tok);
				tok = strtok(NULL,",");
			}
			
			adjacencyMatrix[i][id] = rssi;
		}
	}
}

void PSO_CCP :: runCDDP()
{
	double nonClustered = 0;
	isActive = true;

  	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string info = tmpModule->par("networkInformation").stringValue();
        string status = tmpModule->par("networkStatus").stringValue();
	
	int pointer = 1;
        char_separator<char> sep1(";");
        tokenizer<char_separator<char>> tokens1(status, sep1);
    	for (const auto& t : tokens1) 
	{
	    if (self == pointer)
            {
	    int myStatus = stoi(t);
	    if (myStatus == 0) isActive = false;
	    trace() << " My node ID is " << self << " My status is " << myStatus << "\n";
            }
            pointer++;
	}


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
					isActive = true;
					
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
					trace() << "CH, will add " <<stoi(m) <<" to members \n";
					}
					if ( (self == stoi(m)) && (isActive))
					{
						
							myCH = ch;
							isCM = true;
							myTDMATurn = j;
						trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
						
						
					}
					j++;
					if (myCH == ch ) clusterLength = j;
				}
				
			}

		}
	}


	//////////////////////
	if (isActive) 
	{
		double numberRounds = numberOfRounds;
		collectOutput("Average number of active nodes per round","",1/numberRounds);

	}
	
	/////////////////////
	endFormClus = true;
}


void PSO_CCP :: sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		PSO_CCP_Packet *aggrPacket = new PSO_CCP_Packet("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setPSO_CCP_PacketKind(PSO_CCP_DATA_PACKET);
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

void PSO_CCP :: updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);

		par("numOfNbrs") = neighborTable.size();
}

void PSO_CCP :: processBufferedPacket()
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
		PSO_CCP_Packet *netPacket = dynamic_cast <PSO_CCP_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
	
}

void PSO_CCP :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void PSO_CCP :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void PSO_CCP :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void PSO_CCP :: levelTxPower(int linkBudget)
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

void PSO_CCP :: readXMLparams()
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



