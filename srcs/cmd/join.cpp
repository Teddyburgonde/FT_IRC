/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:53:57 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/08 18:48:21 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"



//!DEBUG, PERMET DE PRINT TOUT LES USER DE TOUT LES CHANNEL, A ENLEVER POUR PUSH
void	print_userInchan(std::vector<Channel> &_channel)
{
	std::vector<Channel>::iterator	it = _channel.begin();
	std::vector<int> id_userInChannel;

	while (it != _channel.end())
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
int	check_active_mode(std::vector<Channel>::iterator	&it_ChanExist, int fd, std::vector<Client> &_clients, std::string &arg_after_channel)
{
	//std::cout << "mode i = " << (*it_ChanExist).getModeI() << std::endl; //!debug
	//EST CE QUE PLUSIEUR ERREURDOIVENT S'QFFICHER SI ERREUR ?? OU JUSTE UNE SEUL ET FAIRE DES ELSE IF
	if ((*it_ChanExist).getModeI() == true && !is_user_in_chan(fd, (*it_ChanExist).getInvitedUser())) //Si invite only et pas invité
	{
		betterSend(ERR_INVITEONLYCHAN(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	if ((*it_ChanExist).getModeK() == true && (arg_after_channel.empty() || arg_after_channel != (*it_ChanExist).getPassword()))
	{
		betterSend(ERR_BADCHANNELKEY(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	if ((*it_ChanExist).getModeL() == true && (*it_ChanExist).get_nb_user_in() >= (*it_ChanExist).get_nb_user_max()) // nb limite de personne
	{
		betterSend(ERR_CHANNELISFULL(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	return (0);
}

static std::string	find_arg_chan(const char *argument, int &index)
{
	bool	full_arg = false;
	int		start = index;

	while (argument[index] && ((argument[index] != ',' && argument[index] != ' ' && argument[index] != '\n' && argument[index] != '\r') || full_arg == true)) //on set f au bout de la chaine qu'on veut recup
	{
		if (argument[index] == ':')
			full_arg = true;
		index++;
	}
	return(std::string(argument + start, argument + index));
}

//La fonction ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
static std::vector<std::string> create_chanName(const char *argument, int &i, int fd)
{
	int	f;
	std::vector<std::string>	chanName;
	std::vector<std::string>	error;
	//le while ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
	//si je fais ##general ca va faire de la merde ??
	while (argument[i] && argument[i] != ' ') //on stop quand espace ou fin de ligne //ATTENTION, PROBABLEMENT \r ou \t je sais plus
	{
		f = i;
		if ((i == 0 && argument[0] == '#') || (i > 0 && argument[i - 1] == ',' && argument[i] == '#')) //si le premier carractere est un #, ou sinon faut que le #soit juste apres une ','.
			chanName.push_back(find_arg_chan(argument, f)); //on recupere une chaine qui debute à i et qui fini à f et on l'ajoute au vecteur chanName;
		else if (i == 0)
		{
			betterSend(ERR_NOSUCHCHANNEL((std::string)argument), fd);
			return (error);
		}
		i++; //on incremente i pour que si 'i' vallais '#', il se décale pour en chercher un autre
	} //On boucle afin de recuperer tout les differents channel dont on parle
	return (chanName);
}

void handleJoin(int fd, Message &msg, std::vector<Channel> &_channel, std::vector<Client> &_clients)
{
    std::vector<Channel>::iterator 				it_ChanExist;
    std::vector<std::string>::const_iterator	it_chanNew;
	std::string									tmp = msg.getArgument();
    const char 									*argument = tmp.c_str();
    int											i = 0;
    const std::vector<std::string>				&chanName = create_chanName(argument, i, fd); //i est incrementé ici
    std::string									arg_after_channel = get_next_argument(argument, i);
    Client										user_sender = find_it_client_with_fd(fd, _clients);

    if (chanName.empty())
    {
		betterSend(ERR_NEEDMOREPARAMS(user_sender.getNickname(), "JOIN"), fd);
		return;
    }
    it_chanNew = chanName.begin();
    while (it_chanNew != chanName.end())
    {
		// Recherche du canal existant
		it_ChanExist = _channel.begin();
		while (it_ChanExist != _channel.end())
		{
			if (it_ChanExist->getName() == *it_chanNew)
				break;
			++it_ChanExist;
		}
		if (it_ChanExist == _channel.end())
		{
			Channel newChan;
			newChan.setName(*it_chanNew);
			newChan.addUser(fd, true);
			_channel.push_back(newChan);
			betterSend(RPL_JOIN(user_sender.getNickname(), user_sender.getUsername(), newChan.getName()), fd);
		}
		//si il est pas deja dans le channel et que ya pas de mode bloquant
		else if (is_user_in_chan(fd, it_ChanExist->getUserInChannel()) == 0 && check_active_mode(it_ChanExist, fd, _clients, arg_after_channel) == 0)
		{
			it_ChanExist->addUser(fd, false);
			std::string message = RPL_JOIN(user_sender.getNickname(), user_sender.getUsername(), it_ChanExist->getName());
			//-1 pour que tout le monde ai le message, meme celui qui envoie
			it_ChanExist->sendMessageToChannel(-1, message);
		}
		++it_chanNew;
	}
}
