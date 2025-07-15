#pragma once 
#include <vector>
#include <string>

struct Commands {

	static Commands& get() {
		static Commands instance;
		return instance;
	}

	void add_command(const std::string& command) {
		commands.push_back(command);
	}

	std::vector<std::string> get_commands() const {
		return commands;
	}

	void clear_commands() {
		commands.clear();
	}

	private:
	std::vector<std::string> commands;

};