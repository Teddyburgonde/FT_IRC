/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 09:43:05 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/01 17:50:36 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include <algorithm>
#include <cstring>
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

Server::Server()
{
	
}

Server::~Server()
{
	
}

/* Getters */

int Server::getFd() const
{
	return this->_fd;
}

void Server::clearClients(int _fd) 
{
	// Parcours toute la liste des fd socket
	for (size_t i = 0; i < _pollFds.size(); i++)
	{	
		// On parcours la liste des fd socket et si on trouve le fd qu'on veut supprimer
		// on le supprime et la fonction s'arrete avec le break;
		if (_pollFds[i].fd == _fd)
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
		if (_clients[i].getFd() == _fd)
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
	{
		throw(std::runtime_error("faild to create socket"));
	}
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

/*
Gere les nouvelles connextion entrante sur le serveur.
1. Preparer les structures necessaires
2. Appeller accept 
*/


void Server::acceptNewClient()
{
	Client client;
	
	struct pollfd newPoll; // c'est une structure qui va contenir toutes les informations de la socket client 
	struct sockaddr_in clientAddr; // Structure pour les informations du client 
	socklen_t clientAddrLen = sizeof(clientAddr); // Taille de la structure
	int _fdAccept = accept(_serverSocketFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (_fdAccept == -1)
		throw(std::runtime_error("accept() faild"));
	if (fcntl(_fdAccept, F_SETFL, O_NONBLOCK) == -1) 
		throw(std::runtime_error("accept() faild"));
	std::cout << "New client connected with FD: " << _fdAccept << std::endl;
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
void analyzeData()
{
	A completer
}
*/


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
		// Afficher les datas 
		std::cout << "Client <" << fd << "> sent:" << buffer << std::endl;
		// analyzeData () ???? 
	}
}

/*
Initialise le server 
*/
void Server::serverInit()
{
	this->_port = 4444; // Port du serveur
	createServerSocket(); // Creation de la socket serveur
	
	std::cout << "Server <" << _serverSocketFd << "> Connected"  << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	// tant qu'on ne reçois pas un signal la boucle continue 
	while (Server::Signal == false)
	{
		// _pollFds[0] c'est le premier element dans le vector. 
		// _pollFds.size() signifie qu'il surveille tous les sockets dans le vecteur 
		// timeout -1 c'est que poll attends indefiniment qu'un evenement se produise
		// -1 signifie qu'il y a une erreur 
		// on a toujours pas recu de signal , donc cela veut dire qu'il y a une erreur mais
		// que cela ne provient pas du signal
		if ((poll(&_pollFds[0], _pollFds.size(), -1) == -1) && Server::Signal == false)
    		throw(std::runtime_error("poll() faild"));
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
}



/*
Ce qui manque pour un serveur complet

Ton serveur est bien configuré pour écouter et 
préparer les connexions, mais pour 
le rendre pleinement fonctionnel, 
tu dois ajouter une boucle principale et 
gérer les interactions avec les clients :
1. Boucle principale (run ou équivalent)

    Implémente une boucle infinie pour surveiller 
	les descripteurs dans _pollFds avec poll() :
        Accepter les connexions entrantes.
        Lire les messages des clients.
        Supprimer les clients déconnectés.

2. Gestion des connexions entrantes

    Utilise accept() lorsque la socket serveur 
	détecte une connexion entrante.
    Ajoute la nouvelle socket client à _pollFds.

3. Lecture des messages des clients

    Lis les messages avec recv().
    Traite ces messages selon le protocole IRC 
	(par exemple, commandes NICK, JOIN, PRIVMSG).

4. Déconnexion des clients

    Si un client se déconnecte ou qu’une erreur survient, 
	supprime sa socket de _pollFds.
*/