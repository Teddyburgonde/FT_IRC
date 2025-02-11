/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverIncomingConnections.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:03:05 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 16:43:30 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

/* Accepts a client connection on a socket */
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

/* Prints the different information about the client */
static void	printClientInfo(int	_fdAccept, struct sockaddr_in &clientAddr)
{
	std::cout << "New client connected with FD: " << _fdAccept << " with ip " << inet_ntoa(clientAddr.sin_addr)
	<< " with Port: " << ntohs(clientAddr.sin_port) << std::endl;
}

/* Adds a client to the client list */
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
}

/* Receives data sent by a connected client. */
void    Server::receiveNewData(int fd)
{
    static std::map<int, std::string> clientBuffers;
    char buffer[1024] = {0}; 
    std::string stockBuffer;
    ssize_t bytes;
    size_t pos;

    bytes = recv(fd, buffer, sizeof(buffer) - 1 , 0);
    if (bytes <= 0)
    {
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        clearClients(fd);
        close(fd);
        clientBuffers.erase(fd);
        return;
    }
    clientBuffers[fd] += std::string(buffer, bytes);
    std::string &clientBuffer = clientBuffers[fd];
    while ((pos = clientBuffer.find('\n')) != std::string::npos)
    {
        stockBuffer = clientBuffer.substr(0, pos);
        if (!stockBuffer.empty() && stockBuffer[stockBuffer.size() - 1] == '\r') 
            stockBuffer.erase(stockBuffer.size() - 1);
        analyzeData(fd, stockBuffer);
        clientBuffer.erase(0, pos + 1);
    }
}
