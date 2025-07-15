#pragma once

#include <string>

// Contract compilation function
void compileContract(const char* sourceFile, const char* outputFile);

// Contract validation function  
void validateContract(const char* bytecodeFile);

// Contract deployment function
void deployContract(const char* bytecodeFile, const char* walletName, const char* network, uint64_t gasLimit, uint64_t gasPrice);

// Contract call function
void callContract(const char* contractAddress, const char* functionName, const char* walletName, const char* network, const char* functionArgs, uint64_t gasLimit, uint64_t gasPrice);

// Helper functions
bool readFileToString(const char* filename, std::string& content);
bool writeStringToFile(const char* filename, const std::string& content);
std::string generateBytecode(const std::string& sourceCode);
std::string convertToBytecode(const std::string& compiledCode);
