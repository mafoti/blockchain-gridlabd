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

	string systemString = "bash -c \"geth --bootnodes enode://ee883cc0b2d1811f74aadab89216caf908409ed1d654f8303d1d6139acb87a70ade89e18d55149f16256164086098ba6f00e93b01b7d2104b71581ea83e8479e@127.0.0.1:30299 --datadir=";
	systemString += datadir;
	systemString += " --identity='mynode-";
	systemString += stringId;
	systemString += "' --port=";
	systemString += stringId;
	systemString += " --rpc --rpcapi='db,eth,net,web3,admin,personal' --rpcport=";
	systemString += stringPort;
	systemString += " --rpcaddr=127.0.0.1 --rpccorsdomain='*' --ipcpath ";
	systemString += datadir;
	systemString += "/geth.ipc --networkid=1923 2>&1 &\"";

	cout << systemString << endl;
	const char * systemCommand = systemString.c_str();

	system(systemCommand);

	//get the address of the newly created account
	jsonrpc::HttpClient httpclient("http://localhost:"+stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts;
	bool created = false;
	while (!created) {
		try {
			accounts = c.eth_accounts();
			cout << "accounts : " << accounts << endl;
			created = true;
		} catch (jsonrpc::JsonRpcException& e) {
			created = false;
		}
	}

	//unlock the account holding eths
	jsonrpc::HttpClient marketclient("http://localhost:8100");
	EthereumAPI market(marketclient);
	try {
		string address = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
		market.personal_unlockAccount(address, "0", 0);
	} catch (jsonrpc::JsonRpcException& e) {
		cout << "error unlocking account " << e.GetMessage() << endl;
	}

	//send eths to the new account
	try
	{
		Json::Value root; // {}
		root["from"] = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
		root["to"] = accounts[0];
		root["value"] = "0x1BC16D674EC80000"; // 2000000000000000000 = 2Eth
		market.eth_sendTransaction(root);
	}
	catch (jsonrpc::JsonRpcException & e)
	{
		cerr << e.what() << endl;
	}

}


