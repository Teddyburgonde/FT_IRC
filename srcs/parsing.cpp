#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>

void Server::analyzeData(int fd,  const std::string &buffer)
{
	Message msg;

	std::vector<std::string> stringBuffer;
	stringBuffer.push_back(std::string(buffer.begin(), buffer.end()));
	parse_buffer(stringBuffer, msg);
	std::cout << "Argument extrait2 : " << msg.getArgument() << std::endl;
	if (msg.getCommand().empty())
	{
		return ;
	}
	std::string oldNick;
	if (strncmp(buffer.data(), "NICK ", 5) == 0)
	{
		oldNick = std::string(buffer.begin() + 5, buffer.end());
		// remove met les caracteres a supprimer a la fin
		// erase coupe la chaine juste avant les caracteres indesirables.
		oldNick.erase(std::remove(oldNick.begin(), oldNick.end(), '\r'), oldNick.end());
		oldNick.erase(std::remove(oldNick.begin(), oldNick.end(), '\n'), oldNick.end());
		if (oldNick.empty())
		{
			std::string response = ERR_NONICKNAMEGIVEN(std::string("Server"), std::string(""));
			// send envoie la reponse au client associé au fd
			send(fd, response.c_str(), response.size(), 0);
			return ;
		}
		for(unsigned long i = 0; i < _clients.size(); i++)
		{
			// si le pseudo est déjà  pris
			if (oldNick == _clients[i].getNickname())
			{
				std::string response = ERR_NICKNAMEINUSE(std::string("Server"), oldNick);
				send(fd, response.c_str(), response.size(), 0);
				return ;
			}
			if (_clients[i].getFd() == fd)
			{
				_clients[i].setNickname(oldNick);
				 std::cout << "Welcome in the server" << std::endl;
				std::cout << "Client FD: " << _clients[i].getFd()
                  << " Nickname: " << _clients[i].getNickname() << std::endl;
				std::string response = RPL_WELCOME(oldNick);
				send(fd, response.c_str(), response.size(), 0);
				break ;
			}
		}
    }
	std::string start;
	size_t spacePos;
	if (strncmp(buffer.data(), "PRIVMSG ", 8) == 0)
	{

		std::string command(buffer.begin(), buffer.end());

		// Trouver le destinataire
		spacePos = command.find(' ', 8);
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
	if (!strncmp(buffer.data(), "JOIN ", 5)) //si c'est join la commande, a changer grace au futur parsing ?
	{
		std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, msg, _chanel);
	}
	if (!strncmp(buffer.data(), "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		//besoins du parsing
		std::string msg = std::string(buffer.data() + 13); //ici je recup le msg apres le SEND #general donc a changer
		//dans l'idee faudrais juste envoye le truc après le # grace au parsing ? Au lieu de channel[0], je retrouve le nom du bon salon.
		_chanel[0].sendMessageToChanel(fd, msg); //chanel[0] == que le premier salon. Faut coder le fais d'envoyé dans le salon ou il est le client
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

	std::cout << "Commande extraite : " << msg.getCommand() << std::endl;
    std::cout << "Argument extrait : " << msg.getArgument() << std::endl;

    return ;
}
