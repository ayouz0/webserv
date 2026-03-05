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

    size_t currentToken = 2; // Start at the first potential mode string
    bool state = Plus;       // Default state

    while (currentToken < tokens.size()) {
        std::string currentStr = tokens[currentToken++];

        // If it doesn't start with + or -, it's either a rogue parameter or malformed.
        if (currentStr[0] != '+' && currentStr[0] != '-') {
            this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_NEEDMOREPARAMS, client->getNickname(), "MODE", "invalid parameter"));
            return;
        }

        for (size_t i = 0; i < currentStr.length(); ++i) {
            char c = currentStr[i];

            if (c == '+') {
                state = Plus;
                continue;
            }
            if (c == '-') {
                state = Minus;
                continue;
            }

            if (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l') {
                this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_UNKNOWNMODE, client->getNickname(), std::string(1, c), "is unknown mode char to me"));
                continue;
            }

            bool requiresParam = false;
            if (c == 'o' || c == 'k') requiresParam = true; 
            if (c == 'l' && state == Plus) requiresParam = true;

            std::string param = "";
            if (requiresParam) {
                if (currentToken < tokens.size()) {
                    param = tokens[currentToken++]; // Consume the next token as the argument
                } else {
                    this->sendMessageToClient(clientSocket, generateErrorResponce(ERR_NEEDMOREPARAMS, client->getNickname(), "MODE", "need more parameters"));
                    return;
                }
            }
            // next
            // channel->applyMode(client, state, c, param);
        }
    }
}