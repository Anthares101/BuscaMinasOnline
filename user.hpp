#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {

	private:

		std::string login;
		std::string password;
		std::string state; //not_registered, registered, in_queue, in_game

	public:

		inline User() { this->login = "-"; this->password = "-"; this->state = "not_registered"; };

		inline getLogin() const { return this->login; };

		inline getPassword() const { return this->password; };

		inline getState() const { return this->state; };

		inline setLogin(const std::string newLogin) { this->login = newLogin; };

		inline setPassword(const std::string newPassword) { this->password = newPassword; };

		inline setState(const std::string newState) {

			assert(newState.compare("not_registered") == 0 ||
					newState.compare("registered") ||
					newState.compare("in_queue") ||
					newState.compare("in_game"));

			this->state = newState;

		};

};

istream & operator>>(istream & stream, User & user);

ostream & operator<<(ostream & stream, const User & user);

bool checkUser(std::string filename, const User & user);

int registerUser(std::string filename, const User & user);

#endif
