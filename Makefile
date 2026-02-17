NAME        = ft_irc

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -fmax-errors=1

SRCS_DIR    = srcs

SRCS        = srcs/main.cpp \
            srcs/Client/Client.cpp \
            srcs/Server/Server.cpp \
            srcs/Server/Commands/Nick.cpp \
            srcs/Server/Commands/Pass.cpp \
            srcs/Server/Commands/User.cpp \
            srcs/Server/Commands/Join.cpp \
            srcs/Server/Commands/privmsg.cpp \
            srcs/Server/Commands/topic.cpp \
            srcs/Server/Commands/invite.cpp \
            srcs/Server/Utils/closeClientConnection.cpp \
            srcs/Server/Utils/generateErrorResponce.cpp \
            srcs/Server/Utils/getChannelByName.cpp \
            srcs/Server/Utils/sendMessageToClient.cpp \
            srcs/Server/Utils/findClientBySocket.cpp \
            srcs/utils/tokenize.cpp \
            srcs/utils/splitter.cpp \
            srcs/Channel/Channel.cpp

OBJS        = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
