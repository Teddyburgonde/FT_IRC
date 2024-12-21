#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>

bool Server::authenticatedClients(int fd,  const std::string &buffer)
{
	if (_authenticatedClients.find(fd) == _authenticatedClients.end())
		_authenticatedClients[fd] = false;
	if (!_authenticatedClients[fd]) 
	{
		// Vérifie si la commande est "PASS"
		if (strncmp(buffer.data(), "PASS ", 5) == 0) 
		{
			std::string clientPassword = std::string(buffer.begin() + 5, buffer.end());
			clientPassword.erase(std::remove(clientPassword.begin(), clientPassword.end(), '\r'), clientPassword.end());
			clientPassword.erase(std::remove(clientPassword.begin(), clientPassword.end(), '\n'), clientPassword.end());

			if (clientPassword == _password) 
			{
				_authenticatedClients[fd] = true; // Authentifie le client
				send(fd, "OK :Password accepted\n", 23, 0);
				return true; 
			}
			else
			{
				send(fd, "ERROR :Invalid password\n", 25, 0);
				return false;
			}
		}
		else
		{
			send(fd, "ERROR :You must authenticate first using PASS\r\n", 47, 0);
			return false;
		}
	}
	return true;
}


void Chanel::testsendMessageToChanel(int userSender, std::string &msg)
{
	(void)userSender;
    // Parcourt tous les clients du canal et envoie le message à tout le monde, y compris l'expéditeur
    for (size_t i = 0; i < _userInChannel.size(); i++)
    {
        send(_userInChannel[i], msg.c_str(), msg.size(), 0);  // Envoie le message à chaque client
    }
}

void Server::analyzeData(int fd,  const std::string &buffer)
{
	 if (!authenticatedClients(fd, buffer))
        return;
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
	if (strncmp(buffer.data(), "USER ", 5) == 0)
	{
		std::string userArguments = std::string(buffer.begin() + 5, buffer.end());
		userArguments.erase(std::remove(userArguments.begin(), userArguments.end(), '\r'), userArguments.end());
		userArguments.erase(std::remove(userArguments.begin(), userArguments.end(), '\n'), userArguments.end());
		handleUser(fd, userArguments);
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
		handlePrivMsg(fd, msg, this->_chanel);
	if (msg.getCommand() == "KICK") 
	{  // Ajout de la commande KICK
        handleKick(fd, msg, this->_chanel);
    }
	if (!strncmp(buffer.data(), "JOIN ", 5))
	{
		//std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, msg, this->_chanel, this->_clients);
	}
	if (strstr(buffer.c_str(), "zut") != NULL)
	{
		// Message à envoyer
		std::string reply = "Insult prohibited in this channel 😡\n";

		// Trouver le canal du client
		std::string clientChannel;

		// Pour chaque canal, vérifier si le client est dedans
		for (size_t i = 0; i < _chanel.size(); i++)
		{
			if (_chanel[i].hasClient(fd))  // Vérifie si le client est dans ce canal
			{
				clientChannel = _chanel[i].getName();  // Récupère le nom du canal où le client est
				break;
			}
		}
		// Si un canal a été trouvé
		if (!clientChannel.empty())
		{
			// Trouver le canal dans la liste des canaux
			for (size_t i = 0; i < _chanel.size(); i++)
			{
				if (_chanel[i].getName() == clientChannel)  // Si c'est le bon canal
				{
					// Utiliser sendMessageToChanel pour envoyer à tous les clients du canal, y compris l'expéditeur
					_chanel[i].testsendMessageToChanel(fd, reply);  // Envoie le message à tous les clients du canal
					break;
				}
			}
		}
    	std::cout << "detected insult ❗" << std::endl;
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
    return ;
}
