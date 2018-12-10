#include "UCP.h"
#include "MCP.h"
#include "Application.h"
#include "ModuleAgentContainer.h"
#define MAX_DEPTH 6

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
	Agent(node),_requestedItemId(requestedItemId), _contributedItemId(contributedItemId), ucc_location(uccLocation), searchDepth(searchDepth)
{
	setState(UCP_ST_INIT);
	// TODO: Save input parameters
}

UCP::UCP(Node * node, uint16_t requestedItemId, uint16_t requested_quantity, uint16_t contributedItemId, uint16_t contributed_quantity, uint16_t actual_amount_contribution, const AgentLocation & uccLocation, unsigned int searchDepth):
	Agent(node), _requestedItemId(requestedItemId), _requested_quantity(requested_quantity), _contributedItemId(contributedItemId), _contributed_quantity(contributed_quantity), _actual_contributed_quantity(actual_amount_contribution), ucc_location(uccLocation), searchDepth(searchDepth)
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
		case UCPState::UCP_ST_INIT:
		{
			setState(UCP_ST_REQUEST);

			PacketHeader packet;
			packet.packetType = PacketType::UCPNegotiateUCCItemRequest;
			packet.srcAgentId = id();
			packet.dstAgentId = ucc_location.agentId;
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

				setState(UCP_ST_SENDING_CONSTRAIN);
				PacketHeader packet;
				packet.packetType = PacketType::UCPNegotiateUCCConstrainResult;
				packet.srcAgentId = id();
				packet.dstAgentId = ucc_location.agentId;
				PacketUCPNegotiateUCCConstrainResult constrain_result;
				constrain_result.agrement = _mcp->negotiationAgreement();
				constrain_result.quantity = _mcp->contributed_quantity();
				OutputMemoryStream stream;
				packet.Write(stream);
				constrain_result.Write(stream);
				sendPacketToAgent(ucc_location.hostIP, ucc_location.hostPort, stream);
			}
			break;
		case UCPState::UCP_ST_SENDING_CONSTRAIN:
			break;
		case UCPState::UCP_ST_FINISHED:
			destroyChildMCP();
			break;
		default:
			break;
		}
		// TODO: Handle states

}

void UCP::stop()
{
	// TODO: Destroy search hierarchy below this agent
	destroyChildMCP();
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
			PacketHeader packet;
			packet.packetType = PacketType::UCPNegotiateUCCConstrainResult;
			PacketUCCNegotiateUCPConstrainRequest item_request;
			PacketUCPNegotiateUCCConstrainResult constrain_result;

			item_request.Read(stream);
			packet.srcAgentId = id();
			packet.dstAgentId = packetHeader.srcAgentId;
			OutputMemoryStream stream;


			if (item_request.itemId == _contributedItemId)
			{
				setState(UCP_ST_SENDING_CONSTRAIN);

				constrain_result.agrement = true;
				constrain_result.quantity = _contributed_quantity;
				packet.Write(stream);
				constrain_result.Write(stream);

				//match

			}
			else if(searchDepth < MAX_DEPTH)
			{
				setState(UCP_ST_RESOLVING_CONSTRAIN);
				int petition_quantity = 0;
				int contribution_quantity = 0;

				bool check = ShowDealProposition(item_request.itemId, _contributedItemId, petition_quantity, contribution_quantity);
				if (!check && _actual_contributed_quantity < contribution_quantity)
				{
					setState(UCP_ST_SENDING_CONSTRAIN);
					constrain_result.agrement = false;
					constrain_result.quantity = _contributed_quantity;
					packet.Write(stream);
					constrain_result.Write(stream);
				}
				else
				{
					createChildMCP(item_request.itemId, petition_quantity);

				}
			}		
			else
			{
				setState(UCP_ST_SENDING_CONSTRAIN);

				constrain_result.agrement = false;
				constrain_result.quantity = _contributed_quantity;

				packet.Write(stream);
				constrain_result.Write(stream);

				//fail
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
			final_agrement = item_ack.agrement;
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

void UCP::createChildMCP(uint16_t request, uint16_t requested_quantity)
{
	destroyChildMCP();
	_mcp.reset();
	_mcp = App->agentContainer->createMCP(node(), request, requested_quantity, _contributedItemId,_contributed_quantity, _actual_contributed_quantity, searchDepth + 1);

}

void UCP::createChildMCP(uint16_t request)
{
	destroyChildMCP();
	_mcp.reset();
	_mcp = App->agentContainer->createMCP(node(), request, _contributedItemId, searchDepth+1);

}

void UCP::EndAgreement()
{
}

void UCP::destroyChildMCP()
{
	if (_mcp.get())
	{
		_mcp->stop();
		_mcp.reset();
	}
}

