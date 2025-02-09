/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 09:43:05 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 20:38:38 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include <algorithm>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../include/Message.hpp"

Server::Server()
{

}

Server::Server(int port, std::string& password) : _port(port), _password(password)
{

}

Server::~Server()
{
	
}


int Server::getFd() const
{
	return (_fd);
}

int	Server::getFdAccept() const
{
	return (_fdAccept);
}

/* Server socket configuration for poll() */
void Server::socketConfigurationForPoll(int fd)
{
	struct pollfd newPoll;

	newPoll.fd = fd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_pollFds.push_back(newPoll);
}

/* This function creates and configures a socket.*/
void Server::createServerSocket()
{
	int enable = 1;
	struct sockaddr_in serverAddr;

	_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(_serverSocketFd,  F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(this->_port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_serverSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(_serverSocketFd, 10) == -1)
		throw(std::runtime_error("listen() faild"));
	socketConfigurationForPoll(_serverSocketFd);
}

bool Server::verifyPassword(const std::string& clientPassword) const 
{
	return (_password == clientPassword);
}

/* Creation and initialization of the server */
void Server::serverInit()
{
	createServerSocket();

	std::cout << "Waiting to accept a connection...\n";
	while (!Server::Signal)
	{
		if ((poll(&_pollFds[0], _pollFds.size(), -1) == -1))
		{
			if (Server::Signal)
				break;
			throw(std::runtime_error("poll() faild"));
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _serverSocketFd)
					acceptNewClient();
				else
					receiveNewData(_pollFds[i].fd);
			}
		}
	}
	closeFds();
	std::cout << "Server is shutting down..." << std::endl;
}
