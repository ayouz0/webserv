#include "Server.hpp"

#include <fcntl.h>
#include <cstring>
#include <iomanip>
#include <cerrno>

Server::Server(std::string const &port, std::string const &password) : _ServerPassword(password), serverName("irc")
{
    this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->serverSocket == -1)
    {
        std::cerr << "Could not create socket" << std::endl;
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(this->serverSocket);
        throw std::runtime_error("Set socket options failed");
    }

    int result = fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);
    if (result == -1)
    {
        close(this->serverSocket);
        std::cerr << "Could not set socket to non-blocking" << std::endl;
        throw std::runtime_error("Setting non-blocking failed");
    }

    memset(&this->serverAddr, 0, sizeof(this->serverAddr));
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
    this->serverAddr.sin_family = AF_INET;

    std::stringstream portStream(port);
    int portNumber;
    portStream >> portNumber;

    if (portStream.fail() || portNumber <= 0 || portNumber > 65535)
    {
        throw std::runtime_error("Invalid port number");
    }

    this->serverAddr.sin_port = htons(portNumber);

    if (bind(this->serverSocket, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
    {
        close(this->serverSocket);
        throw std::runtime_error("Bind failed");
    }

    if (listen(this->serverSocket, MAX_CONNECTIONS) < 0)
    {
        close(this->serverSocket);
        throw std::runtime_error("Listen failed");
    }

    pollfd serverPollFd;
    serverPollFd.fd = this->serverSocket;
    serverPollFd.events = POLLIN;
    this->clientSockets.push_back(serverPollFd);

    std::cout << "Server started on port " << port << std::endl;

    Clients[-1] = new Client(-1, BOT_NAME);
    this->start();
}

Server::~Server()
{
    this->stop();
}

// @brief starts the listening on the clients and server's socket, creates a socket each time the server socket get's a request
void Server::start()
{

    while (true)
    {
        // before askiing operating system what happened, we must tell it first what are we looking for
        for (size_t i = 0; i < this->clientSockets.size(); i++)
        {
            if (this->clientSockets[i].fd == this->serverSocket)
            {
                this->clientSockets[i].events = POLLIN;
                continue; // for server we only cae about incoming connections
            }

            std::map<int, Client *>::iterator it = this->Clients.find(this->clientSockets[i].fd);
            Client *client = (it != this->Clients.end()) ? it->second : NULL;
            short events = POLLIN;
            if (client && client->hasPendingOutput())
                events |= POLLOUT;
            // for each clinet we care about incoming data(POLLIN), but if there is data that needed to be sent we also add POLLOUT
            this->clientSockets[i].events = events;
        }

        // the OS will nlock here untill something of those events we wanted to happen occur
        int result = poll(this->clientSockets.data(), this->clientSockets.size(), -1);
        if (result <= 0)
        {
            if (result < 0)
                std::cerr << "Poll error" << std::endl;
            continue;
        }

        for (size_t i = 0; i < this->clientSockets.size(); i++)
        {
            short revents = this->clientSockets[i].revents;
            int currentFd = this->clientSockets[i].fd;

            if (currentFd == this->serverSocket && (revents & POLLIN))
            {
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);

                int newClientSocket = accept(this->clientSockets[i].fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
                if (newClientSocket < 0)
                {
                    std::cerr << "Accept failed" << std::endl;
                    continue;
                }

                int nbResult = fcntl(newClientSocket, F_SETFL, O_NONBLOCK);
                if (nbResult == -1)
                {
                    close(newClientSocket);
                    std::cerr << "Could not set client socket to non-blocking" << std::endl;
                    continue;
                }

                std::string ipAddress = inet_ntoa(clientAddr.sin_addr); 
                std::cout << "New connection from: " << ipAddress << std::endl;

                pollfd newClientPoll;
                newClientPoll.fd = newClientSocket;
                newClientPoll.events = POLLIN;
                newClientPoll.revents = 0;
                this->clientSockets.push_back(newClientPoll);

                Client *newClient = new Client(newClientSocket, "", ipAddress);
                this->Clients[newClientSocket] = newClient;
                // }
            }
            else
            {
                if (revents & (POLLERR | POLLHUP | POLLNVAL)) // if there any poll error of clinet disconnection or invalid request
                {
                    this->closeClientConnection(currentFd);
                    i--;
                    continue;
                }

                if (revents & POLLIN)
                {
                    char buffer[1024];
                    ssize_t bytesRead = recv(currentFd, buffer, sizeof(buffer) - 1, 0);
                    if (bytesRead < 0)
                    {
                        if (errno != EAGAIN && errno != EWOULDBLOCK)
                        {
                            std::cerr << "Receive failed" << std::endl;
                            this->closeClientConnection(currentFd);
                            i--;
                        }
                        continue;
                    }
                    else if (bytesRead == 0)
                    {
                        this->closeClientConnection(currentFd);
                        i--;
                        continue;
                    }
                    buffer[bytesRead] = '\0';
                    std::map<int, Client *>::iterator it = this->Clients.find(currentFd);
                    if (it == this->Clients.end())
                    {
                        this->closeClientConnection(currentFd);
                        i--;
                        continue;
                    }
                    it->second->appendToInboundBuffer(std::string(buffer));

                    while (true)
                    {
                        std::map<int, Client *>::iterator cur = this->Clients.find(currentFd);
                        if (cur == this->Clients.end())
                            break;

                        std::string cmd = cur->second->getNextCommandFromInboundBuffer();
                        if (cmd.empty())
                            break;

                        this->router(cmd, currentFd);

                    }
                }

                if (revents & POLLOUT)
                {
                    std::map<int, Client *>::iterator it = this->Clients.find(currentFd);
                    if (it != this->Clients.end())
                    {
                        std::string &outbound = it->second->getOutboundBuffer();
                        while (!outbound.empty())
                        {
                            ssize_t sent = send(currentFd, outbound.c_str(), outbound.size(), 0);
                            if (sent > 0)
                            {
                                outbound.erase(0, sent);
                                continue;
                            }
                            if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // failed to send because the socket buffer is full, we'll try again next cycle
                                break;

                            this->closeClientConnection(currentFd); // any other -1 result is an error, so goodbye client
                            i--;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Server::stop()
{
    for (size_t i = 0; i < this->clientSockets.size(); i++)
    {
        close(this->clientSockets[i].fd);
    }
    this->clientSockets.clear();
    std::cout << "Server stopped." << std::endl;
}

void Server::router(const std::string &command, int clientSocket)
{

    std::cout << "Received command: " << command << std::endl; // to be removed
    std::vector<std::string> tokens = Tokenize(command);

    if (tokens.empty())
        return;

    std::transform(tokens.at(0).begin(), tokens.at(0).end(), tokens.at(0).begin(), (int (*)(int))std::toupper);

    Client *client = Clients[clientSocket];

    std::string cmd = tokens.at(0);

    if (cmd == "PASS")
    {
        this->handlePassCommand(tokens, clientSocket);
        return;
    }
    else if (cmd == "NICK")
    {
        this->handleNickCommand(tokens, clientSocket);
        return;
    }
    else if (cmd == "USER")
    {
        this->handleUserCommand(tokens, clientSocket);
        return;
    }

    if (client->isRegistered() == false)
    {
        std::string nickName = client->getNickname().empty() ? "*" : client->getNickname();
        std::string errorMsg = this->generateErrorResponce(ERR_NOTREGISTERED, nickName, tokens[0], "You have not registered yet");
        this->sendMessageToClient(clientSocket, errorMsg);
        return;
    }

    else if (cmd == "JOIN")
    {
        this->handleJoinChannel(clientSocket, tokens);
    }
    else if (cmd == "PRIVMSG")
    {
        handlePrivMsg(clientSocket, tokens);
    }
    else if (cmd == "TOPIC")
    {
        handleTopic(clientSocket, tokens);
    }
    else if (cmd == "INVITE")
    {
        handleInvite(clientSocket, tokens);
    }
    else if (cmd == "PART")
    {
        handlePart(clientSocket, tokens);
    }
    else if (cmd == "KICK")
    {
        handleKick(clientSocket, tokens);
    }
    else if (cmd == "MODE")
    {
        handleMode(clientSocket, tokens);
    }
    else if (cmd == "QUIT")
    {
        handleQuit(clientSocket, tokens);
    }
    else if (cmd == "PONG")
    {
        return ;//ignore it
    }
    else{
        // :<server_name> 421 <nickname> <command> :Unknown command
        Client *c = findClientBySocketId(clientSocket);
        if (!c) return ;
        sendMessageToClient(clientSocket, ":irc 421 " + c->getNickname() + " " + cmd + " :Unknown command");
    }
}


// @brief sends a welcome burst to a client
void Server::welcomeBurst(int clientSocket)
{
    Client *client = this->Clients[clientSocket];

    std::string msgs;

    msgs += generateErrorResponce(1, client->getNickname(), "", "Welcome to the Internet Relay Network " + client->getNickname()) + "\r\n";
    msgs += generateErrorResponce(2, client->getNickname(), "", "Your host is " + this->serverName + ", running version 1.0") + "\r\n";
    msgs += generateErrorResponce(3, client->getNickname(), "", "This server was created today") + "\r\n";
    msgs += generateErrorResponce(4, client->getNickname(), this->serverName, " 1.0");

    this->sendMessageToClient(clientSocket, msgs);
}
