/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 09:43:05 by tebandam          #+#    #+#             */
/*   Updated: 2024/11/27 13:01:09 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

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

*/
bool Server::Signal = false; // On initialise la variable Signal a false ici car c'est une variable statique
void Server::signalHandler(int signal)
{
	(void)signal;
	std::cout << std::endl << "Signal Received!" << std::endl;
	// On change la variable Signal a true
	Server::Signal = true;
}