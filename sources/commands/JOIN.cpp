/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ioztimur <ioztimur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:05:38 by ioztimur          #+#    #+#             */
/*   Updated: 2024/05/14 17:30:02 by ioztimur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/ft_irc.hpp"

JOIN::JOIN(Server *server, bool isAutherized) : Command(server, isAutherized) {}

JOIN::~JOIN() {}

void JOIN::execute(std::vector<std::string> command, ClientInfo *client)
{
	std::string channel = command[0];
	bool channelExists = false;

	for (size_t i = 0; i < server->getChannels().size(); ++i)
	{
		if (server->getChannels()[i].name == channel)
		{
			channelExists = true;
			break;
		}
	}

	if (channelExists)
	{
		for (size_t i = 0; i < server->getChannels().size(); i++)
		{
			if (server->getChannels()[i].name == channel)
			{
				if (server->getChannels()[i].isPublic == false && client->isInvited == false)
				{
					sender(client->client_fd, Prefix(*client) + ":server.name 473 " + client->nickname + " " + channel + " :Cannot join channel (+i) - you must be invited.\r\n");
					return;
				}
				for (size_t j = 0; j < server->getChannels()[i].clients.size(); j++)
				{
					if (server->getChannels()[i].clients[j].client_fd == client->client_fd)
						return;
				}
			}
		}
		for (size_t i = 0; i < server->getChannels().size(); ++i)
		{
			if (server->getChannels()[i].name == channel)
			{
				if (!server->getChannels()[i].password.empty() && (command.size() < 2 || command[1] != server->getChannels()[i].password))
				{
					sender(client->client_fd, Prefix(*client) + ":server.name 475 " + client->nickname + " " + channel + " :Cannot join channel (+k) - bad key.\r\n");
					return;
				}
				if (server->getChannels()[i].capacity < server->getChannels()[i].clients.size() + 1)
				{
					sender(client->client_fd, ERR_CHANNELISFULL(client->hostname, client->nickname, channel));
					return;
				}
				server->getChannels()[i].clients.push_back(*client);
				sender(client->client_fd, Prefix(*client) + " JOIN " + channel + "\r\n");
				if (server->getChannels()[i].topic != "")
					sender(client->client_fd, RPL_TOPIC(client->hostname, client->nickname, server->getChannels()[i].name, server->getChannels()[i].topic));
				for (size_t j = 0; j < server->getChannels()[i].clients.size() - 1; j++)
				{
					sender(client->client_fd, Prefix(server->getChannels()[i].clients[j]) + " JOIN " + channel + "\r\n");
					if (server->getChannels()[i].clients[j].isOperator)
					{
						sender(client->client_fd, "MODE " + channel + " +o " + server->getChannels()[i].clients[j].nickname + "\r\n");
					}
				}
				for (size_t j = 0; j < server->getChannels()[i].clients.size() - 1; j++)
				{
					for (size_t k = 0; k < server->getChannels()[i].clients.size(); k++)
					{
						if (server->getChannels()[i].clients[j].client_fd != server->getChannels()[i].clients[k].client_fd)
						{
							std::string otherUserMessage = Prefix(server->getChannels()[i].clients[k]) + " JOIN " + channel + "\r\n";
							sender(server->getChannels()[i].clients[j].client_fd, otherUserMessage);
							if (server->getChannels()[i].clients[k].isOperator)
							{
								sender(server->getChannels()[i].clients[j].client_fd, "MODE " + channel + " +o " + server->getChannels()[i].clients[k].nickname + "\r\n");
							}
						}
					}
				}
				return;
			}
		}
	}
	else
	{
		if (channel[0] == '#' && channel.size() > 1)
		{

			Channel newChannel;
			newChannel.name = channel;
			newChannel.topic = "";
			newChannel.onlyOps = false;
			newChannel.password = (command.size() == 2) ? command[1] : "";
			newChannel.clients.push_back(*client);
			server->getChannels().push_back(newChannel);

			std::string message = Prefix(*client) + " JOIN " + channel + "\r\n";

			std::string modeMessage = "MODE " + channel + " +o " + client->nickname + "\r\n";
			server->getChannels()[server->getChannels().size() - 1].operators.push_back(client->client_fd);
			server->getChannels()[server->getChannels().size() - 1].clients[0].isOperator = true;
			server->getChannels()[server->getChannels().size() - 1].isPublic = true;
			server->getChannels()[server->getChannels().size() - 1].onlyOps = false;
			sender(client->client_fd, message);
			sender(client->client_fd, modeMessage);
			return;
		}
		else
			sender(client->client_fd, Prefix(*client) + "need #\r\n");
	}
}
