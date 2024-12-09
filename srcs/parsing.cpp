#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>

void Server::handleNick(int fd, const std::string& newNick) 
{
    if (newNick.empty()) 
	{
        std::string response = ERR_NONICKNAMEGIVEN(std::string("Server"), "");
        send(fd, response.c_str(), response.size(), 0);
        return;
    }

    for (size_t i = 0; i < _clients.size(); ++i) 
	{
        if (_clients[i].getNickname() == newNick) {
            std::string response = ERR_NICKNAMEINUSE(std::string("Server"), newNick);
            send(fd, response.c_str(), response.size(), 0);
            return;
        }

        if (_clients[i].getFd() == fd) {
            _clients[i].setNickname(newNick);
            std::string response = RPL_WELCOME(newNick);
            send(fd, response.c_str(), response.size(), 0);
            std::cout << "Client FD: " << fd << " Nickname: " << newNick << std::endl;
            return;
        }
    }
}

void Server::handlePrivMsg(int fd, const std::string& command)
{
	// Trouver le destinataire
		size_t spacePos = command.find(' ', 8);
		if (spacePos == std::string::npos)
		{
			std::string response = ERR_NORECIPIENT(std::string ("Server"), "");
        	send(fd, response.c_str(), response.size(), 0);
        	return ;
		}
		std::string recipient = command.substr(8, spacePos - 8); // Extrait le destinataire
    	if (recipient.empty())
		{
        	std::string response = ERR_NORECIPIENT(std::string("Server"), "");
        	send(fd, response.c_str(), response.size(), 0);
        	return;
    	}
		 // Trouver le message après ":"
    	size_t colonPos = command.find(':', spacePos);
    	if (colonPos == std::string::npos) 
		{
        	std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
        	send(fd, response.c_str(), response.size(), 0);
        	return;
    	}
    	std::string message = command.substr(colonPos + 1); // Tout après ":" est le message
    	if (message.empty()) 
		{
        	std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
        	send(fd, response.c_str(), response.size(), 0);
        	return;
    	}
    	// Envoyer le message au destinataire
    	bool recipientFound = false;
    	for (size_t i = 0; i < _clients.size(); i++) 
		{
        	if (_clients[i].getNickname() == recipient) 
			{
            	send(_clients[i].getFd(), message.c_str(), message.size(), 0);
            	recipientFound = true;
            	break;
        	}
    	}
    	if (!recipientFound) 
		{
        	std::string response = ERR_NOSUCHNICK(std::string("Server"), recipient);
        	send(fd, response.c_str(), response.size(), 0);
   		}
}


/*
wtf 
bool Server::isUserInChannel(int fd, const Chanel &channel)
{
    for (std::vector<int>::const_iterator userIt = channel.getUserInChannel().begin();
         userIt != channel.getUserInChannel().end(); ++userIt)
    {
        if (*userIt == fd)
            return true;
    }
    return false;
}

bool Server::isTargetInChannel(const std::string &targetUser, const Chanel &channel)
{
    for (std::vector<int>::const_iterator userIt = channel.getUserInChannel().begin();
         userIt != channel.getUserInChannel().end(); ++userIt)
    {
        if (std::to_string(*userIt) == targetUser) // Si targetUser est un fd converti
            return true;
    }
    return false;
}



KICK <channel> <target> [<comment>]
1. Ajouter une vérification si l’expéditeur est dans le canal.
2. Vérifier si l’expéditeur a les privilèges nécessaires (admin).
3. Ajouter une vérification si la cible existe avant de chercher dans le canal.
4. Ajouter une gestion de la raison pour l’expulsion.
5. Implémenter une méthode pour retirer la cible du canal et notifier les utilisateurs.

*/


const std::vector<int>& Chanel::getOperators() const
{
	return (_operator);
}


/* Cette fonction Verifie si le format est bien respecter pour la commande KICK
	KICK <channel> <target> [<comment>] 
	Commande valide :
	KICK #general Adrien
	Commande invalide 
	KICK #general
*/

