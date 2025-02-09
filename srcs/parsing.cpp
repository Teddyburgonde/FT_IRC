#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include "../include/Message.hpp"
#include <algorithm>

bool Server::authenticatedClients(int fd,  std::string &buffer)
{
	std::string	cmd;
	std::string	newNick;
	std::string	userArguments;
	int			i;

	i  = 0;
	cmd = get_next_argument(buffer.c_str(), i);
	if (_authenticatedClients[fd] == false) 
	{
		if (strcmp(cmd.c_str(), "PASS") == 0) 
		{
			cmd = get_next_argument(buffer.c_str(), i);
			if (cmd == _password) 
			{
				_authenticatedClients[fd] = true;
				betterSend("OK :Password accepted\n", fd);
			}
			else
				betterSend("ERROR :Invalid password\n", fd);
		}
		else
			betterSend("ERROR :You must authenticate first using PASS\r\n", fd);
		return (false);
	}
	else if (find_nickname_with_fd(fd, this->_clients).empty())
	{
		if (strncmp(buffer.data(), "NICK ", 5) == 0)
		{
			newNick = get_next_argument(buffer.c_str(), i);
			handleNick(fd, newNick);
		}
		else
			betterSend(ERR_NOTREGISTERED(), fd);
		return (false);
	}
	else if (find_username_with_fd(fd, this->_clients).empty())
	{
		if (strncmp(buffer.data(), "USER ", 5) == 0)
		{
			userArguments = get_next_argument(buffer.c_str(), i);
			handleUser(fd, userArguments);
		}
		else
			betterSend(ERR_NOTREGISTERED(), fd);
		return (false);
	}
	return (true);
}

void Server::analyzeData(int fd,  std::string &buffer)
{
	if (authenticatedClients(fd, buffer) == false)
		return;
	Message msg;

	std::vector<std::string> stringBuffer;
	stringBuffer.push_back(std::string(buffer.begin(), buffer.end()));
	parse_buffer(stringBuffer, msg);
	if (msg.getCommand().empty())
		return;
	else if (strncmp(buffer.data(), "TOPIC ", 6) == 0)
		handleTopic(fd, msg, _channel);
	else if (strncmp(buffer.data(), "PRIVMSG ", 8) == 0)
		handlePrivMsg(fd, msg, this->_channel);
	else if (msg.getCommand() == "KICK")
        handleKick(fd, msg, this->_channel);
	else if (!strncmp(buffer.data(), "JOIN ", 5))
		handleJoin(fd, msg, this->_channel, this->_clients);
	else if (!strncmp((msg.getCommand()).c_str(), "INVITE", msg.getCommand().size()))
		inviteCommand(fd, msg, this->_channel, this->_clients);
	else if (!strncmp((msg.getCommand()).c_str(), "MODE", msg.getCommand().size())) 
		modeCommand(fd, msg, this->_channel, _clients);
}

//!POURQUOI UN VECTEUR DE STRING ???
//!Ajout, l'ancienne version est ci dessous. demander a teddy ce quil en pense
void parse_buffer(std::vector <std::string> &buffer, Message& msg)
{
	int	index;

	index = 0;
	msg.setCommand(get_next_argument((*buffer.begin()).c_str(), index)); //!a changer, pourquoi vecteur ..?
	while ((*buffer.begin()).c_str()[index] && (*buffer.begin()).c_str()[index] == ' ')
		index++;
	msg.setArgument(std::string((*buffer.begin()).c_str() + index));
}
// void parse_buffer(std::vector <std::string> &buffer, Message& msg)
// {
// 	if (buffer.empty())
//         throw std::runtime_error("Buffer is empty");
// 	std::string firstElement = buffer.front();
// 	if (firstElement[0] == ' ')
// 	{
// 		std::cout <<  "The command must not be preceded by a space." << std::endl;
// 		return;
// 	}
// 	size_t spacePos = firstElement.find(' ');
// 	if (spacePos != std::string::npos)
// 	{
//     	std::string line = firstElement.substr(0, spacePos);
// 		msg.setCommand(line);
// 		std::string argument = firstElement.substr(spacePos + 1);
//         msg.setArgument(argument);
// 	}
// 	else
// 	{
// 		msg.setCommand(firstElement);
// 		msg.setArgument("");
// 	}
// 	return;
// }

