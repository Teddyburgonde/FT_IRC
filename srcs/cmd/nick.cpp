/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 16:34:07 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 20:35:00 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

void Server::handleNick(int fd, const std::string &newNick)
{
	std::string response;

	if (newNick.empty())
	{
		response = ERR_NONICKNAMEGIVEN(std::string("Server"), ""); //!A changer par CLIENT
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getNickname() == newNick)
		{
			response = ERR_NICKNAMEINUSE(std::string("Server"), newNick);//!A changer par CLIENT
			send(fd, response.c_str(), response.size(), 0);
			return;
		}
	}
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getFd() == fd)
		{
			_clients[i].setNickname(newNick);
			response = RPL_WELCOME(newNick);//!A changer par CLIENT
			send(fd, response.c_str(), response.size(), 0);
			return;
		}
	}
}
