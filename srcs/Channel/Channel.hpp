#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Client/Client.hpp"
#include <vector>
#include <exception>
#include <iostream>
#include "../IrcException/IrcException.hpp"
#include <sys/socket.h>
#include <set>
#include <sstream>
#include "../Server/errors.hpp"


class Server;

class Channel
{
    struct ChannelMember
    {
        Client *client;
        bool moderator;

        ChannelMember(Client &c, bool mod) : client(&c), moderator(mod) {}
    };

    std::vector<ChannelMember> members;
    std::set<unsigned long> invited;
    std::string topic;
    bool topic_lock;
    int id;
    std::string name;
    bool invite_only;
    std::string password;
    static size_t counter;
    bool locked;

    bool isModerator(const Client &c) const
    {
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members[i].client->getNickname() == c.getNickname())
                return members[i].moderator;
        }
        return false;
    }

public:
    Channel(Client &creator, std::string name)
        : topic(""), topic_lock(false), name(name),
          invite_only(false), password(""), locked(false)
    {
        this->id = ++counter;
        members.push_back(ChannelMember(creator, true));
    }

    bool joinChannel(Client &c, std::string password)
    {
        bool is_invited = invited.find(c.getUID()) != invited.end();
        if (invite_only && !is_invited)
            throw IrcException(MSG_INVITEONLYCHAN, ERR_INVITEONLYCHAN); // ERR_INVITEONLYCHAN

        if (locked && password != this->password)
            throw IrcException(MSG_BADCHANNELKEY, ERR_BADCHANNELKEY); // ERR_BADCHANNELKEY
        if (getMemberByNickname(c.getNickname()) != NULL)
            return false; // already a member

        members.push_back(ChannelMember(c, false));
        invited.erase(c.getUID()); // in case of invitation it should be no longer valifd
        return true;
    }

    bool setTopic(const Client &c, std::string topic)
    {
        if (topic_lock && !isModerator(c))
            throw IrcException("You're not channel operator", 482); // ERR_CHANOPRIVSNEEDED

        this->topic = topic;
        return true;
    }

    std::string getName() const
    {
        return name;
    }
    const std::string &getTopic() const
    {
        return topic;
    }

    std::vector<Client *> getMembers() const
    {
        std::vector<Client *> memberList;
        for (size_t i = 0; i < members.size(); i++)
        {
            memberList.push_back(members[i].client);
        }
        return memberList;
    }

    Client *getMemberByNickname(const std::string &nickname) const
    {
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members[i].client->getNickname() == nickname)
                return members[i].client;
        }
        return NULL;
    }

    Client *getMember(unsigned long UID)
    {
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members[i].client->getUID() == UID)
                return members[i].client;
        }
        return NULL;
    }

    bool setPassword(const Client &c, std::string newPass)
    {
        if (!isModerator(c))
            throw IrcException("You're not channel operator", 482); // ERR_CHANOPRIVSNEEDED

        this->password = newPass;
        this->locked = !newPass.empty();
        return true;
    }

    void broadcast(const std::string &message, Client *exclude = NULL)
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

    bool isMember(int socket)
    {
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members.at(i).client->getSocket() == socket)
                return true;
        }
        return false;
    }

    bool isMember(unsigned long UID)
    {
        for (size_t i = 0; i < members.size(); i++)
        {
            if (members.at(i).client->getUID() == UID)
                return true;
        }
        return false;
    }

    void removeClient(unsigned long UID)
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

    bool invite(const Client &c, unsigned long UID)
    {
        if (invite_only && !isModerator(c))
            throw IrcException("invite only channel", ERR_CHANOPRIVSNEEDED);
        invited.insert(UID);
        return true;
    }


    void    leave(unsigned long UID, std::string message){
        if (!isMember(UID)) throw IrcException("You Are Not On That Channel", ERR_NOTONCHANNEL);

        Client *c = getMember(UID);

        if (!c) return ;

        // :<nick>!<user>@<host> PART <channel> :<Part Message>\r\n
        broadcast(":" + c->getNickname() + "!" + c->getUsername() + "@" + c->getIpAddress() + " PART " + name + " :" + message );

        removeClient(UID);
    }


    /*
        @brief send RPL_TOPIC and users list to recent joined user
    */

    void welcome(Server &server, unsigned long UID);
};

#endif