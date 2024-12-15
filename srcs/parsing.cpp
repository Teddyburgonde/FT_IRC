#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>


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
		handlePrivMsg(fd, msg, this->_chanel);
		// handlePrivMsg(fd, std::string(buffer));
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

