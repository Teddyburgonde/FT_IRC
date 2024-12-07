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
        // Vérifier si le nom du canal est vide après extraction
        std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
        send(fd, response.c_str(), response.size(), 0);
        return;
    }
	for (std::vector<Chanel>::iterator i = _chanel.begin() ; i != _chanel.end(); i++)
	{
        if ((*i).getName() == channel)
		{
			msg.getArgument().substr(spacePos + 1);
		}
	}
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
