#pragma once
#include "Agent.h"

class UCC :
	public Agent
{
public:

	// Constructor and destructor
	UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId);
	UCC(Node *node, uint16_t contributedItemId, uint16_t contributed_quantity, uint16_t constraintItemId);
	~UCC();

	// Agent methods
	void update() override { }
	void stop() override;
	UCC* asUCC() override { return this; }
	void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) override;

	bool IsFinish();
	uint16_t _contributedItemId; /**< The contributed item. */
	uint16_t _constraintItemId; /**< The constraint item. */
	uint16_t _contributed_quantity; /**< The constraint item. */
	uint16_t _constraint_quantity; /**< The constraint item. */

	bool final_agrement = false;
			
	// TODO
};

