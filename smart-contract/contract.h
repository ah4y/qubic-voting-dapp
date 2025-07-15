#include "qubic.h"

// Contract constants
#define MAX_VOTES 1000
#define MAX_USER_ID_LENGTH 32
#define MAX_COMMENT_LENGTH 256
#define MAX_PROPOSALS 10

// Vote choice enumeration
enum VoteChoice : unsigned char {
    VOTE_YES = 1,
    VOTE_NO = 2,
    VOTE_ABSTAIN = 3
};

// Vote status enumeration
enum VoteStatus : unsigned char {
    ACTIVE = 1,
    CLOSED = 2,
    PENDING = 3
};

// Proposal structure
struct Proposal {
    unsigned int id;
    char title[64];
    char description[256];
    unsigned long long startTime;
    unsigned long long endTime;
    unsigned int yesVotes;
    unsigned int noVotes;
    unsigned int abstainVotes;
    VoteStatus status;
    m256i creator;
    bool isActive;
};

// Vote structure
struct Vote {
    unsigned int proposalId;
    m256i voterAddress;
    char userId[MAX_USER_ID_LENGTH];
    VoteChoice choice;
    char comment[MAX_COMMENT_LENGTH];
    unsigned long long timestamp;
    unsigned int voteWeight;
    bool isValid;
};

// Voter registration structure
struct Voter {
    m256i address;
    char userId[MAX_USER_ID_LENGTH];
    bool isRegistered;
    unsigned int reputation;
    unsigned long long registrationTime;
};

// Contract state
struct CONTRACT_STATE {
    unsigned int totalVotes;
    unsigned int totalProposals;
    unsigned int totalRegisteredVoters;
    unsigned long long contractCreationTime;
    m256i contractOwner;
    bool isActive;
    
    // Storage arrays
    Vote votes[MAX_VOTES];
    Proposal proposals[MAX_PROPOSALS];
    Voter voters[MAX_VOTES]; // Reuse MAX_VOTES for voters limit
};

// Contract instance
CONTRACT_STATE contractState;

// Function declarations
void initializeContract();
unsigned int createProposal(const char* title, const char* description, unsigned long long duration);
bool registerVoter(const char* userId);
bool castVote(unsigned int proposalId, const char* userId, VoteChoice choice, const char* comment);
unsigned int getVoteCount(unsigned int proposalId);
bool closeProposal(unsigned int proposalId);
bool isVoterRegistered(const char* userId);
bool hasVoterVoted(unsigned int proposalId, const char* userId);
Proposal getProposal(unsigned int proposalId);
Vote getVote(unsigned int voteIndex);
unsigned int getProposalResults(unsigned int proposalId, unsigned int* yesCount, unsigned int* noCount, unsigned int* abstainCount);

// Utility functions
bool isValidProposal(unsigned int proposalId);
bool isProposalActive(unsigned int proposalId);
void updateProposalVoteCounts(unsigned int proposalId);
unsigned long long getCurrentTime();
bool compareStrings(const char* str1, const char* str2);
void copyString(char* dest, const char* src, unsigned int maxLength);

// Initialize contract
void initializeContract() {
    contractState.totalVotes = 0;
    contractState.totalProposals = 0;
    contractState.totalRegisteredVoters = 0;
    contractState.contractCreationTime = getCurrentTime();
    contractState.contractOwner = _currentContractInvocator;
    contractState.isActive = true;
    
    // Initialize arrays
    for (unsigned int i = 0; i < MAX_VOTES; i++) {
        contractState.votes[i].isValid = false;
        contractState.voters[i].isRegistered = false;
    }
    
    for (unsigned int i = 0; i < MAX_PROPOSALS; i++) {
        contractState.proposals[i].isActive = false;
        contractState.proposals[i].status = PENDING;
    }
}

// Create a new proposal
unsigned int createProposal(const char* title, const char* description, unsigned long long duration) {
    if (!contractState.isActive) return 0;
    if (contractState.totalProposals >= MAX_PROPOSALS) return 0;
    
    unsigned int proposalId = contractState.totalProposals;
    Proposal* proposal = &contractState.proposals[proposalId];
    
    proposal->id = proposalId;
    copyString(proposal->title, title, 64);
    copyString(proposal->description, description, 256);
    proposal->startTime = getCurrentTime();
    proposal->endTime = proposal->startTime + duration;
    proposal->yesVotes = 0;
    proposal->noVotes = 0;
    proposal->abstainVotes = 0;
    proposal->status = ACTIVE;
    proposal->creator = _currentContractInvocator;
    proposal->isActive = true;
    
    contractState.totalProposals++;
    return proposalId;
}

// Register a voter
bool registerVoter(const char* userId) {
    if (!contractState.isActive) return false;
    if (contractState.totalRegisteredVoters >= MAX_VOTES) return false;
    if (isVoterRegistered(userId)) return false;
    
    unsigned int voterIndex = contractState.totalRegisteredVoters;
    Voter* voter = &contractState.voters[voterIndex];
    
    voter->address = _currentContractInvocator;
    copyString(voter->userId, userId, MAX_USER_ID_LENGTH);
    voter->isRegistered = true;
    voter->reputation = 100; // Initial reputation
    voter->registrationTime = getCurrentTime();
    
    contractState.totalRegisteredVoters++;
    return true;
}

