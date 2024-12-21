/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:36:39 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/16 18:10:54 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"


/*
TOPIC

A quoi elle sert ?
- Permet de définir ou d'afficher le sujet (topic) d'un canal.

Afficher le sujet d'un canal
- TOPIC <channel>

Changer le sujet d'un canal
- TOPIC <channel> :<new_topic>

Reponses possibles du serveur

1. Afficher le sujet existant :

- 332 <user> <channel> :<topic> : Le sujet actuel du canal.
- 331 <user> <channel> :No topic is set : Aucun sujet n'est défini pour le canal.

2. Changer le sujet :
- 482 <user> <channel> :You're not channel operator : L'utilisateur n'a pas les droits nécessaires.
- 403 <user> <channel> :No such channel : Le canal n'existe pas.
- 461 <user> TOPIC :Not enough parameters : La commande est incomplète.

*/


/* Le server envoie un message d'erreur aux clients connectés */
void Server::sendError(int fd, const std::string &errorMessage)
{
	send(fd, errorMessage.c_str(), errorMessage.size(), 0);
}

/*Permet de trouver le channel */

Chanel* Server::findChannel(const std::string &channelName, std::vector<Chanel> &_chanel)
{
	for (std::vector<Chanel>::iterator it = _chanel.begin(); it != _chanel.end(); ++it)
	{
		if (it->getName() == channelName)
			return &(*it);
	}
	return NULL;
}

void Server::handleTopic(int fd, const Message &msg, std::vector<Chanel> &_chanel)
{
	// 1. Vérifier si un canal est spécifié
	size_t spacePos = msg.getArgument().find(' ');
	std::string channel = (spacePos == std::string::npos) ? msg.getArgument() : msg.getArgument().substr(0, spacePos);

	if (channel.empty())
	{
		sendError(fd, ERR_NEEDMOREPARAMS(std::string("Server"), "TOPIC"));
		return;
	}

	// 2. Trouver le canal correspondant
	Chanel* targetChannel = findChannel(channel, _chanel);
	if (!targetChannel)
	{
		sendError(fd, ERR_NOSUCHCHANNEL(channel));
		return;
	}

	// 3. Vérifier si l'expéditeur est dans le canal
	if (!isSenderInChannel(fd, *targetChannel))
	{
		sendError(fd, ERR_NOTONCHANNEL(std::string("Server"), channel));
		return;
	}

	//!AJOUT GALAAD
	//Si pas operateur et que le mode operateur only est active pour les topics (+t)
	//erreur
	if (!is_user_in_chan(fd, targetChannel->getOperatorUser()) && targetChannel->getModeT())
	{
		send_error(ERR_CHANOPRIVSNEEDED(find_nickname_with_fd(fd, this->_clients), targetChannel->getName()), fd);
		return;
	}

	// 4. Vérifier si un nouveau sujet est fourni
	size_t topicStart = msg.getArgument().find(':');
	if (topicStart == std::string::npos)
	{
		// Pas de nouveau sujet fourni, afficher le sujet actuel
		if (!targetChannel->getTopic().empty())
			sendError(fd, RPL_SEETOPIC(std::string("Server"), targetChannel->getName(), targetChannel->getTopic()));
		else
			sendError(fd, RPL_NOTOPIC(std::string("Server"), targetChannel->getName()));
		return;
	}

	// 5. Extraire et mettre à jour le sujet
	std::string newTopic = msg.getArgument().substr(topicStart + 1);
	if (newTopic.empty())
	{
		sendError(fd, ERR_NEEDMOREPARAMS(std::string("Server"), "TOPIC"));
		return;
	}

	targetChannel->setTopic(newTopic);
	std::ostringstream oss;
	oss << fd;
	std::string notification = ":" + oss.str() + " TOPIC " + targetChannel->getName() + " :" + targetChannel->getTopic() + "\r\n";
	targetChannel->sendMessageToChanel(fd, notification);
}