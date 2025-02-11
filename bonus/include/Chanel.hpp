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
#include <sstream>
#include <algorithm>

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
		bool	_mode_l;

		int		_nb_user_max;
		int		_nb_user_in;

	public:
		Chanel();
		~Chanel();

		std::string	getName();
		void		setName(std::string chanName);
		std::string	getPassword();
		void		setPassword(std::string passwordStr);


		void	addUser(int newUser, bool isOperator);
		std::vector<int>& getUserInChannel();
		void	removeUser(int newUser, int fd, std::vector<Client> &_clients);
		std::vector<int>&	getOperatorUser();


		void	sendMessageToChanel(int userSender, std::string &msg);

		void				setInvitedUser(int fd);
		std::vector<int>	getInvitedUser();

		void	setModeI(bool set);
		void	setModeT(bool set);
		void	setModeK(bool set);
		void	setModeL(bool set);

		bool	getModeI();
		bool	getModeT();
		bool	getModeK();
		bool	getModeL();

		void	set_nb_user_max(int nb);
		void	set_nb_user_in(bool add_remove);

		int		get_nb_user_max();
		int		get_nb_user_in();

		std::string getTopic() const;
    	void setTopic(const std::string&);

		bool hasClient(int fd); //test 21/12/2024
		void addClient(int fd);

		void botMessageToChanel(int userSender, std::string &msg);
};

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients);
void	inviteCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients); //cmd INVITE
void	modeCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients);

int		is_user_in_chan(int fd, std::vector<int> userInChannel); //sert a savoir si un user est dans un channel
std::vector<Chanel>::iterator find_channel_with_name(std::string &channelName, std::vector<Chanel> &_chanel); //trouver un channel a partir de son nom

#endif