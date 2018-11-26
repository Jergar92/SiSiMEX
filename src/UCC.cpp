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
				setState(UCC_ST_WAITING_CONSTRAIN);

				OutputMemoryStream stream;
				PacketHeader packet;
				packet.packetType = PacketType::UCCNegotiateUCPConstrainRequest;
				PacketUCCNegotiateUCPConstrainRequest contrain_request;
				contrain_request.itemId = _constraintItemId;
				packet.Write(stream);
				contrain_request.Write(stream);
				socket->SendPacket(stream.GetBufferPtr(), stream.GetSize());
			}
		}
		else
		{
			wLog << "OnPacketReceived() - UCPNegotiateUCCItemRequest - Unexpected PacketType.";

		}
		// TODO: Handle packets
	case PacketType::UCPNegotiateUCCConstrainResult:
		if (state() == UCC_ST_WAITING_CONSTRAIN)
		{
			setState(UCC_ST_FINISHED);

			PacketUCPNegotiateUCCConstrainResult constrain_results;
			constrain_results.Read(stream);
			final_agrement = constrain_results.agrement;

			OutputMemoryStream stream;
			PacketHeader packet;
			packet.packetType = PacketType::UCCNegotiateUCPACK;
			packet.Write(stream);
			socket->SendPacket(stream.GetBufferPtr(), stream.GetSize());
		}
		else
		{
			wLog << "OnPacketReceived() - UCPNegotiateUCCConstrainResult - Unexpected PacketType.";

		}
		break;
	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}

bool UCC::IsFinish()
{
	return state()==UCCState::UCC_ST_FINISHED;
}
