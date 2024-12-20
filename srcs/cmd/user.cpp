/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 17:55:54 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/20 15:02:30 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Chanel.hpp"
#include "../../include/Message.hpp"
#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Chanel.hpp"
#include "../../include/Message.hpp"

// Valider le format du nom d'utilisateur.
bool isValidUsername(const std::string& user) 
{
    if (user.empty() || user.size() > 10) // Limite de 9 caractères pour le nom d'utilisateur
        return false;
    if (!std::isalpha(user[0])) // Premier caractère doit être une lettre
        return false;
    for (size_t i = 0; i < user.size(); ++i) 
	{
        if (!std::isalnum(user[i]) && user[i] != '_')
            return false; // Seulement des lettres, chiffres et underscores
    }
    return true;
}

void Server::handleUser(int fd, const std::string& user)
{
	if (!isValidUsername(user))
	{
		std::string response = ERR_ERRONEUSNICKNAME(std::string("Server"), user);
		send(fd, response.c_str(), response.size(), 0);
		return ;
	}
    // Vérifier si la liste des clients est vide
    if (_clients.empty()) 
    {
        std::cerr << "Error: No clients connected, handleUser aborted." << std::endl;
        return ;
    }
	// Vérifie si l'utilisateur a déjà défini un nom
	for (size_t i = 0; i < this->_clients.size(); ++i) 
	{
        if (this->_clients[i].getFd() == fd && ! this->_clients[i].getUsername().empty()) {
            std::string response = ERR_ALREADYREGISTRED(std::string("Server"));
            send(fd, response.c_str(), response.size(), 0);
            return ;
        }
    }
	// Associer l'utilisateur à la structure Client
    for (size_t i = 0; i < _clients.size(); ++i) 
	{
        if (_clients[i].getFd() == fd) 
		{
            _clients[i].setUsername(user);
            break ;
        }
    }
	std::string response = RPL_WELCOME(user);
	send(fd, response.c_str(), response.size(), 0);
      std::cout << "Client FD: " << fd << " User: " << user << std::endl;
}
