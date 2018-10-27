#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <cassert>
#include <iostream>

class User {

	private:

		std::string login;
		std::string password;
		std::string state; //not_registered, registered, in_queue, in_game
		int socket_descriptor;

	public:
		//builder
		inline User() { this->login = ""; this->password = ""; this->state = "not_registered"; this->socket_descriptor = -1; };

		//Getters
		inline std::string getLogin() const { return this->login; };
		inline std::string getPassword() const { return this->password; };
		inline std::string getState() const { return this->state; };
		inline int getSocket_descriptor() const { return this->socket_descriptor; };

		//Setter
		inline void setLogin(const std::string newLogin) { this->login = newLogin; };
		inline void setPassword(const std::string newPassword) { this->password = newPassword; };
		inline void setState(const std::string newState) {

			assert(newState.compare("not_registered") == 0 ||
					newState.compare("registered") ||
					newState.compare("in_queue") ||
					newState.compare("in_game"));

			this->state = newState;

		};
		inline void setSocket_descriptor(int sd) { this->socket_descriptor = sd; };

		//checks if the user's login is registered in the file
		bool checkUser(std::string filename);

		//registers a new user, returns 0 if registration was successfully done, -1 otherwise
		int registerUser(std::string filename);

		//verifies that the user's login and password are registered in the file
		bool verifyUser(std::string filename);

		//equals one User to another
		inline User & operator=(const User & user){
			this->setLogin(user.getLogin());
			this->setState(user.getState());
			this->setPassword(user.getPassword());
			this->setSocket_descriptor(user.getSocket_descriptor());

			return *this;
		}

};

//iostream operators

std::istream & operator>>(std::istream & stream, User & user);
std::ostream & operator<<(std::ostream & stream, const User & user);

#endif
