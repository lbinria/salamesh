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
#include "../core/module.h"
#include "../core/renderers/renderer_info.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

// TODO should I get deleter function to clean up ? and create a unload function ?

struct ModuleLoader {
	
	#ifdef WIN32
	std::unique_ptr<Module> load(const std::string& path, IApp& app) {
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

		// Allocate the script
		Script* script = allocator(app);

		std::cout << "Script allocated." << std::endl;

		// Optionally: free the library after usage or manage it with unique_ptr
		// FreeLibrary(handle);  // Uncomment if you want to manage the library lifecycle

		using allocateRendererInfosFunc = RendererInfo**(*)();
		allocateRendererInfosFunc allocateRendererInfos = (allocateRendererInfosFunc)GetProcAddress(handle, "allocateRendererInfos");

		using rendererInfosSizeFunc = int(*)();
		rendererInfosSizeFunc rendererInfosSize = (rendererInfosSizeFunc)GetProcAddress(handle, "rendererInfosSize");

		std::vector<std::unique_ptr<RendererInfo>> rInfos;
		if (allocateRendererInfos && rendererInfosSize) {
			auto raw_renderers = allocateRendererInfos();
			int count = rendererInfosSize();
			for (int i = 0; i < count; ++i) {
				std::unique_ptr<RendererInfo> ptr(raw_renderers[i]);
				rInfos.push_back(std::move(ptr));
			}
		}


		// dlclose(handle);

		return std::make_unique<Module>(std::unique_ptr<Script>(script), std::move(rInfos));
	}

	std::unique_ptr<Module> load(const std::string& path, IApp& app, sol::state &lua) {
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

		// Allocate the script
		Script* script = allocator(app, lua);

		std::cout << "Script allocated." << std::endl;

		// Optionally: free the library after usage or manage it with unique_ptr
		// FreeLibrary(handle);  // Uncomment if you want to manage the library lifecycle

		using allocateRendererInfosFunc = RendererInfo**(*)();
		allocateRendererInfosFunc allocateRendererInfos = (allocateRendererInfosFunc)GetProcAddress(handle, "allocateRendererInfos");

		using rendererInfosSizeFunc = int(*)();
		rendererInfosSizeFunc rendererInfosSize = (rendererInfosSizeFunc)GetProcAddress(handle, "rendererInfosSize");

		std::vector<std::unique_ptr<RendererInfo>> rInfos;
		if (allocateRendererInfos && rendererInfosSize) {
			auto raw_renderers = allocateRendererInfos();
			int count = rendererInfosSize();
			for (int i = 0; i < count; ++i) {
				std::unique_ptr<RendererInfo> ptr(raw_renderers[i]);
				rInfos.push_back(std::move(ptr));
			}
		}


		// dlclose(handle);

		return std::make_unique<Module>(std::unique_ptr<Script>(script), std::move(rInfos));
	}

	#else
	
	std::unique_ptr<Module> load(const std::string path, IApp &app) {

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

		Script* script = (Script*)allocator(app);

		std::cout << "allocated." << std::endl;


		RendererInfo** (*allocateRendererInfos)();
		allocateRendererInfos = (RendererInfo**(*)())dlsym(handle, "allocateRendererInfos");
		int (*rendererInfosSize)();
		rendererInfosSize = (int(*)())dlsym(handle, "rendererInfosSize");

		std::vector<std::unique_ptr<RendererInfo>> rInfos;
		if (allocateRendererInfos && rendererInfosSize) {
			auto raw_renderers = allocateRendererInfos();
			int count = rendererInfosSize();
			for (int i = 0; i < count; ++i) {
				std::unique_ptr<RendererInfo> ptr(raw_renderers[i]);
				rInfos.push_back(std::move(ptr));
			}
		}


		// dlclose(handle);

		return std::make_unique<Module>(std::unique_ptr<Script>(script), std::move(rInfos));
	}

	std::unique_ptr<Module> load(const std::string path, IApp &app, sol::state &lua) {

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

		Script* script = (Script*)allocator(app, lua);

		std::cout << "allocated." << std::endl;

		RendererInfo** (*allocateRendererInfos)();
		allocateRendererInfos = (RendererInfo**(*)())dlsym(handle, "allocateRendererInfos");
		int (*rendererInfosSize)();
		rendererInfosSize = (int(*)())dlsym(handle, "rendererInfosSize");

		std::vector<std::unique_ptr<RendererInfo>> rInfos;
		if (allocateRendererInfos && rendererInfosSize) {
			auto rInfosPtr = allocateRendererInfos();
			int count = rendererInfosSize();
			for (int i = 0; i < count; ++i) {
				auto rInfoPtr = rInfosPtr[i];
				std::unique_ptr<RendererInfo> ptr(rInfoPtr);
				std::cout << ptr->type << std::endl;
				rInfos.push_back(std::move(ptr));
			}
		}

		// dlclose(handle);

		return std::make_unique<Module>(std::unique_ptr<Script>(script), std::move(rInfos));
	}

	#endif
	
};