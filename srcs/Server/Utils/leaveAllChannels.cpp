#include "../Server.hpp"

void    Server::clearChannelsWithNoMembers()
{
    for (size_t i = channels.size(); i > 0; --i)
    {
        if (channels.at(i - 1).getMembers().size() == 0)
            channels.erase(channels.begin() + (i - 1));
    }
}

void    Server::leaveAllChannels(Client &c){
    for (size_t i = 0 ; i < channels.size(); i++)
    {
        Channel &ch = channels.at(i);

        if (ch.isMember(c.getUID()))
        {
            std::ostringstream  oss;

            // oss << ":" << c.getNickname() << "!" << c.getUsername() << "@" << c.getIpAddress() << " PART :" << ch.getName();
            ch.broadcast(":" + c.getNickname() + "!" + c.getUsername() + "@" + c.getIpAddress() + " PART :" + ch.getName());
            ch.removeClient(c.getUID());
        }
    }

    clearChannelsWithNoMembers();
}