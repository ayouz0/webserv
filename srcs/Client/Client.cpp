#include "Client.hpp"

unsigned long Client::nextUID = 1;

void Client::_init()
{
	_hasPassword = false;
	_hasNickname = false;
	_hasUsername = false;
	_isRegistered = false;
}

Client::Client() : _socket(-1), _nickname(""), UID(nextUID++){
}

Client::~Client(){
}

Client::Client (int fd, std::string const &nickname) : _socket(fd), _nickname(nickname), UID(nextUID++){
}

Client::Client(int fd, std::string nickname, std::string ipA) : UID(nextUID++)
{
	_init();
	this->_socket = fd;
	this->_nickname = nickname;
	this->ipAdress = ipA;
}

std::string Client::getIpAddress()
{
	return ipAdress;
}

unsigned long Client::getUID() const
{
	return UID;
}

void Client::appendToInboundBuffer(std::string const &data)
{
	this->inboundBuffer += data;
}

std::string &Client::getInboundBuffer()
{
	return inboundBuffer;
}

void Client::appendToOutboundBuffer(std::string const &data)
{
	this->outboundBuffer += data;
}

bool Client::hasPendingOutput() const
{
	return !outboundBuffer.empty();
}

std::string &Client::getOutboundBuffer()
{
	return outboundBuffer;
}

std::string Client::getNickname() const
{
	return _nickname;
}

void Client::setNickname(std::string const &nickname)
{
	_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	_username = username;
}

std::string Client::getUsername() const
{
	return _username;
}

int Client::getSocket() const
{
	return _socket;
}

std::string Client::getNextCommandFromInboundBuffer()
{
	size_t pos = inboundBuffer.find("\r\n");
	if (pos != std::string::npos)
	{
		std::string command = inboundBuffer.substr(0, pos);
		inboundBuffer.erase(0, pos + 2); // +2 to remove the \r\n
		return command;
	}
	return "";
}

void Client::setPassState(bool state)
{
	_hasPassword = state;
}

bool Client::getPassState() const
{
	return _hasPassword;
}

void Client::setNickState(bool state)
{
	_hasNickname = state;
}

bool Client::getNickState() const
{
	return _hasNickname;
}

void Client::setUserState(bool state)
{
	_hasUsername = state;
}

bool Client::getUserState() const
{
	return _hasUsername;
}

bool Client::canRegister() const
{
	return (_hasPassword && _hasNickname && _hasUsername && !_isRegistered);
}

void Client::setRegistered(bool state)
{
	_isRegistered = state;
}

bool Client::isRegistered() const
{
	return _isRegistered;
}


