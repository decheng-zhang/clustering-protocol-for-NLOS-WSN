#include "PSO_HC.h"
typedef popot::algorithm::ParticleSPSO::VECTOR_TYPE TVector;
typedef CH_Problem ClusteringProblem;

Define_Module(PSO_HC);

void PSO_HC::startup()
{	
	/*--- The .ned file's parameters ---*/
	percentage = par("percentage");
	roundLength = par("roundLength");
	isSink = par("isSink");
	slotLength = par("slotLength");
	advPacketSize = par("advPacketSize");
	joinPacketSize = par("joinPacketSize");
	tdmaPacketSize = par("tdmaPacketSize");
	dataPacketSize = par("dataPacketSize");
	applicationID = par("applicationID").stringValue(); 
	numberOfRounds = par("numberOfRounds");
	swarmSize = par("swarmSize");
	numOfCHs = par("numOfCHs");
	epoch = par("epoch");
	
	/*--- Class parameters ---*/
	
	roundNumber=0;

	candidates = "";
	routes = "";
        neighbours = "";
	numOfNbrs = 0; 
	CH_Frequencey = 0;
	numOfSel = 0;
	networkInformation = " ";	
	secondTier = " ";
	myTDMATurn = 0;
	clusterLength = 0;

	noLiveNodes = 0;	

	neighborTable.clear();
	clusterMembers.clear();
	eligibleNodes.clear();
		
	isCH = false;
	isCM = false;
	isRelayNode = false;
	endFormClus = false;

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");

	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;
		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
		}
		adjacencyMatrix.push_back(r);
	}

	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 
	declareOutput("PDR");


	declareOutput("Convergence");
	declareOutput("Average number of unclustered nodes");
	declareOutput("Number of packets received per cluster head");
	declareOutput("Number of packets received per Sub CH");
      	declareOutput("Average number of CHs per round");
	declareOutput("Number of data packets received at BS");
	//declareOutput("PDR");
	//if (isSink) generateGraph();	
	readXMLparams();
	setTimer(START_ROUND,0.0);
}

void PSO_HC :: fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
	PSO_HC_Packet *netPacket = new PSO_HC_Packet("PSO-HC routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setPSO_HC_PacketKind(PSO_HC_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		// If cluster formation ended and you r not CH, then set dest. to CH and buffer packet
		if (!isCH && endFormClus)
		{
			stringstream buffer;
			buffer << nextHop;
			string dst = buffer.str();
			netPacket->setDestination(dst.c_str());	
			bufferPacket(netPacket);
			trace() << self << " will buffer packet that it will send to"<<dst<<"\n";
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

void PSO_HC :: fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi)
{
	PSO_HC_Packet *netPacket = dynamic_cast <PSO_HC_Packet*>(pkt);

	if (!netPacket) return;
  	
	switch (netPacket->getPSO_HC_PacketKind()) 
	{

		case PSO_HC_FIND_NEIGHBOURS_PACKET:
     		{
			nodeData7 info = netPacket->getInfo();
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

		case PSO_HC_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();

			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				double p = 1 / totalPackets;
				collectOutput("PDR","",p);
				bufferAggregate.push_back(*netPacket);	
			}
			else if (isSubCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{ 
				/*string dstn;
				ostringstream convert;   
  				convert << nextHop;      
  				dstn = convert.str();
				PSO_HC_Packet *fwdPacket = netPacket->dup();
				fwdPacket->setPSO_HC_PacketKind(PSO_HC_DATA_PACKET);
				fwdPacket->setDestination(dstn.c_str());	
				toMacLayer(fwdPacket, resolveNetworkAddress(dstn.c_str()));*/
				tempTXBuffer.push(netPacket->dup());
			}
			else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink)
			{
				int n = netPacket->getNumOfPackets();
				trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				trace() << "Sink Processing Data Packet from "<< netPacket->getSource() <<" \n";
				toApplicationLayer(decapsulatePacket(netPacket));
			}
						
			break;	
		}
		
	}   
}


