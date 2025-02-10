/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 20:38:06 by gmersch           #+#    #+#             */
/*   Updated: 2025/02/10 19:45:45 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include "../include/Message.hpp"
#include <algorithm>

bool Server::authenticatedClients(int fd,  std::string &buffer)
{
	std::string	cmd;
	std::string	newNick;
	std::string	userArguments;
	int			i;

	i  = 0;
	cmd = get_next_argument(buffer.c_str(), i);
	if (_authenticatedClients[fd] == false) 
	{
		if (strcmp(cmd.c_str(), "PASS") == 0) 
		{
			cmd = get_next_argument(buffer.c_str(), i);
			if (cmd == _password) 
			{
				_authenticatedClients[fd] = true;
				betterSend("OK :Password accepted\n", fd);
			}
			else
				betterSend("ERROR :Invalid password\n", fd);
		}
		else
			betterSend("ERROR :You must authenticate first using PASS\r\n", fd);
		return (false);
	}
	else if (find_nickname_with_fd(fd, this->_clients).empty())
	{
		if (strncmp(buffer.data(), "NICK ", 5) == 0)
		{
			newNick = get_next_argument(buffer.c_str(), i);
			handleNick(fd, newNick);
		}
		else
			betterSend(ERR_NOTREGISTERED(), fd);
		return (false);
	}
	else if (find_username_with_fd(fd, this->_clients).empty())
	{
		if (strncmp(buffer.data(), "USER ", 5) == 0)
		{
			userArguments = get_next_argument(buffer.c_str(), i);
			handleUser(fd, userArguments);
		}
		else
			betterSend(ERR_NOTREGISTERED(), fd);
		return (false);
	}
	return (true);
}

void Server::analyzeData(int fd,  std::string &buffer)
{
	int	i;
	std::string nickUserArgument;

	if (authenticatedClients(fd, buffer) == false)
		return;
	Message msg;

	i = 0;
	msg.parse_buffer(buffer, msg);
	nickUserArgument = get_next_argument(msg.getArgument().c_str(), i);
	if (msg.getCommand().empty())
		return;
	else if (msg.getCommand() == "TOPIC")
		handleTopic(fd, msg);
	else if (msg.getCommand() == "PRIVMSG")
		handlePrivMsg(fd, msg);
	else if (msg.getCommand() == "KICK")
        handleKick(fd, msg);
	else if (msg.getCommand() == "JOIN")
		handleJoin(fd, msg);
	else if (msg.getCommand() == "INVITE")
		inviteCommand(fd, msg);
	else if (msg.getCommand() == "MODE")
		modeCommand(fd, msg);
	else if (msg.getCommand() == "NICK")
		handleNick(fd, nickUserArgument);
	else if (msg.getCommand() == "USER")
		handleUser(fd, nickUserArgument);
}

void Message::parse_buffer(const std::string &buffer, Message& msg)
{
	int	index;

	index = 0;
	msg.setCommand(get_next_argument(buffer.c_str(), index));
	while (buffer[index] && buffer[index] == ' ')
		index++;
	msg.setArgument(std::string(buffer.c_str() + index));
}
