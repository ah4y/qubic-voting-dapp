#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstring>
#include <cstdlib>

// Real Qubic Dev Kit execution types
enum ExecutionType {
    EXECUTION_CONTRACT_CALL = 1,
    EXECUTION_CONTRACT_DEPLOY = 2,
    EXECUTION_TRANSFER = 3
};

// Transaction status enumeration
enum TransactionStatus {
    TX_STATUS_PENDING = 0,
    TX_STATUS_CONFIRMED = 1,
    TX_STATUS_FAILED = 2,
    TX_STATUS_NOT_FOUND = 3
};

// Execution parameters structure
struct ExecutionParams {
    ExecutionType executionType;
    char network[16];                // "testnet" or "mainnet"
    char nodeIp[64];                // Node IP address
    int nodePort;                   // Node port
    char privateKey[128];           // Private key for signing
    char contractAddress[64];       // Contract address (for calls)
    char functionName[64];          // Function name (for calls)
    char functionArgs[512];         // Function arguments (for calls)
    char bytecodeFile[256];         // Bytecode file path (for deployment)
    char destinationAddress[64];    // Destination address (for transfers)
    unsigned long long amount;      // Amount to transfer
    int timeoutSeconds;             // Timeout for confirmation
};

// Transaction structure
struct Transaction {
    ExecutionType type;
    char sourceAddress[64];
    char destinationAddress[64];
    unsigned long long amount;
    unsigned long long timestamp;
    unsigned char data[4096];       // Transaction data
    unsigned int dataSize;
    unsigned char signature[64];    // Transaction signature
};

// Contract call data structure
struct ContractCallData {
    char functionName[64];
    char arguments[512];
};

// Main execution functions
bool executeRealQubicTransaction(const ExecutionParams& params);
bool executeRealContractCall(const char* contractAddress, const char* functionName, 
                           const char* arguments, const char* privateKey, 
                           const char* network, std::string& result);
bool executeRealContractDeployment(const char* bytecodeFile, const char* privateKey, 
                                 const char* network, std::string& contractAddress);

// Core execution functions
bool validateExecutionParams(const ExecutionParams& params);
bool connectToQubicNetwork(const char* nodeIp, int nodePort);
bool prepareTransaction(const ExecutionParams& params, Transaction& tx);
bool prepareContractCallData(const ExecutionParams& params, Transaction& tx);
bool prepareContractDeployData(const ExecutionParams& params, Transaction& tx);
bool signTransaction(Transaction& tx, const char* privateKey);
bool broadcastTransaction(const Transaction& tx, std::string& txId);
bool waitForConfirmation(const std::string& txId, int timeoutSeconds);

// Helper functions
bool deriveAddressFromPrivateKey(const char* privateKey, char* address);
bool encodeContractArguments(const char* args, char* encoded);
bool readBytecodeFromFile(const char* filename, std::string& bytecode);
bool validateBytecode(const std::string& bytecode);
bool convertPrivateKeyToBytes(const char* privateKey, unsigned char* bytes);
bool createTransactionHash(const Transaction& tx, unsigned char* hash);
bool signHash(const unsigned char* hash, const unsigned char* privateKey, unsigned char* signature);
bool serializeTransaction(const Transaction& tx, std::string& serialized);
unsigned long long getCurrentTimestamp();
TransactionStatus getTransactionStatus(const std::string& txId);
std::string getContractCallResult(const char* contractAddress, const char* functionName);
std::string generateContractAddress(const char* privateKey, const char* bytecodeFile);
std::string generateTransactionId(const Transaction& tx);

// Real execution API - High-level interface
class RealQubicExecutor {
public:
    RealQubicExecutor(const char* network, const char* nodeIp, int nodePort);
    ~RealQubicExecutor();
    
    // Contract operations
    bool deployContract(const char* bytecodeFile, const char* privateKey, std::string& contractAddress);
    bool callContract(const char* contractAddress, const char* functionName, const char* arguments, 
                     const char* privateKey, std::string& result);
    
    // Voting-specific operations
    bool createVotingProposal(const char* contractAddress, const char* title, const char* description, 
                            unsigned long long duration, const char* privateKey, std::string& proposalId);
    bool castVote(const char* contractAddress, const char* proposalId, const char* userId, 
                 int choice, const char* comment, const char* privateKey, std::string& result);
    bool getVotingResults(const char* contractAddress, const char* proposalId, std::string& results);
    
    // Network operations
    bool isConnected();
    bool getBalance(const char* address, unsigned long long& balance);
    bool transferQubic(const char* fromPrivateKey, const char* toAddress, unsigned long long amount, std::string& txId);
    
private:
    std::string m_network;
    std::string m_nodeIp;
    int m_nodePort;
    bool m_connected;
    
    bool ensureConnection();
    bool executeTransaction(const ExecutionParams& params, std::string& result);
};

// Python integration helpers (for SmartGuard integration)
extern "C" {
    // C interface for Python integration
    int execute_real_contract_call(const char* contract_address, const char* function_name, 
                                  const char* arguments, const char* private_key, 
                                  const char* network, char* result_buffer, int buffer_size);
    
    int execute_real_contract_deployment(const char* bytecode_file, const char* private_key, 
                                       const char* network, char* contract_address_buffer, int buffer_size);
    
    int get_real_balance(const char* address, const char* network, unsigned long long* balance);
    
    int create_real_voting_proposal(const char* contract_address, const char* title, 
                                   const char* description, unsigned long long duration, 
                                   const char* private_key, const char* network, 
                                   char* proposal_id_buffer, int buffer_size);
    
    int cast_real_vote(const char* contract_address, const char* proposal_id, 
                      const char* user_id, int choice, const char* comment, 
                      const char* private_key, const char* network, 
                      char* result_buffer, int buffer_size);
    
    int get_real_voting_results(const char* contract_address, const char* proposal_id, 
                               const char* network, char* results_buffer, int buffer_size);
}

// Configuration constants
#define REAL_QUBIC_TESTNET_NODE_IP "127.0.0.1"
#define REAL_QUBIC_TESTNET_NODE_PORT 21841
#define REAL_QUBIC_MAINNET_NODE_IP "127.0.0.1"
#define REAL_QUBIC_MAINNET_NODE_PORT 21841

#define REAL_QUBIC_DEFAULT_TIMEOUT 60
#define REAL_QUBIC_DEPLOY_TIMEOUT 120
#define REAL_QUBIC_MAX_RETRIES 3
#define REAL_QUBIC_RETRY_DELAY 5000  // milliseconds

// Error codes
#define REAL_QUBIC_SUCCESS 0
#define REAL_QUBIC_ERROR_INVALID_PARAMS -1
#define REAL_QUBIC_ERROR_CONNECTION_FAILED -2
#define REAL_QUBIC_ERROR_TRANSACTION_FAILED -3
#define REAL_QUBIC_ERROR_TIMEOUT -4
#define REAL_QUBIC_ERROR_INVALID_RESPONSE -5