bool Server::validateKickArgs(int fd, Message &msg, std::string &channel, std::string &targetUser) 
{
    // Trouver la position du premier espace dans les arguments de la commande.
    size_t spacePos = msg.getArgument().find(' ');
    
    // Si aucun espace n'est trouvé apres le #channel
	// cela signifie que le format est invalide.
    if (spacePos == std::string::npos) 
	{
        std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
        // Envoyer la réponse d'erreur au client qui a envoyé la commande.
        send(fd, response.c_str(), response.size(), 0);
        // false le format n'est pas correct.
        return false;
    }
    // Extraire le nom du canal en prenant la sous-chaîne avant l'espace.
    // donc la il y a #channel dans la variable channel 
	channel = msg.getArgument().substr(0, spacePos);
    // Extraire le nom de l'utilisateur cible en prenant la sous-chaîne après l'espace.
    // substr(spacePos + 1) extrait le texte apres l'espace 
	// donc la il y a le nom de l'utilisateur par exemple "Galaad"
	targetUser = msg.getArgument().substr(spacePos + 1);

    // Si le nom du canal ou le nom de l'utilisateur cible est vide, le format est invalide.
    if (channel.empty() || targetUser.empty()) 
	{
        std::string response = ERR_NEEDMOREPARAMS(std::string("Server"), "KICK");
        // Envoyer la réponse d'erreur au client qui a envoyé la commande.
        send(fd, response.c_str(), response.size(), 0);
         // false le format n'est pas correct.
        return false;
    }
    // Command valid
    return true;
}

/*
Vérifie si l'expéditeur est dans le canal
L'expediteur c'est celui qui fais la command KICK
Par Exemple si Galaad est connecté au server et q'il tape KICK #general Toto 
Galaad est l'expediteur, Toto la target.
*/

bool Server::isSenderInChannel(int fd, Chanel &channel)
{	
	for (std::vector<int>::iterator userIt = channel.getUserInChannel().begin();
			userIt != channel.getUserInChannel().end(); ++userIt)
	{
		if (*userIt == fd) // il a trouvé l'expéditeur. 
			return true;
	}
	return false;	
}

/*
Cette fonction vérifie si l'expéditeur est un operateur.
Cela permet de savoir si l'utilisateur a les privilèges pour faire la command KICK
*/
bool Server::isSenderOperator(int fd, Chanel &channel)
{
	// dans channel il y a une liste d'operator 
	// on stock la liste de fd (les operator) dans le vector operators.
	std::vector<int> operators = channel.getOperators();

	// Si le fd est trouvé dans le vecteur operators, 
	// std::find retourne un itérateur pointant vers la position où l'élément a été trouvé.
	// pour resumer si il a trouver l'operator il renvoie true sinon false
	bool isOperator = std::find(operators.begin(), operators.end(), fd) != operators.end();
	return (isOperator);
}

/*
Cherche si dans la liste des utilisateurs il y a la cible a kick , si c'est le cas on 
le retire de la liste des utilisateurs.
*/
bool Server::isTargetInChannel(const std::string &targetUser, Chanel &channel)
{
	for (std::vector<int>::iterator userIt = channel.getUserInChannel().begin();
			userIt != channel.getUserInChannel().end(); ++userIt)
	{
		if (std::to_string(*userIt) == targetUser) 
		{
			channel.getUserInChannel().erase(userIt);
			return true;
		}
	}
	return false;
}

/*
Cette fonction informe tous les utilisateurs d'un canal qu'un utilisateur a été expulsé.
*/

void Server::notifyKick(Chanel &channel, const std::string &sender, const std::string &targetUser, const std::string &reason) 
{
	// Creation d'un message sous forme de chaîne .
    std::string kickMessage = ":" + sender + " KICK " + channel.getName() + " " + targetUser + " :" + reason + "\r\n";
    
	// -1 pour envoyer le message a tout le monde sauf a celui qui viens de se faire kick
	channel.sendMessageToChanel(-1, kickMessage);
}