void PSO_HC :: timerFiredCallback(int index)
{
    switch (index) 
    {

	case START_ROUND:
	{
		//trace() << "PSO_HC - START ROUND NO. " << roundNumber;
		setStateRx();

		par("neighbours") = "";
		par("energy") = 18720 - resMgrModule->getSpentEnergy();

		//setPowerLevel(maxPower);

		if (getTimer(START_SLOT) != 0) 
		{ 
			cancelTimer(START_SLOT);
		}
			
		candidates = "";
		heads.clear();
		neighborTable.clear();
		networkInfo.clear();
		clusterHeads.clear();
		clusterMembers.clear();
		eligibleNodes.clear();
		
		isCH = false;
		isCM = false;
		isSubCH = false;
		isSubCM = false;
		endFormClus = false;
		isRelayNode = false;

		nextHop = 0;
		myRxTurn = 0;
		myTDMATurn = 0;
		clusterLength = 0;
		firstTier = 0;
		noLiveNodes = 0;

		myCH.id = 0;
		myCH.rssi = 0;
	
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
			setTimer(RUN_CDDP,7.0);
			setTimer(ENTER_STEADY_PHASE,8.0);
        	}
				
		//setTimer (TEST, 10.0);

        	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);

		break;
	}
	
	case FIND_NEIGHBORS:
     	{  
		nodeData7 info;
		info.id = self;
		info.energy = par("energy").doubleValue();
		PSO_HC_Packet *crtlPkt = new PSO_HC_Packet("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setPSO_HC_PacketKind(PSO_HC_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setInfo(info);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case RUN_PSO:
	{
		trace() << "In RUN_PSO";
		initProtocol();	
		RNG_GENERATOR::rng_srand();
		RNG_GENERATOR::rng_warm_up();

	    	numOfCHs =  ( noLiveNodes * percentage ) / 100;
		trace() << "Number of CHS = "<< numOfCHs << "\n";

           	ClusteringProblem p(numOfCHs,networkInfo,eligibleNodes,adjacencyMatrix);
  	    
		//Let's create a swarm 
		auto algo = popot::algorithm::spso2011(swarmSize,numOfCHs,
                                         [&p] (size_t index) -> double { return p.get_lbound(index); },
                                         [&p] (size_t index) -> double { return p.get_ubound(index); },
                                         [&p] (double fitness, int epoch) -> bool { return p.stop(fitness, epoch);},
                                         [&p] (TVector &pos) -> double { return p.evaluate(pos.getValuesPtr());}
                                         );


  		algo.run();

  		//algo.generateGraph("connections.dot");
		//trace() << "Best minimum found in " << algo.epoch << " steps \n";
		vector <int> indexes;
		for (int i = 0 ; i < numOfCHs ; i++)
  		{ 
      			int index =  algo.getBest().getPosition()[i];
			indexes.push_back(index);
			//trace()<< "index " << index << "\n"; 
		}
		sort (indexes.begin(),indexes.end());
		indexes.erase (unique(indexes.begin(),indexes.end()),indexes.end());
		
		for (int j = 0 ; j < indexes.size() ; j++)
		{
			int index = indexes[j];
			NodeControlInfo cand = eligibleNodes[index];
			heads.push_back(cand.id);
      			clusterHeads.push_back(cand);
			//trace()<< cand.id << " ";
		}
		
		double m = clusterHeads.size();
		double numberRounds = numberOfRounds;

		collectOutput("Average number of CHs per round","",m/numberRounds);

		//trace() << "\n";
		clusterHeads = p.getTheWholeNetwork(clusterHeads);
		//trace() << "clustered size " << clusterHeads.size() << "\n";

 		string c;		
		stringstream out;

		string c2;		
		stringstream out2;
  		
		//trace() << "PSO_HC candidates are:  \n"; 
		for (int i = 0 ; i < clusterHeads.size() ; i++)
		{
			int cand =  clusterHeads[i].id;
			trace() << cand << " ";
			out << cand << ",";
			//trace() << "Number of members = " << clusterHeads[i].members.size()<<"\n";
			for (int j = 0 ; j < clusterHeads[i].members.size() ; j++)
			{	
				int member = clusterHeads[i].members[j].id;
				//trace() << "member " << member << " ";
		//trace() << "clusterHeads[i].members[j].isSubCH " << clusterHeads[i].members[j].isSubCH << "\n";				
				out << member;
				if (clusterHeads[i].members[j].isSubCH)
				{
					//trace() << "Second tier " << " \n";
					//out << clusterHeads[i].members[j].id << "-";
					out2 << clusterHeads[i].members[j].id << ",";
					for (int y = 0 ; y < clusterHeads[i].members[j].members.size() ; y++)
					{
					//trace() << " " << clusterHeads[i].members[j].members[y].id << " ";
					out2 << clusterHeads[i].members[j].members[y].id << " ";
					out << "-"<< clusterHeads[i].members[j].members[y].id;
					}
					out2 <<";";
					
				}
				out <<" ";
				//trace() << "\n";
				
			}
			//trace()<<"\n";

			out << ";";
		}
		//trace() << "\n";

		c = out.str();
		c2 = out2.str();
		par("networkInformation") = c;
		par("secondTier") = c2;
		trace() << " First tier " << c << "\n";
		trace() << " Second tier " << c2 << "\n";	  
 		break;
	}

	case RUN_CDDP:
	{
		runCDDP();
                break;
	}

	case ENTER_STEADY_PHASE:
	{
		if (!isCH && !isSubCH && !isCM && !isSubCM ) setStateSleep(); // I am not clustered
		
		if (isCH)
		{
			if (clusterMembers.size()!=0)
			{
				setTimer(START_SLOT, clusterLength * slotLength);
			}
			else
			{
				setTimer(START_SLOT, slotLength);	
			}
		}
		else if (isCM || isSubCM || isSubCH)

		{
			setStateSleep();
			setTimer(START_SLOT, myTDMATurn * slotLength);
		}
		break;
	}
    
	case SET_STATE_RX:
	{
		trace() << "Ready to receive now \n";
		setStateRx();
		break;
	}

    	case START_SLOT:
	{
		trace() << self << " In start slot \n";
				
		if (!isCH && !isSubCH && !isCM && !isSubCM ) setStateSleep(); // I am not clustered

		if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
		else setTimer(START_SLOT, clusterLength * slotLength);

		if (isCH) 
		{
			sendAggregate(); 
			processBufferedPacket();
		}

		else if (isCM || isSubCM || isSubCH)
		{
			double subClusterLength = slotLength;
			if (isSubCH) subClusterLength = ((clusterMembers.size() + 2)  * slotLength) ;
			setTimer(END_SLOT, subClusterLength);

			processBufferedPacket();
			
		}
		break;
	}

	case END_SLOT:
	{
		trace() << self << " will sleep now \n";
		setStateSleep();
		break;
	}
	
	case TEST:
	{
		
		break;
	}

   }
}

