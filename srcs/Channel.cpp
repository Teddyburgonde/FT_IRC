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

// Récupérer le sujet actuel du canal
std::string Channel::getTopic() const
{
	return (_topic);
}

// Définir ou modifier le sujet du canal
void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

std::string	Channel::getPassword()
{
	return (this->_password);
}

void	Channel::setPassword(std::string passwordStr) //setter de _name
{
	this->_password = passwordStr;
}

void	Channel::addUser(int newUser, bool isOperator)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _userInChannel
	std::vector<int>::iterator op_it;

	us_it = std::find(this->_userInChannel.begin(), this->_userInChannel.end(), newUser);
	if (us_it == _userInChannel.end())
	{
		this->_userInChannel.push_back(newUser);
		this->set_nb_user_in(true);//on ajoute 1 au nombre de personne dans le channel
	}
	op_it = std::find(this->_operator.begin(), this->_operator.end(), newUser);
	if (isOperator) // il doit etre op
	{
		if (op_it == _operator.end())
			this->_operator.push_back(newUser);
	}
	else //le user ne dois pas etre op
	{
		if (op_it != _operator.end())
			this->_operator.erase(op_it);
	}
}

void	Channel::removeUser(int newUser, int fd, std::vector<Client> &_clients)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _userInChannel
	std::vector<int>::iterator op_it;
	std::vector<int>::iterator iv_it; //invite list
	bool	user_found = false;

	us_it = std::find(this->_userInChannel.begin(), this->_userInChannel.end(), newUser);
	if (us_it != _userInChannel.end())
	{
		this->_userInChannel.erase(us_it);
		user_found = true;
	}
	op_it = std::find(this->_operator.begin(), this->_operator.end(), newUser);
	if (op_it != this->_operator.end())
	{
		this->_operator.erase(op_it);
	}
	if (!this->_invitedUser.empty())
	{
		iv_it = std::find(this->_invitedUser.begin(), this->_invitedUser.end(), newUser);
		if (iv_it != this->_invitedUser.end())
			this->_invitedUser.erase(iv_it);
	}
	if (user_found == false)
	{
		betterSend(ERR_NOTONCHANNEL(find_nickname_with_fd(newUser, _clients), this->_name), fd);
		return;
	}
	this->set_nb_user_in(false);
}

std::vector<int>&	Channel::getOperatorUser()//getter de _operator
{
	return (this->_operator);
}

void	Channel::sendMessageToChannel(int userSender, std::string &msg)
{
	std::vector<int>::iterator it =  this->_userInChannel.begin(); //je set l'iterateur au debut du vecteur _userInChannel qui contient tout les user du channel

	while (it != this->_userInChannel.end()) //tant que ont a pas envoyé à touts les user du chan
	{
		if (*it != userSender)//si ce n'est pas l'envoyeur (on veux pas envoyé le message a sois meme)
			send(*it, msg.c_str()/*faut env des char, obligé de c_str*/, msg.size(), 0); //on envoie le message au socket des users.
		//send est comme write : on envoie dans quel fd on veux ecrire, on envoye le message et la taille du message comme write
		//sauf que send permet de mettre des flags mais c'est a pousser.
		it++; //on passe au user suivant
	}
	//if (*it != userSender)//si ce n'est pas l'envoyeur (on veux pas envoyé le message a sois meme)
	//	send(*it, msg.c_str(), msg.size(), 0);
}


std::vector<int>&	Channel::getUserInChannel()
{
	return (this->_userInChannel);
}

void	Channel::setInvitedUser(int fd)//setter de _invitedUser
{
	this->_invitedUser.push_back(fd);
}

std::vector<int>	Channel::getInvitedUser()//getter de _invitedUser
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

//if add_remove = true, add somewone. If = false, remove somewone.
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