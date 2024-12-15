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
#include <sstream> 

class Message;
class Chanel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string _password;

		std::vector<int> _userInChannel;
		std::vector<int> _operator;
		std::vector<int> _invitedUser;

		bool	_mode_i;
		bool	_mode_t;
		bool	_mode_k;
		bool	_mode_o;
		bool	_mode_l;

		int		_nb_user_max;
		int		_nb_user_in;

	public:
		Chanel();
		~Chanel();

		std::string	getName(); //getter de _name
		void		setName(std::string chanName); //setter de _name
		std::string	getPassword();
		void		setPassword(std::string passwordStr); //setter de _name


		void	addUser(int newUser, bool isOperator); //setter de _userInChannel, isOperator set le user aussi dans _operator si 'true'
		std::vector<int>& getUserInChannel(); //getter de _userInChannel
		void	removeUser(int newUser); //enlever un user de _userInChannel
		std::vector<int>&	getOperatorUser();//getter de _operator


		void	sendMessageToChanel(int userSender, std::string &msg);

		void				setInvitedUser(int fd);//setter de _invitedUser
		std::vector<int>	getInvitedUser();//getter de _invitedUser

		void	setModeI(bool set);
		void	setModeT(bool set);
		void	setModeK(bool set);
		void	setModeO(bool set);
		void	setModeL(bool set);

		bool	getModeI();
		bool	getModeT();
		bool	getModeK();
		bool	getModeO();
		bool	getModeL();

		void	set_nb_user_max(int nb);
		void	set_nb_user_in(bool add_remove);

		int		get_nb_user_max();
		int		get_nb_user_in();

		std::string getTopic() const;
    	void setTopic(const std::string&); 

};

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients);
void	inviteCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients); //cmd INVITE
void	modeCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients);

int		is_user_in_chan(int fd, std::vector<int> userInChannel); //sert a savoir si un user est dans un channel
std::vector<Chanel>::iterator find_channel_with_name(std::string &channelName, std::vector<Chanel> &_chanel); //trouver un channel a partir de son nom

#endif