#include "CPCP.h"

Define_Module(CPCP);

void CPCP::startup()
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
	percentage = percentage /100;
	sensingRange = par("sensingRange");
	initialEnergy = par("initialEnergy");
	/*--- Class parameters ---*/
	CHcandidates.clear();
	clusterMembers.clear();
	roundNumber=0;
	probability = 0;
	isCH = false;
	isCM = false;
	endFormClus = false;
	isCt = false;

	theSNModule = getParentModule()->getParentModule()->getParentModule();
	networkSize = getParentModule()->getParentModule()->getParentModule()->par("numNodes");		
	width = getParentModule()->getParentModule()->getParentModule()->par("field_x");
	height = getParentModule()->getParentModule()->getParentModule()->par("field_y");
	
	double simTime = atof(ev.getConfig()->getConfigValue("sim-time-limit")); 
	totalPackets = networkSize * simTime * 1; 

	cubeWidth = 5 ; //sensingRange / sqrt(5);
	numOfCubes = pow ((width / cubeWidth), 3);

	declareOutput("Coverage Ratio");
	declareOutput("Maximum Number of Nodes covering a cube");
	declareOutput("Minimum Number of Nodes covering a cube");
	declareOutput("Average Number of Nodes covering a cube");


	declareOutput("PDR");
	declareOutput("Total Received Packets");
        declareOutput("Total Generated Packets");
	
	declareOutput("Average number of CHs per round");
      	declareOutput("Average number of active nodes per round");
	declareOutput("Average number of unclustered nodes per round");

	declareOutput("Total # of Active Nodes without CHs");
	
		
	for (int i = 0 ; i < networkSize ; i++)
	{
		vector<double> r;
		for (int j = 0 ; j < networkSize ; j++)
		{
			r.push_back(0);
			minWghtCost.push_back(0);
		}
		adjacencyMatrix.push_back(r);
        }

	if (isSink) //Initialize Coverage Matrix
	{
		for (int j = 0 ; j < numOfCubes ; j++)
		{	
				vector<double> c;
				
				for (int i = 0 ; i < networkSize  ; i++)
				{
					c.push_back(0);
				}
				coverageMatrix.push_back(c);
				energyPerCell.push_back(0.0);
		}

        	
	}

	if (isSink)
	{
		double c1_x = 0;
		double c1_y = 0;
		double c1_z = 0;

		double c2_x = 0;
		double c2_y = 0;
		double c2_z = 0;
	
		double c3_x = 0;
		double c3_y = 0;
		double c3_z = 0;
		
		double c4_x = 0;
		double c4_y = 0;
		double c4_z = 0;

		double c5_x = 0;
		double c5_y = 0;
		double c5_z = 0;

		double c6_x = 0;
		double c6_y = 0;
		double c6_z = 0;
	
		double c7_x = 0;
		double c7_y = 0;
		double c7_z = 0;
		
		double c8_x = 0;
		double c8_y = 0;
		double c8_z = 0;

		double total = 0;
		double counter = 0;
		bool isCovered = false;	
		
		int cubeNumber = 0;
		int cubeCounter = 0;

		cModule *n; 
		cModule *c;
		ResourceManager *r; 
		VirtualMobilityManager *l;

		for (int i = 0 ; i < (int)(width / cubeWidth) ; i++)
		{

		c1_y = 0;
		c2_y = 0;
		c5_y = 0;
		c6_y = 0;			

		c3_y = cubeWidth;
		c4_y = cubeWidth;
		c7_y = cubeWidth;
		c8_y = cubeWidth;

		c5_z+= cubeWidth;
		c6_z+= cubeWidth;
		c7_z+= cubeWidth;
		c8_z+= cubeWidth;
		
		for (int j = 0 ; j < (int)(width / cubeWidth) ; j++)
		{
			 c1_x = 0;
			 c2_x = cubeWidth;
	   		 c3_x = 0;
			 c4_x = cubeWidth;
	   		 c5_x = 0;
	   		 c6_x = cubeWidth;
	  		 c7_x = 0;
	  		 c8_x = cubeWidth;
	
			for (int K = 0 ; K < (int)(width / cubeWidth) ; K++)
			{
				counter = 0;
				isCovered = false;
				
				for (int j = 0 ; j < networkSize ; j++)
				{
                                	coverageMatrix[cubeCounter][j] = 0;

	                		double farthest = -1;
					n = theSNModule->getSubmodule("node",j);
					r = check_and_cast<ResourceManager*>(n->getSubmodule("ResourceManager"));
					l = check_and_cast<VirtualMobilityManager*>(n->getSubmodule("MobilityManager"));
				
					double s_x = l->getLocation().x;
					double s_y = l->getLocation().y;
					double s_z = l->getLocation().z;

					double dist1 = sqrt (pow ((c1_x - s_x),2) + pow ((c1_y - s_y),2) + pow ((c1_z - s_z),2));
					double dist2 = sqrt (pow ((c2_x - s_x),2) + pow ((c2_y - s_y),2) + pow ((c2_z - s_z),2) );
					double dist3 = sqrt (pow ((c3_x - s_x),2) + pow ((c3_y - s_y),2) + pow ((c3_z - s_z),2) );
					double dist4 = sqrt (pow ((c4_x - s_x),2) + pow ((c4_y - s_y),2) + pow ((c4_z - s_z),2));
					double dist5 = sqrt (pow ((c5_x - s_x),2) + pow ((c5_y - s_y),2) + pow ((c5_z - s_z),2));
					double dist6 = sqrt (pow ((c6_x - s_x),2) + pow ((c6_y - s_y),2) + pow ((c6_z - s_z),2) );
					double dist7 = sqrt (pow ((c7_x - s_x),2) + pow ((c7_y - s_y),2) + pow ((c7_z - s_z),2) );
					double dist8 = sqrt (pow ((c8_x - s_x),2) + pow ((c8_y - s_y),2) + pow ((c8_z - s_z),2));

					if ( dist1 > farthest)	farthest = dist1;
					if ( dist2 > farthest)	farthest = dist2;
					if ( dist3 > farthest)	farthest = dist3;
					if ( dist4 > farthest)	farthest = dist4;
					if ( dist5 > farthest)	farthest = dist5;
					if ( dist6 > farthest)	farthest = dist6;
					if ( dist7 > farthest)	farthest = dist7;
					if ( dist8 > farthest)	farthest = dist8;


					if (farthest < sensingRange) 
					{ 
						coverageMatrix[cubeCounter][j] = 1;
						isCovered = true;
						//trace() << "Sensor no " << j << " covers cube no " << cubeCounter << "\n";
					}
                                	counter+= coverageMatrix[cubeCounter][j];

				}

				total+= counter;


				c1_x+= cubeWidth;
				c2_x+= cubeWidth;
				c3_x+= cubeWidth;
				c4_x+= cubeWidth;
				c5_x+= cubeWidth;
				c6_x+= cubeWidth;
				c7_x+= cubeWidth;
				c8_x+= cubeWidth;

				cubeCounter++;
			}
			c1_y+= cubeWidth;
			c2_y+= cubeWidth;
			c5_y+= cubeWidth;
			c6_y+= cubeWidth;

			 c3_y+= cubeWidth;
			 c4_y+= cubeWidth;
			 c7_y+= cubeWidth;
			 c8_y+= cubeWidth;

		}
		

		c1_z+= cubeWidth;
		c2_z+= cubeWidth;
		c3_z+= cubeWidth;
		c4_z+= cubeWidth;
	}		

	}	

	
	readXMLparams();
	setTimer(START_ROUND, 0.0);
}

