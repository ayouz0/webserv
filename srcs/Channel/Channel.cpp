#include "Channel.hpp"
#include "../Server/Server.hpp"
#include <cerrno>
#include <cstdlib>
#include <limits>
size_t Channel::counter = 0;

Channel::ChannelMember::ChannelMember(Client &c, bool mod) : client(&c), moderator(mod) {}

Channel::Channel(Client &creator, std::string name)
	: topic(""), topic_lock(false), name(name),
	invite_only(false), password(""), locked(false), limit(-1)
{
	this->id = ++counter;
	members.push_back(ChannelMember(creator, true));
}

bool Channel::joinChannel(Client &c, std::string password)
{
	bool is_invited = invited.find(c.getUID()) != invited.end();
	if (invite_only && !is_invited)
		throw IrcException(name, MSG_INVITEONLYCHAN, ERR_INVITEONLYCHAN);

	if (locked && password != this->password)
		throw IrcException(name, MSG_BADCHANNELKEY, ERR_BADCHANNELKEY);
	if (getMemberByNickname(c.getNickname()) != NULL)
		return false; // already a member
	if (members.size() >= limit)
		throw IrcException(name, MSG_CHANNELISFULL, ERR_CHANNELISFULL);

	members.push_back(ChannelMember(c, false));
	invited.erase(c.getUID()); // in case of invitation it should be no longer valifd
	return true;
}

bool Channel::isModerator(const Client &c) const
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i].client->getUID() == c.getUID())
			return members[i].moderator;
	}
	return false;
}

bool Channel::setTopic(const Client &c, std::string topic)
{
	if (topic_lock && !isModerator(c))
		throw IrcException(name, MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED);

	this->topic = topic;
	return true;
}

std::string Channel::getName() const
{
	return name;
}

const std::string &Channel::getTopic() const
{
	return topic;
}

std::vector<Client *> Channel::getMembers() const
{
	std::vector<Client *> memberList;
	for (size_t i = 0; i < members.size(); i++)
	{
		memberList.push_back(members[i].client);
	}
	return memberList;
}

Client *Channel::getMemberByNickname(const std::string &nickname) const
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i].client->getNickname() == nickname)
			return members[i].client;
	}
	return NULL;
}

Client *Channel::getMember(unsigned long UID)
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i].client->getUID() == UID)
			return members[i].client;
	}
	return NULL;
}

bool Channel::setPassword(const Client &c, std::string newPass)
{
	if (!isModerator(c))
		throw IrcException(name, MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED);

	this->password = newPass;
	this->locked = !newPass.empty();
	return true;
}

void Channel::broadcast(const std::string &message, Client *exclude)
{
	std::string finalMsg = message;
	if (finalMsg.length() < 2 || finalMsg.substr(finalMsg.length() - 2) != "\r\n")
		finalMsg += "\r\n";

	for (size_t i = 0; i < members.size(); i++)
	{
		if (!exclude || exclude->getSocket() != members[i].client->getSocket())
			members[i].client->appendToOutboundBuffer(finalMsg);
	}
}

bool Channel::isMember(int socket)
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members.at(i).client->getSocket() == socket)
			return true;
	}
	return false;
}

bool Channel::isMember(unsigned long UID)
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members.at(i).client->getUID() == UID)
			return true;
	}
	return false;
}

void Channel::removeClient(unsigned long UID)
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i].client->getUID() == UID)
		{
			members.erase(members.begin() + i);
			break;
		}
	}
	invited.erase(UID);
}

bool Channel::invite(const Client &c, unsigned long UID)
{
	if (invite_only && !isModerator(c))
		throw IrcException(name, MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED);
	invited.insert(UID);
	return true;
}

void Channel::leave(unsigned long UID, std::string message)
{
	if (!isMember(UID))
		throw IrcException(name, MSG_NOTONCHANNEL, ERR_NOTONCHANNEL);

	Client *c = getMember(UID);

	if (!c)
		return;

	// :<nick>!<user>@<host> PART <channel> :<Part Message>\r\n
	broadcast(":" + c->getNickname() + "!" + c->getUsername() + "@" + c->getIpAddress() + " PART " + name + " :" + message);

	removeClient(UID);
}

