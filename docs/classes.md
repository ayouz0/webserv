# Classes

## Client

Holds all per‑connection state and implements the buffer
management/registration logic the server relies on.

### members

- `int _socket` – file descriptor of the TCP socket.
- `std::string inboundBuffer` – raw bytes read from the socket,
  commands are extracted from it.
- `std::string outboundBuffer` – replies waiting to be written.
- `std::string _nickname`, `_username` – values supplied by NICK/USER.
- `std::string ipAdress` – peer address recorded at accept time.
- `const unsigned long UID` – unique identifier, allocated from
  `static unsigned long nextUID`.

- registration flags (`_hasPassword`, `_hasNickname`, `_hasUsername`,
  `_isRegistered`) – used by `canRegister()` to know when the client
  has issued PASS, NICK and USER and is allowed to send other
  commands.

### methods

- constructors / destructor – create object for new connection; UID
  automatically increments.
- `_init()` – reset registration flags, called by constructors.
- `appendToInboundBuffer`/`appendToOutboundBuffer` – accumulate data.
- `getNextCommandFromInboundBuffer` – scan for `"\r\n"`, return one
  complete IRC line and remove it from the buffer.
- getters/setters for nickname, username, socket, IP, UID.
- state accessors: `setPassState`, `getPassState`, etc.
- `canRegister()` – true when PASS, NICK and USER have all been
  received and the client is not yet marked registered.
- `setRegistered`/`isRegistered` – flip the registration flag.
- `hasPendingOutput()` – used by the server’s select loop to know when
  to write.

This class exists so that the server’s main loop can treat each
connection as an opaque object carrying its I/O buffers and IRC
state.  All command handlers take pointers/references to `Client`
objects.

## Server

Central event loop.  Contains:

- `std::vector<Client>` / map of clients, list of `Channel`s.
- listening socket, password, port.
- `run()` – main `select` loop: accept, read, dispatch, write,
  clean up.
- `dispatchCommand(Client&, const std::string&)` – parse the line,
  look up handler.
- utility functions for broadcasting, replying, finding clients.

The server is responsible for knowing about all clients and channels,
for invoking the correct command file in `Server/Commands/`.

## Channel

Represents an IRC channel.

### members

- `std::string name` – channel name.
- `std::string topic` – current topic.
- `std::set<Client*> members` – pointers to clients in the channel.
- `std::set<Client*> operators` – users with op privileges.
- `std::set<Client*> invited` – clients who have been invited.
- mode flags: `inviteOnly`, `key`, `limit`, `banList`, etc.

### methods

- add/remove members, ops, invites.
- `broadcast(const std::string&)` – send a message to every member.
- topic getters/setters.
- mode checks: `isBanned`, `isOp`, `canJoin`, etc.

Channels exist so that command handlers such as `JOIN`, `PART`,
`PRIVMSG` and `INVITE` can manipulate a shared object instead of
maintaining their own membership tables.

## Utility functions

Located under `srcs/Server/Utils` and `srcs/utils`.

- `findClientBySocket`, `findClientByNickname` – lookup helpers.
- `getChannelByName` – channel lookup.
- `generateErrorResponce` – format numeric replies.
- `splitter`, `tokenize` – string parsing helpers.

They are simple stateless functions pulled in where needed by command
handlers and the server loop.

---

Add this file to `docs/` and refer to it from `README.md` or
`architecture.md`.  The purpose is to have a quick reference for you
and your friend when reading the code.# Classes

## Client

Holds all per‑connection state and implements the buffer
management/registration logic the server relies on.

### members

- `int _socket` – file descriptor of the TCP socket.
- `std::string inboundBuffer` – raw bytes read from the socket,
  commands are extracted from it.
- `std::string outboundBuffer` – replies waiting to be written.
- `std::string _nickname`, `_username` – values supplied by NICK/USER.
- `std::string ipAdress` – peer address recorded at accept time.
- `const unsigned long UID` – unique identifier, allocated from
  `static unsigned long nextUID`.

- registration flags (`_hasPassword`, `_hasNickname`, `_hasUsername`,
  `_isRegistered`) – used by `canRegister()` to know when the client
  has issued PASS, NICK and USER and is allowed to send other
  commands.

### methods

- constructors / destructor – create object for new connection; UID
  automatically increments.
- `_init()` – reset registration flags, called by constructors.
- `appendToInboundBuffer`/`appendToOutboundBuffer` – accumulate data.
- `getNextCommandFromInboundBuffer` – scan for `"\r\n"`, return one
  complete IRC line and remove it from the buffer.
- getters/setters for nickname, username, socket, IP, UID.
- state accessors: `setPassState`, `getPassState`, etc.
- `canRegister()` – true when PASS, NICK and USER have all been
  received and the client is not yet marked registered.
- `setRegistered`/`isRegistered` – flip the registration flag.
- `hasPendingOutput()` – used by the server’s select loop to know when
  to write.

This class exists so that the server’s main loop can treat each
connection as an opaque object carrying its I/O buffers and IRC
state.  All command handlers take pointers/references to `Client`
objects.

## Server

Central event loop.  Contains:

- `std::vector<Client>` / map of clients, list of `Channel`s.
- listening socket, password, port.
- `run()` – main `select` loop: accept, read, dispatch, write,
  clean up.
- `dispatchCommand(Client&, const std::string&)` – parse the line,
  look up handler.
- utility functions for broadcasting, replying, finding clients.

The server is responsible for knowing about all clients and channels,
for invoking the correct command file in `Server/Commands/`.

## Channel

Represents an IRC channel.

### members

- `std::string name` – channel name.
- `std::string topic` – current topic.
- `std::set<Client*> members` – pointers to clients in the channel.
- `std::set<Client*> operators` – users with op privileges.
- `std::set<Client*> invited` – clients who have been invited.
- mode flags: `inviteOnly`, `key`, `limit`, `banList`, etc.

### methods

- add/remove members, ops, invites.
- `broadcast(const std::string&)` – send a message to every member.
- topic getters/setters.
- mode checks: `isBanned`, `isOp`, `canJoin`, etc.

Channels exist so that command handlers such as `JOIN`, `PART`,
`PRIVMSG` and `INVITE` can manipulate a shared object instead of
maintaining their own membership tables.

## Utility functions

Located under `srcs/Server/Utils` and `srcs/utils`.

- `findClientBySocket`, `findClientByNickname` – lookup helpers.
- `getChannelByName` – channel lookup.
- `generateErrorResponce` – format numeric replies.
- `splitter`, `tokenize` – string parsing helpers.

They are simple stateless functions pulled in where needed by command
handlers and the server loop.

---

Add this file to `docs/` and refer to it from `README.md` or
`architecture.md`.  The purpose is to have a quick reference for you
and your friend when reading the code.