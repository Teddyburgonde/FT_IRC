/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 09:43:05 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/03 15:19:48 by tebandam         ###   ########.fr       */
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


/* Constructor */
Server::Server()
{

}

Server::Server(int port, std::string& password) : _port(port), _password(password)
{

}

/* Destructor */
Server::~Server()
{
	// closeFds(); // Fermer tous les descripteurs ouverts
    // std::cout << "Server resources have been cleaned up!" << std::endl; ???
}

/* Getters */

int Server::getFd() const
{
	return (this->_fd);
}


/*
Le but de cette fonction est de créer une socket serveur qui :

   - Ecoute sur un port donné.
   - Accepte les connexions entrantes des clients;
   - Utilise un prodotocole reseau fiable, comme TCP

*/
void Server::createServerSocket()
{
	struct sockaddr_in serverAddr; // Cette structure est déjà déclarée dans #include <netinet/in.h>
	struct pollfd newPoll; // Cette structure est déjà déclarée dans #include <poll.h>
	// 1. Creation de la socket
	_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));
	std::cout << "The socket is created with fd: " << _serverSocketFd << std::endl;
	// 2. Configurer des options spécifiques pour une socket
	int en = 1; // en = enable.
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	// 3. Configuration les sockets en mode non bloquant pour gerer plusieurs connexions clients simultanées.
	if (fcntl(_serverSocketFd,  F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	// 4. Configuration de la structure sockaddr_in
	serverAddr.sin_family = AF_INET; // La famille d'adresse
	serverAddr.sin_port = htons(this->_port); // Le port utilisé
	serverAddr.sin_addr.s_addr = INADDR_ANY; // Accepte les connexions depuis toutes les interfaces réseau.
	// 5. Associe la socket à l'adresse et au port définis dans serverAddr
	if (bind(_serverSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	// 6. Mettre le server en mode ecoute
	if (listen(_serverSocketFd, 10) == -1)
		throw(std::runtime_error("listen() faild"));
	std::cout << "Server is now listening for connections..." << std::endl;
	// 7. Configuration de la socket serveur pour qu'apres on peut utiliser poll()
	newPoll.fd = _serverSocketFd; // Ajoute la sockeur server
	newPoll.events = POLLIN; // Surveiller les événements de lecture
	newPoll.revents = 0; // Initialiser à 0 (sera rempli par poll())
	_pollFds.push_back(newPoll); // Ajouter à la liste des descripteurs surveillés
}


bool Server::verifyPassword(const std::string& clientPassword) const 
{
        return (_password == clientPassword);
}

/*
Initialise le server
*/
void Server::serverInit()
{
	createServerSocket(); // Creation de la socket serveur

	std::cout << "Server <" << _serverSocketFd << "> Connected"  << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	// tant qu'on ne reçois pas un signal la boucle continue
	while (!Server::Signal)
	{
		// _pollFds[0] c'est le premier element dans le vector.
		// _pollFds.size() signifie qu'il surveille tous les sockets dans le vecteur
		// timeout -1 c'est que poll attends indefiniment qu'un evenement se produise
		// -1 signifie qu'il y a une erreur
		// on a toujours pas recu de signal , donc cela veut dire qu'il y a une erreur mais
		// que cela ne provient pas du signal
		if ((poll(&_pollFds[0], _pollFds.size(), -1) == -1))
		{
			if (Server::Signal)
				break;
    		throw(std::runtime_error("poll() faild"));
		}
		for (size_t i = 0; i < _pollFds.size(); i++) // Parcours le vector là où est stocke tous les fd des sockets
		{			// & c'est operateur bits. Cela signifie que le bits correponds a POLLIN
			if (_pollFds[i].revents & POLLIN) // revents c'est qu'il a detecter un evenement et POLLIN c'est de type data en attente de lecture
			{
				// c'est _serverSocketFd qui ecoute les evenements
				if (_pollFds[i].fd == _serverSocketFd) // si le fd client coresponds a la socket server (Un client essai de se connecter)
					acceptNewClient(); // On accepte le client
				else // Cela signifie qu'un client qui est deja connecter envoie des données au server.
					receiveNewData(_pollFds[i].fd); //RceiveNewData permet de lire les donner envoyer
			}
		}
	}
	closeFds(); // ferme tous les fd ouvert
	std::cout << "Server is shutting down..." << std::endl;
}
