package node.communication.routing.leachCentralized;

simple LeachCentralized like node.communication.routing.iRouting
{
parameters:

	string applicationID = default ("throughputTest");
	bool isSink = default (false);
	bool collectTraceInfo= default(true);

	int percentage;	
	double roundLength @unit(s);	
	double slotLength;

	double energy;

	int swarmSize;
	int epoch;


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
        string networkInformation = "";
		
		
gates: 
	output toCommunicationModule;
	output toMacModule;
	input fromCommunicationModule;
	input fromMacModule;
	input fromCommModuleResourceMgr;
}

