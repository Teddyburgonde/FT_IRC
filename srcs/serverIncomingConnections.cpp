/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverIncomingConnections.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:03:05 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/08 19:41:29 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Server.hpp"
#include "../include/Client.hpp"

/* Accepte une connection client sur une socket */
int Server::acceptionConnection(struct sockaddr_in &clientAddr)
{
	socklen_t	clientAddrLen;
	int			_fdAccept;

	clientAddrLen = sizeof(clientAddr);	
	_fdAccept = accept(_serverSocketFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (_fdAccept == -1)
		throw(std::runtime_error("accept() faild"));
	if (fcntl(_fdAccept, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("accept() faild"));
	return (_fdAccept);
}

/* Imprime les differentes informations sur le client */
static void	printClientInfo(int	_fdAccept, struct sockaddr_in &clientAddr)
{
	std::cout << "New client connected with FD: " << _fdAccept << " with ip " << inet_ntoa(clientAddr.sin_addr)
	<< " with Port: " << ntohs(clientAddr.sin_port) << std::endl;
}

/* Ajoute un client dans la liste des clients */
void	Server::addClientToList(int _fdAccept, struct sockaddr_in &clientAddr)
{
	Client	client;
	std::string ipAddress;

	ipAddress = inet_ntoa(clientAddr.sin_addr);
	
	client.setFd(_fdAccept);
	client.setIpAddress(ipAddress);
	_clients.push_back(client);
}

void Server::sendEnterPasswordMessage(int fd)
{
	const char* message;

	message  = "enter password :\n";
	if (send(fd, message, strlen(message), 0) == -1)
		throw std::runtime_error("Failed to send welcome message");
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	int	fdAccept;
	
	fdAccept = acceptionConnection(clientAddr);
	addClientToList(fdAccept, clientAddr);
	socketConfigurationForPoll(fdAccept);
	printClientInfo(fdAccept, clientAddr);
	//sendEnterPasswordMessage(fdAccept);
}

/* Reçoit les données envoyées par un client connecté. */
void	Server::receiveNewData(int fd)
{
	char buffer[1024] = {0};
	ssize_t bytes;
	std::string			stockBuffer;

	bytes = recv(fd, buffer, sizeof(buffer) - 1 , 0);
	if (bytes <= 0)
	{
		std::cout << "Client <" << fd << "> Disconnected" << std::endl;
		clearClients(fd);
		close(fd);
	}
	else
	{
		buffer[bytes] = '\0';
		std::string bufferVector(buffer, buffer + bytes);
		std::stringstream	tmpStream(buffer);
		while (std::getline(tmpStream, stockBuffer))
			analyzeData(fd, stockBuffer);
	}
}
