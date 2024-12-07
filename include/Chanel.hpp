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

		std::vector<int> _user;
		std::vector<int> _operator;
		std::vector<int> _invitedUser;

		bool	_mode_i;
		bool	_mode_t;
		bool	_mode_k;
		bool	_mode_o;
		bool	_mode_l;

	public:
		Chanel();
		~Chanel();

		void	addUser(int newUser, bool isOperator); //setter de _user, isOperator set le user aussi dans _operator si 'true'
		std::vector<int> getUserInChannel(); //getter de _user
		void	removeUser(int newUser); //enlever un user de _user

		std::string	getName(); //getter de _name
		void		setName(std::string chanName); //setter de _name

		void	sendMessageToChanel(int userSender, std::string &msg);

		void				setInvitedUser(int fd);//setter de _invitedUser
		std::vector<int>	getInvitedUser();//getter de _invitedUser

};

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel); //cmd JOIN
int		is_user_in_chan(int fd, std::vector<int> userInChannel); //sert a savoir si un user est dans un channel
void	inviteCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients); //cmd INVITE


#endif