void PSO_HC :: updateNeighborTable(int nodeID , double rssi , double energy)
{
		NeighborRecord newRec;
		newRec.id = nodeID;
		newRec.rssi = rssi;
		newRec.remainingEnergy = energy;
		neighborTable.push_back(newRec);
		
		par("numOfNbrs") = neighborTable.size();
}


void PSO_HC :: runCDDP()
{
 	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string info = tmpModule->par("networkInformation").stringValue();
	string second = tmpModule->par("secondTier").stringValue();
	
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
					//trace() << "I am a CH !!! \n";
				}
				i++;
			}
			else
			{
				//trace() << "Members are " << d;
				char_separator<char> sep3(" ");
				tokenizer<char_separator<char>> mmbr(d, sep3);
				int j = 0;
				int tdmaTurn = 0;

				for (const auto& m : mmbr) 
				{
					if (self == ch)
					{
						clusterMembers.push_back(stoi(m));
					//trace() << "CH, will add " <<stoi(m) <<" to members \n";
					}
					if ( self == stoi(m) )
					{
							myCH.id = ch;
							myCH.rssi = getRSSI(ch);
							nextHop = ch;
							isCM = true;
							myTDMATurn = tdmaTurn;
						//trace() << " CM, my TDMA is " <<myTDMATurn <<" \n";
					}
					//if (self == 10)trace() << stoi(m) << " has TDMA "<< tdmaTurn <<"\n";
					tdmaTurn++;
					clusterLength = tdmaTurn;
					firstTier++;
					//if (self == 10)trace() << "tdmaTurn++ = " <<  tdmaTurn <<"\n";

					char_separator<char> sep4("-");
					tokenizer<char_separator<char>> smbr(m, sep4);
					//trace() << "m " << m << "\n";
					int x = 0;
					int subCH = 0;
					int sTdmaTurn  = 0;

					for (const auto& sm : smbr) 
					{
						if ( sm != m ) 
						{
							if ( x == 0)
							{
								subCH = stoi (sm);
								if (self == subCH)
								{
									isSubCH = true;
									//trace() << "I am a SubCH !!! \n";
								}
								x++;
							}
							else
							{
								if ( self == subCH )
								{
									clusterMembers.push_back(stoi(sm));
									//trace() << " sm " << sm << "\n";

								}
								if ( self == stoi(sm) )
								{
									//trace() << "My CH is " << subCH <<"\n";
									myCH.id = subCH;
									myCH.rssi = getRSSI(subCH);
									nextHop = subCH;
									isSubCM = true;
									myTDMATurn = tdmaTurn;
									
									//trace() << " s_CM, my TDMA is " <<myTDMATurn <<" \n";
								}
					//if (self == 10) trace() << stoi (sm) << " has TDMA = "<< tdmaTurn;
								tdmaTurn++;
								sTdmaTurn++;
								
							}
						}
								
					}//for sm*/
						
				}//for m
				
			}//else

		}//for d
	}//for t

	//myRxTurn = myTDMATurn;
	
	//if (isSubCH) myTDMATurn += clusterMembers.size();
	//if (isSubCM) myTDMATurn -= 1 ;	

	//trace() << "myRxTurn = " << myRxTurn << "\n";
	//trace() << "My TDMA Turn is " << myTDMATurn ;
	
	if (!isCH && !isSubCH && !isCM && !isSubCM )
	{
		double numberRounds = numberOfRounds;
		collectOutput("Average number of unclustered nodes","",1/numberRounds);
		setStateSleep(); // I am not clustered
	} 

	endFormClus = true;

}

