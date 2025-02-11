/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClose.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:07:58 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 17:05:42 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Server::closeFds()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Closing client fd: " << _clients[i].getFd() << " > Disconnected" << std::endl;
		close(_clients[i].getFd());
	}

	_clients.clear();
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		std::cout << "Closing poll fd: " << _pollFds[i].fd << " > Disconnected" << std::endl;
		close(_pollFds[i].fd);
	}
	_pollFds.clear();

	if (_serverSocketFd != -1)
	{
		std::cout << "Closing server socket fd: " << _serverSocketFd << std::endl;
		close(_serverSocketFd);
		_serverSocketFd = -1;
	}
}
