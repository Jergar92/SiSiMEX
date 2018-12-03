#include "MCC.h"
#include "UCC.h"
#include "Application.h"
#include "ModuleAgentContainer.h"


enum State
{
	ST_INIT,
	ST_REGISTERING,
	ST_IDLE,
	ST_NEGOTIATING,
	ST_UNREGISTERING,
	ST_NEGOTIATION_FINISH,

	// TODO: Other states

	ST_FINISHED
};

MCC::MCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId)
{
	setState(ST_INIT);
}


MCC::~MCC()
{
}

void MCC::update()
{
	switch (state())
	{
	case ST_INIT:
		if (registerIntoYellowPages()) {
			setState(ST_REGISTERING);
		}
		else {
			setState(ST_FINISHED);
		}
		break;

	case ST_REGISTERING:
		
		// See OnPacketReceived()
		break;
	case ST_IDLE:

		break;
		// TODO: Handle other states
	case ST_NEGOTIATING:
		if (_ucc->IsFinish())
		{
			if (_ucc->final_agrement)
			{
				final_agreement = true;
				setState(ST_NEGOTIATION_FINISH);
			}
			else			
				setState(ST_IDLE);

			destroyChildUCC();

		}
		break;
	case ST_NEGOTIATION_FINISH:
		break;
	case ST_UNREGISTERING:
		break;
	case ST_FINISHED:
		destroy();
	}
}

void MCC::stop()
{
	// Destroy hierarchy below this agent (only a UCC, actually)
	destroyChildUCC();

	unregisterFromYellowPages();
	setState(ST_UNREGISTERING);
}


void MCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;

	switch (packetType)
	{
	case PacketType::RegisterMCCAck:
		if (state() == ST_REGISTERING)
		{
			setState(ST_IDLE);
			socket->Disconnect();
		}
		else
		{
			wLog << "OnPacketReceived() - PacketType::RegisterMCCAck was unexpected.";
		}
		break;
	case PacketType::MCPNegotiateMCCRequest:
		if (isIdling())
		{
			setState(ST_NEGOTIATING);
			createChildUCC();
			
			PacketHeader pkt_header;
			pkt_header.packetType = PacketType::MCCNegotiateMCPAnswer;
			pkt_header.srcAgentId = id();
			pkt_header.dstAgentId = packetHeader.srcAgentId;
			OutputMemoryStream packet;
			PacketMCCNegotiateMCPAnswer pkt_answer;
			AgentLocation ucclocation;

			ucclocation.hostIP = socket->RemoteAddress().GetIPString();
			ucclocation.agentId = _ucc->id();
			ucclocation.hostPort = LISTEN_PORT_AGENTS;

			pkt_answer.accepted = true;
			
			pkt_answer.ucc_location = ucclocation;
			
			pkt_header.Write(packet);
			pkt_answer.Write(packet);
		
			socket->SendPacket(packet.GetBufferPtr(), packet.GetSize());


		}
		else
		{
			PacketHeader pkt_header;
			pkt_header.packetType = PacketType::MCCNegotiateMCPAnswer;
			pkt_header.srcAgentId = id();
			pkt_header.dstAgentId = packetHeader.srcAgentId;
			OutputMemoryStream packet;
			PacketMCCNegotiateMCPAnswer pkt_answer;

			pkt_answer.accepted = false;

			pkt_header.Write(packet);
			pkt_answer.Write(packet);

			socket->SendPacket(packet.GetBufferPtr(), packet.GetSize());

		}
		break;
	case PacketType::UnregisterMCC:
		if (state()== ST_UNREGISTERING)
		{
			setState(ST_FINISHED);
			socket->Disconnect();
		}
		break;
	// TODO: Handle other packets

	default:
		wLog << "OnPacketReceived() - Unexpected PacketType.";
	}
}

bool MCC::isIdling() const
{
	return state() == ST_IDLE;
}

bool MCC::negotiationFinished() const
{
	return state() == ST_NEGOTIATION_FINISH;
}

bool MCC::negotiationAgreement() const
{
	// If this agent finished, means that it was an agreement
	// Otherwise, it would return to state ST_IDLE
	return final_agreement;
}

bool MCC::registerIntoYellowPages()
{
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::RegisterMCC;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketRegisterMCC packetData;
	packetData.itemId = _contributedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	return sendPacketToYellowPages(stream);
}

void MCC::unregisterFromYellowPages()
{
	// Create message
	PacketHeader packetHead;
	packetHead.packetType = PacketType::UnregisterMCC;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketUnregisterMCC packetData;
	packetData.itemId = _contributedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	sendPacketToYellowPages(stream);
}

void MCC::createChildUCC()
{
	_ucc.reset();
	_ucc = App->agentContainer->createUCC(node(), _contributedItemId, _constraintItemId);

	// TODO: Create a unicast contributor
}

void MCC::destroyChildUCC()
{
	if (_ucc.get())
	{
		_ucc->stop();
		_ucc.reset();
	}
	// TODO: Destroy the unicast contributor child
}
