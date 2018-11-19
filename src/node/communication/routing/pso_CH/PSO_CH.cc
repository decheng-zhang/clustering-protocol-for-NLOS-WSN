#include "PSO_CH.h"
#include <assert.h>
Define_Module(PSO_CH);

void PSO_CH::startup()
{	
	/*--- The .ned file's parameters ---*/
	isSink = par("isSink");
	slotLength = par("slotLength");	
	percentage = par("percentage");
	roundLength = par("roundLength");
	advPacketSize = par("advPacketSize");
	joinPacketSize = par("joinPacketSize");
	tdmaPacketSize = par("tdmaPacketSize");
	dataPacketSize = par("dataPacketSize");
	numberOfRounds = par("numberOfRounds");
	applicationID = par("applicationID").stringValue(); 
         sensingRange = par("sensingRange");
	/*--- Class parameters ---*/
	roundNumber=0;

	neighbours = "";
	candidates = "";
	networkInformation = " ";

	networkInfo.clear();	
	clusterMembers.clear();
	eligibleNodes.clear(); 
	
	isCH = false;
	isCM = false;
	endFormClus = false;

	myCH = 0;
	myTDMATurn = 0;
	clusterLength = 0;

	noLiveNodes = 0;

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");
	

	initialEnergy = getParentModule()->getParentModule()->getSubmodule("ResourceManager")->par("initialEnergy");
	
	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;
		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
		}
		adjacencyMatrix.push_back(r);
	}
	Sensors.clear();
	DEM.clear();
	if(isSink){

	   DEM = vector<vector<double>>(20, vector<double> (20,0.0));
	   loadDEMData();
	   //Sensors = vector<SensorInfo> (100);
	   //srand (time(NULL));
	   updateSensorInfo();
	}
	declareOutput("PDR");
	declareOutput("Convergence");
	declareOutput("Average number of CHs per round");
	declareOutput("Number of unclustered nodes");
      	declareOutput("Average number of unclustered nodes per round");
	collectOutput("Average number of active nodes per round");
	declareOutput("Number of data packets received at BS");
	declareOutput("Coverage Redundancy of CHs per round");
	
	readXMLparams();
	setTimer(START_ROUND,0.0);
};
void PSO_CH :: updateSensorInfo(){
  cModule *n, *c;
  double x1,y1,z1;
  ResourceManager *r;
  VirtualMobilityManager *l ;
 	for (int i = 0 ; i < networkSize ; i++)
	{
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));

		x1 = l->getLocation().x;
		y1 = l->getLocation().y;
		z1 = l->getLocation().z;

  		//dzhang no time, so just plugin in.
  assert(std::abs(sensingRange - 20) < 0.001);
  SensorInfo tmp_sen = SensorInfo();
  tmp_sen.id = i;
  tmp_sen.x = x1;
  tmp_sen.y = y1;
  tmp_sen.z = z1;
  tmp_sen.sensorRadius = sensingRange;
  Sensors.push_back(tmp_sen);

	}

};
void PSO_CH :: loadDEMData()
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
};

void PSO_CH :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		PSOPacket *netPacket = new PSOPacket("PSO-CH routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setPSOPacketKind(PSO_DATA_PACKET);
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

	
};

void PSO_CH :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	PSOPacket *netPacket = dynamic_cast <PSOPacket*>(pkt);

	if (!netPacket)	return;

	switch (netPacket->getPSOPacketKind()) 
	{

		case PSO_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData9 info = netPacket->getInfo();
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

		case PSO_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				int source = atoi(netPacket->getSource());
				//actualMembers.push_back(source);
				trace() << "CH, Aggregate Data Frame from "<< source << "\n";
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
				bufferAggregate.push_back(*netPacket);	
			}
			else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink) 
			{
		//trace() << "SINK, Processing Data Packet from "<< atoi(netPacket->getSource()) <<"\n";
				int n = netPacket->getNumOfPackets();
				trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
			}
			break;	
		}
	}
};


