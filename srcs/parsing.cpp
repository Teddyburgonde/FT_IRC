#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>

void Server::handleNick(int fd, const std::string& newNick) 
{
    if (newNick.empty()) 
	{
        std::string response = ERR_NONICKNAMEGIVEN(std::string("Server"), "");
        send(fd, response.c_str(), response.size(), 0);
        return;
    }

    for (size_t i = 0; i < _clients.size(); ++i) 
	{
        if (_clients[i].getNickname() == newNick) {
            std::string response = ERR_NICKNAMEINUSE(std::string("Server"), newNick);
            send(fd, response.c_str(), response.size(), 0);
            return;
        }

        if (_clients[i].getFd() == fd) {
            _clients[i].setNickname(newNick);
            std::string response = RPL_WELCOME(newNick);
            send(fd, response.c_str(), response.size(), 0);
            std::cout << "Client FD: " << fd << " Nickname: " << newNick << std::endl;
            return;
        }
    }
}

void Server::handlePrivMsg(int fd, const std::string& command)
{
	// Trouver le destinataire
		size_t spacePos = command.find(' ', 8);
		if (spacePos == std::string::npos)
		{
			std::string response = ERR_NORECIPIENT(std::string ("Server"), "");
        	send(fd, response.c_str(), response.size(), 0);
        	return ;
		}
		std::string recipient = command.substr(8, spacePos - 8); // Extrait le destinataire
    	if (recipient.empty())
		{
        	std::string response = ERR_NORECIPIENT(std::string("Server"), "");
        	send(fd, response.c_str(), response.size(), 0);
        	return;
    	}
		 // Trouver le message après ":"
    	size_t colonPos = command.find(':', spacePos);
    	if (colonPos == std::string::npos) 
		{
        	std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
        	send(fd, response.c_str(), response.size(), 0);
        	return;
    	}
    	std::string message = command.substr(colonPos + 1); // Tout après ":" est le message
    	if (message.empty()) 
		{
        	std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
        	send(fd, response.c_str(), response.size(), 0);
        	return;
    	}
    	// Envoyer le message au destinataire
    	bool recipientFound = false;
    	for (size_t i = 0; i < _clients.size(); i++) 
		{
        	if (_clients[i].getNickname() == recipient) 
			{
            	send(_clients[i].getFd(), message.c_str(), message.size(), 0);
            	recipientFound = true;
            	break;
        	}
    	}
    	if (!recipientFound) 
		{
        	std::string response = ERR_NOSUCHNICK(std::string("Server"), recipient);
        	send(fd, response.c_str(), response.size(), 0);
   		}
}


/*
wtf 
bool Server::isUserInChannel(int fd, const Chanel &channel)
{
    for (std::vector<int>::const_iterator userIt = channel.getUserInChannel().begin();
         userIt != channel.getUserInChannel().end(); ++userIt)
    {
        if (*userIt == fd)
            return true;
    }
    return false;
}

bool Server::isTargetInChannel(const std::string &targetUser, const Chanel &channel)
{
    for (std::vector<int>::const_iterator userIt = channel.getUserInChannel().begin();
         userIt != channel.getUserInChannel().end(); ++userIt)
    {
        if (std::to_string(*userIt) == targetUser) // Si targetUser est un fd converti
            return true;
    }
    return false;
}



KICK <channel> <target> [<comment>]
1. Ajouter une vérification si l’expéditeur est dans le canal.
2. Vérifier si l’expéditeur a les privilèges nécessaires (admin).
3. Ajouter une vérification si la cible existe avant de chercher dans le canal.
4. Ajouter une gestion de la raison pour l’expulsion.
5. Implémenter une méthode pour retirer la cible du canal et notifier les utilisateurs.

*/

