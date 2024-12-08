#ifndef CHANEL_HPP
# define CHANEL_HPP

#include <iostream>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/poll.h>
#include <iostream>
#include <string.h>
#include "Client.hpp"
#include "Server.hpp" //??

class Message;
class Chanel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string _mode;

		// j'ai modifier le nom de cette variable
		std::vector<int> _userInChannel;
		std::vector<int> _operator;

	public:
		Chanel();
		~Chanel();

		void	addUser(int newUser, bool isOperator);
		void	removeUser(int newUser);
		void	sendMessageToChanel(int userSender, std::string &msg);

		std::string	getName();
		void	setName(std::string chanName);
		std::vector<int> getUserInChannel();
		const std::vector<int>& getOperators() const;
};

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel);

#endif