*This project has been created as part of the 42 curriculum by aalahyan, aaitabde.*

## Description

This project focuses on creating a fully functional Internet Relay Chat server using C++98. The server handles multiple concurrent clients via non-blocking TCP/IP sockets. It relies on a single `poll()` to manage all I/O operations without hanging. Users can authenticate, join channels, and send private messages.

## Instructions

**Compilation**
Run `make` at the root directory. This compiles the source files using `c++` with `-Wall -Wextra -Werror -std=c++98` flags.

**Execution**
Start the server by providing a listening port and a connection password:

```bash
./ircserv <port> <password>

```

**Testing**
You can test the server using a reference IRC client like limechat or use `nc` to manually send commands:

```bash
nc -C 127.0.0.1 <port>


```

## Server Features and Commands

**Authentication Process**

* **PASS**: This must be the first command sent to authenticate with the server password.

* **NICK**: Sets the client's nickname. The order does not matter as long as it comes after the PASS command.

* **USER**: Sets the client's username.



**Channel Operator Commands** 

**KICK**: Eject a client from the channel.


**INVITE**: Invite a client to a channel.


**TOPIC**: Change or view the channel topic.


**MODE**: Modify channel settings.


* `i`: Set/remove Invite-only channel.


* `t`: Set/remove TOPIC restrictions for regular users.


* `k`: Set/remove channel password (key).


* `o`: Give/take channel operator privileges.


* `l`: Set/remove the channel user limit.





## Resources

**IRC Protocol & Standards**

* [RFC 1459: Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
* [RFC 2812: Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
* [Modern IRC Client Protocol](https://modern.ircdocs.horse/)

**Network Programming**

* [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
* [IBM: Using poll() instead of select()](https://www.ibm.com/docs/en/i/7.4?topic=designs-using-poll-instead-select)

**Client Specifics**

* [What's a valid nickname?](https://forums.mirc.com/ubbthreads.php/topics/186181/nickname-valid-characters)

**AI Usage**

* AI was used during this project to achieve two objectives*.
1. To help in explanation of irc protocols, network programming, sockets and c++ knewledge
2. To help in genertaing some static content used in this project like data set of bot (bonus part), and refining the writing of this README file

