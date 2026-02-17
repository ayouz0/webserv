# include "Channel.hpp"
# include "../Server/Server.hpp"

size_t Channel::counter = 0;

void Channel::welcome(Server &server, unsigned long UID)
{
	Client *member = getMember(UID);
	if (!member)
		return;

	std::ostringstream oss;

	if (topic.empty())
		oss << ":irc " << RPL_NOTOPIC << " " << member->getNickname() <<  " " <<  name <<  " :No Topic Set";
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