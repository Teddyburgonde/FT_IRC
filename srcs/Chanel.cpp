#include "../include/Chanel.hpp"

Chanel::Chanel() : _mode_i(false), _mode_t(false), _mode_k(false), _mode_o(false), _mode_l(false)
{

}

Chanel::~Chanel()
{

}

void	Chanel::addUser(int newUser, bool isOperator)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _userInChannel
	std::vector<int>::iterator op_it;

	us_it = this->_userInChannel.begin();
	op_it = this->_operator.begin();
	while (us_it != this->_userInChannel.end()) //on parcour la list
		us_it++;
	if (us_it == _userInChannel.end())
		this->_userInChannel.push_back(newUser);
	if (isOperator)
	{
		while (op_it != this->_userInChannel.end()) //pareil mais pour la list op
			op_it++;
		if (us_it == _operator.end())
			this->_operator.push_back(newUser);
	}
	//else //si on à rien changé (donc deja operator si on voulais le mettre op, ou deja client si on voulais juste le mettre en client)
		//erreur, user deja dans le chanel/ deja op
}

void	Chanel::removeUser(int newUser)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _userInChannel
	std::vector<int>::iterator op_it;

	us_it = this->_userInChannel.begin();
	op_it = this->_operator.begin();
	while (us_it != this->_userInChannel.end()) //on parcour la list
	{
		if (*us_it == newUser)//si il est trouve
		{
			this->_userInChannel.erase(us_it);//on le supprime
			break;
		}
		us_it++;
	}
	while (op_it != this->_userInChannel.end()) //pareil mais pour la list op
	{
		if (*op_it == newUser)
		{
			this->_userInChannel.erase(op_it);
			break;
		}
		op_it++;
	}
	//else //si on à rien changé (donc etait pas dans le chanel)
		//erreur, usr pas dans le chan
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
	if (*it != userSender)//si ce n'est pas l'envoyeur (on veux pas envoyé le message a sois meme)
		send(*it, msg.c_str(), msg.size(), 0);
}

std::string	Chanel::getName()
{
	return (this->_name);
}

void	Chanel::setName(std::string chanName)
{
	this->_name = chanName;
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

void	Chanel::setModeO(bool set)
{
	this->_mode_o = set;
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

bool	Chanel::getModeO()
{
	return (this->_mode_o);
}

bool	Chanel::getModeL()
{
	return (this->_mode_l);
}

// Ajoue Teddy 11/12/2024

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