/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:53:57 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/16 20:35:24 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

static std::string	find_arg_chan(const char *argument, int &index)
{
	bool	full_arg = false;
	int		start = index;

	while (argument[index] && ((argument[index] != ',' && argument[index] != ' ' && argument[index] != '\n') || full_arg == true)) //on set f au bout de la chaine qu'on veut recup
	{
		if (argument[index] == ':')
			full_arg = true;
		index++;
	}
	return(std::string(argument + start, argument + index));
}

//La fonction ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
std::vector<std::string> create_chanName(const char *argument, int &i)
{
	int	f;
	std::vector<std::string>	chanName;
	//le while ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
	//si je fais ##general ca va faire de la merde ??
	while (argument[i] && argument[i] != ' ') //on stop quand espace ou fin de ligne //ATTENTION, PROBABLEMENT \r ou \t je sais plus
	{
		f = i;
		if ((i == 0 && argument[0] == '#') || (i > 0 && argument[i - 1] == ',' && argument[i] == '#')) //si le premier carractere est un #, ou sinon faut que le #soit juste apres une ','.
			chanName.push_back(find_arg_chan(argument, f)); //on recupere une chaine qui debute à i et qui fini à f et on l'ajoute au vecteur chanName;
		else if (i == 0)
			std::cout << find_arg_chan(argument, f) << " :Invalid channel name" << std::endl;
		i++; //on incremente i pour que si 'i' vallais '#', il se décale pour en chercher un autre
	} //On boucle afin de recuperer tout les differents channel dont on parle
	return (chanName);
}

//DEBUG, PERMET DE PRINT TOUT LES USER DE TOUT LES CHANNEL, A GARDER DE COTE
void	print_userInchan(std::vector<Chanel> &_chanel)
{
	std::vector<Chanel>::iterator	it = _chanel.begin();
	std::vector<int> id_userInChannel;

	while (it != _chanel.end())
	{
		std::cout << "Dans le channel " << (*it).getName();;
		id_userInChannel = (*it).getUserInChannel();
		std::vector<int>::iterator	it_id = id_userInChannel.begin();
		while (it_id != id_userInChannel.end())
		{
			std::cout << "	dans channel: " << *it_id << std::endl;
			it_id++;
		}
		it++;
		std::cout << std::endl;
	}
}

