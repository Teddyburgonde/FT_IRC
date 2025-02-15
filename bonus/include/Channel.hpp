#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/poll.h>
#include "Client.hpp"
#include "Server.hpp"

class Message;
class Channel
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
		Channel();
		~Channel();

		std::string	getName();
		void		setName(std::string chanName);
		std::string	getPassword();
		void		setPassword(std::string passwordStr);


		void	addUser(int newUser, bool isOperator);
		std::vector<int>& getUserInChannel();
		void	removeUser(int userRemove, int fd, std::vector<Client> &_clients);
		std::vector<int>&	getOperatorUser();


		void	sendMessageToChannel(int userSender, std::string &msg);

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
		void	setTopic(const std::string&);
};

#endif