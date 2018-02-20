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
#include <iomanip>
#include <fstream>
#include <cmath>

#include "ethereumapi.h"
#include <jsonrpccpp/client/connectors/httpclient.h>


using namespace std;

string contractAddress = "0x368cbd3514a671e3a6c7d5ca865576a6face12fc";


void blockNode::initNode(int id) {

	accountCreated = false;
	accountHasBalance = false;
	nodeId = 30300 + id;
	rpcPort = 8101 + id;
	printf("Init nodeId = %d\n", this->nodeId);

	std::stringstream sp;
	sp << rpcPort;
	//stringPort :  rpcPort
	stringPort = sp.str();

	std::stringstream ss;
	ss << nodeId;
	//stringId :  password for new nodes, port, id
	stringId = ss.str();


	/*std::string root = "/home/ubuntu/tmp/eth/1923";
	std::string datadir = root + "/data/" + stringId;

	const char * dirPath = datadir.c_str();
	DIR* directory = opendir(dirPath);

	if (directory == NULL) //if is not directory initialize node
	{
		//init node

		//string systemString = "bash -c \"geth --datadir ";
		//systemString += datadir;
		//systemString += " init /home/ubuntu/tmp/eth/1923/genesis.json\"";

		//cout << systemString << endl;
		//const char * systemCommand = systemString.c_str();
		//system(systemCommand);
		string systemString = "bash -c \"mkdir "+ datadir + "/keystore\"";
		const char * systemCommand = systemString.c_str();
		system(systemCommand);

		//create new account
		systemString = "bash -c \"geth --datadir ";
		systemString += datadir;
		systemString += " --password <(echo -n ";
		systemString += stringId;
		systemString += ") account new\"";

		//cout << systemString << endl;
		systemCommand = systemString.c_str();
		system(systemCommand);

	}
	closedir(directory);*/


}

void blockNode::sendBalance(std::string account) {
	jsonrpc::HttpClient marketclient("http://localhost:8100");
	EthereumAPI market(marketclient);
	try {
		Json::Value root; // {}
		root["from"] = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
		root["to"] = account;
		root["value"] = "0x1BC16D674EC80000"; // 2000000000000000000 = 2Eth
		market.eth_sendTransaction(root);
		//cout << "balance send " << endl;
	} catch (jsonrpc::JsonRpcException& e) {
		cerr << "balance send error " << e.what() << endl;
	}
}

void blockNode::startNode(char *_url) {

	string _blockChain_url(_url);
	this->blockChain_url = _blockChain_url;

	cout << "nodeId = " << this->nodeId << " blockChain_url= " << blockChain_url << " _url= " << _url << endl;

	/*std::stringstream ss;
	ss << this->nodeId;

	//stringId :  password for new nodes, port, id
	std::string stringId(ss.str());

	std::stringstream sp;
	sp << this->rpcPort;

	//stringPort :  rpcPort
	std::string stringPort(sp.str());


	std::string root = "/home/ubuntu/tmp/eth/1923";
	std::string datadir = root + "/data/" + stringId;

	string systemString = "bash -c \"geth --bootnodes enode://b0ea708b8fa8d69bc0464d26290188d6822b6e9b8160999809d24fa5786d12a31718ad8388f9e2a320f9de1f40532ae0247227207b4a2f6b515f5f79b10a63e0@127.0.0.1:30299 --fast --cache=1024 --datadir=";
	systemString += datadir;
	systemString += " --identity='mynode-";
	systemString += stringId;
	systemString += "' --unlock 0 --password <(echo ";
	systemString += stringId;
	systemString += ") --port=";
	systemString += stringId;
	systemString += " --rpc --rpcapi='eth,net,admin,personal' --rpcport=";
	systemString += stringPort;
	systemString += " --rpcaddr=127.0.0.1 --rpccorsdomain='*' --ipcpath ";
	systemString += datadir;
	systemString += "/geth.ipc --networkid=1923 2>&1 | tee " + datadir;
	systemString += "/log.log > " + datadir;
	systemString += "/mylog.log  &\"";

	//cout << systemString << endl;
	const char * systemCommand = systemString.c_str();

	system(systemCommand);*/

}

