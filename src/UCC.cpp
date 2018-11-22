#include "UCC.h"


// TODO: Make an enum with the states
enum UCCState
{

	UCC_ST_WAITING_REQUEST,
	UCC_ST_WAITING_CONSTRAIN,

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
	case PacketType::UCPNegotiateUCCItemRequest:
		if (state() == UCC_ST_WAITING_REQUEST)
		{
			PacketUCPNegotiateUCCItemRequest item_request;
			item_request.Read(stream);
			if (item_request.itemId == _contributedItemId)
			{

			}
		}
		else
		{
			wLog << "OnPacketReceived() - UCPNegotiateUCCItemRequest - Unexpected PacketType.";

		}
		// TODO: Handle packets

	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}

bool UCC::IsFinish()
{
	return state()==UCCState::UCC_ST_FINISHED;
}