double PSO_HC :: getRSSI(int nbr)
{
	double nbrRSSI;

	for (int i = 0 ; i < neighborTable.size() ; i++)
	{
		if (neighborTable[i].id == nbr)
		{
			nbrRSSI = neighborTable[i].rssi;
			break;
		}

	}
	//trace() << "nbrRSSI "<< nbrRSSI <<"\n";
	return nbrRSSI;
}	

void PSO_HC::finishSpecific()
{
	//if (isSink) trace()<< "neighborTable size "<< neighborTable.size() << "\n";
	/*if (isCH) trace() << " I am a CH \n";
	if (isSubCH)
	{
 		trace()	<< "I am a Sub CH, members are: \n";
		for(int i = 0 ; i < clusterMembers.size() ; i++)
		{
			trace() << "	"<< clusterMembers[i] << "\n";
		}

	}*/
	trace() << "My TDMA Turn is " << myTDMATurn ;
	trace() << "My RX Turn is " << myRxTurn ;
	trace() << "Cluster Length  " << clusterLength;
	trace() << "First Tier " << firstTier; 
}


void PSO_HC::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}



void PSO_HC :: initProtocol()
{
	double totalEnergy;
	networkInfo.clear();
	eligibleNodes.clear();

	noLiveNodes = 0;
	
	for (int i = 0 ; i < networkSize ; i++)
	{
		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;
		
                //trace() << "Node " << i << " \n";
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));

		NodeControlInfo info;
		info.id = i;
		info.isCH = false;
		info.isSubCH = false;
		info.clustered = false;
		info.energy = c->par("energy").doubleValue(); 
		//trace() << "Energy = " << info.energy <<"\n";
		info.numOfNbrs = c->par("numOfNbrs"); 
		//trace() << "Num of nbrs = " << info.numOfNbrs <<"\n";
		int numOfNbrs = c->par("numOfNbrs"); 
                //trace()<< "INIT- numOfNbrs " << numOfNbrs <<"\n";
		info.nbrs.reserve(numOfNbrs);	
		const char* nbrs = c->par("neighbours").stringValue();
		if (i == 0) trace()<< "neighbors are: " << nbrs << "\n";
		//trace()<< i << " neighbors " << nbrs << "\n";
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
			NeighborRecord rec;
                        for (int j = 0 ; j < 3 ; j++)
			{
	  			if (j==0)rec.id = atoi (tok);
				else if (j==1) 	rec.rssi = atof (tok);
				else if (j==2)  rec.remainingEnergy = atof (tok);
				tok = strtok(NULL,",");
			 }
			 //trace() << "token "<< rec.id << "\n";
			 info.nbrs.push_back(rec);
			 adjacencyMatrix[info.id][rec.id] = rec.rssi;
			 //myfile <<"(n"<<i<<") edge node {"<<rec.rssi<<"} (n"<<rec.id<<")\n";
				
		}

		totalEnergy = totalEnergy + info.energy;

		if (info.energy > 5) noLiveNodes++;

                networkInfo.push_back(info);
	}

	double avgEnergy = totalEnergy / networkSize;

	for (int j = 1 ; j < networkSize ; j++)
	{
		if ( networkInfo[j].energy >= avgEnergy)
		{
			eligibleNodes.push_back(networkInfo[j]);
//
		}
	}

	trace() << "Eligable Nodes size = "<< eligibleNodes.size();

 }


