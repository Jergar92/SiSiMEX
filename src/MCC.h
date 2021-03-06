#pragma once
#include "Agent.h"

// Forward declaration
class UCC;
using UCCPtr = std::shared_ptr<UCC>;

class MCC :
	public Agent
{
public:

	// Constructor and destructor
	MCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId);
	MCC(Node *node, uint16_t contributedItemId, uint16_t contributed_quantity, uint16_t constraintItemId);

	~MCC();

	// Agent methods
	void update() override;
	void stop() override;
	MCC* asMCC() override { return this; }
	void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) override;

	// Getters
	bool isIdling() const;
	uint16_t contributedItemId() const { return _contributedItemId; }
	uint16_t constraintItemId() const { return _constraintItemId; }

	uint16_t contributed_quantity() const { return _contributed_quantity; }
	uint16_t constrain_quantity() const { return _constrain_quantity; }
	// Whether or not the negotiation finished
	bool negotiationFinished() const;

	// Whether or not there was a negotiation agreement
	bool negotiationAgreement() const;

private:

	bool registerIntoYellowPages();
	
	void unregisterFromYellowPages();
	bool validMCC(uint16_t quantity_request);
	void createChildUCC();

	void destroyChildUCC();

	uint16_t _contributedItemId; /**< The contributed item. */
	uint16_t _total_contributed_quantity; /**< The Total contributed quantity. */
	uint16_t _contributed_quantity; /**< The contributed quantity. */

	uint16_t _constraintItemId; /**< The constraint item. */
	uint16_t _constrain_quantity; /**< The constrain quantity. */

	bool participating = false;
	bool final_agreement = false;
	UCCPtr _ucc; /**< Child UCC. */
};