std::string Channel::generateModes()
{
	// order chosen: i t l k
	std::string result;

	bool state = invite_only;

	result += (invite_only ? "+" : "-");
	result += "i";

	if (topic_lock != state)
		result += (state ? "-" : "+");
	state = !state;
	result += "t";

	if ((limit != -1) != state)
		result += (state ? "-" : "+");
	state = !state;
	result += "l";

	if (locked != state)
		result += (state ? "-" : "+");
	state = !state;

	result += "k ";

	if (limit != -1)
		result += limit;
	if (locked)
		result += (" " + password);

	return result;
}

void Channel::welcome(Server &server, unsigned long UID)
{
	Client *member = getMember(UID);
	if (!member)
		return;

	std::ostringstream oss;

	if (topic.empty())
		oss << ":irc " << RPL_NOTOPIC << " " << member->getNickname() << " " << name << " :No Topic Set";
	else
		oss << ":irc " << RPL_TOPIC << " " << member->getNickname() << " " << name << " :" << topic;

	server.sendMessageToClient(member->getSocket(), oss.str());

	oss.str("");

	oss << ":irc " << RPL_NAMREPLY << " " << member->getNickname() << " = " << name << " :";

	for (size_t i = 0; i < members.size(); i++)
	{
		if (members.at(i).moderator)
			oss << "@";
		oss << members.at(i).client->getNickname() << " ";
	}
	server.sendMessageToClient(member->getSocket(), oss.str());

	oss.str("");

	oss << ":irc " << RPL_ENDOFNAMES << " " << member->getNickname() << " " << name << " :End of NAMES list";

	server.sendMessageToClient(member->getSocket(), oss.str());
}

void Channel::kickMultipleMembers(unsigned long UID, std::vector<std::string> &targets, Server &server, std::string comment)
{

	for (size_t i = 0; i < targets.size(); i++)
	{
		try
		{
			Client *executor = getMember(UID);
			if (!executor)
				throw IrcException(name, MSG_NOTONCHANNEL, ERR_NOTONCHANNEL); // executor not on channel
			if (!isModerator(*executor))
				throw IrcException(name, MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED); // executor is in channel but not moderator
			Client *target = server.getClientByNickname(targets.at(i));
			if (!target)
				throw IrcException(targets.at(i) + " " + name, MSG_USERNOTINCHANNEL, ERR_USERNOTINCHANNEL);
			// :<executor_nick>!<executor_user>@<executor_host> KICK <channel> <target_nick> :comment or nick
			std::string response = ":" + executor->getNickname() + "!" + executor->getUsername() +
								"@" + executor->getIpAddress() + " KICK " + name + " " + target->getNickname() + " :" + comment + "\r\n";
			broadcast(response);
			removeClient(target->getUID());
		}
		catch (const IrcException &e)
		{
			Client *c = server.getClient(UID);
			if (c)
				server.sendMessageToClient(c->getSocket(), server.generateErrorResponce(e.getCode(), c->getNickname(), e.getContext(), e.what()));
		}
	}
}

//: irc.localhost 482 user1 #mychannel :You're not channel operator

// we need broadcasting
void Channel::applyMode(Server &server, Client *client, bool state, char mode, std::string parameter)
{
	if (this->isModerator(*client) == false)
		throw IrcException(name, MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED);

	switch (mode)
	{
	case 'i':
		if (state == true)
			this->invite_only = true;
		else if (state == false)
		{
			this->invite_only = false;
		}
		break;

	case 't':
		topic_lock = state;
		break;

	case 'k':
		if (state && parameter.empty())
			throw IrcException("k", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
		locked = state;
		password = parameter;
		break;
	case 'o':
		if (parameter.empty())
			throw IrcException("o", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
		locked = state;
		for (size_t i = 0; i < members.size(); i++)
		{
			ChannelMember &m = members.at(i);
			if (m.client->getNickname() == parameter)
			{
				m.moderator = state;
				return;
			}
		}
		throw IrcException("o", MSG_USERNOTINCHANNEL, ERR_USERNOTINCHANNEL);
		break;
	case 'l':
		if (state && parameter.empty())
			throw IrcException("l", MSG_NEEDMOREPARAMS, ERR_NEEDMOREPARAMS);
		if (!state)
			limit = -1;
		else
		{
			char *rest = NULL;
			errno = 0;
			long limitToSet = std::strtol(parameter.c_str(), &rest, 10);
			if (limitToSet >= std::numeric_limits<unsigned int>::max() || limitToSet < 0 || *rest || errno) return ;
			limit = limitToSet;
		}
	}
}