void PSO_CH :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		par("neighbours") = "";
		par("numOfNbrs") = 0;
		par("energy") = initialEnergy - resMgrModule->getSpentEnergy();
	
		trace() << "PSO_CH - START ROUND NO. " << roundNumber << "\n";
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
		nodeData9 info;
		info.id = self;
		info.energy = par("energy").doubleValue();
		PSOPacket *crtlPkt = new PSOPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setPSOPacketKind(PSO_FIND_NEIGHBOURS_PACKET);
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

		  PSOProblem1 * p = new PSOProblem1(adjacencyMatrix);
		  problem = p;
		
		  algorithm = new StandardPSO2011(problem);

		  // Algorithm parameters
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
		 
		  double numberRounds = numberOfRounds;
		  collectOutput("Average number of CHs per round","",numberOfCHs/numberRounds);
			
		  Variable ** decisionVariables  = candidateSolution->getDecisionVariables();

		  trace() << "Cluster heads are  ";
	          for (int i = 0 ; i < numberOfCHs ; i++) 
		  {
			ClusterHeads[i] = (int)decisionVariables[i]->getValue() ;
			trace() << ClusterHeads[i] << " "; 
		  }
 	   	  trace() << "\n";

		  //Dzhang coverage solver
		  WCoverage * coverageSolver = new WCoverage( Sensors, DEM);
		  double  redundancyOfCHsInThisRound =coverageSolver->evaluateCoverageRedun(ClusterHeads);
		  delete coverageSolver;
		  collectOutput("Coverage Redundancy of CHs per round", "", redundancyOfCHsInThisRound );

		  string configuration = p->returnConfiguration(ClusterHeads);
		  par("networkInformation") = configuration;

		  trace() << " Network Info " << configuration << "\n";

		  //		  delete mutation;
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
	
	case TEST:
	{
		break;
	}
	
    }
}

void PSO_CH :: initProtocol()
{

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

	float distanceThreshold = d0 *	pow(10.0,(maxTxPower - signalDeliveryThreshold - PLd0 + 3 * sigma) / (10.0 * pathLossExponent));

	cModule *n, *n1; 
	cModule *c, *c1;
	ResourceManager *r, *r1; 
	VirtualMobilityManager *l, *l1;

	for (int i = 0 ; i < networkSize ; i++)
	{
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));

		x1 = l->getLocation().x;
		y1 = l->getLocation().y;
		z1 = l->getLocation().z;

		distFar = false;

		trace()<<"Sensor " << i << " x = " << x1 << " y = "<< y1 << " z1 = "<< z1 << "\n";

		for (int j = 0 ; (j < networkSize) & (!distFar) ; j++)
		{

			n1 = theSNModule->getSubmodule("node",j);
			r1 = check_and_cast<ResourceManager*>(n1->getSubmodule("ResourceManager"));
			l1 = check_and_cast<VirtualMobilityManager*>(n1->getSubmodule("MobilityManager"));

			x2 = l1->getLocation().x;
			y2 = l1->getLocation().y;
			z2 = l1->getLocation().z;

			dist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
			

			if ( i == j ) PLd = 0; // Pathloss to self = 0
			else
			{
				if (dist > distanceThreshold) distFar = true;

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
			double energy = c->par("energy").doubleValue(); 
			adjacencyMatrix[i][j] = maxPower - PLd;
			adjacencyMatrix[i][i] = energy;
		}
       	}
		
	/*double totalEnergy;
	networkInfo.clear();
	eligibleNodes.clear();

	noLiveNodes = 0;
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
                trace() << "Node " << i << " \n";
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));

		
		double energy = c->par("energy").doubleValue(); 
		adjacencyMatrix[i][i] = energy;

		const char* nbrs = c->par("neighbours").stringValue();
		trace()<< "neighbors are: " << nbrs << "\n";
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
	}*/
}

void PSO_CH :: runCDDP()
{
  double nonClustered = 0;

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

	if (!isCH && !isCM)
	{
		trace()<<"Iam not clustered!!\n";
		trace()<<"numberOfRounds "<<numberOfRounds<<"\n";
		double numberRounds = numberOfRounds;
		if (!isCH && !isCM )collectOutput("Number of unclustered nodes","",1);
		if (!isCH && !isCM )
		{
			collectOutput("Average number of unclustered nodes per round","",1/numberRounds);
			collectOutput("Average number of active nodes per round","",1/numberRounds);
			setStateSleep(); // I am not clustered
		}
	} 
	//trace() << "My Cluster Length = " << clusterLength <<"\n";

	endFormClus = true;
}


void PSO_CH :: sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		PSOPacket *aggrPacket = new PSOPacket("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setPSOPacketKind(PSO_DATA_PACKET);
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
		trace() << "CH, will buffer aggregated data \n";

		bufferPacket(aggrPacket);
		bufferAggregate.clear();
	}
}

void PSO_CH :: updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);

		par("numOfNbrs") = neighborTable.size();
}


void PSO_CH :: processBufferedPacket()
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
		PSOPacket *netPacket = dynamic_cast <PSOPacket*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}
	
	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
	
}

void PSO_CH :: setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void PSO_CH :: setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void PSO_CH :: setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void PSO_CH :: levelTxPower(int linkBudget)
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

void PSO_CH :: readXMLparams()
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

void PSO_CH :: finishSpecific()
{
  DEM.clear();
  Sensors.clear();
  
}