//return 1 if error
int	check_active_mode(std::vector<Chanel>::iterator	&it_ChanExist, int fd, std::vector<Client> &_clients, std::string &arg_after_channel)
{
	//EST CE QUE PLUSIEUR ERREURDOIVENT S'QFFICHER SI ERREUR ?? OU JUSTE UNE SEUL ET FAIRE DES ELSE IF
	if ((*it_ChanExist).getModeI() == true && is_user_in_chan(fd, (*it_ChanExist).getInvitedUser())) //Si invite only et pas invité
	{
		send_error(ERR_INVITEONLYCHAN(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	if ((*it_ChanExist).getModeK() == true && (arg_after_channel.empty() || arg_after_channel != (*it_ChanExist).getPassword()))
	{
		send_error(ERR_BADCHANNELKEY(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	if ((*it_ChanExist).getModeL() == true && (*it_ChanExist).get_nb_user_in() == (*it_ChanExist).get_nb_user_max()) // nb limite de personne
	{
		send_error(ERR_CHANNELISFULL(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	return (0);
}

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator 		it_ChanExist; //channel existant
	std::vector<std::string>::const_iterator	it_chanNew;
	int i = 0;
	std::string argumentStr = msg.getArgument();
	const char	*argument = argumentStr.c_str(); //arguement  est egale a la string stocker dans la class msg._argument
	const std::vector<std::string> 			&chanName = create_chanName(argument, i);
	std::string arg_after_channel = get_next_argument(argument, i);
	Client		user_sender = find_it_client_with_fd(fd, _clients);

	if (chanName.empty()) //si y'a pas de channel valide dans la commande reçu
	{
		std::cout << "No channel joined. Try JOIN #<channel>" << std::endl; //erreur, pqs cout
		return;
	}
	it_chanNew = chanName.begin();
	while (it_chanNew != chanName.end())
	{
		it_ChanExist = _chanel.begin(); //on repart du debut de la list de channel existant afin de la parcourir en entier et de bien chercher partout
		while (it_ChanExist != _chanel.end()) //tant qu'on a pas chercher dans tout ceux existant
		{
			if ((*it_ChanExist).getName() == *it_chanNew) //si le nom du chan est le meme qu'un chan existant
				break; //on break pour garder l'iterator sur le bon chan
			it_ChanExist++;
		}
		if (it_ChanExist == _chanel.end()) //si chan existe pas, (donc 'it' est a la fin car on a tout parcouru sans trouver)
		{
			std::cout << "Create chan: " << *it_chanNew << std::endl; //!debug, à retirer!
			Chanel newChan;
			newChan.setName(*it_chanNew);
			newChan.addUser(fd, true);
			_chanel.push_back(newChan);
			std::string joinMessage = RPL_JOIN(user_sender.getNickname(), newChan.getName());
			send (fd, joinMessage.c_str(), joinMessage.size(), 0);
		}
		else if (is_user_in_chan(fd, (*it_ChanExist).getUserInChannel()) == 0 && check_active_mode(it_ChanExist, fd, _clients, arg_after_channel) == 0)//sinon, donc le channel existais deja
		{
			(*it_ChanExist).addUser(fd, false);
			std::string joinMessage = RPL_JOIN(user_sender.getNickname(), (*it_ChanExist).getName());
			(*it_ChanExist).sendMessageToChanel(fd, joinMessage); //on envoie le message comme quoi quelun  a join a tout les gens du salon
			send (fd, joinMessage.c_str(), joinMessage.size(), 0);
		}
		it_chanNew++; //On passe au prochain channel que la personne veut rejoindre
	}
}

// void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
// {
// 	std::vector<Chanel>::iterator 		it_ChanExist; //channel existant
// 	std::vector<std::string>::const_iterator	it_chanNew;
// 	std::string argumentStr = msg.getArgument();
// 	const char	*argument = argumentStr.c_str(); //arguement  est egale a la string stocker dans la class msg._argument
// 	int i = 0;
// 	const std::vector<std::string> 			&chanName = create_chanName(argument, i);

// 	if (chanName.empty()) //si y'a pas de channel valide dans la commande reçu
// 	{
// 		std::cout << "No channel joined. Try JOIN #<channel>" << std::endl;
// 		return;
// 	}
// 	//ici on va rechercher si les noms stocké dans chanName sont déjà des noms de channels existant, afin que si ils n'existent pas, on les créés !
// 	it_chanNew = chanName.begin(); //init de l'iterator des differents nom de chan qu'on cherche
// 	while (it_chanNew != chanName.end()) //Tant que je trouve pas 'general' par exemple dans la liste des channels existantes
// 	{
// 		it_ChanExist = _chanel.begin(); //on repart du debut de la list de channel existant afin de la parcourir en entier et de bien chercher partout
// 		while (it_ChanExist != _chanel.end()) //tant qu'on a pas chercher dans tout ceux existant
// 		{
// 			if (!strcmp(((*it_ChanExist).getName()).c_str(), (*it_chanNew).c_str())) //si le nom du chan est le meme qu'un chan existant
// 				break; //on break pour garder l'iterator sur le bon chan
// 			it_ChanExist++;
// 		}
// 		if (it_ChanExist == _chanel.end()) //si chan existe pas, (donc 'it' est a la fin car on a tout parcouru sans trouver)
// 		{
// 			std::cout << "Create chan: " << *it_chanNew << std::endl; //!debug, à retirer!
// 			Chanel newChan; //On créé un nouveau channel
// 			newChan.setName(*it_chanNew); //on set le nom du channel qu'on viens de cree
// 			newChan.addUser(fd, true); // il rejoint en operateur psk c'est lui qui l'a créé
// 			_chanel.push_back(newChan); //on ajoute le nouveau channel a la list de channel existant
// 		}
// 		// else if (is_user_in_chan(fd, (*it_ChanExist).getUserInChannel()) == 0)//sinon, donc le channel existais deja
// 		// {
// 		// 	if ((*it_ChanExist).getModeI() == true && is_user_in_chan(fd, (*it_ChanExist).getInvitedUser())) //Si invite only et pas invité
// 		// 	{
// 		// 		std::string error = /*nomduserv*/ "473 " + find_nickname_with_fd(fd, _clients) + " " + (*it_ChanExist).getName() + " :Cannot join channel (+i)";
// 		// 		send(fd, error.c_str(), error.size(), 0);
// 		// 	}
// 		// 	else
// 		// 		(*it_ChanExist).addUser(fd, false); //on ajoute la personne qui a fais la commande join à la liste des personnes qui sont dans ce channel.
// 		// }
// 		else if (is_user_in_chan(fd, (*it_ChanExist).getUserInChannel()) == 0)
// {
//     if ((*it_ChanExist).getModeI() == true && is_user_in_chan(fd, (*it_ChanExist).getInvitedUser()))
//     {
//         std::string error = "473 " + find_nickname_with_fd(fd, _clients) + " " + (*it_ChanExist).getName() + " :Cannot join channel (+i)";
//         send(fd, error.c_str(), error.size(), 0);
//     }
//     else
//     {
//         (*it_ChanExist).addUser(fd, false);
//         std::cout << "User " << fd << " added to channel " << (*it_ChanExist).getName() << std::endl;
//     }
// }
// 		it_chanNew++; //On passe au prochain channel que la personne veut rejoindre
// 	}
// }
