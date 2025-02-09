/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClose.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:07:58 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 20:03:59 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

/*
The purpose of this function is to close the file descriptors of clients connected to the server.
We browse the list of clients connected to the server and close the file descriptors of each client.
We also close the server file descriptor.
This avoids memory leaks.
*/

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
