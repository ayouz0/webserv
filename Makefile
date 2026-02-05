NAME=ft_irc
SRCS=srcs/main.cpp srcs/Server/Server.cpp srcs/Client/Client.cpp srcs/utils/tokenize.cpp
OBJS=$(SRCS:.cpp=.o)
CXX=c++
CXXFLAGS=#-Wall -Wextra -Werror -std=c++98
HEADERS=srcs/Server/Server.hpp srcs/Client/Client.hpp

all: $(NAME)
$(NAME): $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)

re: fclean all
