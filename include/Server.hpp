/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 18:58:16 by teddybandam       #+#    #+#             */
/*   Updated: 2024/12/01 16:15:55 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/poll.h>
#include <iostream>
#include <string.h>

class Client;
/*
Dans la class Server, il y a toute les informations  sur le serveur.
Il y a aussi un vecteur de clients qui sont connectes au serveur.
*/
class Server
{
	private:
		int	_fd; // file descriptor du client
		int _port;  // port du serveur
		int _serverSocketFd; // file descriptor du serveur socket
		static bool Signal; // variable pour le signal
		std::vector<Client> _clients; // liste de personnes connectees au serveur via HexChat
		std::vector<struct pollfd> _pollFds; // tableau où sera stocker tous les sockets à surveiller
	public:
		Server();
		~Server();
		void serverInit(); // initialisation du serveur
		void createServerSocket(); // creation du serveur socket
		static void signalHandler(int signal); // handler pour le signal
		void closeFds(); // fermeture des file descriptors
		void clearClients(int fd); // effacer les clients
		void acceptNewClient(); // Accepter les nouveaux clients
		void receiveNewData(int fd); // Reception de la data 
	public:
		int getFd() const; // getter pour le file descriptor
};

#endif