/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 11:36:47 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 17:02:11 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

bool Server::validateKickArgs(int fd, Message &msg, std::string &channel, std::string &targetUser)
{
	int index;

	index = 0;
	channel = get_next_argument(msg.getArgument().c_str(), index);
	targetUser = get_next_argument(msg.getArgument().c_str(), index);
	if (channel.empty() || targetUser.empty())
	{
		std::string response = ERR_NEEDMOREPARAMS(CLIENT(find_nickname_with_fd(fd, _clients), find_username_with_fd(fd, _clients)), "KICK");
		betterSend(response, fd);
		return (false);
	}
	std::vector<Channel>::iterator it = find_channel_with_name(channel, _channel);
	if (it == _channel.end())
	{
		std::string response = ERR_NOSUCHCHANNEL(channel);
		betterSend(response, fd);
		return (false);
	}
	return (true);
}

bool Server::isSenderInChannel(int fd, Channel &channel)
{
	const std::vector<int>& users = channel.getUserInChannel();
	for (std::vector<int>::const_iterator userIt = users.begin(); userIt != users.end(); ++userIt)
	{
		if (*userIt == fd)
			return (true);
	}
	return (false);
}

bool Server::isSenderOperator(int fd, Channel &channel)
{
	std::vector<int> operators = channel.getOperatorUser();

	bool isOperator = std::find(operators.begin(), operators.end(), fd) != operators.end();
	return (isOperator);
}

bool Server::isTargetInChannel(const std::string &targetUser, Channel &channel, int fd)
{
	std::vector<int>& users = channel.getUserInChannel();
	for (std::vector<int>::iterator userIt = users.begin(); userIt != users.end(); ++userIt)
	{
		std::string stock = find_nickname_with_fd(*userIt, this->_clients);
		if (stock == targetUser)
		{
			channel.removeUser(*userIt, fd, this->_clients);
			return (true);
		}
	}
	return (false);
}

void Server::handleKick(int fd, Message &msg)
{
	std::string	channel;
	std::string	targetUser;
	std::string	username_sender;
	std::string	nickname_sender;

	if (!validateKickArgs(fd, msg, channel, targetUser))
        return;
	for (std::vector<Channel>::iterator i = _channel.begin(); i != _channel.end(); i++)
	{
		if (i->getName() == channel)
		{
			if (!isSenderInChannel(fd, *i))
			{
				std::string response = ERR_NOTONCHANNEL(CLIENT(username_sender, nickname_sender), channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}
			if (!isSenderOperator(fd, *i))
			{
				std::string response = ERR_CHANOPRIVSNEEDED(CLIENT(username_sender, nickname_sender), channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}
			if (!isTargetInChannel(targetUser, *i, fd))
			{
				std::string response = ERR_USERNOTINCHANNEL(CLIENT(username_sender, nickname_sender), targetUser, channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}
			username_sender = find_username_with_fd(fd, this->_clients);
			nickname_sender = find_nickname_with_fd(fd, this->_clients);
			std::string message = RPL_KICK(CLIENT(username_sender, nickname_sender), i->getName(), targetUser);
			i->sendMessageToChannel(-1, message);
			return;
		}
	}
}