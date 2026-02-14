#include "../Server.hpp"

void Server::sendMessageToClient(int clientSocket, std::string msg) {
    if ((msg.length() < 2) || (msg.substr(msg.length() - 2) != "\r\n")){
        msg += "\r\n";
    }

    size_t totalSent = 0;
    size_t length = msg.length();
    const char *ptr = msg.c_str();

    while (totalSent < length) {
        ssize_t sent = send(clientSocket, ptr + totalSent, length - totalSent, 0);

        if (sent == -1) { 
            this->closeClientConnection(clientSocket);
            return;
        }
        totalSent += sent;
    }
}