void Server::handleKick(int fd, Message &msg, std::vector<Chanel> &_chanel)
{
	std::string channel;
	std::string targetUser;

	if (!validateKickArgs(fd, msg, channel, targetUser))
        return;
	for (std::vector<Chanel>::iterator i = _chanel.begin(); i != _chanel.end(); i++)
	{
		if ((*i).getName() == channel)
		{
			if (!isSenderInChannel(fd, *i))
			{
				std::string response = ERR_NOTONCHANNEL(std::string("Server"), channel);
				send(fd, response.c_str(), response.size(), 0);
				return;
			}
			if (!isSenderOperator(fd, *i))
			{
    			std::string response = ERR_CHANOPRIVSNEEDED(std::string("Server"), channel);
   				send(fd, response.c_str(), response.size(), 0);
    			return;
			}
			if (!isTargetInChannel(targetUser, *i))
			{
				std::string response = ERR_USERNOTINCHANNEL(std::string("Server"), targetUser, channel);
				send(fd, response.c_str(), response.size(), 0);
				return ;
			}
			notifyKick(*i, std::to_string(fd), targetUser, "Kicked by operator");
			return ;
		}
	}
	// Si aucun canal correspondant n'est trouvé
	std::string response = ERR_NOSUCHCHANNEL(channel);
	send(fd, response.c_str(), response.size(), 0);
}

void Server::analyzeData(int fd,  const std::string &buffer)
{
	Message msg;

	/*ajout*/
	msg.setCommand("KICK");
	msg.setArgument("#general Romain");
	handleKick(fd, msg, this->_chanel);

	std::vector<std::string> stringBuffer;
	stringBuffer.push_back(std::string(buffer.begin(), buffer.end()));
	parse_buffer(stringBuffer, msg);
	if (msg.getCommand().empty())
		return ;
	std::string newNick;
	if (strncmp(buffer.data(), "NICK ", 5) == 0) 
	{
    	std::string newNick = std::string(buffer.begin() + 5, buffer.end());
    	newNick.erase(std::remove(newNick.begin(), newNick.end(), '\r'), newNick.end());
    	newNick.erase(std::remove(newNick.begin(), newNick.end(), '\n'), newNick.end());
    	handleNick(fd, newNick);
	}
	if (strncmp(buffer.data(), "PRIVMSG ", 8) == 0)
		handlePrivMsg(fd, std::string(buffer));
	if (msg.getCommand() == "KICK") 
	{  // Ajout de la commande KICK
        handleKick(fd, msg, this->_chanel);
    }
	if (!strncmp(buffer.data(), "JOIN ", 5)) //si c'est join la commande, a changer grace au futur parsing ?
	{
		//std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, msg, this->_chanel);
	}
	if (!strncmp(buffer.data(), "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		//besoins du parsing
		std::string msg = std::string(buffer.data() + 13); //ici je recup le msg apres le SEND #general donc a changer
		//dans l'idee faudrais juste envoye le truc après le # grace au parsing ? Au lieu de channel[0], je retrouve le nom du bon salon.
		this->_chanel[0].sendMessageToChanel(fd, msg); //chanel[0] == que le premier salon. Faut coder le fais d'envoyé dans le salon ou il est le client
		std::cout << "send a message to general" << std::endl;
	}
}

void parse_buffer(std::vector <std::string> &buffer, Message& msg)
{

	//Message msg;
	
	// Est t'il vide ?
	if (buffer.empty())
        throw std::runtime_error("Buffer is empty");
	
	
	std::string firstElement = buffer.front();
	if (firstElement[0] == ' ')
	{
		std::cout <<  "The command must not be preceded by a space." << std::endl;
		return ;
		//throw(std::runtime_error("The command must not be preceded by a space."));
	}
	size_t spacePos = firstElement.find(' ');

	if (spacePos != std::string::npos) 
	{
    	std::string line = firstElement.substr(0, spacePos);
		msg.setCommand(line);
		std::string argument = firstElement.substr(spacePos + 1);
        msg.setArgument(argument);
	} 
	else 
	{
    	msg.setCommand(firstElement);
        msg.setArgument(""); // Aucun argument
	}

	//std::cout << "Commande extraite : " << msg.getCommand() << std::endl;
    //std::cout << "Argument extrait : " << msg.getArgument() << std::endl;

    return ;
}