void PSO_HC::generateGraph()
{
	cModule *n; 
	cModule *c;
	ResourceManager *r; 
	VirtualMobilityManager *l;
		

	ofstream myfile;
	myfile.open("TiKz-Trace.tex", std::ios::app);

	myfile <<"\\documentclass{article}\n";
	myfile<<"\\usepackage{tikz}\n";
	myfile<<"\\usetikzlibrary{shapes}\n";
	myfile<<"\\usetikzlibrary{arrows}\n";
	myfile<<"\\begin{document}\n";
	myfile<<"\\begin{tikzpicture}\n";
	myfile<<"[scale = .4, auto = left, every node/.style={circle,fill=blue!15}, -latex, bkeg/.style={draw=red!50}]\n";
       	for (int i = 0 ; i < networkSize ; i++)
	{
		n = theSNModule->getSubmodule("node",i);
		r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
		c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
		l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
		myfile<<"\\node (n"<<i<<") at ("<<l->getLocation().x<<","<<l->getLocation().y<<") {"<<i<<"};\n"; 

	}
	
	for (int x = 0 ; x < adjacencyMatrix.size() ; x++)
       {
	for (int y = 0 ; y < adjacencyMatrix[x].size() ; y++)
	{
           if (adjacencyMatrix[y][x] < 0)
	   myfile << "\\draw[bkeg] (n"<<x<<") to (n"<<y<<");\n";
          }
       }
   	 myfile<<"\\end{tikzpicture}\n";
	myfile<<"\\end{document}\n";
	myfile.close();

}

void PSO_HC::sendAggregate()
{
	trace() << self << " will send aggregated packet now \n";
	string dst;
	ostringstream convert;   
  	convert << nextHop;      
  	dst = convert.str();

	if(bufferAggregate.size() != 0)
	{	
		trace()<< "bufferAggregate size > 0 \n";
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		PSO_HC_Packet *aggrPacket = new PSO_HC_Packet("CH agg pkt", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setPSO_HC_PacketKind(PSO_HC_DATA_PACKET);
		aggrPacket->setSource(SELF_NETWORK_ADDRESS);
		aggrPacket->setDestination(SINK_NETWORK_ADDRESS);
		trace()<< "buffer size = "	<<	bufferAggregate.size() <<"\n";
		aggrPacket->setNumOfPackets(bufferAggregate.size());

		ApplicationPacket *newPacket = new ApplicationPacket("App gen pkt", APPLICATION_PACKET);
		newPacket->setData(0);
		newPacket->getAppNetInfoExchange().destination = string("0");
		newPacket->getAppNetInfoExchange().source = selfAddress;
		newPacket->getAppNetInfoExchange().timestamp = simTime();
		newPacket->setApplicationID(applicationID.c_str());
		encapsulatePacket(aggrPacket, newPacket);

		bufferPacket(aggrPacket);
		bufferAggregate.clear();
	}
}

void PSO_HC::processBufferedPacket()
{
	string dst;
	ostringstream convert;   
  	convert << nextHop;      
  	dst = convert.str();
		
	trace() << self << " will send packets now to " << dst << "\n";
	while (!tempTXBuffer.empty())
	{
		cPacket *pkt = tempTXBuffer.front();	
		PSO_HC_Packet *netPacket = dynamic_cast <PSO_HC_Packet*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}

	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		trace() << "Sending \n";
		TXBuffer.pop();
	}	
}

void PSO_HC::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void PSO_HC::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void PSO_HC::levelTxPower(int linkBudget)
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

void PSO_HC::readXMLparams()
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

