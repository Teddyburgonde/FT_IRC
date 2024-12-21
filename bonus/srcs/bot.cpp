/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 17:55:35 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/21 17:57:32 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Chanel.hpp"

// Fonction pour envoyer un message à tous les clients du canal
void Chanel::botMessageToChanel(int userSender, std::string &msg)
{
	(void)userSender;
    // Parcourt tous les clients du canal et envoie le message à tout le monde, y compris l'expéditeur
    for (size_t i = 0; i < _userInChannel.size(); i++)
    {
        send(_userInChannel[i], msg.c_str(), msg.size(), 0);  // Envoie le message à chaque client
    }
}

// Fonction pour gérer les insultes
void Server::handleBot(int fd)
{
	// Message à envoyer
	std::string reply = "Insult prohibited in this channel 😡\n";

	// Trouver le canal du client
	std::string clientChannel;

	// Pour chaque canal, vérifier si le client est dedans
	for (size_t i = 0; i < _chanel.size(); i++)
	{
		if (_chanel[i].hasClient(fd))  // Vérifie si le client est dans ce canal
		{
			clientChannel = _chanel[i].getName();  // Récupère le nom du canal où le client est
			break;
		}
	}
	// Si un canal a été trouvé
	if (!clientChannel.empty())
	{
		// Trouver le canal dans la liste des canaux
		for (size_t i = 0; i < _chanel.size(); i++)
		{
			if (_chanel[i].getName() == clientChannel)  // Si c'est le bon canal
			{
				// Envoyer un message à tous les clients du canal
				_chanel[i].botMessageToChanel(fd, reply);  // Envoie le message à tous les clients du canal
				break;
			}
		}
	}
	std::cout << "detected insult ❗" << std::endl;
}