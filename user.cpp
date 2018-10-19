#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>

#include "user.hpp"

bool User::checkUser(std::string filename) {

	bool isRegistered = false;
	std::ifstream file(filename.c_str());

	std::string loginf, passwordf;

	while(file >> loginf >> passwordf) {

		if(loginf.compare(this->getLogin())) {

			isRegistered = true;
			break;

		}

	}

	file.close();

	return isRegistered;

}

int User::registerUser(std::string filename) {

	int retCode;
	std::ofstream file;

	if(!this->checkUser(filename)) {

		file.open(filename.c_str());

		file << *this;

		retCode = 0;

	}
	else
		retCode = -1;

	file.close();

	return retCode;

}

bool verifyUser(std::string filename) {

	std::ifstream file;
	std::string loginf, passwordf;
	bool verified = false;

	if(this->checkUser(filename.c_str())) {

		file.open(filename.c_str());

		while(file >> loginf >> passwordf) {

			if(!loginf.compare(this->getLogin()) && !passwordf.compare(this->getPassword())) {

				verified = true;
				break;

			}

		}

	}

	file.close();

	return verified;

}

std::istream & operator>>(std::istream & stream, User & user) {

	std::string login, password, state;

	stream >> state;

	if(state.compare("not_registered") ||
		state.compare("registered") ||
		state.compare("in_queue") ||
		state.compare("in_game")) {

		stream >> login;
		stream >> password;

		user.setLogin(login);
		user.setPassword(password);
		user.setState(state);

	}
	else
		exit(-1);

	return stream;

}

std::ostream & operator<<(std::ostream & stream, const User & user) {

	stream << user.getLogin() << "\t" << user.getPassword() << "\n";

	return stream;

}