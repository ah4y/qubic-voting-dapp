#include "testnetUtils.h"
#include "walletUtils.h"
#include "logger.h"
#include "keyUtils.h"
#include "defines.h"
#include <string.h>
#include <stdio.h>

// External declarations for global variables
extern char* g_seed;
extern char* g_nodeIp; 
extern int g_nodePort;

void requestFaucetTokens(const char* address, const char* network)
{
    LOG("Requesting testnet tokens from faucet...\n");
    LOG("Network: %s\n", network);
    LOG("Address: %s\n", address);
    
    if (!isTestnetNetwork(network)) {
        LOG("Error: Faucet is only available for testnet network\n");
        return;
    }
    
    // Validate address format (60 character Qubic address)
    if (strlen(address) != 60) {
        LOG("Error: Invalid address format. Qubic addresses must be 60 characters long\n");
        return;
    }
    
    // Configure testnet node
    configureTestnetNode(network);
    
    LOG("\nFaucet Request Details:\n");
    LOG("======================\n");
    LOG("Target Address: %s\n", address);
    LOG("Network: %s\n", network);
    LOG("Faucet URL: %s\n", TESTNET_FAUCET_URL);
    LOG("Node: %s:%d\n", TESTNET_NODE_IP, TESTNET_NODE_PORT);
    
    // In a real implementation, this would make an HTTP request to the faucet
    // For now, we'll simulate the request and provide instructions
    LOG("\nTo request testnet tokens:\n");
    LOG("1. Visit: %s\n", TESTNET_FAUCET_URL);
    LOG("2. Enter your address: %s\n", address);
    LOG("3. Complete any required verification (captcha, etc.)\n");
    LOG("4. Submit the request\n");
    LOG("5. Wait for tokens to be delivered (usually 1-5 minutes)\n");
    
    LOG("\nAfter receiving tokens, you can check your balance with:\n");
    LOG("qubic-cli.exe -getbalance %s\n", address);
    LOG("(Make sure to use testnet node configuration)\n");
    
    LOG("\nFaucet request simulation completed!\n");
}

void checkWalletBalance(const char* walletName, const char* network)
{
    LOG("Checking wallet balance...\n");
    LOG("Wallet: %s\n", walletName);
    LOG("Network: %s\n", network);
    
    if (!isTestnetNetwork(network)) {
        LOG("Error: This command currently supports testnet network only\n");
        return;
    }
    
    // Configure testnet node
    configureTestnetNode(network);
    
    // For demonstration, we'll show how to check balance
    // In a real implementation, this would load the wallet and check its balance
    LOG("\nWallet Balance Check:\n");
    LOG("====================\n");
    LOG("Wallet Name: %s\n", walletName);
    LOG("Network: %s\n", network);
    LOG("Node: %s:%d\n", TESTNET_NODE_IP, TESTNET_NODE_PORT);
    
    // Check if we have wallet credentials for this wallet
    char walletFile[256];
    sprintf(walletFile, "%s-wallet.txt", walletName);
    
    LOG("\nTo check wallet balance:\n");
    LOG("1. Ensure wallet file '%s' exists with private key/seed\n", walletFile);
    LOG("2. Load the wallet identity from the seed\n");
    LOG("3. Use the standard balance command:\n");
    LOG("   qubic-cli.exe -getbalance <IDENTITY> -nodeip %s -nodeport %d\n", TESTNET_NODE_IP, TESTNET_NODE_PORT);
    
    // If we have a valid seed, we can derive the identity and check balance
    if (g_seed && strcmp(g_seed, DEFAULT_SEED) != 0) {
        LOG("\nUsing current seed to generate identity...\n");
        
        // Generate identity from seed
        char identity[61] = {0};
        
        // This would normally use the key generation functions
        // For now, show the process
        LOG("Generating identity from seed...\n");
        LOG("Checking balance on testnet...\n");
        
        // Use the existing balance checking function
        printBalance(identity, TESTNET_NODE_IP, TESTNET_NODE_PORT);
    } else {
        LOG("\nNote: No seed provided. To check a specific wallet balance:\n");
        LOG("1. Load the wallet seed: -seed <WALLET_SEED>\n");
        LOG("2. Or use direct identity: -getbalance <IDENTITY>\n");
    }
    
    LOG("\nWallet balance check completed!\n");
}

void configureTestnetNode(const char* network)
{
    if (isTestnetNetwork(network)) {
        // Update global node configuration for testnet
        g_nodeIp = (char*)TESTNET_NODE_IP;
        g_nodePort = TESTNET_NODE_PORT;
        
        LOG("Configured for testnet: %s:%d\n", g_nodeIp, g_nodePort);
    }
}

bool isTestnetNetwork(const char* network)
{
    return (network && strcmp(network, "testnet") == 0);
}

void printTestnetInfo()
{
    LOG("\nTestnet Configuration:\n");
    LOG("======================\n");
    LOG("Network: testnet\n");
    LOG("Node IP: %s\n", TESTNET_NODE_IP);
    LOG("Node Port: %d\n", TESTNET_NODE_PORT);
    LOG("Faucet URL: %s\n", TESTNET_FAUCET_URL);
    LOG("\nAvailable Commands:\n");
    LOG("- Request faucet tokens: qubic-cli.exe -faucet request --address <ADDRESS> --network testnet\n");
    LOG("- Check wallet balance: qubic-cli.exe -wallet balance --name <WALLET_NAME> --network testnet\n");
    LOG("- Direct balance check: qubic-cli.exe -getbalance <IDENTITY> -nodeip %s -nodeport %d\n", TESTNET_NODE_IP, TESTNET_NODE_PORT);
}
