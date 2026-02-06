#include "Server.hpp"


#include <fcntl.h>
#include <cstring>

Server::Server(std::string const &port, std::string const &password) : _ServerPassword(password), serverName("irc"){

    this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->serverSocket == -1){
        std::cerr << "Could not create socket" << std::endl;
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        close(this->serverSocket);
        throw std::runtime_error("Set socket options failed");
    }

    int result = fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);
    if(result == -1){
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

    if (portStream.fail() || portNumber <= 0 || portNumber > 65535) {
        throw std::runtime_error("Invalid port number");
    }


    this->serverAddr.sin_port = htons(portNumber);

    if (bind(this->serverSocket, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) < 0) {
        close(this->serverSocket);
        throw std::runtime_error("Bind failed");
    }

    if (listen(this->serverSocket, MAX_CONNECTIONS) < 0) { // creates the queue
        close(this->serverSocket);
        throw std::runtime_error("Listen failed");
    }

    pollfd serverPollFd;
    serverPollFd.fd = this->serverSocket;
    serverPollFd.events = POLLIN;
    this->clientSockets.push_back(serverPollFd);

    std::cout << "Server started on port " << port << std::endl;

    this->start();
}


Server::~Server(){
    this->stop();
}


// @brief starts the listening on the clients and server's socket, creates a socket each time the server socket get's a request

void Server::start(){

    while (true){
    
        int result = poll(this->clientSockets.data(), this->clientSockets.size(), -1); // blocked
        if (result < 0) {
            std::cerr << "Poll error" << std::endl;
            continue;
        }

        for (size_t i = 0; i < this->clientSockets.size(); i++){
            if (result == 0) {
                continue;
            }
            if ((this->clientSockets[i].revents & POLLIN) != 1){
                continue;
            }
            if (this->clientSockets[i].fd == this->serverSocket){
                int newClientSocket = accept(this->clientSockets[i].fd, NULL, NULL);
                if (newClientSocket < 0) {
                    std::cerr << "Accept failed" << std::endl;
                    continue;
                }
                pollfd newClientPoll;
                newClientPoll.fd = newClientSocket;
                newClientPoll.events = POLLIN;
                newClientPoll.revents = 0;
                this->clientSockets.push_back(newClientPoll);

                Client * newClient = new Client(newClientSocket, "");
                this->Clients[newClientSocket] = newClient;
            }
            else {

                int currentClientFd = this->clientSockets[i].fd;
                char buffer[1024];
                ssize_t bytesRead = recv(currentClientFd, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead < 0) {
                    std::cerr << "Receive failed" << std::endl;
                    continue;
                }
                else if (bytesRead == 0) {
                    close(currentClientFd);
                    this->clientSockets.erase(this->clientSockets.begin() + i);
                    i--;
                    continue;
                }
                buffer[bytesRead] = '\0';
                this->Clients[currentClientFd]->appendToBuffer(std::string(buffer));
                std::string s(buffer);
                this->router(this->Clients[currentClientFd]->getNextCommandFromBuffer(), currentClientFd);
            }
        }
    }
}

void    Server::stop(){
    for (size_t i = 0; i < this->clientSockets.size(); i++){
        close(this->clientSockets[i].fd);
    }
    this->clientSockets.clear();
    std::cout << "Server stopped." << std::endl;
}


void    Server::router(const std::string &command, int clientSocket){

    std::vector<std::string> tokens = Tokenize(command);

    if (tokens.empty()) return ;

    std::transform( tokens.at( 0 ).begin( ), tokens.at( 0 ).end( ), tokens.at( 0 ).begin( ), ( int( * ) ( int ) )std::toupper );

    std::string cmd = tokens.at(0);
    if (cmd == "PASS"){
        this->handlePassCommand(tokens, clientSocket);
    }
    else if (cmd == "NICK"){
        this->handleNickCommand(tokens, clientSocket);
    }
    // else if (cmd == "USER"){
    //     this->handleUserCommand(tokens, clientSocket);
    // }

}
