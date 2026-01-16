#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
// #define NOMINMAX
// #include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include "../core/app_interface.h"
#include "../core/script.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

// TODO should I get deleter function to clean up ? and create a unload function ?

struct ModuleLoader {
	
	#ifdef WIN32
	std::unique_ptr<Script> load(const std::string& path, IApp& app) {
		// Load the DLL
		HMODULE handle = LoadLibrary(path.c_str());
		
		if (!handle) {
			DWORD error = GetLastError();
			throw std::runtime_error("Failed to load library '" + path + "': " + std::to_string(error));
		}

		std::cout << "Library opened." << std::endl;

		// Define function pointer type
		using AllocatorFunc = Script* (*)(IApp&);
		AllocatorFunc allocator = (AllocatorFunc)GetProcAddress(handle, "allocator");

		if (!allocator) {
			std::cout << "Failed to find allocator function." << std::endl;
			FreeLibrary(handle);  // Free the library before returning
			return nullptr;
		}

		std::cout << "Allocator function created." << std::endl;

		// Allocate the component
		Script* component = allocator(app);

		std::cout << "Script allocated." << std::endl;

		// Optionally: free the library after usage or manage it with unique_ptr
		// FreeLibrary(handle);  // Uncomment if you want to manage the library lifecycle

		return std::unique_ptr<Script>(component);
	}

	std::unique_ptr<Script> load(const std::string& path, IApp& app, sol::state &lua) {
		// Load the DLL
		HMODULE handle = LoadLibrary(path.c_str());
		
		if (!handle) {
			DWORD error = GetLastError();
			throw std::runtime_error("Failed to load library '" + path + "': " + std::to_string(error));
		}

		std::cout << "Library opened." << std::endl;

		// Define function pointer type
		using AllocatorFunc = Script* (*)(IApp&, sol::state&);
		AllocatorFunc allocator = (AllocatorFunc)GetProcAddress(handle, "allocator");

		if (!allocator) {
			std::cout << "Failed to find allocator function." << std::endl;
			FreeLibrary(handle);  // Free the library before returning
			return nullptr;
		}

		std::cout << "Allocator function created." << std::endl;

		// Allocate the component
		Script* component = allocator(app, lua);

		std::cout << "Script allocated." << std::endl;

		// Optionally: free the library after usage or manage it with unique_ptr
		// FreeLibrary(handle);  // Uncomment if you want to manage the library lifecycle

		return std::unique_ptr<Script>(component);
	}

	#else
	
	std::unique_ptr<Script> load(const std::string path, IApp &app) {

		void *handle = dlopen(path.c_str(), RTLD_NOW);

		if (!handle) {
            const char* error = dlerror();
            throw std::runtime_error(std::string("Failed to load library '") + path + "': " + 
                                   (error ? error : "Unknown error"));
			return nullptr;
		}

		std::cout << "opened." << std::endl;

		Script* (*allocator)(IApp&);
		allocator = (Script*(*)(IApp&))dlsym(handle, "allocator");
		
		if (!allocator) {
			std::cout << "fail to allocator func." << std::endl;
			return nullptr;
		}
		
		std::cout << "func allocator created." << std::endl;

		Script* component = (Script*)allocator(app);

		std::cout << "allocated." << std::endl;

		// dlclose(handle);

		return std::unique_ptr<Script>(component);
	}

	std::unique_ptr<Script> load(const std::string path, IApp &app, sol::state &lua) {

		void *handle = dlopen(path.c_str(), RTLD_NOW);

		if (!handle) {
            const char* error = dlerror();
            throw std::runtime_error(std::string("Failed to load library '") + path + "': " + 
                                   (error ? error : "Unknown error"));
			return nullptr;
		}

		std::cout << "opened." << std::endl;

		Script* (*allocator)(IApp&, sol::state &);
		allocator = (Script*(*)(IApp&, sol::state&))dlsym(handle, "allocator");
		
		if (!allocator) {
			std::cout << "fail to allocator func." << std::endl;
			return nullptr;
		}
		
		std::cout << "func allocator created." << std::endl;

		Script* component = (Script*)allocator(app, lua);

		std::cout << "allocated." << std::endl;

		// dlclose(handle);

		return std::unique_ptr<Script>(component);
	}

	#endif
	
};