/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClose.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:07:58 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/19 16:48:54 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Le but de cette fonction est de fermer les file descriptors des clients connectés au serveur.
On parcours la liste des clients connectés au serveur et on ferme les file descriptors de chaque client.
On ferme aussi le file descriptor du serveur.
Cela evite les fuites de memoire.
*/

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Server::closeFds()
{
    // Fermer les descripteurs des clients et vider le vector _clients
    for (size_t i = 0; i < _clients.size(); i++)
    {
        std::cout << "Closing client fd: " << _clients[i].getFd() << " > Disconnected" << std::endl;
        close(_clients[i].getFd());
    }
    _clients.clear(); // Efface tous les clients

    // Fermer tous les descripteurs dans _pollFds
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        std::cout << "Closing poll fd: " << _pollFds[i].fd << " > Disconnected" << std::endl;
        close(_pollFds[i].fd);
    }
    _pollFds.clear(); // Efface tous les pollfds

    // Fermer le descripteur de la socket serveur
    if (_serverSocketFd != -1)
    {
        std::cout << "Closing server socket fd: " << _serverSocketFd << std::endl;
        close(_serverSocketFd);
        _serverSocketFd = -1;
    }
}
