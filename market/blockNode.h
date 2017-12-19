/*
 * blockNode.h
 *
 *  Created on: Jul 11, 2017
 *      Author: ubuntu
 */

#ifndef BLOCKNODE_H_
#define BLOCKNODE_H_
#include <string>

class blockNode {
public:
	//static int id;
	int nodeId;
	int rpcPort;
	void initNode(int id);
	void startNode(int id);
	void clearMarket(int id);
	void submitConsumptionBid(int id, int price, int quantity);
	void submitGenerationBid(int id, int price, int quantity);
	blockNode();
	virtual ~blockNode();
};

#endif /* BLOCKNODE_H_ */
