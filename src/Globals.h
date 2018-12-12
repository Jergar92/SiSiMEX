#pragma once
#include <cinttypes>

// Constants ///////////////////////////////////////////////////////////

/** Hostname (or IP address) of the YellowPages process. */
static const char *HOSTNAME_YP = "localhost";

/** Listen port used by the YellowPages process. */
static const uint16_t LISTEN_PORT_YP = 8000;

/** Listen port used by the multi-agent application. */
static const uint16_t LISTEN_PORT_AGENTS = 8001;

/**
 * Constant used to specify that a message was sent to,
 * or received from no agent. This is the case when
 * communicating with the YellowPages. Yellow pages is
 * a global service that contains information about
 * contributor agents, but uses no agents to work.
 */
static const uint16_t NULL_AGENT_ID = 0;

/*
 * RANDOM INITIALIZATION:
 * Whether or not perform a random initialization of items among nodes.
 * The initialzation takes place at ModuleNodeCluster::startSystem().
 */
#define RANDOM_INITIALIZATION

/**
 * MAX_ITEMS:
 * This constant defines which is the maximum number of items of the catalogue.
 * Items will be identified by an index between 0 and MAX_ITEMS - 1.
 *
 * MAX_NODES:
 * This constant defines which is the maximum number of items of the catalogue.
 * Items will be identified by an index between 0 and MAX_ITEMS - 1.
 */

#if defined(RANDOM_INITIALIZATION)
enum ItemType
{
	COMMON,
	RARE,
	EPIC,
	LEGEND,
	NO_TYPE
};
static const unsigned int MAX_COMMON_ITEMS = 4U;
static const unsigned int MAX_RARE_ITEMS = 3U;
static const unsigned int MAX_EPIC_ITEMS = 2U;
static const unsigned int MAX_LEGEND_ITEMS = 1U;
static const unsigned int MAX_ITEMS = MAX_COMMON_ITEMS+ MAX_RARE_ITEMS+ MAX_EPIC_ITEMS+ MAX_LEGEND_ITEMS;
static const unsigned int MAX_NODES = 10U;

static const unsigned int GetItemType(unsigned int i)
{
	unsigned int check = MAX_COMMON_ITEMS;
	if (i < check)
		return ItemType::COMMON;
	check += MAX_RARE_ITEMS;
	if(i<check)
		return ItemType::RARE;
	check += MAX_EPIC_ITEMS;
	if (i<check)
		return ItemType::EPIC;
	check += MAX_LEGEND_ITEMS;
	if (i<check)
		return ItemType::LEGEND;

	return ItemType::NO_TYPE;
}

static const bool ShowDealProposition(int requestedItem, int contributionItem, int& requested_quantity, int& contribution_quantity)
{
	ItemType requested_type = static_cast<ItemType>(GetItemType(requestedItem));
	ItemType contribution_type = static_cast<ItemType>(GetItemType(contributionItem));

		
	if (requested_type == ItemType::NO_TYPE || contribution_type == ItemType::NO_TYPE)
		return false;
	requested_quantity = 1;
	contribution_quantity = 1;
	if (requested_type == contribution_type)
		return true;

	if (requested_type > contribution_type)
	{
		requested_quantity = 1;
		for (int i = contribution_type; i < requested_type; i++)
		{
			contribution_quantity *= 2;
		}
	}
	else
	{
		contribution_quantity = 1;
		for (int i = requested_type; i < contribution_type; i++)
		{
			requested_quantity *= 2;
		}
	}
	return true;
}
#else

static const unsigned int MAX_ITEMS = 4U;
static const unsigned int MAX_NODES = 4U;

#endif

/**
 * MAX_SEARCH_DEPTH
 * This the maximum depth of the search performed by MCP/UCP agents.
 * If MAX_SEARCH_DEPTH == 0, only bilateral exchanges will be found.
 * If MAX_SEARCH_DEPTH == 1, trilateral exchanges will be also found.
 * etc.
 */
#define MAX_SEARCH_DEPTH 4
