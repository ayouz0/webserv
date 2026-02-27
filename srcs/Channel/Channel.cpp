#include "Channel.hpp"
#include "../Server/Server.hpp"

size_t Channel::counter = 0;

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
				throw IrcException(name, MSG_NOTONCHANNEL, ERR_NOTONCHANNEL);
			if (!isModerator(*executor))
				throw IrcException(name, MSG_CHANOPRIVSNEEDED, ERR_CHANOPRIVSNEEDED);
			Client *target = server.getClientByNickname(targets.at(i));
			if (!target)
				throw IrcException(targets.at(i) + " " + name, MSG_USERNOTINCHANNEL, ERR_USERNOTINCHANNEL);
			// :<executor_nick>!<executor_user>@<executor_host> KICK <channel> <target_nick> :comment or nick
			std::string response = ":" + executor->getNickname() + "!" + executor->getUsername() +
								   "@" + executor->getIpAddress() + " KICK " + name + +" " + target->getNickname() + " :" + comment + "\r\n";
			broadcast(response);
			removeClient(target->getUID());
		}
		catch (const IrcException &e)
		{
			Client	*c = server.getClient(UID);
			if (c)
				server.sendMessageToClient(c->getSocket(), server.generateErrorResponce(e.getCode(), c->getNickname(), e.getContext(), e.what()));
		}
	}
}