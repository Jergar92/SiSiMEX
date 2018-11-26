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
			sendPacketToAgent(ucc_location.hostIP, ucc_location.hostPort, stream);

		}
				break;
		case UCPState::UCP_ST_REQUEST:
				break;
		case UCPState::UCP_ST_RESOLVING_CONSTRAIN:
			if (_mcp->negotiationFinished())
			{
				PacketHeader packet;
				packet.packetType = PacketType::UCPNegotiateUCCConstrainResult;

				PacketUCPNegotiateUCCConstrainResult constrain_result;
				constrain_result.agrement = _mcp->negotiationAgreement();

				OutputMemoryStream stream;
				packet.Write(stream);
				constrain_result.Write(stream);
				sendPacketToAgent(ucc_location.hostIP, ucc_location.hostPort, stream);
			}
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
			OutputMemoryStream stream;
			PacketUCPNegotiateUCCConstrainResult constrain_result;
			PacketHeader packet;
			packet.packetType = PacketType::UCPNegotiateUCCConstrainResult	;

			if (item_request.itemId == _contributedItemId)
			{
				constrain_result.agrement = true;

				packet.Write(stream);
				constrain_result.Write(stream);

				//match

			}
			else
			{
				constrain_result.agrement = false;

				packet.Write(stream);
				constrain_result.Write(stream);

				//fail
			}
			if (searchDepth < 6)
			{
				createChildMCP(item_request.itemId);
			}
			socket->SendPacket(stream.GetBufferPtr(), stream.GetSize());
		}
		else
		{
			wLog << "OnPacketReceived() - UCCNegotiateUCPConstrainRequest - Unexpected PacketType.";

		}
		// TODO: Handle packets
		break;
	case PacketType::UCCNegotiateUCPACK:
		if (state() == UCP_ST_SENDING_CONSTRAIN)
		{
			PacketUCCNegotiateUCPACK item_ack;
			item_ack.Read(stream);
			final_agrement = item_ack.agrement = final_agrement;
			setState(UCP_ST_FINISHED);
		}
		else
		{
			wLog << "OnPacketReceived() - UCCNegotiateUCPACK - Unexpected PacketType.";

		}
		// TODO: Handle packets
		break;
	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}

bool UCP::IsFinish()
{
	return state()==UCP_ST_FINISHED;
}

void UCP::createChildMCP(uint16_t request)
{
	_mcp.reset(new MCP(node(), request, _contributedItemId, searchDepth));
}

