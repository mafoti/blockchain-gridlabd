/*
 * blockNode.h
 *
 *  Created on: Jul 11, 2017
 *      Author: ubuntu
 */

#ifndef BLOCKNODE_H_
#define BLOCKNODE_H_
#include <string>
#include <vector>
struct marketStruct {
	  int price;
	  int quantity;
	  int type;
	};

class blockNode {
public:

	//static int id;
	int nodeId;
	int rpcPort;
	std::string stringPort;
	std::string stringId;
	std::vector <double> prices;
	std::string blockChain_url;
	bool accountCreated;
	bool accountHasBalance;
	std::string account;
	void initNode(int id);
	void startNode(char *_url);
	void clearMarket();
	void readClearing();
	double getLastPrice();
	double getAvgPrice();
	double getStdPrice();
	void submitConsumptionBid(int price, int quantity);
	void submitGenerationBid(int price, int quantity);
	blockNode();
	virtual ~blockNode();

private:
	int hexadecimalToDecimal(std::string hexVal);
	void sendBalance(std::string accounts);
};

#endif /* BLOCKNODE_H_ */
