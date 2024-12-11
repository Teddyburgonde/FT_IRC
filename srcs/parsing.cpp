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


Pseudo code : 

1. Savoir si le channel existe
Cette étape doit toujours être la première. Si le channel n'existe pas, il est inutile d'effectuer les autres vérifications.

2. Vérifier si l'utilisateur est dans le channel
Cette étape est essentielle pour éviter que des utilisateurs non membres accèdent ou modifient le sujet.

3. Vérifier si _topic contient une valeur
Une fois les prérequis remplis (le channel existe, et l'utilisateur est membre), tu peux vérifier si _topic est défini ou non.

4. Si _topic est vide, définir une valeur
Logique correcte ici. Tu peux ajouter une vérification pour les permissions (seuls les opérateurs peuvent définir un sujet).

5. Si _topic est déjà défini, vérifier les droits pour le modifier
Au lieu d'interdire complètement la modification, vérifie si l'utilisateur a les droits (souvent réservé aux opérateurs). Si ce n’est pas le comportement voulu, reste sur l'idée que le sujet est immuable après sa définition.

6. Afficher la valeur de _topic si elle est demandée
Finalement, affiche le sujet si tout est conforme. Si _topic est vide, renvoie RPL_NOTOPIC.

*/

void Server::handleTopic(int fd, const Message &msg, std::vector<Chanel> &_chanel)
{
	// 1. Verification si le channel existe 
	size_t spacePos = msg.getArgument().find(' ');
	std::string channel; 
	channel = msg.getArgument().substr(0, spacePos);
	if (channel.empty())
	{
		std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "TOPIC");
        // Envoyer la réponse d'erreur au client qui a envoyé la commande.
        send(fd, response.c_str(), response.size(), 0);
         // false le format n'est pas correct.
        return ;
	}
	bool channelFound = false;
    for (std::vector<Chanel>::iterator it = _chanel.begin(); it != _chanel.end(); ++it)
	{
        if (it->getName() == channel) 
		{
            // Le canal existe, passer à l'étape suivante
			channelFound = true;
            break ;
        }
    }
	if (!channelFound) 
	{
		// Si le canal n'a pas été trouvé
    	std::string response = ERR_NOSUCHCHANNEL(channel);
    	send(fd, response.c_str(), response.size(), 0);
    	return; // On arrête la fonction si le canal n'existe pas.
	}
	// 2. Vérifier si le canal existe dans _chanel
	if (!isSenderInChannel(fd, *it)) 
	{
    	std::string response = ERR_NOTONCHANNEL(std::string("Server"), channel);
    	send(fd, response.c_str(), response.size(), 0);
    	return;
	}

	// 3. Vérifier si _topic contient une valeur
	// - TOPIC <channel> :<new_topic>

    // Si aucun canal correspondant n'est trouvé
    std::string response = ERR_NOSUCHCHANNEL(channel);
    send(fd, response.c_str(), response.size(), 0);

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
