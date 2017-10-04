/*
 * blockNode.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: ubuntu
 */

#include "blockNode.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <sstream>

//int blockNode::id;

using namespace std;

void blockNode::initNode(int id) {

	nodeId = 30300 + id;
	rpcPort = 8101 + id;
	printf("nodeId = %d \n", this->nodeId);
	std::stringstream ss;
	ss << nodeId;

	//stringId :  password for new nodes, port, id
	std::string stringId(ss.str());
	std::string root = "/home/ubuntu/tmp/eth/13212";
	std::string datadir = root + "/data/" + stringId;

	const char * dirPath = datadir.c_str();
	DIR* directory = opendir(dirPath);

	if (directory == NULL) //if is not directory initialize node
	{
		//init node and get enode

		string systemString = "bash -c \"geth --datadir ";
		systemString += datadir;
		systemString += " --port ";
		systemString += stringId;
		systemString += " --networkid 13212 init /home/ubuntu/tmp/eth/13212/genesis.json";
		systemString += " js <(echo 'console.log(admin.nodeInfo.enode); exit();')\" 2>/dev/null | grep enode | tee >> /home/ubuntu/tmp/eth/nodes";

		cout << systemString << endl;
		const char * systemCommand = systemString.c_str();
		system(systemCommand);

	}
	closedir(directory);

	std::string keystorePathS = root + "/keystore";
	const char * keystorePath = keystorePathS.c_str();
	DIR* directoryKeystore = opendir(keystorePath);
	if (directoryKeystore == NULL) //if is not directory create it
	{
		cout << "directory " << keystorePathS << "does not exist, create it" <<endl;
		mkdir(keystorePath, 0777);
	}
	closedir(directoryKeystore);


	std::string accountPathS = keystorePathS + "/" + stringId;
	const char * accountPath = accountPathS.c_str();
	DIR* directoryAccount = opendir(accountPath);
	if (directoryAccount == NULL) //if is not directory create new account and make keystore folders
	{
		cout << "directory " << accountPathS << "does not exist, create it and create a new account" <<endl;
		mkdir(accountPath, 0777);

		//create new account
		string systemString = "bash -c \"geth --datadir ";
		systemString += datadir;
		systemString += " --password <(echo -n ";
		systemString += stringId;
		systemString += ") account new\"";

		cout << systemString << endl;
		const char * systemCommand = systemString.c_str();
		system(systemCommand);

		//copy keystore from /home/ubuntu/tmp/eth/13212/data/30300 to /home/ubuntu/tmp/eth/13212/keystore/30300
		systemString = "cp -R ";
		systemString += datadir;
		systemString += "/keystore ";
		systemString += accountPathS;

		cout << systemString << endl;
		systemCommand = systemString.c_str();
		system(systemCommand);

		//copy keystore from /home/ubuntu/tmp/eth/13212/keystore/30300 to /home/ubuntu/tmp/eth/13212/data/30300/keystore
		systemString = "cp -R ";
		systemString += accountPathS;
		systemString += "/keystore ";
		systemString += datadir;
		systemString += "/keystore";

		cout << systemString << endl;
		systemCommand = systemString.c_str();
		system(systemCommand);

	}
	closedir(directoryAccount);

}


void blockNode::startNode(int id) {

	//copy static-nodes.json
	//start node
	nodeId = 30300 + id;
	rpcPort = 8101 + id;

	printf("nodeId = %d \n", this->nodeId);

	std::stringstream ss;
	ss << this->nodeId;

	//stringId :  password for new nodes, port, id
	std::string stringId(ss.str());

	std::stringstream sp;
	sp << this->rpcPort;

	//stringPort :  rpcPort
	std::string stringPort(sp.str());


	std::string root = "/home/ubuntu/tmp/eth/13212";
	std::string datadir = root + "/data/" + stringId;

	string systemString = "bash -c \"geth --datadir=";
	systemString += datadir;
	systemString += " --identity=\"mynode-";
	systemString += stringId;
	systemString += "\" --nodiscover --port=";
	systemString += stringId;
	systemString += " --password=<(echo -n ";
	systemString += stringId;
	systemString += ") --rpc --rpcport=";
	systemString += stringPort;
	systemString += "  --rpccorsdomain='*' home/ubuntu/tmp/eth/13212/genesis.json 2>&1 &\"";

	cout << systemString << endl;
	const char * systemCommand = systemString.c_str();

	system(systemCommand);

	//$GETH --datadir=$datadir --identity="$dd" --nodiscover --port=$port --password=<(echo -n $dd) --rpc --rpcport=$rpcport --rpccorsdomain='*' $* 2>&1 | tee "$stablelog" > "$log" &

}

blockNode::blockNode() {

	/*id++;
	printf("id = %d \n", this->id);
	nodeId = 30300 + id;
	rpcPort = 8101 + id;*/
}

blockNode::~blockNode() {
	// TODO Auto-generated destructor stub
}
