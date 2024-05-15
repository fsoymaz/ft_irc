/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ioztimur <ioztimur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 16:05:08 by iremoztimur       #+#    #+#             */
/*   Updated: 2024/05/14 18:44:11 by ioztimur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ft_irc.hpp"

class Server;

class Command
{
	protected:
		Server	*server;
		bool	isAutherized;

	public:
		Command(Server *server, bool isAutherized = true): server(server), isAutherized(isAutherized) {};
		virtual ~Command(){};

		bool getAuthStatus() const { return isAutherized; };

		virtual void execute(std::vector<std::string> command, ClientInfo *client) = 0;
};


class PASS: public Command
{
	public:
		PASS(Server *server, bool isAutherized);
		~PASS();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class NICK: public Command
{
    public:
        NICK(Server *server, bool isAutherized);
        ~NICK();

        void execute(std::vector<std::string> command, ClientInfo *client);
};

class USER: public Command
{
    public:
        USER(Server *server, bool isAutherized);
        ~USER();

        void execute(std::vector<std::string> command, ClientInfo *client);
};

class QUIT: public Command
{
    public:
        QUIT(Server *server, bool isAutherized);
        ~QUIT();

        void execute(std::vector<std::string> command, ClientInfo *client);
};

class JOIN: public Command
{
    public:
        JOIN(Server *server, bool isAutherized);
        ~JOIN();

        void execute(std::vector<std::string> command, ClientInfo *client);
};

class PRIVMSG: public Command
{
    public:
        PRIVMSG(Server *server, bool isAutherized);
        ~PRIVMSG();

        void execute(std::vector<std::string> command, ClientInfo *client);
};

class WHO: public Command
{
	public:
		WHO(Server *server, bool isAutherized);
		~WHO();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class MODE: public Command
{
	public:
		MODE(Server *server, bool isAutherized);
		~MODE();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class PING: public Command
{
	public:
		PING(Server *server, bool isAutherized);
		~PING();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class PART: public Command
{
	public:
		PART(Server *server, bool isAutherized);
		~PART();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class KICK: public Command
{
	public:
		KICK(Server *server, bool isAutherized);
		~KICK();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class TOPIC: public Command
{
	public:
		TOPIC(Server *server, bool isAutherized);
		~TOPIC();

		void execute(std::vector<std::string> command, ClientInfo *client);
};

class INVITE: public Command
{
	public:
		INVITE(Server *server, bool isAutherized);
		~INVITE();

		void execute(std::vector<std::string> command, ClientInfo *client);
};
