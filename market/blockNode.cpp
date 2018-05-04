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

string contractAddress = "0xf176c2f03773b63a6e3659423d7380bfa276dcb3";
static map<int,std::string> bids;

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
			system(systemCommand);
		}
		catch (jsonrpc::JsonRpcException & e)
		{
			cerr << "market clearing error " << e.what() << endl;
		}
	}

}

void blockNode::submitBidsAtMarketClearing(){

	map<int,std::string>::iterator it;
	std::cout << bids.size() << " bids to be submitted" << endl;
	for ( it = bids.begin(); it != bids.end(); it++ )
	{
	    const char * systemCommand = it->second.c_str();
	    system(systemCommand);
	}

}

void blockNode::submitConsumptionBid(int price, int quantity){

	jsonrpc::HttpClient httpclient("http://" + blockChain_url + ":" + stringPort);
	EthereumAPI c(httpclient);
	Json::Value accounts;

	if(!accountCreated){
		try {
			//accounts = c.eth_accounts();
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
					//cout << "submitConsumptionBid price : " << price << " quantity : "<< quantity << endl;
					std::stringstream priceStream;
					std::stringstream quantityStream;

					priceStream << setfill('0') << setw(64) << std::hex << price;
					std::string resultprice( priceStream.str() );

					quantityStream << setfill('0') << setw(64) << std::hex << quantity;
					std::string resultquantity( quantityStream.str() );
					string systemString = "curl -X POST -H \"Content-Type: application/json\" --data '{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":\""+account+"\", \"to\":\""+contractAddress+"\", \"gas\":\"0x30D40\", \"data\":\"0x7f495ea5"+resultquantity+resultprice+"\"}],\"id\":1}' http://"+blockChain_url+":"+stringPort+" &";

					bids.insert(std::pair<int, std::string>(nodeId, systemString));
					//const char * systemCommand = systemString.c_str();
					//system(systemCommand);
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
			//accounts = c.eth_accounts();
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
				//cout << "submitGenerationBid price : " << price << " quantity : "<< quantity << endl;
				std::stringstream priceStream;
				std::stringstream quantityStream;

				priceStream << setfill('0') << setw(64) << std::hex << price;
				std::string resultprice( priceStream.str() );


				quantityStream << setfill('0') << setw(64) << std::hex << quantity;
				std::string resultquantity( quantityStream.str() );
				string systemString = "curl -X POST -H \"Content-Type: application/json\" --data '{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":\""+account+"\", \"to\":\""+contractAddress+"\", \"gas\":\"0x30D40\", \"data\":\"0x0d31d41a"+resultquantity+resultprice+"\"}],\"id\":1}' http://"+blockChain_url+":"+stringPort+" &";

				bids.insert(std::pair<int, std::string>(nodeId, systemString));
				//const char * systemCommand = systemString.c_str();
				//system(systemCommand);
			}
			catch (jsonrpc::JsonRpcException & e)
			{
				cerr << "submitGenerationBid error "  << stringPort << " string port" << e.what() << endl;
			}
		}
	}
}

void blockNode::readClearing(char t[]) {

	jsonrpc::HttpClient httpclient("http://" + blockChain_url + ":" + stringPort);
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
			Json::Value result = c.eth_getMarketClearing();
			cout << "get market clearing result " << result << endl;
			std::ofstream myfile;
			string fileName = "clearingResults" + stringPort + ".txt";
			myfile.open(fileName.c_str(), std::ios_base::app);
			if (myfile.is_open()) {
				myfile << t << endl;
				myfile << result << endl;
				myfile.close();
			} else
				cout << "Unable to open file";


		}
		catch (jsonrpc::JsonRpcException & e)
		{
			cerr << e.what() << endl;
		}
	}

}

double blockNode::getLastPrice(){
	double last_p = 0.0;
	/*if(prices.size()>1){
		last_p = prices.back();
	}
	cout << "getLastPrice " << last_p << endl;*/
	return last_p;
}

double blockNode::getAvgPrice() {
	float sum = 0.0, mean;
	int i;

	/*if(prices.size()==0){
		return 0.0;
	}
	for (i = 0; i < prices.size(); ++i) {
		sum += prices[i];
	}

	mean = sum / prices.size();*/
	cout << "getAvgPrice " << mean << endl;
	return mean;
}

double blockNode::getStdPrice() {
	float sum = 0.0, mean, standardDeviation = 0.0;
	int i;
	double result = 0;
	/*if(prices.size()==0){
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

	result = sqrt(standardDeviation / prices.size());*/
	cout << "getStdPrice " << result << endl;
	return result;
}

blockNode::blockNode() {

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
