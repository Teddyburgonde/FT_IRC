# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teddybandama <teddybandama@student.42.f    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/26 21:27:01 by teddybandam       #+#    #+#              #
#    Updated: 2024/11/26 21:32:55 by teddybandam      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#=================== NAME ===================#

NAME = irc

#================ COMPILATOR ================#

CPP = c++

#=================== FLAGS ==================#

CFLAGS = -Wall -Wextra -Werror -g -std=c++98

#================== SOURCES =================#

SRCS = main.cpp \
		server.cpp 
	

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