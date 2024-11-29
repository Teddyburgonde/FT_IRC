/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teddybandama <teddybandama@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 09:43:05 by tebandam          #+#    #+#             */
/*   Updated: 2024/11/29 22:09:59 by teddybandam      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include <algorithm>
#include <netinet/in.h>
#include <sys/socket.h>


/*
Le but de cette fonction est de supprimer un client de la liste des clients connectés au serveur.
Elle est en deux parties : 
- La première boucle sert a supprimer 
le fd du client de la liste des fd socket.
- La deuxième boucle sert a supprimer le client de la liste des clients connectés.
Un client se sert d'une socket pour se connecter au serveur.
*/

void Server::clearClients(int fd) 
{
	// Parcours toute la liste des fd socket
	for (size_t i = 0; i < _pollFds.size(); i++)
	{	
		// On parcours la liste des fd socket et si on trouve le fd qu'on veut supprimer
		// on le supprime et la fonction s'arrete avec le break;
		if (_pollFds[i].fd == fd)
		{
			// erase supprime l'element a l'index i
			_pollFds.erase(_pollFds.begin() + i);
			break; // On sort de la boucle car le fd a été trouvé et supprimé 
		}
	}
	// On parcours la liste des clients connectés au serveur
	// et on supprime le client qui a le fd qu'on veut supprimer
	// de la liste des clients connectés.
	// on sort de la boucle avec le break;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		// getFd() permet de recuperer le fd du client (c'est un getter)
		if (_clients[i].getFd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
}

/*
Le but de cette fonction c'est de dire qu'on a reçu un signal 
*/
bool Server::Signal = false; // On initialise la variable Signal a false ici car c'est une variable statique
void Server::signalHandler(int signal)
{
	(void)signal;
	std::cout << std::endl << "Signal Received!" << std::endl;
	// On change la variable Signal a true
	Server::Signal = true;
}

/*
Le but de cette fonction est de fermer les file descriptors des clients connectés au serveur.
On parcours la liste des clients connectés au serveur et on ferme les file descriptors de chaque client.
On ferme aussi le file descriptor du serveur.
Cela evite les fuites de memoire.
*/
void Server::closeFds()
{
	// On ferme tous les file descriptors des clients connectés au serveur
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Closing fd: " << _clients[i].getFd() << "> Disconnected" << std::endl;
		close(_clients[i].getFd());
	}
	// On ferme le file descriptor du serveur
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		std::cout << "Closing fd: " << _pollFds[i].fd << "> Disconnected" << std::endl;
		close(_pollFds[i].fd);
	}
	// On ferme le file descriptor du serveur
	if (_serverSocketFd != -1)
	{
		std::cout << "Closing server socket" << std::endl;
		close(_serverSocketFd);
	}	
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



// 1. Initialisation des structures nécessaires au serveur (comme _pollFds)
// 2. Création et configuration de la socket serveur (via createServerSocket)
// 3. Ajout de la socket serveur à _pollFds pour la surveiller avec poll()
// 4. Configuration des signaux (facultatif)
// 5. Affichage d’un message indiquant que le serveur est prêt

// Faire la fonction server init 
void Server::serverInit()
{
	this->_port = 4444; // Port du serveur
	createServerSocket(); // Creation de la socket serveur
	
	std::cout << "Server <" << _serverSocketFd << "> Connected"  << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}
