#pragma once
#include "Agent.h"

// Forward declaration
class MCP;
using MCPPtr = std::shared_ptr<MCP>;

class UCP :
	public Agent
{
public:

	// Constructor and destructor
	UCP(Node *node, uint16_t requestedItemId, uint16_t contributedItemId, const AgentLocation &uccLoc, unsigned int searchDepth);
	UCP(Node *node, uint16_t requestedItemId, uint16_t requested_quantity, uint16_t contributedItemId, uint16_t contributed_quantity, uint16_t actual_amount_contribution ,const AgentLocation &uccLoc, unsigned int searchDepth);
	~UCP();

	// Agent methods
	void update() override;
	void stop() override;
	UCP* asUCP() override { return this; }
	void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) override;
	bool IsFinish();
	void createChildMCP(uint16_t request, uint16_t requested_quantity, uint16_t contributed_quantity);
	void createChildMCP(uint16_t request);
	void EndAgreement();
	void destroyChildMCP();
	uint16_t _requestedItemId;
	uint16_t _contributedItemId;
	uint16_t _requested_quantity;
	uint16_t _contributed_quantity;
	uint16_t _actual_contributed_quantity;

	bool final_agrement = false;
	AgentLocation ucc_location;
	int searchDepth = 0;
	// TODO
	MCPPtr _mcp;

};

