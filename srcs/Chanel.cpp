#include "../include/Chanel.hpp"

Chanel::Chanel() : _mode_i(false), _mode_t(false), _mode_k(false), _mode_o(false), _mode_l(false), _nb_user_in(0)
{

}

Chanel::~Chanel()
{

}

std::string	Chanel::getName()
{
	return (this->_name);
}

void		Chanel::setName(std::string chanName)
{
	this->_name = chanName;
}

// Récupérer le sujet actuel du canal
std::string Chanel::getTopic() const
{
    return _topic;
}

// Définir ou modifier le sujet du canal
void Chanel::setTopic(const std::string& topic)
{
    _topic = topic;
}

std::string	Chanel::getPassword()
{
	return (this->_password);
}

void	Chanel::setPassword(std::string passwordStr) //setter de _name
{
	this->_password = passwordStr;
}

void	Chanel::addUser(int newUser, bool isOperator)
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

void	Chanel::removeUser(int newUser, int fd, std::vector<Client> &_clients)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _userInChannel
	std::vector<int>::iterator op_it;
	bool	user_found = false;

	us_it = std::find(this->_userInChannel.begin(), this->_userInChannel.end(), newUser);
	if (us_it != _userInChannel.end())
	{
		this->_userInChannel.erase(us_it);
		user_found = true;
	}
	op_it = std::find(this->_operator.begin(), this->_operator.end(), newUser);
	if (op_it != _operator.end())
	{
		this->_operator.erase(op_it);
	}
	if (user_found == false)
	{
		send_error(ERR_NOTONCHANNEL(find_nickname_with_fd(newUser, _clients), this->_name), fd);
		return;
	}
	this->set_nb_user_in(false);
}

std::vector<int>&	Chanel::getOperatorUser()//getter de _operator
{
	return (this->_operator);
}

void	Chanel::sendMessageToChanel(int userSender, std::string &msg)
{
	std::vector<int>::iterator it =  this->_userInChannel.begin(); //je set l'iterateur au debut du vecteur _userInChannel qui contient tout les user du chanel

	while (it != this->_userInChannel.end()) //tant que ont a pas envoyé à touts les user du chan
	{
		std::cout << "user in chan: " << *it << std::endl;
		if (*it != userSender)//si ce n'est pas l'envoyeur (on veux pas envoyé le message a sois meme)
			send(*it, msg.c_str()/*faut env des char, obligé de c_str*/, msg.size(), 0); //on envoie le message au socket des users.
		//send est comme write : on envoie dans quel fd on veux ecrire, on envoye le message et la taille du message comme write
		//sauf que send permet de mettre des flags mais c'est a pousser.
		it++; //on passe au user suivant
	}
	//if (*it != userSender)//si ce n'est pas l'envoyeur (on veux pas envoyé le message a sois meme)
	//	send(*it, msg.c_str(), msg.size(), 0);
}


std::vector<int>&	Chanel::getUserInChannel()
{
	return (this->_userInChannel);
}

void	Chanel::setInvitedUser(int fd)//setter de _invitedUser
{
	this->_invitedUser.push_back(fd);
}

std::vector<int>	Chanel::getInvitedUser()//getter de _invitedUser
{
	return (this->_invitedUser);
}

void	Chanel::setModeI(bool set)
{
	this->_mode_i = set;
}

void	Chanel::setModeT(bool set)
{
	this->_mode_t = set;
}

void	Chanel::setModeK(bool set)
{
	this->_mode_k = set;
}

void	Chanel::setModeL(bool set)
{
	this->_mode_l = set;
}

bool	Chanel::getModeI()
{
	return (this->_mode_i);
}

bool	Chanel::getModeT()
{
	return (this->_mode_t);
}

bool	Chanel::getModeK()
{
	return (this->_mode_k);
}

bool	Chanel::getModeL()
{
	return (this->_mode_l);
}

void	Chanel::set_nb_user_max(int nb)
{
	this->_nb_user_max = nb;
}

int		Chanel::get_nb_user_max()
{
	return (this->_nb_user_max);
}

//if add_remove = true, add somewone. If = false, remove somewone.
void	Chanel::set_nb_user_in(bool add_remove)
{
	if (add_remove)
		this->_nb_user_in++;
	else
		this->_nb_user_in--;
}

int	Chanel::get_nb_user_in()
{
	return (this->_nb_user_in);
}