void blockNode::clearMarket(int id){
	cout << "MARKET CLEARING CALLED " << endl;
	jsonrpc::HttpClient httpclient("http://localhost:8100");
	EthereumAPI c(httpclient);

	try {
		//string address = accounts[0].asString();
		string address = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
		c.personal_unlockAccount(address, "0", 0);
		cout << "ACCOUNT UNLOCKED " << endl;
	} catch (jsonrpc::JsonRpcException& e) {
		cout << "ERROR UNLOCKING ACCOUNT " << e.GetMessage() << endl;
	}

	try
	{
		//contract address: 0x4291ae232bcbeb5a64b612a971384efac3dab88d
		//default account : 0xad56cedb7d9ee48b3b93f682a9e2d87f80221768
		Json::Value root; // {}
		root["from"] = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
		root["to"] = "0x0f0a020a9545abc51b6c18b86bcdbdd4c7382dcf";
		root["data"] = "0x256a9ea1";
		string result = c.eth_sendTransaction(root);
		cout << "MARKET CLEARING HASH : " << result << endl;
	}
	catch (jsonrpc::JsonRpcException & e)
	{
		cerr << e.what() << endl;
	}
}
//"7f495ea5": "consumptionBid(int256,int256)",
void blockNode::submitConsumptionBid(int id, int price, int quantity){
	//comment out when gas problem solved
	nodeId = 30300 + id;
	rpcPort = 8101 + id;

	std::stringstream sp;
	sp << rpcPort;
	std::string stringPort(sp.str());

	std::stringstream ss;
	ss << nodeId;
	std::string stringId(ss.str());


	cout << "submitConsumptionBid httpclient http://localhost:" + stringPort << "nodeId "<< nodeId << endl;

	jsonrpc::HttpClient httpclient("http://localhost:" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts = c.eth_accounts();
	cout << "accounts : " << accounts[0] << endl;

	//UnlockAccount
	try {
		string address = accounts[0].asString();
		c.personal_unlockAccount(address, stringId, 0);
	} catch (jsonrpc::JsonRpcException& e) {
		cout << "error unlocking account " << e.GetMessage() << endl;
	}



	std::stringstream stream;
	stream << setfill('0') << setw(64) << std::hex << price;
	std::string resultprice( stream.str() );

	stream << setfill('0') << setw(64) << std::hex << quantity;
	std::string resultquantity( stream.str() );

	cout << "submitConsumptionBid priceHEX:" << resultprice << " quantityHEX:" << resultquantity << endl;

	try
	{
		//contract address: 0x4291ae232bcbeb5a64b612a971384efac3dab88d
		//default account : 0xad56cedb7d9ee48b3b93f682a9e2d87f80221768
		// 027cb7c6 clear();
		//Json::Value result = "\"from\": \"0x1d1ae163d75d6689c6c70c7367bbd08ac5361e4e\", \"to\": \"0x35bc45bb2c4c8f311ed9e0e867287ecb9ca90f8b\", \"data\": \"0x256a9ea1\"";
		Json::Value root; // {}
		root["from"] = accounts[0];
		root["to"] = "0x0f0a020a9545abc51b6c18b86bcdbdd4c7382dcf";
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
	//comment out when gas problem solved
	nodeId = 30300 + id;
	rpcPort = 8101 + id;

	std::stringstream sp;
	sp << rpcPort;
	std::string stringPort(sp.str());

	std::stringstream ss;
	ss << nodeId;
	std::string stringId(ss.str());


	cout << "submitGenerationBid httpclient http://localhost:" + stringPort << "nodeId "<< nodeId << endl;

	jsonrpc::HttpClient httpclient("http://localhost:" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts = c.eth_accounts();
	cout << "accounts : " << accounts[0] << endl;

	//UnlockAccount
	try {
		string address = accounts[0].asString();
		c.personal_unlockAccount(address, stringId, 0);
	} catch (jsonrpc::JsonRpcException& e) {
		cout << "error unlocking account " << e.GetMessage() << endl;
	}



	std::stringstream stream;
	stream << setfill('0') << setw(64) << std::hex << price;
	std::string resultprice( stream.str() );

	stream << setfill('0') << setw(64) << std::hex << quantity;
	std::string resultquantity( stream.str() );

	cout << "submitGenerationBid priceHEX:" << resultprice << " quantityHEX:" << resultquantity << endl;

	try
	{
		//contract address: 0x4291ae232bcbeb5a64b612a971384efac3dab88d
		//default account : 0xad56cedb7d9ee48b3b93f682a9e2d87f80221768
		// 027cb7c6 clear();
		//Json::Value result = "\"from\": \"0x1d1ae163d75d6689c6c70c7367bbd08ac5361e4e\", \"to\": \"0x35bc45bb2c4c8f311ed9e0e867287ecb9ca90f8b\", \"data\": \"0x256a9ea1\"";
		Json::Value root; // {}
		root["from"] = accounts[0];
		root["to"] = "0x0f0a020a9545abc51b6c18b86bcdbdd4c7382dcf";
		root["data"] = "0x0d31d41a"+resultquantity+resultprice;
		c.eth_sendTransaction(root);
		//cout << c.eth_accounts() << endl;
	}
	catch (jsonrpc::JsonRpcException & e)
	{
		cerr << e.what() << endl;
	}
}

marketStruct blockNode::readClearing(int id) {
	marketStruct market = marketStruct();
	nodeId = 30300 + id;
	rpcPort = 8101 + id;

	std::stringstream sp;
	sp << rpcPort;
	std::string stringPort(sp.str());

	std::stringstream ss;
	ss << nodeId;
	std::string stringId(ss.str());

	jsonrpc::HttpClient httpclient("http://localhost:" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts = c.eth_accounts();

	try{
		Json::Value root; // {}
		root["from"] = accounts[0];
		root["to"] = "0x0f0a020a9545abc51b6c18b86bcdbdd4c7382dcf";
		root["data"] = "0x901a40a7";

		string result = c.eth_call(root, "latest");
		int price = strtoul(result.c_str(), NULL, 16);
		cout << "get market result : " << price << endl;
	}
	catch (jsonrpc::JsonRpcException & e)
	{
		cerr << e.what() << endl;
	}

	/*market.price = result["clearingPrice"].asInt();
	market.quantity = result["clearingQuantity"].asInt();
	market.type = result["clearingType"].asInt();*/

	market.price = 10;
	market.quantity = 100;
	market.type = 0;

	return market;
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
