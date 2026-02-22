#include "../Server.hpp"

bool    isValidNickname(const std::string &nickname){
    if (nickname.empty() || nickname.length() > 9){
        return false;
    }
    for (size_t i = 0; i < nickname.length(); i++){
        char c = nickname[i];   // `|^_-{}[] and \'
        if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '{' && c != '}' && c != '^' && c != '|'){
            return false;
        }
    }
    return true;
}

void    Server::handleNickCommand(const std::vector<std::string> &tokens, int clientSocket){

    Client *client = this->findClientBySocketId(clientSocket);

    if (!client) return ; //  just in case

    if ( client->getPassState() == false ){
        std::string nickName = "*";
        std::string errorMsg = this->generateErrorResponce(431, nickName, "NICK", "Client not registered");
        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    if ( tokens.size() < 2 ){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(431, nickName, "NICK", "No nickname given");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    std::string nickname = tokens[1];
    if ( nickname.empty() || !isValidNickname(nickname) || isdigit(nickname[0])){
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(432, nickName, nickname, "Erroneous nickname");

        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    for ( std::map<int, Client*>::iterator it = this->Clients.begin(); it != Clients.end(); it ++ ){
        if ( (it->second->getNickname() == nickname) && (it->second->getSocket() != clientSocket)){ // duplicate
            std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
            std::string errorMsg = this->generateErrorResponce(436, nickName, "NICK", "Nickname already in use");
            this->sendMessageToClient(clientSocket, errorMsg);
            return;
        }
    }

    client->setNickname(nickname);
    client->setNickState(true);

    if ( client->canRegister() == true ) {
        client->setRegistered(true);
    }

}
