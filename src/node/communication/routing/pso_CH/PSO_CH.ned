package node.communication.routing.pso_CH;

simple PSO_CH like node.communication.routing.iRouting
{
parameters:

	string applicationID = default ("throughputTest");
	bool isSink = default (false);
	bool collectTraceInfo= default(true);

	double percentage;	
	double roundLength @unit(s);	
	double slotLength;

	double energy;
	string networkInformation = "";
	
	int numOfCHs;
	int epoch;
	double sensingRange;
	int numOfNbrs = 0;
	int CH_Frequencey = 0;
        string candidates = "";
	string routes = "";
	string neighbours = "";	


        //routing layer packet sizes
	int advPacketSize = default (9); // Type + Source + Destination = 9 bytes
	int joinPacketSize = default (9);
	int tdmaPacketSize = default (150); // Type + Source + Destination + tdma = 150 bytes
	int dataPacketSize = default (9);
	int numberOfRounds = default(1);

	//Parameters used to power control.
	xml powersConfig;

	int maxNetFrameSize = default (0);
	int netDataFrameOverhead = default (14);
	int netBufferSize = default (32);
       
		
		
gates: 
	output toCommunicationModule;
	output toMacModule;
	input fromCommunicationModule;
	input fromMacModule;
	input fromCommModuleResourceMgr;
}

