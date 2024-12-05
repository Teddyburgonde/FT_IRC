#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>


void	parsing(int fd, char buffer[1024], std::vector<Chanel> &_chanel) // a remane, contiendra tout les strncmp
{
	//Les espaces après la commande cherché ?? Interressant tant que pas de parsing pour etre sur que la commande est pas JOINOITURE par exemple
	// /join #general
	//std::cout << buffer << std::endl;
	if (!strncmp(buffer, "JOIN ", 5)) //si c'est join la commande, a changer grace au futur parsing ?
	{
		std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, buffer, _chanel);
	}
	if (!strncmp(buffer, "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		//besoins du parsing
		std::string msg = std::string(buffer + 13); //ici je recup le msg apres le SEND #general donc a changer
		//dans l'idee faudrais juste envoye le truc après le # grace au parsing ? Au lieu de channel[0], je retrouve le nom du bon salon.
		_chanel[0].sendMessageToChanel(fd, msg); //chanel[0] == que le premier salon. Faut coder le fais d'envoyé dans le salon ou il est le client
		std::cout << "send a message to general" << std::endl;
	}
}

void	handleJoin(int fd, char buffer[1024], std::vector<Chanel> &_chanel)
{
	int i = 0;
	int f = 0;
	std::vector<Chanel>::iterator it = _chanel.begin();
	std::string chanName;

	while (buffer[i] != '#') //Grace au parsing, le code ici sera beaucoup moins long
		i++;
	f = i;
	while (buffer[f] && buffer[f] != ',') //pour le parsing
		f++;
	//stocket dans une variable le bon truc de buffer
	chanName = std::string(buffer + i, buffer + f);
	it = _chanel.begin();
	while (it != _chanel.end()) //on cherche si le chanel existe deja:
	{
		if ((*it).getName() == chanName) //si le nom du chan est le meme qu'un chan existant
			break; //on break pour garder l'iterator sur le bon chan
		it++;
	}
	std::cout << "HERE 1" << std::endl;
	if (it == _chanel.end() && it == _chanel.begin()) //si chan existe pas
	{
		std::cout << "create chan " << chanName << std::endl;
		Chanel newChan;
		newChan.setName(chanName);
		newChan.addUser(fd, true); //rejoin en ope psk c'est lui qui l'a cree
		_chanel.push_back(newChan);
	}
	else
	{
		(*it).addUser(fd, false);
		std::cout << "User: " << fd << "added to " << (*it).getName() << std::endl; //debug aussi, a retirer
	}
	std::cout << "HERE 2" << std::endl;
}


void Server::analyzeData(int fd,  const std::vector<char> &buffer)
{
	Message msg;

	std::vector<std::string> stringBuffer;
	stringBuffer.push_back(std::string(buffer.begin(), buffer.end()));
	std::cout << "je suis dans analyzeData" << std::endl;
	msg = parse_buffer(stringBuffer);
	//msg = parse_buffer(buffer);
	//parsing(fd, (char*)buffer, this->_chanel); //ici sera tout les strncmp

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
}

Message parse_buffer(std::vector <std::string> &buffer)
{

	Message msg;
	
	// Est t'il vide ?
	if (buffer.empty())
        throw std::runtime_error("Buffer is empty");
	
	
	std::string firstElement = buffer.front();
	if (firstElement[0] == ' ')
		throw(std::runtime_error("The command must not be preceded by a space."));

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

    return msg;
}