// Cast a vote
bool castVote(unsigned int proposalId, const char* userId, VoteChoice choice, const char* comment) {
    // Validation checks
    if (!contractState.isActive) return false;
    if (contractState.totalVotes >= MAX_VOTES) return false;
    if (!isValidProposal(proposalId)) return false;
    if (!isProposalActive(proposalId)) return false;
    if (!isVoterRegistered(userId)) return false;
    if (hasVoterVoted(proposalId, userId)) return false;
    if (choice < VOTE_YES || choice > VOTE_ABSTAIN) return false;
    
    // Create vote
    unsigned int voteIndex = contractState.totalVotes;
    Vote* vote = &contractState.votes[voteIndex];
    
    vote->proposalId = proposalId;
    vote->voterAddress = _currentContractInvocator;
    copyString(vote->userId, userId, MAX_USER_ID_LENGTH);
    vote->choice = choice;
    copyString(vote->comment, comment, MAX_COMMENT_LENGTH);
    vote->timestamp = getCurrentTime();
    vote->voteWeight = 1; // Default weight
    vote->isValid = true;
    
    // Update proposal vote counts
    Proposal* proposal = &contractState.proposals[proposalId];
    switch (choice) {
        case VOTE_YES:
            proposal->yesVotes++;
            break;
        case VOTE_NO:
            proposal->noVotes++;
            break;
        case VOTE_ABSTAIN:
            proposal->abstainVotes++;
            break;
    }
    
    contractState.totalVotes++;
    return true;
}

// Get vote count for a proposal
unsigned int getVoteCount(unsigned int proposalId) {
    if (!isValidProposal(proposalId)) return 0;
    
    Proposal* proposal = &contractState.proposals[proposalId];
    return proposal->yesVotes + proposal->noVotes + proposal->abstainVotes;
}

// Close a proposal
bool closeProposal(unsigned int proposalId) {
    if (!isValidProposal(proposalId)) return false;
    
    Proposal* proposal = &contractState.proposals[proposalId];
    
    // Only creator or contract owner can close
    if (!(proposal->creator == _currentContractInvocator || 
          contractState.contractOwner == _currentContractInvocator)) {
        return false;
    }
    
    // Check if voting period has ended
    if (getCurrentTime() >= proposal->endTime) {
        proposal->status = CLOSED;
        proposal->isActive = false;
        return true;
    }
    
    return false;
}

// Check if voter is registered
bool isVoterRegistered(const char* userId) {
    for (unsigned int i = 0; i < contractState.totalRegisteredVoters; i++) {
        if (contractState.voters[i].isRegistered && 
            compareStrings(contractState.voters[i].userId, userId)) {
            return true;
        }
    }
    return false;
}

// Check if voter has already voted for a proposal
bool hasVoterVoted(unsigned int proposalId, const char* userId) {
    for (unsigned int i = 0; i < contractState.totalVotes; i++) {
        if (contractState.votes[i].isValid &&
            contractState.votes[i].proposalId == proposalId &&
            compareStrings(contractState.votes[i].userId, userId)) {
            return true;
        }
    }
    return false;
}

// Get proposal by ID
Proposal getProposal(unsigned int proposalId) {
    if (isValidProposal(proposalId)) {
        return contractState.proposals[proposalId];
    }
    
    // Return empty proposal if invalid
    Proposal emptyProposal;
    emptyProposal.isActive = false;
    return emptyProposal;
}

// Get vote by index
Vote getVote(unsigned int voteIndex) {
    if (voteIndex < contractState.totalVotes) {
        return contractState.votes[voteIndex];
    }
    
    // Return empty vote if invalid
    Vote emptyVote;
    emptyVote.isValid = false;
    return emptyVote;
}

// Get proposal results
unsigned int getProposalResults(unsigned int proposalId, unsigned int* yesCount, unsigned int* noCount, unsigned int* abstainCount) {
    if (!isValidProposal(proposalId)) return 0;
    
    Proposal* proposal = &contractState.proposals[proposalId];
    *yesCount = proposal->yesVotes;
    *noCount = proposal->noVotes;
    *abstainCount = proposal->abstainVotes;
    
    return *yesCount + *noCount + *abstainCount;
}

// Utility function: Check if proposal ID is valid
bool isValidProposal(unsigned int proposalId) {
    return proposalId < contractState.totalProposals && 
           contractState.proposals[proposalId].isActive;
}

// Utility function: Check if proposal is active
bool isProposalActive(unsigned int proposalId) {
    if (!isValidProposal(proposalId)) return false;
    
    Proposal* proposal = &contractState.proposals[proposalId];
    return proposal->status == ACTIVE && 
           getCurrentTime() < proposal->endTime;
}

// Utility function: Get current time (simplified)
unsigned long long getCurrentTime() {
    return _currentEpoch; // Use Qubic epoch as timestamp
}

// Utility function: Compare strings
bool compareStrings(const char* str1, const char* str2) {
    unsigned int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return str1[i] == str2[i];
}

// Utility function: Copy string with length limit
void copyString(char* dest, const char* src, unsigned int maxLength) {
    unsigned int i = 0;
    while (i < maxLength - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Contract entry points
QX_EXPORT(initializeContract, void);
QX_EXPORT(createProposal, unsigned int, const char*, const char*, unsigned long long);
QX_EXPORT(registerVoter, bool, const char*);
QX_EXPORT(castVote, bool, unsigned int, const char*, VoteChoice, const char*);
QX_EXPORT(getVoteCount, unsigned int, unsigned int);
QX_EXPORT(closeProposal, bool, unsigned int);
QX_EXPORT(getProposal, Proposal, unsigned int);
QX_EXPORT(getVote, Vote, unsigned int);
QX_EXPORT(getProposalResults, unsigned int, unsigned int, unsigned int*, unsigned int*, unsigned int*);

// Main contract initialization
void CONTRACT_INIT() {
    initializeContract();
}