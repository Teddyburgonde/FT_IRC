/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 19:05:34 by gmersch           #+#    #+#             */
/*   Updated: 2025/02/10 14:02:12 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel() : _mode_i(false), _mode_t(false), _mode_k(false), _mode_l(false), _nb_user_in(0)
{

}

Channel::~Channel()
{

}

std::string	Channel::getName()
{
	return (this->_name);
}

void		Channel::setName(std::string chanName)
{
	this->_name = chanName;
}

std::string Channel::getTopic() const
{
	return (_topic);
}

void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

std::string	Channel::getPassword()
{
	return (this->_password);
}

void	Channel::setPassword(std::string passwordStr)
{
	this->_password = passwordStr;
}

void	Channel::addUser(int newUser, bool isOperator)
{
	std::vector<int>::iterator	us_it;
	std::vector<int>::iterator	op_it;

	us_it = std::find(this->_userInChannel.begin(), this->_userInChannel.end(), newUser);
	if (us_it == _userInChannel.end())
	{
		this->_userInChannel.push_back(newUser);
		this->set_nb_user_in(true);
	}
	op_it = std::find(this->_operator.begin(), this->_operator.end(), newUser);
	if (isOperator)
	{
		if (op_it == _operator.end())
			this->_operator.push_back(newUser);
	}
	else
	{
		if (op_it != _operator.end())
			this->_operator.erase(op_it);
	}
}

void	Channel::removeUser(int userRemove, int fd, std::vector<Client> &_clients)
{
	Server	server;
	std::vector<int>::iterator	us_it;
	std::vector<int>::iterator	op_it;
	std::vector<int>::iterator	iv_it;
	bool						user_found = false;

	us_it = std::find(this->_userInChannel.begin(), this->_userInChannel.end(), userRemove);
	if (us_it != _userInChannel.end())
	{
		this->_userInChannel.erase(us_it);
		user_found = true;
	}
	op_it = std::find(this->_operator.begin(), this->_operator.end(), userRemove);
	if (op_it != this->_operator.end())
		this->_operator.erase(op_it);
	if (!this->_invitedUser.empty())
	{
		iv_it = std::find(this->_invitedUser.begin(), this->_invitedUser.end(), userRemove);
		if (iv_it != this->_invitedUser.end())
			this->_invitedUser.erase(iv_it);
	}
	if (user_found == false)
	{
		server.betterSend(ERR_NOTONCHANNEL(server.find_nickname_with_fd(userRemove, _clients), this->_name), fd);
		return;
	}
	this->set_nb_user_in(false);
}

std::vector<int>&	Channel::getOperatorUser()
{
	return (this->_operator);
}

void	Channel::sendMessageToChannel(int userSender, std::string &msg)
{
	std::vector<int>::iterator it =  this->_userInChannel.begin();

	while (it != this->_userInChannel.end())
	{
		if (*it != userSender)
			send(*it, msg.c_str(), msg.size(), 0);
		it++; 
	}
}


std::vector<int>&	Channel::getUserInChannel()
{
	return (this->_userInChannel);
}

void	Channel::setInvitedUser(int fd)
{
	this->_invitedUser.push_back(fd);
}

std::vector<int>	Channel::getInvitedUser()
{
	return (this->_invitedUser);
}

void	Channel::setModeI(bool set)
{
	this->_mode_i = set;
	if (!set && !this->_invitedUser.empty())
		this->_invitedUser.clear();
}

void	Channel::setModeT(bool set)
{
	this->_mode_t = set;
}

void	Channel::setModeK(bool set)
{
	this->_mode_k = set;
}

void	Channel::setModeL(bool set)
{
	this->_mode_l = set;
}

bool	Channel::getModeI()
{
	return (this->_mode_i);
}

bool	Channel::getModeT()
{
	return (this->_mode_t);
}

bool	Channel::getModeK()
{
	return (this->_mode_k);
}

bool	Channel::getModeL()
{
	return (this->_mode_l);
}

void	Channel::set_nb_user_max(int nb)
{
	this->_nb_user_max = nb;
}

int		Channel::get_nb_user_max()
{
	return (this->_nb_user_max);
}

//if add_remove = true, add someone. If = false, remove someone.
void	Channel::set_nb_user_in(bool add_remove)
{
	if (add_remove)
		this->_nb_user_in++;
	else
		this->_nb_user_in--;
}

int	Channel::get_nb_user_in()
{
	return (this->_nb_user_in);
}