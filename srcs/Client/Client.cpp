#include "Client.hpp"

Client::Client() : _socket(-1), _nickname(""){
}
Client::~Client(){
}

Client::Client (int fd, std::string const &nickname) : _socket(fd), _nickname(nickname){
}

