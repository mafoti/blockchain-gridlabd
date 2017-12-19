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

#include  <iomanip>

#include "ethereumapi.h"
#include <jsonrpccpp/client/connectors/httpclient.h>


using namespace std;

void blockNode::initNode(int id) {

	nodeId = 30300 + id;
	rpcPort = 8101 + id;
	printf("nodeId = %d \n", this->nodeId);
	std::stringstream ss;
	ss << nodeId;

	//stringId :  password for new nodes, port, id
	std::string stringId(ss.str());
	std::string root = "/home/ubuntu/tmp/eth/1923";
	std::string datadir = root + "/data/" + stringId;

	const char * dirPath = datadir.c_str();
	DIR* directory = opendir(dirPath);

	if (directory == NULL) //if is not directory initialize node
	{
		//init node and get enode

		string systemString = "bash -c \"geth --datadir ";
		systemString += datadir;
		systemString += " init /home/ubuntu/tmp/eth/1923/genesis.json\"";

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

		//copy keystore from /home/user/tmp/eth/1923/data/30300 to /home/user/tmp/eth/1923/keystore/30300
		systemString = "cp -R ";
		systemString += datadir;
		systemString += "/keystore ";
		systemString += accountPathS;

		cout << systemString << endl;
		systemCommand = systemString.c_str();
		system(systemCommand);

		//copy keystore from /home/user/tmp/eth/1923/keystore/30300 to /home/user/tmp/eth/1923/data/30300/keystore
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


	std::string root = "/home/ubuntu/tmp/eth/1923";
	std::string datadir = root + "/data/" + stringId;

	string systemString = "bash -c \"geth --datadir=";
	systemString += datadir;
	systemString += " --identity \mynode-";
	systemString += stringId;
	systemString += "\ --unlock --port ";
	systemString += stringId;
	systemString += " --password <(echo -n ";
	systemString += stringId;
	systemString += ") --rpc --rpcapi=\"db,eth,net,web3,personal\" --rpcport ";
	systemString += stringPort;
	systemString += " --ipcpath ";
	systemString += datadir;
	systemString += "/geth.ipc --networkid 1923 2>&1 &\"";

	cout << systemString << endl;
	const char * systemCommand = systemString.c_str();

	system(systemCommand);

	jsonrpc::HttpClient httpclientt("http://localhost:" + stringPort);
	EthereumAPI cc(httpclientt);
	Json::Value accounts;
	bool created=false;
	while(!created){
		try {
			accounts = cc.eth_accounts();
			cout << "accounts : " << accounts << endl;
			created=true;
		} catch (jsonrpc::JsonRpcException& e) {
			created=false;
		}
	}
	//string address = accounts[0].asString();
	//systemString = "bash -c \"geth --datadir "+datadir+" --unlock " + address + " --password <(echo -n " + stringId +")\"";
	//cout << systemString << endl;
	//systemCommand = systemString.c_str();
	//system(systemCommand);


	/**/
	//geth --datadir /home/user/tmp/eth/1923/data/30300 --identity mynode-30300 --port 30300 --rpcport 8101 --ipcpath /home/user/tmp/eth/1923/data/30300/geth.ipc --networkid 1923 console
	//$GETH --datadir=$datadir --identity="$dd" --nodiscover --port=$port --password=<(echo -n $dd) --rpc --rpcport=$rpcport --ipcpath=/home/user/tmp/eth/1923/data/30300/geth.ipc --rpccorsdomain='*' $* 2>&1 | tee "$stablelog" > "$log" &

}


void blockNode::clearMarket(int id){

	jsonrpc::HttpClient httpclient("http://localhost:8100");
	EthereumAPI c(httpclient);
	try
	{
		//contract address: 0xf176c2f03773b63a6e3659423d7380bfa276dcb3
		//default account : 0xad56cedb7d9ee48b3b93f682a9e2d87f80221768
		// 027cb7c6 clear();
		//Json::Value result = "\"from\": \"0x1d1ae163d75d6689c6c70c7367bbd08ac5361e4e\", \"to\": \"0x35bc45bb2c4c8f311ed9e0e867287ecb9ca90f8b\", \"data\": \"0x256a9ea1\"";
		Json::Value root; // {}
		root["from"] = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
		root["to"] = "0xf176c2f03773b63a6e3659423d7380bfa276dcb3";
		root["data"] = "0x256a9ea1";
		string result = c.eth_call(root,"latest");
		cout << "market clearing result : " << result << endl;
	}
	catch (jsonrpc::JsonRpcException & e)
	{
		cerr << e.what() << endl;
	}
}
//"7f495ea5": "consumptionBid(int256,int256)",
void blockNode::submitConsumptionBid(int id, int price, int quantity){
	//comment out when gas problem solved
	id=0;
	nodeId = 30300 + id;
	rpcPort = 8101 + id;

	std::stringstream sp;
	sp << this->rpcPort;
	std::string stringPort(sp.str());
	cout << "httpclient     http://localhost:" + stringPort << "nodeId "<< nodeId << endl;
	jsonrpc::HttpClient httpclient("http://localhost:" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts = c.eth_accounts();
	cout << "accounts : " << accounts[0] << endl;

	std::stringstream stream;
	stream << setfill('0') << setw(64) << std::hex << price;
	std::string resultprice( stream.str() );

	stream << setfill('0') << setw(64) << std::hex << quantity;
	std::string resultquantity( stream.str() );

	cout << "submitConsumptionBid priceHEX:" << resultprice << " quantityHEX:" << resultquantity << endl;

	try
	{
		//contract address: 0xf176c2f03773b63a6e3659423d7380bfa276dcb3
		//default account : 0xad56cedb7d9ee48b3b93f682a9e2d87f80221768
		// 027cb7c6 clear();
		//Json::Value result = "\"from\": \"0x1d1ae163d75d6689c6c70c7367bbd08ac5361e4e\", \"to\": \"0x35bc45bb2c4c8f311ed9e0e867287ecb9ca90f8b\", \"data\": \"0x256a9ea1\"";
		Json::Value root; // {}
		root["from"] = accounts[0];
		root["to"] = "0xf176c2f03773b63a6e3659423d7380bfa276dcb3";
		root["data"] = "0x7f495ea5"+resultquantity+resultprice;
		c.eth_sendTransaction(root);
		//cout << c.eth_accounts() << endl;
	}
	catch (jsonrpc::JsonRpcException & e)
	{
		cerr << e.what() << endl;
	}
}


//"0d31d41a": "generationBid(int256,int256)",
void blockNode::submitGenerationBid(int id, int price, int quantity){
	cout << "submitConsumptionBid price:" << price << " quantity:" << quantity << endl;
	jsonrpc::HttpClient httpclient("http://localhost:8100");
		EthereumAPI c(httpclient);
		try
		{
			//contract address: 0xf176c2f03773b63a6e3659423d7380bfa276dcb3
			//default account : 0xad56cedb7d9ee48b3b93f682a9e2d87f80221768
			// 027cb7c6 clear();
			//Json::Value result = "\"from\": \"0x1d1ae163d75d6689c6c70c7367bbd08ac5361e4e\", \"to\": \"0x35bc45bb2c4c8f311ed9e0e867287ecb9ca90f8b\", \"data\": \"0x256a9ea1\"";
			Json::Value root; // {}
			root["from"] = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
			root["to"] = "0xf176c2f03773b63a6e3659423d7380bfa276dcb3";
			root["data"] = "0x7f495ea5";
			c.eth_call(root,"latest");
			//cout << c.eth_accounts() << endl;
		}
		catch (jsonrpc::JsonRpcException & e)
		{
			cerr << e.what() << endl;
		}
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
