#include "Client.hpp"

unsigned long Client::nextUID = 1;

Client::Client() : _socket(-1), _nickname(""), UID(nextUID++){
}
Client::~Client(){
}

Client::Client (int fd, std::string const &nickname) : _socket(fd), _nickname(nickname), UID(nextUID++){
}


