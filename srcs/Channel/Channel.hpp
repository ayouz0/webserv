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

        ChannelMember(Client &c, bool mod);
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
    unsigned int limit;


public:
    Channel(Client &creator, std::string name);

    bool joinChannel(Client &c, std::string password);

    bool isModerator(const Client &c) const;

    bool setTopic(const Client &c, std::string topic);

    std::string getName() const;
    const std::string &getTopic() const;

    std::vector<Client *> getMembers() const;

    Client *getMemberByNickname(const std::string &nickname) const;

    Client *getMember(unsigned long UID);

    bool setPassword(const Client &c, std::string newPass);

    void broadcast(const std::string &message, Client *exclude = NULL);

    bool isMember(int socket);

    bool isMember(unsigned long UID);

    void removeClient(unsigned long UID);

    bool invite(const Client &c, unsigned long UID);

    void leave(unsigned long UID, std::string message);

    /*
        WARNING: this throws IRcException
    */
    void    kickMultipleMembers(unsigned long UID, std::vector<std::string> &targets, Server &server, std::string comment);

    /*
        @brief send RPL_TOPIC and users list to recent joined user
    */

    void welcome(Server &server, unsigned long UID);

    void applyMode(Server &server, Client *client, bool state, char mode, std::string parameter);

    std::string generateModes();
};

#endif