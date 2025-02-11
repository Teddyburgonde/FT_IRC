/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClear.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:04:29 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 17:05:21 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Channel.hpp"

void Server::clearClients(int _fd)
{
	std::vector<Channel>::iterator	it_chan;
	std::vector<int>::iterator		it_user;

	for (it_chan = this->_channel.begin(); it_chan != this->_channel.end(); it_chan++)
	{
		it_user = std::find(it_chan->getUserInChannel().begin(), it_chan->getUserInChannel().end(), _fd);
		if (it_user != it_chan->getUserInChannel().end())
			it_chan->removeUser(_fd, 0, this->_clients);
	}

	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == _fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == _fd)
		{
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
	_authenticatedClients.erase(_fd);
}