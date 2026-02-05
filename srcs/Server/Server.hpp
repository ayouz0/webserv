#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include "../Client/Client.hpp"
#include <unistd.h>
# include "../utils/utils.hpp"
# include <algorithm>
#define MAX_CONNECTIONS SOMAXCONN
/*
    brief Server class to handle incoming client connections and manage communication
*/

class Server {
    private:

        int                     serverSocket;
        std::string             _ServerPassword;
        struct sockaddr_in      serverAddr;
        std::vector<pollfd>     clientSockets;
        std::map<int, Client *> Clients;
        std::string serverName;

    public: 

        Server(std::string const &port, std::string const &password);
        ~Server();
        void    authenticateClient(int clientSocket) throw() ;

        void    closeClientConnection(int clientSocket) throw();

        void    start();
        void    stop();

        void        sendMessageToClient(int clientSocket, std::string msg);

        void        handlePassCommand(const std::vector<std::string> &tokens, int clientSocket);
        void        handleNickCommand(const std::vector<std::string> &tokens, int clientSocket);
        // void        handleUserCommand(const std::vector<std::string> &tokens, int clientSocket);

        std::string generateErrorResponce(int numericCode, std::string targetNick, std::string errorParams, std::string reason);

    /*
        @brief routes the command to match the execution path for each request
        @param command the buffered string until \r\n, and the clientId making the request
        @note throws std::runtime_error
    */

    void router(const std::string &command, int clientSocket);
};

#endif