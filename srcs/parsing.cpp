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


Test de la fonction 
1. Definir un sujet ✅
2.Cas où aucun sujet n'est fourni ✅
3. Cas où un autre utilisateur demande le sujet ✅ 
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


void Server::analyzeData(int fd,  const std::string &buffer)
{
	Message msg;

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
	if (strncmp(buffer.data(), "TOPIC ", 6) == 0)
	{
		std::string topicArguments = std::string(buffer.begin() + 6, buffer.end());
    	topicArguments.erase(std::remove(topicArguments.begin(), topicArguments.end(), '\r'), topicArguments.end());
    	topicArguments.erase(std::remove(topicArguments.begin(), topicArguments.end(), '\n'), topicArguments.end());
    	Message msg;
		msg.setCommand("TOPIC");
		msg.setArgument(topicArguments);
		handleTopic(fd, msg, _chanel);
	}
	if (strncmp(buffer.data(), "PRIVMSG ", 8) == 0)
		handlePrivMsg(fd, std::string(buffer));
	if (msg.getCommand() == "KICK") 
	{  // Ajout de la commande KICK
        handleKick(fd, msg, this->_chanel);
    }
	if (!strncmp(buffer.data(), "JOIN ", 5))
	{
		//std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, msg, this->_chanel, this->_clients);
	}
	if (!strncmp(buffer.data(), "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		//besoins du parsing
		std::string msg = std::string(buffer.data() + 13); //ici je recup le msg apres le SEND #general donc a changer
		//dans l'idee faudrais juste envoye le truc après le # grace au parsing ? Au lieu de channel[0], je retrouve le nom du bon salon.
		this->_chanel[0].sendMessageToChanel(fd, msg); //chanel[0] == que le premier salon. Faut coder le fais d'envoyé dans le salon ou il est le client
		std::cout << "send a message to general" << std::endl;
	}
	if (!strncmp((msg.getCommand()).c_str(), "INVITE", msg.getCommand().size()))
	{
		//std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, msg, this->_chanel, this->_clients);
	}
	if (!strncmp((msg.getCommand()).c_str(), "MODE", msg.getCommand().size())) 
	{
		//std::cout << "made join " << std::endl; //debug, a retirer
		modeCommand(fd, msg, this->_chanel, _clients);
		std::cout << "MODE MADE" << std::endl;
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
