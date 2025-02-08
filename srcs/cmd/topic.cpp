/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:36:39 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/08 16:52:18 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

/*
TOPIC

A quoi elle sert ?
- Permet de définir ou d'afficher le sujet (topic) d'un canal.

Afficher le sujet d'un canal
- TOPIC <Channel>

Changer le sujet d'un canal
- TOPIC <Channel> :<new_topic>

Reponses possibles du serveur

1. Afficher le sujet existant :

- 332 <user> <Channel> :<topic> : Le sujet actuel du canal.
- 331 <user> <Channel> :No topic is set : Aucun sujet n'est défini pour le canal.

2. Changer le sujet :
- 482 <user> <Channel> :You're not Channel operator : L'utilisateur n'a pas les droits nécessaires.
- 403 <user> <Channel> :No such Channel : Le canal n'existe pas.
- 461 <user> TOPIC :Not enough parameters : La commande est incomplète.

*/

/*Permet de trouver le Channel */

Channel* Server::findChannel(const std::string &ChannelName, std::vector<Channel> &_Channel)
{
	for (std::vector<Channel>::iterator it = _Channel.begin(); it != _Channel.end(); ++it)
	{
		if (it->getName() == ChannelName)
			return &(*it);
	}
	return (NULL);
}

void Server::handleTopic(int fd, const Message &msg, std::vector<Channel> &_Channel)
{
	// 1. Vérifier si un canal est spécifié
	int			index;
	std::string channel = get_next_argument(msg.getArgument().c_str(), index);
	if (channel.empty())
	{
		betterSend(ERR_NEEDMOREPARAMS(std::string("Server"), "TOPIC"), fd);
		return;
	}

	// 2. Trouver le canal correspondant
	Channel* targetChannel = findChannel(channel, _Channel);
	if (!targetChannel)
	{
		betterSend(ERR_NOSUCHCHANNEL(channel), fd);
		return;
	}

	// 3. Vérifier si l'expéditeur est dans le canal
	if (!isSenderInChannel(fd, *targetChannel))
	{
		betterSend(ERR_NOTONCHANNEL(std::string("Server"), channel), fd);
		return;
	}

	// 4. Vérifier si un nouveau sujet est fourni
	std::string newTopic = get_next_argument(msg.getArgument().c_str(), index);
	if (newTopic.empty())
	{
		// Pas de nouveau sujet fourni, afficher le sujet actuel
		if (!targetChannel->getTopic().empty())
			betterSend(RPL_SEETOPIC(std::string("Server"), targetChannel->getName(), targetChannel->getTopic()), fd);
		else
			betterSend(RPL_NOTOPIC(std::string("Server"), targetChannel->getName()), fd);
		return;
	}
	
	//!AJOUT GALAAD
	//Si pas operateur et que le mode operateur only est active pour les topics (+t), erreur
	if (!is_user_in_chan(fd, targetChannel->getOperatorUser()) && targetChannel->getModeT())
	{
		betterSend(ERR_CHANOPRIVSNEEDED(find_nickname_with_fd(fd, this->_clients), targetChannel->getName()), fd);
		return;
	}
	
	targetChannel->setTopic(newTopic);
	//!on envoie a tout le monde le nouveau topic. Peux etre pas le bon message !
	std::string notification = RPL_SEETOPIC(std::string("Server"), targetChannel->getName(), targetChannel->getTopic());
	//là on envoie le message au gens du channel
	targetChannel->sendMessageToChannel(-1, notification);
}