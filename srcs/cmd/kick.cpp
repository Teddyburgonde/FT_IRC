/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 11:36:47 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/16 21:00:55 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"
#include <algorithm> //A METTRE .H


/* Cette fonction Verifie si le format est bien respecter pour la commande KICK
	KICK <channel> <target> [<comment>]
	Commande valide :
	KICK #general Adrien
	Commande invalide
	KICK #general
*/

bool Server::validateKickArgs(int fd, Message &msg, std::string &channel, std::string &targetUser)
{
    // Trouver la position du premier espace dans les arguments de la commande.
    size_t spacePos = msg.getArgument().find(' ');

    // Si aucun espace n'est trouvé apres le #channel
	// cela signifie que le format est invalide.
    if (spacePos == std::string::npos)
	{
        std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
        // Envoyer la réponse d'erreur au client qui a envoyé la commande.
        send(fd, response.c_str(), response.size(), 0);
        // false le format n'est pas correct.
        return false;
    }
    // Extraire le nom du canal en prenant la sous-chaîne avant l'espace.
    // donc la il y a #channel dans la variable channel
	channel = msg.getArgument().substr(0, spacePos);
    // Extraire le nom de l'utilisateur cible en prenant la sous-chaîne après l'espace.
    // substr(spacePos + 1) extrait le texte apres l'espace
	// donc la il y a le nom de l'utilisateur par exemple "Galaad"
	targetUser = msg.getArgument().substr(spacePos +1);
    // Si le nom du canal ou le nom de l'utilisateur cible est vide, le format est invalide.
	targetUser.erase(std::remove(targetUser.begin(), targetUser.end(), '\r'), targetUser.end());
	targetUser.erase(std::remove(targetUser.begin(), targetUser.end(), '\n'), targetUser.end());
	if (channel.empty() || targetUser.empty())
	{
        std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
        // Envoyer la réponse d'erreur au client qui a envoyé la commande.
        send(fd, response.c_str(), response.size(), 0);
         // false le format n'est pas correct.
        return false;
    }
    // Command valid
    return true;
}

/*
Vérifie si l'expéditeur est dans le canal
L'expediteur c'est celui qui fais la command KICK
Par Exemple si Galaad est connecté au server et q'il tape KICK #general Toto
Galaad est l'expediteur, Toto la target.
*/

bool Server::isSenderInChannel(int fd, Chanel &channel)
{
	const std::vector<int>& users = channel.getUserInChannel();
	for (std::vector<int>::const_iterator userIt = users.begin();
			userIt != users.end(); ++userIt)
	{
		if (*userIt == fd) // il a trouvé l'expéditeur.
			return true;
	}
	return false;
}

/*
Cette fonction vérifie si l'expéditeur est un operateur.
Cela permet de savoir si l'utilisateur a les privilèges pour faire la command KICK
*/
bool Server::isSenderOperator(int fd, Chanel &channel)
{
	// dans channel il y a une liste d'operator
	// on stock la liste de fd (les operator) dans le vector operators.
	std::vector<int> operators = channel.getOperatorUser();

	// Si le fd est trouvé dans le vecteur operators,
	// std::find retourne un itérateur pointant vers la position où l'élément a été trouvé.
	// pour resumer si il a trouver l'operator il renvoie true sinon false
	bool isOperator = std::find(operators.begin(), operators.end(), fd) != operators.end();
	return (isOperator);
}

/*
Cherche si dans la liste des utilisateurs il y a la cible a kick , si c'est le cas on
le retire de la liste des utilisateurs.
*/

// bool Server::isTargetInChannel(const std::string &targetUser, Chanel &channel)
// {
//     std::vector<int>& users = channel.getUserInChannel(); // Référence non-constante pour pouvoir modifier
//     for (std::vector<int>::iterator userIt = users.begin(); userIt != users.end(); ++userIt)
//     {
//         // std::ostringstream oss;
//         // oss << *userIt; // converti un entier en chaine de caractere
//         std::ostringstream oss;
//         oss << *userIt; // converti un entier en chaine de caractere
// 		std::cout << oss.str();
// 		if (find_nickname_with_fd(*userIt, this->_clients) == targetUser)
//         {
//             channel.removeUser(*userIt); // Supprime l'utilisateur de la liste
//             return true;
//         }
//     }
//     return false;
// }

bool Server::isTargetInChannel(const std::string &targetUser, Chanel &channel, int fd)
{
    std::vector<int>& users = channel.getUserInChannel(); // Référence non-constante pour pouvoir modifier
    for (std::vector<int>::iterator userIt = users.begin(); userIt != users.end(); ++userIt)
    {
        std::ostringstream oss;
        oss << *userIt; // converti un entier en chaine de caractere
		std::cout << oss.str();
		std::string stock = find_nickname_with_fd(*userIt, this->_clients);
		if (find_nickname_with_fd(*userIt, this->_clients) == targetUser)
        {
           // users.erase(userIt); // Supprime l'utilisateur de la liste
			channel.removeUser(*userIt, fd, this->_clients); // Supprime l'utilisateur de la liste
            return true;
        }
    }
    return false;
}

/*
Cette fonction informe tous les utilisateurs d'un canal qu'un utilisateur a été expulsé.
*/

void Server::notifyKick(Chanel &channel, const std::string &sender, const std::string &targetUser, const std::string &reason)
{
	// Creation d'un message sous forme de chaîne .
    std::string kickMessage = ":" + sender + " KICK " + channel.getName() + " " + targetUser + " :" + reason + "\r\n";

	// -1 pour envoyer le message a tout le monde sauf a celui qui viens de se faire kick
	channel.sendMessageToChanel(-1, kickMessage);
}


void Server::handleKick(int fd, Message &msg, std::vector<Chanel> &_chanel)
{
	std::string channel;
	std::string targetUser;

	if (!validateKickArgs(fd, msg, channel, targetUser))
        return;
	for (std::vector<Chanel>::iterator i = _chanel.begin(); i != _chanel.end(); i++)
	{
		if ((*i).getName() == channel)
		{
			if (!isSenderInChannel(fd, *i))
			{
				std::string response = ERR_NOTONCHANNEL(std::string("Server"), channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}
			if (!isSenderOperator(fd, *i))
			{
    			std::string response = ERR_CHANOPRIVSNEEDED(std::string("Server"), channel);
   				send(fd, response.c_str(), response.size(), 0);
    			return;
			}
			if (!isTargetInChannel(targetUser, *i, fd))
			{
				std::string response = ERR_USERNOTINCHANNEL(std::string("Server"), targetUser, channel);
				send(fd, response.c_str(), response.size(), 0);
				return ;
			}
			std::ostringstream oss;

			oss << fd;
			std::string fdStr = oss.str();
			notifyKick(*i, fdStr, targetUser, "Kicked by operator");
			return ;
		}
	}
	// Si aucun canal correspondant n'est trouvé
	std::string response = ERR_NOSUCHCHANNEL(channel);
	send(fd, response.c_str(), response.size(), 0);
}