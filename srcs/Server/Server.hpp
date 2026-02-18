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
#include "../Channel/Channel.hpp"
# include <algorithm>
#include <arpa/inet.h>


#include "errors.hpp"

#define MAX_CONNECTIONS SOMAXCONN
/*
    @brief Server class to handle incoming client connections and manage communication
*/

class Server {
    private:

    int serverSocket;
    std::string _ServerPassword;
    struct sockaddr_in serverAddr;
    std::vector<pollfd> clientSockets;
    std::map<int, Client *> Clients;
    std::vector<Channel> channels;
    std::string serverName;


    Client* getClientByNickname(std::string &name){
        std::map<int, Client *>::iterator   it = Clients.begin();

        while (it != Clients.end())
        {
            if (it->second->getNickname() == name) return it->second;
            it++;
        }
        return NULL;
    }        

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
        void        handleUserCommand(const std::vector<std::string> &tokens, int clientSocket);

        std::string generateErrorResponce(int numericCode, std::string targetNick, std::string errorParams, std::string reason);


        Channel *getChannelByName(std::string name);

    /*
         @brief add clinet to the channel
        @param SockerId - clinetSocker id
        @param name - channel name
        @note throws std::runtime_error
    */
   void    handleJoinChannel(int socketId, std::vector<std::string> channelData);


   /*
    @brief send private message to a client or a channel @param clientSocket - the sender's socket id @param tokens - the command tokens, where tokens[1] is the target and tokens[2] is the message @note throws std::runtime_error
   */
    void    handlePrivMsg(int clientSocket, std::vector<std::string> &tokens);

    /*
    @brief set or get the topic of a channel, if tokens size is 2 it's a get request, if it's 3 it's a set request
    @param clientSocket - the sender's socket id
    @param tokens - the command tokens, where tokens[1] is the channel name and tokens[2] is the topic (optional)
    */
    void    handleTopic(int clientSocket, std::vector<std::string> &tokens);


    /*
        @brief routes the command to match the execution path for each request
        @param command the buffered string until \r\n, and the clientId making the request
        @note throws std::runtime_error
    */
    void router(const std::string &command, int clientSocket);


    /*
        @brief invites user to channel
        @param clientSocker clinet docket serve as id, and the tokens which are recieved from client
        @note syntax INVITE <nickname> <channel>
    */

    void    handleInvite(int clientSocket, std::vector<std::string> tokens);


    void    handlePart(int clientSocket, std::vector<std::string> tokens );


    Client* findClientBySocketId(int socketId) const;


    void    leaveAllChannels(Client &c);

    void    clearChannelsWithNoMembers();

    /*
        @brief debug function to print the clients and channels data, to be removed later
    */
    void debug() const;
};

#endif