void blockNode::clearMarket(){

	cout << "MARKET CLEARING CALLED " << endl;
	jsonrpc::HttpClient httpclient("http://" + blockChain_url + ":" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts;

	if(!accountCreated){
		try {
			accounts = c.eth_accounts();
			accountCreated = true;
			account = accounts[0].asString();
			cout << "account : " << account << "created " << accountCreated << endl;
		} catch (jsonrpc::JsonRpcException& e) {
			cerr << "eth_accounts clearMarket error "  << stringPort << " string port" << e.what() << endl;
		}
	}
	if(accountCreated){
		try
		{
			string systemString = "curl -X POST -H \"Content-Type: application/json\" --data '{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":\""+account+"\", \"to\":\""+contractAddress+"\", \"gas\":\"0x4C4B40\", \"data\":\"0x256a9ea1\"}],\"id\":1}' http://"+blockChain_url+":"+stringPort+" &";

			const char * systemCommand = systemString.c_str();
			//cout << systemString <<endl;
			system(systemCommand);
			//Json::Value root; // {}
			//root["from"] = "0xad56cedb7d9ee48b3b93f682a9e2d87f80221768";
			//root["to"] = contractAddress;
			//root["gas"] = "0x4C4B40";
			//root["data"] = "0x256a9ea1";
			//string result = c.eth_sendTransaction(root);
		}
		catch (jsonrpc::JsonRpcException & e)
		{
			cerr << "market clearing error " << e.what() << endl;
		}
	}

}

void blockNode::submitConsumptionBid(int price, int quantity){

	jsonrpc::HttpClient httpclient("http://" + blockChain_url + ":" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts;

	if(!accountCreated){
		try {
			accounts = c.eth_accounts();
			accountCreated = true;
			account = accounts[0].asString();
			cout << "account : " << account << "created " << accountCreated << endl;
		} catch (jsonrpc::JsonRpcException& e) {
			cerr << "eth_accounts submitConsumptionBid error "  << stringPort << " string port" << e.what() << endl;
		}
	}
	if(accountCreated){
			if(quantity>0){
				try
				{
					cout << "submitConsumptionBid price : " << price << " quantity : "<< quantity << endl;
					std::stringstream priceStream;
					std::stringstream quantityStream;

					priceStream << setfill('0') << setw(64) << std::hex << price;
					std::string resultprice( priceStream.str() );

					quantityStream << setfill('0') << setw(64) << std::hex << quantity;
					std::string resultquantity( quantityStream.str() );
					string systemString = "curl -X POST -H \"Content-Type: application/json\" --data '{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":\""+account+"\", \"to\":\""+contractAddress+"\", \"gas\":\"0x30D40\", \"data\":\"0x7f495ea5"+resultquantity+resultprice+"\"}],\"id\":1}' http://"+blockChain_url+":"+stringPort+" &";
					//cout << systemString << endl;
					const char * systemCommand = systemString.c_str();
					system(systemCommand);
					//Json::Value root; // {}
					//root["from"] = account;
					//root["to"] = contractAddress;
					//root["gas"] = "0x30D40";
					//root["data"] = "0x7f495ea5"+resultquantity+resultprice;
					//c.eth_sendTransaction(root);

				}
				catch (jsonrpc::JsonRpcException & e)
				{
					cerr << "submitConsumptionBid error " << stringPort << " string port" << e.what() << endl;
				}
			}
	}

}

void blockNode::submitGenerationBid(int price, int quantity){

	jsonrpc::HttpClient httpclient("http://" + blockChain_url + ":" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts;
	if(!accountCreated){
		try {
			accounts = c.eth_accounts();
			accountCreated = true;
			account = accounts[0].asString();
			cout << "account : " << account << "created " << accountCreated << endl;

		} catch (jsonrpc::JsonRpcException& e) {
			cerr << "eth_accounts submitGenerationBid error "  << stringPort << " string port" << e.what() << endl;
		}
	}

	if(accountCreated){
		if(quantity>0){
			try
			{
				cout << "submitGenerationBid price : " << price << " quantity : "<< quantity << endl;
				std::stringstream priceStream;
				std::stringstream quantityStream;

				priceStream << setfill('0') << setw(64) << std::hex << price;
				std::string resultprice( priceStream.str() );


				quantityStream << setfill('0') << setw(64) << std::hex << quantity;
				std::string resultquantity( quantityStream.str() );
				string systemString = "curl -X POST -H \"Content-Type: application/json\" --data '{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":\""+account+"\", \"to\":\""+contractAddress+"\", \"gas\":\"0x30D40\", \"data\":\"0x0d31d41a"+resultquantity+resultprice+"\"}],\"id\":1}' http://"+blockChain_url+":"+stringPort+" &";
				//cout << systemString << endl;
				const char * systemCommand = systemString.c_str();
				system(systemCommand);

				//Json::Value root; // {}
				//root["from"] = account;
				//root["to"] = contractAddress;
				//root["gas"] = "0x30D40";
				//root["data"] = "0x0d31d41a"+resultquantity+resultprice;
				//c.eth_sendTransaction(root);
			}
			catch (jsonrpc::JsonRpcException & e)
			{
				cerr << "submitGenerationBid error "  << stringPort << " string port" << e.what() << endl;
			}
		}
	}
}

void blockNode::readClearing() {

	/*jsonrpc::HttpClient httpclient("http://" + blockChain_url + ":" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts;

	if(!accountCreated){
			try {
				accounts = c.eth_accounts();
				account = accounts[0].asString();
				accountCreated = true;
				cout << "account : " << account << " created " << accountCreated << endl;
			} catch (jsonrpc::JsonRpcException& e) {
				cerr << "eth_accounts readClearing error "  << stringPort << " string port" << e.what() << endl;
			}
		}
	if(accountCreated){
		try{
			Json::Value root; // {}
			root["from"] = account;
			root["to"] = contractAddress;
			root["data"] = "0x027cb7c6";

			string result = c.eth_call(root, "latest");
			cout << "result size " << result.size() << endl;
			if(result.size()==194){
				result = result.substr(2,result.size());

				string quantity = result.substr(0,64);
				string price = result.substr(64,64);
				string type = result.substr(128,64);

				int price_int = hexadecimalToDecimal(quantity);
				prices.push_back(price_int/100);
			}

			if(prices.size()>100)
			{
				prices.erase(prices.begin());
			}
		}
		catch (jsonrpc::JsonRpcException & e)
		{
			cerr << e.what() << endl;
		}
	}*/

}

double blockNode::getLastPrice(){
	double last_p = 0.0;
	if(prices.size()>1){
		last_p = prices.back();
	}
	cout << "getLastPrice " << last_p << endl;
	return last_p;
}

double blockNode::getAvgPrice() {
	float sum = 0.0, mean;
	int i;

	if(prices.size()==0){
		return 0.0;
	}
	for (i = 0; i < prices.size(); ++i) {
		sum += prices[i];
	}

	mean = sum / prices.size();
	cout << "getAvgPrice " << mean << endl;
	return mean;
}

double blockNode::getStdPrice() {
	float sum = 0.0, mean, standardDeviation = 0.0;
	int i;

	if(prices.size()==0){
			return 0.0;
	}

	for (i = 0; i < prices.size(); ++i) {
		sum += prices[i];
	}

	mean = sum / prices.size();

	for (i = 0; i < prices.size(); ++i){
		double tmp = prices[i] - mean;
		standardDeviation += pow(tmp, 2);
	}

	double result = sqrt(standardDeviation / prices.size());
	cout << "getStdPrice " << result << endl;
	return result;
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

int blockNode::hexadecimalToDecimal(string hexVal)
{
    int len = hexVal.size();

    // Initializing base value to 1, i.e 16^0
    int base = 1;

    int dec_val = 0;

    // Extracting characters as digits from last character
    for (int i=len-1; i>=0; i--)
    {
        // if character lies in '0'-'9', converting
        // it to integral 0-9 by subtracting 48 from
        // ASCII value.
        if (hexVal.at(i)>='0' && hexVal.at(i)<='9')
        {
            dec_val += (hexVal.at(i) - 48)*base;

            // incrementing base by power
            base = base * 16;
        }

        // if character lies in 'A'-'F' , converting
        // it to integral 10 - 15 by subtracting 55
        // from ASCII value
        else if (hexVal.at(i)>='A' && hexVal.at(i)<='F')
        {
            dec_val += (hexVal[i] - 55)*base;

            // incrementing base by power
            base = base*16;
        }
    }

    return dec_val;
}
