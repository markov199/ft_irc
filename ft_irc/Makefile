
NAME		= ircserv

CXX			= c++

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

RM			= rm -rf

SRCS		= main.cpp Server.cpp Client.cpp Command.cpp Channel.cpp Utils.cpp \
			Auth.cpp Join.cpp Invite.cpp Part.cpp Kick.cpp Mode.cpp Privmsg.cpp Topic.cpp

HEADER		= Server.hpp Client.hpp Command.hpp Channel.hpp responses.hpp

OBJS		= $(SRCS:.cpp=.o)

all:		$(NAME)
			@printf "\033[35m-> executable done! <-\033[0m\n"

$(NAME):	$(OBJS) $(HEADER)
			$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)
			@printf "\033[33m-> executable deleted! <-\033[0m\n"

re:			fclean all

.PHONY:		all clean fclean re
