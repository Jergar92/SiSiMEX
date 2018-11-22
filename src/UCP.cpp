#include "UCP.h"
#include "MCP.h"
#include "Application.h"
#include "ModuleAgentContainer.h"


// TODO: Make an enum with the states
enum UCPState
{
	UCP_ST_INIT,
	UCP_ST_REQUEST,
	UCP_ST_RESOLVING_CONSTRAIN,
	UCP_ST_SENDING_CONSTRAIN,

	// TODO: Other states

	UCP_ST_FINISHED
};

UCP::UCP(Node *node, uint16_t requestedItemId, uint16_t contributedItemId, const AgentLocation &uccLocation, unsigned int searchDepth) :
	Agent(node),_requestedItemId(requestedItemId), _contributedItemId(contributedItemId), ucc_location(uccLocation)
{
	setState(UCP_ST_INIT);
	// TODO: Save input parameters
}

UCP::~UCP()
{
}

void UCP::update()
{
	switch (state())
	{
		switch (state())
		{
		case UCPState::UCP_ST_INIT:
		{
			PacketHeader packet;
			packet.packetType = PacketType::UCPNegotiateUCCItemRequest;

			PacketUCPNegotiateUCCItemRequest item_request;
			item_request.itemId = _requestedItemId;

			OutputMemoryStream stream;
			packet.Write(stream);
			item_request.Write(stream);
		}
			//sendPacketToAgent(ucc_location.hostIP, ucc_location.hostPort)
				break;
		case UCPState::UCP_ST_REQUEST:
				break;
		case UCPState::UCP_ST_RESOLVING_CONSTRAIN:
			break;
		case UCPState::UCP_ST_SENDING_CONSTRAIN:
			break;
		case UCPState::UCP_ST_FINISHED:
			break;
		default:
			break;
		}
		// TODO: Handle states

	default:;
	}
}

void UCP::stop()
{
	// TODO: Destroy search hierarchy below this agent

	destroy();
}

void UCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	switch (packetType)
	{
	case PacketType::UCCNegotiateUCPConstrainRequest:
		if (state() == UCP_ST_REQUEST)
		{
			PacketUCCNegotiateUCPConstrainRequest item_request;
			item_request.Read(stream);
			if (item_request.itemId == _contributedItemId)
			{
				//match
			}
			else
			{
				//fail
			}
		}
		else
		{
			wLog << "OnPacketReceived() - UCCNegotiateUCPConstrainRequest - Unexpected PacketType.";

		}
		// TODO: Handle packets

	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}
