# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/26 21:27:01 by teddybandam       #+#    #+#              #
#    Updated: 2025/02/13 17:29:19 by tebandam         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#=================== NAME ===================#

NAME = ircserv

#================ COMPILATOR ================#

CPP = c++

#=================== FLAGS ==================#

CFLAGS = -std=c++98 -Wall -Wextra -Werror -I../include

#================== SOURCES =================#

SRCS = srcs/main.cpp \
	   srcs/Server.cpp \
		srcs/Client.cpp \
		srcs/Channel.cpp \
		srcs/parsing.cpp \
		srcs/Message.cpp \
		srcs/utils.cpp \
		srcs/serverClose.cpp \
		srcs/serverSignals.cpp \
		srcs/serverClear.cpp \
		srcs/serverIncomingConnections.cpp \
		srcs/cmd/join.cpp \
		srcs/cmd/invite.cpp \
		srcs/cmd/mode.cpp \
		srcs/cmd/kick.cpp \
		srcs/cmd/nick.cpp \
		srcs/cmd/privMsg.cpp \
		srcs/cmd/topic.cpp \
		srcs/cmd/user.cpp \


#============== OBJECT FILES ================#

OBJS = $(SRCS:.cpp=.o)

#==========GENERATION BINARY FILES ==========#

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@


#================= COLORS ===================#

GREEN = \033[32m
RESET = \033[0m

#================ UTILS PART ================#

RM = rm -f

all: $(NAME)

$(NAME) : $(OBJS)
	$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN) EXECUTABLE READY !$(RESET)"

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re