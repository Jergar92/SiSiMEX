#include "UCC.h"


// TODO: Make an enum with the states
enum UCCState
{
	UCC_ST_INIT,
	UCC_ST_REGISTERING,
	UCC_ST_IDLE,
	UCC_ST_NEGOTIATING,
	UCC_ST_UNREGISTERING,

	// TODO: Other states

	UCC_ST_FINISHED
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node), _contributedItemId(contributedItemId), _constraintItemId(constraintItemId)
{
	// TODO: Save input parameters
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

	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}

bool UCC::IsFinish()
{
	return state()==UCCState::UCC_ST_FINISHED;
}
