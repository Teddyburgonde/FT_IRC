/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverIncomingConnections.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:03:05 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/07 16:53:55 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include <ostream>
#include <sstream>
#include <string>

/*
Gere les nouvelles connexion entrante sur le serveur.
1. Preparer les structures necessaires
2. Appeller accept
*/

void Server::acceptNewClient()
{
	Client client;

	const char* message = "enter password :\n";
	struct pollfd newPoll; // c'est une structure qui va contenir toutes les informations de la socket client
	struct sockaddr_in clientAddr; // Structure pour les informations du client
	socklen_t clientAddrLen = sizeof(clientAddr); // Taille de la structure
	int _fdAccept = accept(_serverSocketFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (_fdAccept == -1)
		throw(std::runtime_error("accept() faild"));
	if (fcntl(_fdAccept, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("accept() faild"));
	std::cout << "New client connected with FD: " << _fdAccept << std::endl;
	send(_fdAccept, message, strlen(message), 0);
	client.setFd(_fdAccept); // On set le fd du client
	client.setIpAddress(inet_ntoa(clientAddr.sin_addr)); // On set l'adresse IP du client
	_clients.push_back(client); // On ajoute le client a la liste des clients connectes
	newPoll.fd = _fdAccept;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_pollFds.push_back(newPoll);
	// Affiche l'adresse IP et le port du client
    std::cout << "Client connected from IP: " << inet_ntoa(clientAddr.sin_addr)
	<< " Port: " << ntohs(clientAddr.sin_port) << std::endl;
}

/*
Cette fonction permet de recevoir les données
envoyées par un client connecté.
*/
void	Server::receiveNewData(int fd)
{
	char buffer[1024] = {0};  // Pour stocker les données et on l'a initialiser a 0
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1 , 0);
	if (bytes <= 0) // client déconnecté ou qu’une erreur est survenue.
	{
		std::cout << "Client <" << fd << "> Disconnected" << std::endl;
		clearClients(fd);
		close(fd);
	}
	else
	{
		buffer[bytes] = '\0';
		std::string bufferVector(buffer, buffer + bytes);
		std::stringstream	tmp_stream(buffer);
		std::string			tmp_buffer;
		while (std::getline(tmp_stream, tmp_buffer))
		{
			if (!tmp_buffer.find("CAP"))
				continue ;
			std::cout << "cuted buffer: " << tmp_buffer << std::endl;
			analyzeData(fd, tmp_buffer);
		}
	}
}
// void Server::analyzeData(int fd,  const std::string &buffer);