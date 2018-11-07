#include "MCC.h"
#include "Application.h"
#include "ModuleAgentContainer.h"

// With these states you have enough so far...
enum State
{
	ST_INIT,
	ST_REGISTERING,
	ST_IDLE,
	ST_UNREGISTERING,
	ST_FINISHED
};

MCC::MCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId)
{
}


MCC::~MCC()
{
}

void MCC::start()
{
	setState(ST_INIT);
	// TODO: Set the initial state
}

void MCC::update()
{
	switch (state())
	{
	case ST_INIT:
		break;
	case ST_REGISTERING:
		registerIntoYellowPages();
		break;
	case ST_UNREGISTERING:
		unregisterFromYellowPages();
		break;
	case ST_FINISHED:
		destroy();
		break;
		// TODO:
		// - Register or unregister into/from YellowPages depending on the state
		//       Use the functions registerIntoYellowPages and unregisterFromYellowPages
		//       so that this switch statement remains clean and readable
		// - Set the next state when needed ...
	default:
		setState(ST_INIT);
		break;
	}
}

void MCC::stop()
{
	unregisterFromYellowPages();
	setState(ST_UNREGISTERING);
}


void MCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	// Taking the state into account, receive and deserialize packets (Ack packets) and set next state
	if (state() == ST_REGISTERING && packetHeader.packetType == PacketType::RegisterMCCAck)
	{
		// TODO: Set the next state (Idle in this case)
		setState(ST_IDLE);
		// TODO: Disconnect the socket (we don't need it anymore)
		socket->Disconnect();	
	}
	// TODO: Do the same for unregistering
	if (state() == ST_UNREGISTERING && packetHeader.packetType == PacketType::UnregisterMCCAck)
	{
		// TODO: Set the next state (Idle in this case)
		setState(ST_IDLE);
		// TODO: Disconnect the socket (we don't need it anymore)
		socket->Disconnect();

	}
}

bool MCC::negotiationFinished() const
{
	return false;
}

bool MCC::negotiationAgreement() const
{
	return false;
}

bool MCC::registerIntoYellowPages()
{
	// TODO: Create a PacketHeader (make it in Packets.h)
	PacketHeader packet_header;
	packet_header.packetType = PacketType::RegisterMCC;
	packet_header.srcAgentId = id();
	// TODO: Create a PacketRegisterMCC (make it in Packets.h)
	PacketRegisterMCC packet_register_mcc;
	packet_register_mcc.itemId = contributedItemId();
	// TODO: Serialize both packets into an OutputMemoryStream
	OutputMemoryStream stream;
	packet_header.Write(stream);
	packet_register_mcc.Write(stream);
	sendPacketToYellowPages(stream);
	// TODO: Send the stream (Agent::sendPacketToYellowPages)

	return false;
}

bool MCC::unregisterFromYellowPages()
{
	// TODO: Create a PacketHeader (make it in Packets.h)
	PacketHeader packet_header;
	packet_header.packetType = PacketType::UnregisterMCC;
	packet_header.srcAgentId = id();
	
	// TODO: Create a PacketUnregisterMCC (make it in Packets.h)
	PacketUnregisterMCC packet_unregister_mcc;
	packet_unregister_mcc.itemId = contributedItemId();

	// TODO: Serialize both packets into an OutputMemoryStream
	OutputMemoryStream stream;
	packet_header.Write(stream);
	packet_unregister_mcc.Write(stream);
	sendPacketToYellowPages(stream);
	// TODO: Send the stream (Agent::sendPacketToYellowPages)

	return false;
}
