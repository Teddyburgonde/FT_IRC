# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/26 21:27:01 by teddybandam       #+#    #+#              #
#    Updated: 2025/02/06 15:49:12 by gmersch          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#=================== NAME ===================#

NAME = ircserv
NAME_BONUS = bot

#================ COMPILATOR ================#

CPP = c++

#=================== FLAGS ==================#

CFLAGS = -std=c++98 -Wall -Wextra -Werror -I../include

#================== SOURCES =================#

SRCS = main.cpp \
	   Server.cpp \
		Client.cpp \
		Channel.cpp \
		parsing.cpp \
		Message.cpp \
		utils.cpp \
		serverClose.cpp \
		serverSignals.cpp \
		serverClear.cpp \
		serverIncomingConnections.cpp \
		cmd/join.cpp \
		cmd/invite.cpp \
		cmd/mode.cpp \
		cmd/kick.cpp \
		cmd/nick.cpp \
		cmd/privMsg.cpp \
		cmd/topic.cpp \
		cmd/user.cpp \

SRCS_BONUS = ../bonus/main.cpp \
			../bonus/Server.cpp \
			../bonus/serverIncomingConnections_bonus.cpp \


#============== OBJECT FILES ================#

OBJS = $(SRCS:.cpp=.o)
OBJS_BONUS = $(SRCS_BONUS:.cpp=.o)

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

bonus: $(OBJS_BONUS)
	$(CPP) $(CFLAGS) $(OBJS_BONUS) -o $(NAME_BONUS)
	@echo "$(GREEN) EXECUTABLE BONUS READY !$(RESET)"

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus