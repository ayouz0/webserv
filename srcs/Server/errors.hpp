#ifndef ERRORS_HPP
# define ERRORS_HPP

#define RPL_WELCOME 1
#define RPL_AWAY 301
#define RPL_TOPIC 332
#define RPL_NOTOPIC 331
#define RPL_INVITING 341
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHSERVER 402
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS 405
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_BADCHANMASK 476
#define ERR_CHANOPRIVSNEEDED 482


// ==========================================
// SUCCESS / REPLY MESSAGES (RPL_)
// ==========================================
// Note: RPL_AWAY, RPL_TOPIC, RPL_INVITING, and RPL_NAMREPLY 
// do not have static strings. Their text is completely dynamic 
// (e.g., the actual topic, or the list of users).
#define MSG_WELCOME "Welcome to the Internet Relay Network"
#define MSG_NOTOPIC "No topic is set"
#define MSG_ENDOFNAMES "End of NAMES list"

// ==========================================
// ERROR MESSAGES (ERR_)
// ==========================================
#define MSG_NOSUCHNICK "No such nick/channel"
#define MSG_NOSUCHSERVER "No such server"
#define MSG_NOSUCHCHANNEL "No such channel"
#define MSG_CANNOTSENDTOCHAN "Cannot send to channel"
#define MSG_TOOMANYCHANNELS "You have joined too many channels"
#define MSG_NORECIPIENT "No recipient given"
#define MSG_NOTEXTTOSEND "No text to send"
#define MSG_UNKNOWNCOMMAND "Unknown command"
#define MSG_NONICKNAMEGIVEN "No nickname given"
#define MSG_ERRONEUSNICKNAME "Erroneous nickname"
#define MSG_NICKNAMEINUSE "Nickname is already in use"
#define MSG_USERNOTINCHANNEL "They aren't on that channel"
#define MSG_NOTONCHANNEL "You're not on that channel"
#define MSG_USERONCHANNEL "is already on channel"
#define MSG_NOTREGISTERED "You have not registered"
#define MSG_NEEDMOREPARAMS "Not enough parameters"
#define MSG_ALREADYREGISTRED "Unauthorized command (already registered)"
#define MSG_PASSWDMISMATCH "Password incorrect"
#define MSG_CHANNELISFULL "Cannot join channel (+l)"
#define MSG_UNKNOWNMODE "is unknown mode char to me for" // Special case (no colon at start, read below)
#define MSG_INVITEONLYCHAN "Cannot join channel (+i)"
#define MSG_BANNEDFROMCHAN "Cannot join channel (+b)"
#define MSG_BADCHANNELKEY "Cannot join channel (+k)"
#define MSG_BADCHANMASK "Bad Channel Mask"
#define MSG_CHANOPRIVSNEEDED "You're not channel operator"

#endif