void CPCP::fromApplicationLayer(cPacket *pkt, const char *destination)
{	
	if(!isSink)
	{
		string dst(destination);
		CPCPPacket *netPacket = new CPCPPacket("CPCP routing data packet", NETWORK_LAYER_PACKET);
		netPacket->setCPCPPacketKind(CPCP_DATA_PACKET);
		netPacket->setByteLength(dataPacketSize);
		netPacket->setSource(SELF_NETWORK_ADDRESS);
		encapsulatePacket(netPacket, pkt);
		if (!isCH && endFormClus)
		{
			CHInfo info = *CHcandidates.begin();
			stringstream buffer;
			buffer << info.src;
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

void CPCP::fromMacLayer(cPacket *pkt, int macAddress, double rssi, double lqi){CPCPPacket *netPacket = dynamic_cast <CPCPPacket*>(pkt);

	if (!netPacket)
		return;

	switch (netPacket->getCPCPPacketKind()) 
	{

		case CPCP_DATA_PACKET:
		{
			string dst(netPacket->getDestination());
			const char *src = netPacket->getSource();
			if (isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0){
				//trace() << "Node " << self << " Aggregate Data Frame \n";
				bufferAggregate.push_back(*netPacket);	
				//double p = 1 / totalPackets;
				collectOutput("PDR","",1);
				collectOutput("Total Received Packets","",1);

			}else if (dst.compare(SINK_NETWORK_ADDRESS) == 0 && isSink)  {
				//trace() << "Node " << self << " Processing Data Packet \n";
				int n = netPacket->getNumOfPackets();
				trace() << "n= "<< n << "\n";
				collectOutput("Number of data packets received at BS","",n);
				toApplicationLayer(decapsulatePacket(netPacket));
				//collectOutput("Number of Data Packets received at The Base Station", 1);
			}
			break;	
		}
		case CPCP_ADV_PACKET:
		{
			
			if(!isCH && !isSink)
			{	
				//trace() << "Node " << self << " Received a Advertisement Message of node " << netPacket->getSource() << " with RSSI: " << rssi << "\n";
				isCM = true;	
				par("isCM") = true;	
				CHInfo rec;
				rec.src = atoi(netPacket->getSource());
				rec.rssi = rssi;
				CHcandidates.push_front(rec);
			}
			break;
		}

		case CPCP_JOIN_PACKET:
		{
			string dst(netPacket->getDestination());
			if(isCH && dst.compare(SELF_NETWORK_ADDRESS) == 0)
			{
				trace() << "Node " << self << " Received a Join Request from " << atoi(netPacket->getSource()) << " Adding to clusterMembers\n";
				clusterMembers.push_back(atoi(netPacket->getSource()));
			}
	 		break;
		}

		case CPCP_ACTIVATION_PACKET:
		{
			if(!isCH && !isSink)
			{
				areaIsCovered = true;
				isActive = false;
				par("isActive") = false;
				trace() << "Node " << self << " Received a Activation msg\n";
				trace() << "My area is covered, I will sleep ZZZZ \n";
				setStateSleep();
			}
	 		break;
		}

		case CPCP_TDMA_PACKET:
		{
			if(!isCH && !isSink)
			{
				clusterLength = netPacket->getScheduleArraySize();
				for(int i=0; i<netPacket->getScheduleArraySize(); i++)
				{
					if (netPacket->getSchedule(i)==atoi(SELF_NETWORK_ADDRESS)) 
					{
						assignedTDMA = true;
                                                myTDMATurn = i ;
						setStateSleep();
				
						break;
					}			
				}
				isCM = true;	
				par("isCM") = true;
			}
			break;
		}
	}
}

void CPCP::timerFiredCallback(int index)
{

	switch (index) 
       {

	case START_ROUND:
	{
		trace() << "CPCP - STARTNG ROUND NO. " << roundNumber;
		par("energy") = 18720 - resMgrModule->getSpentEnergy();
		setStateRx();
		if (getTimer(START_SLOT) != 0) //New Round - end transmission of last round
		{ 
			cancelTimer(START_SLOT);
		}
		
		isCH = false;
		isCM = false;
		areaIsCovered = false;
		isActive = true;
		endFormClus = false;
		CHcandidates.clear();
		clusterMembers.clear();
		energyPerCell.clear();
		minWghtCost.clear();
		par("minWghts") = "";

		double timer = uniform (0.0,5.0);
     
		setTimer(FIND_NEIGHBORS,timer);
	
		if (isSink) //Find the coverage cost for all the sensors.
		{
			setTimer(CALCULATE_COVERAGE_COST,6.0);
		}
		else
		{
			setTimer(FIND_COVERAGE_COST,8.0);
			setTimer(ENTER_STEADY_PHASE,50);	

		}
		setTimer(TEST, 400);

        	roundNumber++;
		if (roundNumber >= numberOfRounds) break;
		
		setTimer(START_ROUND, roundLength);
		break;
	}

	case FIND_NEIGHBORS:
     	{  
		CPCPPacket *crtlPkt = new CPCPPacket("Find-Nbrs",NETWORK_LAYER_PACKET);
		crtlPkt->setCPCPPacketKind(CPCP_FIND_NEIGHBOURS_PACKET);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
     	}

	case CALCULATE_COVERAGE_COST:
     	{  
		energyPerCell.clear();
		double totalEnergyPerCell = 0;

		cModule *n; 
		cModule *c;

 		for (int i = 0 ; i < numOfCubes ; i++)
		{ 
			
			totalEnergyPerCell = 0;

			for (int j = 0 ; j < networkSize ; j++)
			{
				if ( coverageMatrix[i][j] == 1 ) 

				{
  					n = theSNModule->getSubmodule("node",j);
					c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
					double e = c->par("energy").doubleValue();
					//trace() << "Node " << j << " energy = " << e << "\n";
					totalEnergyPerCell+= c->par("energy").doubleValue()/initialEnergy; 
				}
			}

			energyPerCell[i] = totalEnergyPerCell;
			trace() << "energyPerCell " << i << " is " <<  totalEnergyPerCell << "\n";
		}

		double minWght =-100;
		
		stringstream out;
		for (int j = 0 ; j < networkSize ; j++)
		{	
			minWght = -100;

			for (int i = 0 ; i < numOfCubes ; i++)
			{
				if ( coverageMatrix[i][j] == 1 ) // node j covers cell i
				{
					double wght = 1/energyPerCell[i];
  					if (wght > minWght) minWght = wght;
				}
			}
                        minWghtCost[j] = minWght;

			trace() << "MIn. Wght cost for node " << j << " is " <<  minWght << "\n";
			out << minWghtCost[j] <<";";
		}

		par("minWghts") = out.str();
		break;
     	}

	case FIND_COVERAGE_COST:
	{
		runCDDP(); // To know my weight
		double myActivationTime = myWght;
		trace() << " myActivationTime " <<  myActivationTime << "\n";
               	setTimer(CHECK_CH_STATUS, myActivationTime);
		break;
	}

	case CHECK_CH_STATUS:
	{
		trace() << "In CHECK_CH_STATUS \n ";
		if (!isCH && !isCM)
		{
			isCH = true;
			isActive = true;
			par("isActive") = true;
			par("isCH") = true;
			trace() << "I am a CH \n";
			setTimer(SEND_ADV, 0);
			setTimer(MAKE_TDMA, 6.0 );
		}
                setTimer(JOIN_CH, 4.0);
		
	}
	
	case CHECK_ACTIVATION_STATUS:
	{

		if (!isCH)
		{	
			if (!areaIsCovered)
			{
				trace() <<" My area is not covered, I should be active -> Broadcast activation msg \n";
				CPCPPacket *crtlPkt = new CPCPPacket("ACTIVATION Packet", NETWORK_LAYER_PACKET);
				crtlPkt->setByteLength(tdmaPacketSize);
				crtlPkt->setCPCPPacketKind(CPCP_ACTIVATION_PACKET);
				crtlPkt->setSource(SELF_NETWORK_ADDRESS);
				crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
				toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
				isActive = true;
				par("isActive") = true;
			}
			
		}
		
		break;
	}


	case MAKE_TDMA:
        {
	    //trace() << "Make TDMA \n";
		if (clusterMembers.size()!=0)
		{
		    	CPCPPacket *crtlPkt = new CPCPPacket("ClusterHead TDMA Packet", NETWORK_LAYER_PACKET);
			crtlPkt->setByteLength(tdmaPacketSize);
			crtlPkt->setCPCPPacketKind(CPCP_TDMA_PACKET);
			crtlPkt->setSource(SELF_NETWORK_ADDRESS);
			crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
			clusterLength = clusterMembers.size();	
			crtlPkt->setScheduleArraySize(clusterMembers.size());	
			for(int i=0; i<clusterLength; i++) crtlPkt->setSchedule(i,clusterMembers[i]);
			toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
			//trace() << "Node " << self << " Sent TDMA pkt";
			setTimer(START_SLOT, clusterLength*slotLength);
		}
			else setTimer(START_SLOT, slotLength);					
			break;
	}

	case SEND_ADV:
	{	
		CPCPPacket *crtlPkt = new CPCPPacket("ClusterHead Announcement Packet", NETWORK_LAYER_PACKET);
		crtlPkt->setByteLength(advPacketSize);
		crtlPkt->setCPCPPacketKind(CPCP_ADV_PACKET);
		crtlPkt->setSource(SELF_NETWORK_ADDRESS);
		crtlPkt->setDestination(BROADCAST_NETWORK_ADDRESS);
		toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
		break;
	}

	case JOIN_CH:
	{
		if(CHcandidates.size()!=0)
		{	
			CHcandidates.sort(cmpRssi);
			CPCPPacket *crtlPkt = new CPCPPacket("ClusterMember Join Packet", NETWORK_LAYER_PACKET);
			crtlPkt->setCPCPPacketKind(CPCP_JOIN_PACKET);
			crtlPkt->setByteLength(joinPacketSize);	
			crtlPkt->setSource(SELF_NETWORK_ADDRESS);
			CHInfo info = *CHcandidates.begin();
			stringstream buffer;
			buffer << info.src;
			string dst = buffer.str();
			crtlPkt->setDestination(dst.c_str());
			//trace() << "Send Join Packet to " << dst << "\n";			
			toMacLayer(crtlPkt, BROADCAST_MAC_ADDRESS);
			endFormClus = true;
			//trace() << "Node " << self << " Sent Join Request to " << dst;
		}
		double myActivationTime = myWght;
		setTimer(CHECK_ACTIVATION_STATUS, myActivationTime);
			break;
	}
case ENTER_STEADY_PHASE:
	{
		if (isCH)
		{
			if (clusterMembers.size()!=0)
			{
				clusterLength = clusterMembers.size();
				trace() << "CH will start slot after " <<  clusterLength * slotLength << "\n";
				setTimer(START_SLOT, clusterLength * slotLength);
			}
			else
			{
				trace() << "CH no members will start slot in  " <<  slotLength << "\n";
				setTimer(START_SLOT, slotLength);	
			}
		}
		else if (assignedTDMA)
		{
			setStateSleep();
			trace() << "CM will start slot in  " <<  myTDMATurn * slotLength << "\n";
			setTimer(START_SLOT, myTDMATurn * slotLength);
		}
		break;
	}
    	
	case START_SLOT:
	{
		if(isCH && clusterMembers.size()==0) setTimer(START_SLOT, slotLength);	
		else setTimer(START_SLOT, clusterLength * slotLength);
				
		if (isCH) 
		{
			sendAggregate(); 
			processBufferedPacket();
		}

		else if (isActive)
		{
			processBufferedPacket();
			setTimer(END_SLOT, slotLength);
		}
		break;
	}




		case END_SLOT:
		{
			trace() << "Node " << self << " Sleept"  << "\n";
			if (!isSink && !isCH) setStateSleep();
			break;
		}

		case TEST:
		{

			double numberRounds = numberOfRounds;
			if (isActive) collectOutput("Average number of active nodes per round","",1/numberRounds);
		
			if (isActive && !isCH )
			{
				collectOutput("Total # of Active Nodes without CHs","",1/numberRounds);
				 cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	                 	 double n = tmpModule->par("noActiveNodes").doubleValue();
				 double u = n + 1;
                                 tmpModule->par("noActiveNodes").setDoubleValue(u);
				 double pkts = roundLength * 1;
				 if (isCM) collectOutput("Total Generated Packets","",pkts);
				 collectOutput("Total Generated Packets2","",pkts);
                        }
			if (!isCH && !isCM) collectOutput("Average Number of unclustered nodes per Round","",1/numberRounds);
			if (isCH ) collectOutput("Average number of CHs per round","",1/numberRounds);

			if (isSink)
			{
				cModule *n; 
				cModule *c;
			
				vector<int> sleepNodes;

				double noOfActiveNodes = 0;

 				for (int j = 1 ; j < networkSize ; j++)
				{
					n = theSNModule->getSubmodule("node",j);
					c = check_and_cast<cModule*>(n->getSubmodule("Communication")->getSubmodule("Routing"));
					bool isActiveNode = c->par("isActive").boolValue();
					bool isCH = c->par("isCH").boolValue();
					bool isCM = c->par("isCM").boolValue();
					if (!isActiveNode) sleepNodes.push_back(j);
					if (isActiveNode && !isCH && isCM) noOfActiveNodes++;
				}

				
				double tp = noOfActiveNodes * roundLength * 1; //to Calculate PDR
				collectOutput("Total Sent Packets","",tp);

			
				vector<vector<double> > tmpCoverageMatrix(numOfCubes, vector<double>(networkSize,0));
	        
               
				double total = 0;
				double minNodes = 1000000;
				double maxNodes = -1000000;

				
			 	for (int i = 0 ; i < numOfCubes; i++)
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
                                      
					if (counter < minNodes)
					{
                                          minNodes = counter;
					   trace() << "minNodes " << minNodes << "\n";
					}
					if (counter > maxNodes) 
					{
					maxNodes = counter;
					 trace() << "maxNodes " << maxNodes << "\n";
					}
					total+= counter; 
					

					if (isCovered) collectOutput("Coverage Ratio","",1/numOfCubes);
 			                      trace() << "F: minNodes " << minNodes << "\n";
				}

                                  
				   

                		double avgRedunduncy = total/numOfCubes;
				collectOutput("Average Number of Nodes covering a cube","",avgRedunduncy/numberRounds);
				collectOutput("Minimum Number of Nodes covering a cube","",minNodes/numberRounds);
				collectOutput("Maximum Number of Nodes covering a cube","",maxNodes/numberRounds);
			}
			break;

		}

	}
	
}

void CPCP :: runCDDP()
{
	myWght = 0;

	cModule *tmpModule =check_and_cast<cModule*> (theSNModule->getSubmodule("node",0)->getSubmodule("Communication")->getSubmodule("Routing"));
	
 	string wghts = tmpModule->par("minWghts").stringValue();
       
	int pointer = 0;
        char_separator<char> sep1(";");
        tokenizer<char_separator<char>> tokens1(wghts, sep1);
    	for (const auto& t : tokens1) 
	{
	    if (self == pointer)
            {
	      myWght = stod(t);
	      trace() << " My node ID is " << self << " My myWght is " << myWght << "\n";
            }
            pointer++;
	}


}

void CPCP :: finishSpecific()
{
	
}

	void CPCP::sendAggregate()
{
	if(bufferAggregate.size() != 0)
	{	
		double bitsLength = bufferAggregate.size() * dataPacketSize * 1000;
		double energyBit = (aggrConsumption / pow(10,9)) * bitsLength;
		//powerDrawn(energyBit);
		CPCPPacket *aggrPacket = new CPCPPacket("ClusterHead Aggredated Packet", NETWORK_LAYER_PACKET);
		aggrPacket->setByteLength(dataPacketSize+4);
		aggrPacket->setCPCPPacketKind(CPCP_DATA_PACKET);
		aggrPacket->setSource(SELF_NETWORK_ADDRESS);
		aggrPacket->setDestination(SINK_NETWORK_ADDRESS);
		aggrPacket->setNumOfPackets(bufferAggregate.size());

		ApplicationPacket *newPacket = new ApplicationPacket("App generic packet", APPLICATION_PACKET);
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

void CPCP::processBufferedPacket()
{
	string dst;
	CHInfo info = *CHcandidates.begin();
	if (isCH)
	{
		ostringstream convert;   
  		convert << 0;      
  		dst = convert.str();
	}
	else if (isCM)
	{
		ostringstream convert;   
  		convert << info.src;      
  		dst = convert.str();
	}
	while (!tempTXBuffer.empty())
	{
		cPacket *pkt = tempTXBuffer.front();	
		CPCPPacket *netPacket = dynamic_cast <CPCPPacket*>(pkt);
		netPacket->setDestination(dst.c_str());
		bufferPacket(netPacket); 
		tempTXBuffer.pop();
	}

	while (!TXBuffer.empty()) {
		toMacLayer(TXBuffer.front(), resolveNetworkAddress(dst.c_str()));
		TXBuffer.pop();
	}	
}

void CPCP::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

void CPCP::setPowerLevel(double powerLevel)
{
	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
}

void CPCP::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}

void CPCP::levelTxPower(int linkBudget)
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

void CPCP::readXMLparams()
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

bool cmpRssi(CHInfo a, CHInfo b){
	return (a.rssi > b.rssi);
}
