/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 17:55:54 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 19:01:10 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Message.hpp"

// Validate the format of username
static bool isValidUsername(const std::string& user) 
{
	if (user.empty() || user.size() > 10)
		return (false);
	if (!std::isalpha(user[0]))
		return (false);
	for (size_t i = 0; i < user.size(); ++i) 
	{
		if (!std::isalnum(user[i]) && user[i] != '_')
			return (false);
	}
	return (true);
}

void Server::handleUser(int fd, const std::string& user)
{
	std::string response;

	if (!isValidUsername(user))
	{
		response = ERR_ERRONEUSNICKNAME(std::string("Server"), user);//!A changer par CLIENT
		betterSend(response, fd);
		return;
	}
	for (size_t i = 0; i < this->_clients.size(); ++i) 
	{
		if (this->_clients[i].getFd() == fd && ! this->_clients[i].getUsername().empty())
		{
			response = ERR_ALREADYREGISTRED(std::string("Server"));//!A changer par CLIENT
			betterSend(response, fd);
			return;
		}
	}
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getFd() == fd) 
		{
			_clients[i].setUsername(user);
			break;
		}
	}
	response = RPL_WELCOME(user);//!A changer par CLIENT
	betterSend(response, fd);
}
