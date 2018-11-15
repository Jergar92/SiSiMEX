#include "MCP.h"
#include "Application.h"
#include "ModuleAgentContainer.h"


enum State
{
	ST_INIT,
	ST_REQUESTING_MCCs,
	ST_ITERATING_OVER_MCCs,
	ST_NEGOTIATION_FINISHED
};

MCP::MCP(Node *node, uint16_t requestedItemID, uint16_t contributedItemID) :
	Agent(node),
	_requestedItemId(requestedItemID),
	_contributedItemId(contributedItemID),
	_negotiationAgreement(false)
{
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
		// To do in the next session...
	default:;
	}
}

void MCP::stop()
{
	destroy();
}

void MCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;
	if (state() == ST_REQUESTING_MCCs && packetType == PacketType::ReturnMCCsForItem)
	{
		iLog << "OnPacketReceived PacketType::ReturnMCCsForItem " << _requestedItemId;
		PacketReturnMCCsForItem packet;

		packet.Read(stream);
		for (std::list<AgentLocation>::iterator it= packet.adresses.begin(); it != packet.adresses.end(); it++)
		{
			iLog << "MCC agent agentId " << it->agentId;
			iLog << "MCC agent hostIP " << it->hostIP;
			iLog << "MCC agent hostPort " << it->hostPort;

		}
		socket->Disconnect();
		setState(ST_ITERATING_OVER_MCCs);

		// TODO:
		// 1) Deserialize the packet
		// 2) Log the received MCC agent locations
		// 3) Disconnect the socket
		// 4) Set the next state (ST_ITERATING_OVER_MCCs) to start the search (for the next session)
	}
}

bool MCP::negotiationFinished() const
{
	return state() == ST_NEGOTIATION_FINISHED;
}

bool MCP::negotiationAgreement() const
{
	return _negotiationAgreement;
}


bool MCP::queryMCCsForItem(int itemId)
{
	// TODO:
	// 1) Create a query packet and fill it
	PacketQueryMCCsForItem packet;
	packet.itemId = itemId;

	// 2) Serialize it into an output stream
	OutputMemoryStream stream;
	PacketHeader packetHead;
	packetHead.packetType = PacketType::QueryMCCsForItem;
	packetHead.srcAgentId = id();

	packetHead.Write(stream);
	packet.Write(stream);
	// 3) Send it to the yellow pages (sendPacketToYellowPages() method)

	return 	sendPacketToYellowPages(stream);;
}
