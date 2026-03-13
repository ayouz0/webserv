#include "Server/Server.hpp"

enum modeChar{
    op_i,
    op_t,
    op_k,
    op_o,
    op_l
};

#define Plus  true
#define Minus false
struct ModeChange {
    bool state;
    char mode;
    std::string param;
};

// MODE #channel *( ( "+" / "-" ) *( "i" / "t" / "k" / "o" / "l" ) ) [args...]
void Server::handleMode(int clientSocket, std::vector<std::string> &tokens) {
    Client *client = Clients[clientSocket];

    if (tokens.size() < 2) {
        this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_NEEDMOREPARAMS, client->getNickname(), "MODE", "need more parameters"));
        return;
    }

    std::string target = tokens[1];
    if (target[0] != '&' && target[0] != '#') {
        this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_BADCHANMASK, client->getNickname(), target, "bad channel mask"));
        return;
    }

    Channel *channel = getChannelByName(target);
    if (channel == NULL) {
        this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_NOSUCHCHANNEL, client->getNickname(), target, "no such channel"));
        return;
    }

    if (tokens.size() == 2) {
        // Return current channel modes
        return;
    }

    std::vector<ModeChange> modeQueue;
    bool state = true; 
    int paramModesCount = 0;
    
    std::string modeString = tokens[2];
    size_t paramIndex = 3;

    for (size_t i = 0; i < modeString.length(); ++i) {
        char c = modeString[i];

        if (c == '+') {
            state = true;
            continue;
        }
        if (c == '-') {
            state = false;
            continue;
        }

        if (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l') {
            this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_UNKNOWNMODE, client->getNickname(), std::string(1, c), "is unknown mode char to me"));
            continue;
        }

        bool requiresParam = false;
        if (c == 'o' || c == 'k') requiresParam = true; 
        if (c == 'l' && state == true) requiresParam = true;

        std::string param = "";
        if (requiresParam) {
            if (paramModesCount >= 3) {
                continue; // standard
            }
            
            if (paramIndex < tokens.size()) {
                param = tokens[paramIndex++];
                paramModesCount++;
            } else {
                this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_NEEDMOREPARAMS, client->getNickname(), std::string(1, c), "need more parameters"));

            }
        }

        modeQueue.push_back({state, c, param});
    }

    
    for (size_t i = 0; i < modeQueue.size(); ++i) {
        bool status = channel->applyMode(*this, client, modeQueue[i].state, modeQueue[i].mode, modeQueue[i].param);
        if (!status){

        }
    }

    // 3. Broadcast the successfully applied modes to the channel here.
}