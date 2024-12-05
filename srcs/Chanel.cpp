#include "../include/Chanel.hpp"

Chanel::Chanel()
{

}

Chanel::~Chanel()
{

}

void	Chanel::addUser(int newUser, bool isOperator)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _user
	std::vector<int>::iterator op_it;

	us_it = this->_user.begin();
	op_it = this->_operator.begin();
	while (us_it != this->_user.end()) //on parcour la list
		us_it++;
	if (us_it == _user.end())
		this->_user.push_back(newUser);
	if (isOperator)
	{
		while (op_it != this->_user.end()) //pareil mais pour la list op
			op_it++;
		if (us_it == _operator.end())
			this->_operator.push_back(newUser);
	}
	//else //si on à rien changé (donc deja operator si on voulais le mettre op, ou deja client si on voulais juste le mettre en client)
		//erreur, user deja dans le chanel/ deja op
}

void	Chanel::removeUser(int newUser)
{
	std::vector<int>::iterator us_it; //comme un pointeur sur une case de notre tableau _user
	std::vector<int>::iterator op_it;

	us_it = this->_user.begin();
	op_it = this->_operator.begin();
	while (us_it != this->_user.end()) //on parcour la list
	{
		if (*us_it == newUser)//si il est trouve
		{
			this->_user.erase(us_it);//on le supprime
			break;
		}
		us_it++;
	}
	while (op_it != this->_user.end()) //pareil mais pour la list op
	{
		if (*op_it == newUser)
		{
			this->_user.erase(op_it);
			break;
		}
		op_it++;
	}
	//else //si on à rien changé (donc etait pas dans le chanel)
		//erreur, usr pas dans le chan
}

void	Chanel::sendMessageToChanel(int userSender, std::string &msg)
{
	std::vector<int>::iterator it =  this->_user.begin(); //je set l'iterateur au debut du vecteur _user qui contient tout les user du chanel

	while (it != this->_user.end()) //tant que ont a pas envoyé à touts les user du chan
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

std::vector<int>	Chanel::getUserInChannel()
{
	return (this->_user);
}

