#pragma once

#include "structs.h"
#include "connection.h"

// Testnet configuration
#define TESTNET_FAUCET_URL "https://testnet-faucet.qubic.li"
#define TESTNET_NODE_IP "testnet.qubic.li"
#define TESTNET_NODE_PORT 21841

// Testnet utility functions
void requestFaucetTokens(const char* address, const char* network);
void checkWalletBalance(const char* walletName, const char* network);
void configureTestnetNode(const char* network);
bool isTestnetNetwork(const char* network);
void printTestnetInfo();