void Server::handleKick(int fd, Message &msg, std::vector<Chanel> &_chanel)
{
	size_t spacePos = msg.getArgument().find(' ');
	if (spacePos == std::string::npos)
	{
		std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	std::string channel = msg.getArgument().substr(0, spacePos);
	if (channel.empty()) 
	{
		std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	std::string targetUser = msg.getArgument().substr(spacePos + 1);
	if (targetUser.empty())
	{
		std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}

	// Parcourir les canaux pour trouver le canal correspondant
	for (std::vector<Chanel>::iterator i = _chanel.begin(); i != _chanel.end(); i++)
	{
		if ((*i).getName() == channel)
		{
			// Vérifier si l'expéditeur est dans le canal
			bool senderInChannel = false;
			for (std::vector<int>::iterator userIt = (*i).getUserInChannel().begin();
				 userIt != (*i).getUserInChannel().end(); ++userIt)
			{
				if (*userIt == fd) // Vérifie si l'expéditeur est dans le canal
				{
					senderInChannel = true;
					break;
				}
			}

			if (!senderInChannel)
			{
				std::string response = ERR_NOTONCHANNEL(std::string("Server"), channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}

			// Vérifier si l'utilisateur cible est dans le canal
			bool targetInChannel = false;
			for (std::vector<int>::iterator userIt = (*i).getUserInChannel().begin();
				 userIt != (*i).getUserInChannel().end(); ++userIt)
			{
				 if (std::to_string(*userIt) == targetUser) 
				{
					targetInChannel = true;
					break;
				}
			}

			if (!targetInChannel)
			{
				std::string response = ERR_USERNOTINCHANNEL(std::string("Server"), targetUser, channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}

			// Les vérifications sont terminées, vous pouvez avancer à l'étape suivante
			return; // Ici, vous pouvez continuer vers la vérification des privilèges
		}
	}

	// Si aucun canal correspondant n'est trouvé
	std::string response = ERR_NOSUCHCHANNEL(channel);
	send(fd, response.c_str(), response.size(), 0);
	return;
}


void Server::analyzeData(int fd,  const std::string &buffer)
{
	Message msg;

	/*ajout*/
	msg.setCommand("KICK");
	msg.setArgument("#general Romain");
	handleKick(fd, msg, this->_chanel);

	std::vector<std::string> stringBuffer;
	stringBuffer.push_back(std::string(buffer.begin(), buffer.end()));
	parse_buffer(stringBuffer, msg);
	if (msg.getCommand().empty())
		return ;
	std::string newNick;
	if (strncmp(buffer.data(), "NICK ", 5) == 0) 
	{
    	std::string newNick = std::string(buffer.begin() + 5, buffer.end());
    	newNick.erase(std::remove(newNick.begin(), newNick.end(), '\r'), newNick.end());
    	newNick.erase(std::remove(newNick.begin(), newNick.end(), '\n'), newNick.end());
    	handleNick(fd, newNick);
	}
	if (strncmp(buffer.data(), "PRIVMSG ", 8) == 0)
		handlePrivMsg(fd, std::string(buffer));
	if (msg.getCommand() == "KICK") 
	{  // Ajout de la commande KICK
        handleKick(fd, msg, this->_chanel);
    }
	if (!strncmp(buffer.data(), "JOIN ", 5)) //si c'est join la commande, a changer grace au futur parsing ?
	{
		//std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, msg, this->_chanel);
	}
	if (!strncmp(buffer.data(), "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		//besoins du parsing
		std::string msg = std::string(buffer.data() + 13); //ici je recup le msg apres le SEND #general donc a changer
		//dans l'idee faudrais juste envoye le truc après le # grace au parsing ? Au lieu de channel[0], je retrouve le nom du bon salon.
		this->_chanel[0].sendMessageToChanel(fd, msg); //chanel[0] == que le premier salon. Faut coder le fais d'envoyé dans le salon ou il est le client
		std::cout << "send a message to general" << std::endl;
	}
}

void parse_buffer(std::vector <std::string> &buffer, Message& msg)
{

	//Message msg;
	
	// Est t'il vide ?
	if (buffer.empty())
        throw std::runtime_error("Buffer is empty");
	
	
	std::string firstElement = buffer.front();
	if (firstElement[0] == ' ')
	{
		std::cout <<  "The command must not be preceded by a space." << std::endl;
		return ;
		//throw(std::runtime_error("The command must not be preceded by a space."));
	}
	size_t spacePos = firstElement.find(' ');

	if (spacePos != std::string::npos) 
	{
    	std::string line = firstElement.substr(0, spacePos);
		msg.setCommand(line);
		std::string argument = firstElement.substr(spacePos + 1);
        msg.setArgument(argument);
	} 
	else 
	{
    	msg.setCommand(firstElement);
        msg.setArgument(""); // Aucun argument
	}

	//std::cout << "Commande extraite : " << msg.getCommand() << std::endl;
    //std::cout << "Argument extrait : " << msg.getArgument() << std::endl;

    return ;
}
