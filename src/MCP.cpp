#include "MCP.h"
#include "UCP.h"
#include "Application.h"
#include "ModuleAgentContainer.h"


enum State
{
	ST_INIT,
	ST_REQUESTING_MCCs,
	ST_ITERATING_OVER_MCCs,
	ST_WAITING_ACCEPTANCE,
	ST_NEGOTIATING,
	// TODO: Other states

	ST_NEGOTIATION_FINISHED
};

MCP::MCP(Node *node, uint16_t requestedItemID, uint16_t contributedItemID, unsigned int searchDepth) :
	Agent(node),
	_requestedItemId(requestedItemID),
	_contributedItemId(contributedItemID),
	_searchDepth(searchDepth)
{
	final_agreement = false;
	setState(ST_INIT);
}

MCP::~MCP()
{
}

void MCP::update()
{
	switch (state())
	{
	case ST_INIT:
		queryMCCsForItem(_requestedItemId);
		setState(ST_REQUESTING_MCCs);
		break;

	case ST_ITERATING_OVER_MCCs:
	{
		if (_mccRegisters.empty() || _mccRegisters.size() <= _mccRegisterIndex)
		{
			setState(ST_NEGOTIATION_FINISHED);
			break;
		}
		OutputMemoryStream packet;
		PacketHeader pkt_header;
		PacketMCPNegotiateMCCRequest pkt_request;
		AgentLocation agent = _mccRegisters[_mccRegisterIndex++];
		pkt_header.packetType = PacketType::MCPNegotiateMCCRequest;
		pkt_header.srcAgentId = id();
		pkt_header.dstAgentId = agent.agentId;


		pkt_header.Write(packet);
		pkt_request.Write(packet);

		sendPacketToAgent(agent.hostIP, agent.hostPort, packet);
		setState(ST_WAITING_ACCEPTANCE);

	}
			// TODO: Handle this state
		break;

	case ST_WAITING_ACCEPTANCE:
		
		break;
	case ST_NEGOTIATING:
		if (_ucp->IsFinish())
		{
			if (_ucp->final_agrement)
			{
				final_agreement = true;
				setState(ST_NEGOTIATION_FINISHED);
			}
			else
			{
				setState(ST_ITERATING_OVER_MCCs);
			}
			destroyChildUCP();

		}
		
		break;

	// TODO: Handle other states

	default:;
	}
}

void MCP::stop()
{
	// TODO: Destroy the underlying search hierarchy (UCP->MCP->UCP->...)
	destroyChildUCP();
	destroy();
}

void MCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;

	switch (packetType)
	{
	case PacketType::ReturnMCCsForItem:
		if (state() == ST_REQUESTING_MCCs)
		{
			// Read the packet
			PacketReturnMCCsForItem packetData;
			packetData.Read(stream);

			// Log the returned MCCs
			for (auto &mccdata : packetData.mccAddresses)
			{
				uint16_t agentId = mccdata.agentId;
				const std::string &hostIp = mccdata.hostIP;
				uint16_t hostPort = mccdata.hostPort;
				//iLog << " - MCC: " << agentId << " - host: " << hostIp << ":" << hostPort;
			}

			// Store the returned MCCs from YP
			_mccRegisters.swap(packetData.mccAddresses);

			// Select the first MCC to negociate
			final_agreement = false;

			_mccRegisterIndex = 0;
			setState(ST_ITERATING_OVER_MCCs);

			socket->Disconnect();
		}
		else
		{
			wLog << "OnPacketReceived() - PacketType::ReturnMCCsForItem was unexpected.";
		}
		break;
	case PacketType::MCCNegotiateMCPAnswer:
		if (state() == ST_WAITING_ACCEPTANCE)
		{
			PacketMCCNegotiateMCPAnswer pkt_answer;
			pkt_answer.Read(stream);
			if (pkt_answer.accepted)
			{
				setState(ST_NEGOTIATING);				
				createChildUCP(pkt_answer.ucc_location);
			}
			else
			{
				setState(ST_ITERATING_OVER_MCCs);

			}
		}
		else
		{
			wLog << "OnPacketReceived() - PacketType::MCCNegotiateMCPAnswer was unexpected.";
		}
		break;
	// TODO: Handle other packets

	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}

bool MCP::negotiationFinished() const
{
	return state() == ST_NEGOTIATION_FINISHED;
}

bool MCP::negotiationAgreement() const
{
	return final_agreement; // TODO: Did the child UCP find a solution?
}


bool MCP::queryMCCsForItem(int itemId)
{
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::QueryMCCsForItem;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketQueryMCCsForItem packetData;
	packetData.itemId = _requestedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);
	// 1) Ask YP for MCC hosting the item 'itemId'
	return sendPacketToYellowPages(stream);
}

void MCP::createChildUCP(const AgentLocation &ucc_location)
{
	_ucp.reset();
	_ucp = App->agentContainer->createUCP(node(), _requestedItemId, _contributedItemId, ucc_location, _searchDepth);
	
}

void MCP::destroyChildUCP()
{
	if (_ucp.get())
	{
		_ucp->stop();
		_ucp.reset();
	}
}
