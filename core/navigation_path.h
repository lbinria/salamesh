#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

struct NavigationPath {

	NavigationPath(std::vector<std::string> path = {}) : 
		path(path) {}

	void set(std::vector<std::string> newPath) {
		path = newPath;
	}

	void set(std::string newPath) {
		auto pathComponents = split(newPath, '/');
		set(pathComponents);
	}

	std::vector<std::string> get() const {
		return path;
	}

	std::string str() const {
		if (path.empty()) return {};

		std::ostringstream oss;
		oss << path[0];
		for (size_t i = 1; i < path.size(); ++i) 
			oss << '/' << path[i];

		return oss.str();
	}

	operator std::string() const {
		return str();
	}

	bool operator ==(const NavigationPath &other) const {
		return this->path == other.path;
	}

    // Compare with std::string
    bool operator==(const std::string& str) const {
        return this->str() == str;
    }
    
    bool operator!=(const NavigationPath& other) const {
        return !(*this == other);
    }
    
    bool operator!=(const std::string& str) const {
        return !(this->str() == str);
    }

	void push(std::string pathComponent) {
		auto oldPath = path;
		path.push_back(pathComponent);
	}

	void pop() {
		if (path.size() <= 0)
			return;

		auto oldPath = path;
		path.erase(path.end() - 1);
	}

	bool startsWith(std::string strPath) const {
		auto pathComponents = split(strPath, '/');
		return startsWith(pathComponents);
	}

	bool startsWith(std::vector<std::string> head) const noexcept {
		if (head.size() > path.size())
			return false;

		for (int i = 0; i < head.size(); ++i) {
			if (path[i] != head[i])
				return false;
		}

		return true;
	}

	private:
	std::vector<std::string> path;

	std::vector<std::string> split(std::string s, char delim) const {
		std::stringstream ss(s);
		std::string segment;
		std::vector<std::string> segments;

		while(std::getline(ss, segment, delim))
			segments.push_back(segment);

		return segments;
	}

};