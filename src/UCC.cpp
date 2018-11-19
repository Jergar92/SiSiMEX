#include "UCC.h"


// TODO: Make an enum with the states
enum State
{	
	ST_INIT,
	ST_WAITING_ITEM_REQEST,
	ST_WAITING_ITEM_CONSTRAINT,
	ST_IDLE,
	// TODO: Other states

	ST_NEGOTIATION_FINISHED
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node)
{
	// TODO: Save input parameters
    _contributedItemId = constraintItemId;
	_constraintItemId = constraintItemId;
}

UCC::~UCC()
{
}

void UCC::stop()
{
	destroy();
}

void UCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	switch (packetType)
	{
		// TODO: Handle packets
	case PacketType::ItemRequest:
		{
		if (state() == ST_WAITING_ITEM_REQEST)
		{
			setState(ST_IDLE);
		}
		}

	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}
