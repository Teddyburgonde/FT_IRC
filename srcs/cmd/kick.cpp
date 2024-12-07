/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 11:36:47 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/07 14:00:28 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Client.hpp"
#include "include/Server.hpp"
#include "include/Message.hpp"

/*
KICK <channel> <target> [<comment>]
L'expediteur doit etre dans le canal
L'utilisateur a expulser doit egalement etre dans le canal.
L'expéditeur doit avoir les privilèges requis pour expulser un utilisateur (opérateur du canal).


:<expéditeur> KICK <channel> <target> :<comment>

ERR_NOSUCHCHANNEL : Le canal n'existe pas. (Je l'ai).
ERR_NOTONCHANNEL : L'expéditeur n'est pas sur le canal. (je l'ai).
ERR_USERNOTINCHANNEL : L'utilisateur à expulser n'est pas sur le canal. (je l'ai) 
ERR_CHANOPRIVSNEEDED : L'expéditeur n'a pas les droits pour expulser. (je l'ai)

*/


// void Server::handleKick(int fd, Message &msg, std::vector<Chanel> &_chanel)
// {
	
// }