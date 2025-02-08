#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include "../include/Message.hpp"
#include <algorithm>


bool Server::authenticatedClients(int fd,  std::string &buffer)
{
	std::string	pass;
	std::string newNick;
	std::string userArguments;
	int	i;

	i  = 0;
	if (_authenticatedClients[fd] == false) 
	{
		pass = get_next_argument(buffer.c_str(), i);
		if (strcmp(pass.c_str(), "PASS") == 0) 
		{
			pass = get_next_argument(buffer.c_str(), i);
			if (pass == _password) 
			{
				_authenticatedClients[fd] = true;
				send(fd, "OK :Password accepted\n", 23, 0);
			}
			else
				send(fd, "ERROR :Invalid password\n", 25, 0);
		}
		else
			send(fd, "ERROR :You must authenticate first using PASS\r\n", 47, 0);
		return (false);
	}
	else if (find_nickname_with_fd(fd, this->_clients).empty())
	{
		if (strncmp(buffer.data(), "NICK ", 5) == 0)
		{
			newNick = get_next_argument(buffer.c_str(), i);
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
		return;
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
	return;
}
