/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teddybandama <teddybandama@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 18:58:16 by teddybandam       #+#    #+#             */
/*   Updated: 2024/11/26 21:23:06 by teddybandam      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <vector>

class Client;
class Server
{
	private:
		int _port;  // port du serveur
		int _serverSocketFd; // file descriptor du serveur socket
		static bool Signal; // variable pour le signal
		std::vector<Client> _clients; // liste de personnes connectees au serveur via HexChat
		std::vector<struct pollfd> _pollFds; // tableau de pollfd, struct ???
	public:
		Server();
		~Server();
		void serverInit(); // initialisation du serveur
		void createServerSocket(); // creation du serveur socket
		void receiveMessage(); // reception des messages
		static void signalHandler(int signal); // handler pour le signal
		void closeFds(); // fermeture des file descriptors
		void clearClients(); // effacer les clients
};


#endif