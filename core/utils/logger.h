#pragma once
#include <string>
#include <iostream>
#include <fstream>

struct Logger {


	enum Verbosity {
		LOW,
		MEDIUM,
		HIGH
	};

	static bool outputFile;


	static void println(const std::string s) {
		std::cout << s << std::endl;

		if (!outputFile)
			return;

		std::ofstream f;
		f.open("logs.txt", std::ios::app);
		f << s << std::endl;
		f.close();
	}

	static void infoln(const std::string s) {
		std::ofstream f;
		f.open("logs.txt", std::ios::app);
		f << " - INFO: " << s << std::endl;
		f.close();
	}

	static void warningln(const std::string s) {
		std::ofstream f;
		f.open("logs.txt", std::ios::app);
		f << " - WARNING: " << s << std::endl;
		f.close();
	}

	static void errorln(const std::string s) {
		std::cerr << s << std::endl;

		std::ofstream f;
		f.open("logs.txt", std::ios::app);
		f << " - ERROR: " << s << std::endl;
		f.close();
	}

};