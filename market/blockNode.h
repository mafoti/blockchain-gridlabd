/*
 * blockNode.h
 *
 *  Created on: Jul 11, 2017
 *      Author: ubuntu
 */

#ifndef BLOCKNODE_H_
#define BLOCKNODE_H_
#include <string>
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
	bool accountCreated;
	bool accountHasBalance;
	std::string account;
	void initNode(int id);
	void startNode(int id);
	void clearMarket(int id);
	marketStruct readClearing(int id);
	void submitConsumptionBid(int id, int price, int quantity);
	void submitGenerationBid(int id, int price, int quantity);
	blockNode();
	virtual ~blockNode();

private:
	void sendBalance(std::string accounts);
};

#endif /* BLOCKNODE_H_ */
