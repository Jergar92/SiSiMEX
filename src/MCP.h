#pragma once
#include "Agent.h"

// Forward declaration
class UCP;
using UCPPtr = std::shared_ptr<UCP>;

class MCP :
	public Agent
{
public:

	// Constructor and destructor
	MCP(Node *node, uint16_t requestedItemID, uint16_t contributedItemID, unsigned int searchDepth);
	MCP(Node *node, uint16_t requestedItemID, uint16_t requested_quantity, uint16_t contributedItemID, uint16_t contributed_quantity, unsigned int searchDepth);

	~MCP();

	// Agent methods
	void update() override;
	void stop() override;
	MCP* asMCP() override { return this; }
	void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) override;

	// Getters
	uint16_t requestedItemId() const { return _requestedItemId; }
	uint16_t contributedItemId() const { return _contributedItemId; }

	// Whether or not the negotiation finished
	bool negotiationFinished() const;

	// Whether or not there was a negotiation agreement
	bool negotiationAgreement() const;

	// It returns the search depth of this MCP
	unsigned int searchDepth() const { return _searchDepth; }

private:

	bool queryMCCsForItem(int itemId);


	void createChildUCP(const AgentLocation &ucc_location);

	void destroyChildUCP();

	uint16_t _requestedItemId;
	uint16_t _contributedItemId;

	uint16_t _requested_quantity=1;
	uint16_t _contributed_quantity=1;
	bool final_agreement = false;
	int _mccRegisterIndex; /**< Iterator through _mccRegisters. */
	std::vector<AgentLocation> _mccRegisters; /**< MCCs returned by the YP. */

	unsigned int _searchDepth;

	UCPPtr _ucp; /**< Child UCC. */
	
	// TODO: Add extra attributes and methods?
};

