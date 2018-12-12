#pragma once

#include "Module.h"
#include "net/Net.h"
#include "Node.h"
#include "MCC.h"
#include "MCP.h"
#include <map>

class ModuleNodeCluster : public Module, public TCPNetworkManagerDelegate
{
public:

	// Virtual methods from parent class Module

	bool init() override;

	bool start() override;

	bool update() override;

	bool updateGUI() override;

	bool cleanUp() override;

	bool stop() override;

	bool validMCC(uint16_t index, uint16_t agendtTD);

	void AddNegotation(uint16_t agendtTD, uint16_t requested_item);
	void RemoveNegotiation(uint16_t agendtTD, uint16_t requested_item);

	// TCPNetworkManagerDelegate virtual methods

	void OnAccepted(TCPSocketPtr socket) override;

	void OnPacketReceived(TCPSocketPtr socket, InputMemoryStream &stream) override;

	void OnDisconnected(TCPSocketPtr socket) override;

private:
	void SpawnAgentMCC();

	void ClearAgent();

	bool startSystem();

	void runSystem();

	void stopSystem();

	void CleanNegotiation();

	void ShowValueExchange(uint16_t contribution, uint16_t request, uint16_t contribution_quantity, uint16_t request_quantity);

	void ShowBox(uint16_t rarity_value);
	void spawnMCP(int nodeId, int requestedItemId,int requested_quantity, int contributedItemId,int contributed_quantity,int actual_amount_contribution);
	void spawnMCP(int nodeId, int requestedItemId, int contributedItemId);

	void spawnMCC(int nodeId, int contributedItemId, int constraintItemId);
	void spawnMCC(int nodeId, int contributedItemId, int contributed_cuantity, int constraintItemId);

	bool ValidateSpawn(int requestedItem, int petition_quantity);

	std::vector<NodePtr> _nodes; /**< Array of nodes spawn in this host. */
	std::map<uint16_t, std::vector<uint16_t>> current_negotiation;

	int state = 0; /**< State machine. */
};
