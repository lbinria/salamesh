#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>

// #ifdef _WIN32
// #define NOMINMAX
// #include <Windows.h>
// // #else
// // #include <dlfcn.h>
// #endif

#include "../core/app_interface.h"
#include "../core/component.h"

struct ModuleLoader {
	
	// #ifdef WIN32
	std::unique_ptr<Component> load(const std::string& path, IApp& app) {
		// Load the DLL
		HMODULE handle = LoadLibrary(path.c_str());
		
		if (!handle) {
			DWORD error = GetLastError();
			throw std::runtime_error("Failed to load library '" + path + "': " + std::to_string(error));
		}

		std::cout << "Library opened." << std::endl;

		// Define function pointer type
		using AllocatorFunc = Component* (*)(IApp&);
		AllocatorFunc allocator = (AllocatorFunc)GetProcAddress(handle, "allocator");

		if (!allocator) {
			std::cout << "Failed to find allocator function." << std::endl;
			FreeLibrary(handle);  // Free the library before returning
			return nullptr;
		}

		std::cout << "Allocator function created." << std::endl;

		// Allocate the component
		Component* component = allocator(app);

		std::cout << "Component allocated." << std::endl;

		// Optionally: free the library after usage or manage it with unique_ptr
		// FreeLibrary(handle);  // Uncomment if you want to manage the library lifecycle

		return std::unique_ptr<Component>(component);
	}

	// #else
	
	// std::unique_ptr<Component> load(const std::string path, IApp &app) {

	// 	void *handle = dlopen(path.c_str(), RTLD_NOW);

	// 	if (!handle) {
    //         const char* error = dlerror();
    //         throw std::runtime_error(std::string("Failed to load library '") + path + "': " + 
    //                                (error ? error : "Unknown error"));
	// 		return nullptr;
	// 	}

	// 	std::cout << "opened." << std::endl;

	// 	Component* (*allocator)(IApp&);
	// 	allocator = (Component*(*)(IApp&))dlsym(handle, "allocator");
		
	// 	if (!allocator) {
	// 		std::cout << "fail to allocator func." << std::endl;
	// 		return nullptr;
	// 	}
		
	// 	std::cout << "func allocator created." << std::endl;

	// 	Component* component = (Component*)allocator(app);

	// 	std::cout << "allocated." << std::endl;

	// 	// dlclose(handle);

	// 	return std::unique_ptr<Component>(component);
	// }

	// #endif
	
};