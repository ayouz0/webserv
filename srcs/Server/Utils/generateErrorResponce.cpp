#include "../Server.hpp"

std::string Server::generateErrorResponce(int numericCode, std::string targetNick, std::string errorParams, std::string reason){
    std::string serverName = this->serverName;
    std::stringstream responce ;
    responce <<  ":" << serverName << " " << numericCode << " " << targetNick << " " << errorParams << " :" << reason;

    return responce.str();
}