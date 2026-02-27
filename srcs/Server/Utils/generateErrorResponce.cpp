#include "../Server.hpp"

std::string Server::generateErrorResponce(int numericCode, std::string targetNick, std::string errorParams, std::string reason){
    std::stringstream ss;

    ss << numericCode;
    std::string paddedNumericCode = ss.str();

    if (numericCode < 100){
        if (numericCode < 10){
            paddedNumericCode = "00" + paddedNumericCode;
        }
        else{
            paddedNumericCode = "0" + paddedNumericCode;

        }
    }

    std::string serverName = this->serverName;
    std::stringstream responce ;
    responce <<  ":" << serverName << " " << paddedNumericCode << " " << targetNick << " " << errorParams << " :" << reason;

    return responce.str();
}