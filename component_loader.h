#include <iostream>
#include <memory>
#include <dlfcn.h>

#include "core/component.h"

struct ComponentLoader {
	
	std::unique_ptr<Component> load(const std::string path, Hexahedra &hex, InputState &st) {

		void *handle = dlopen(path.c_str(), RTLD_NOW);

		if (!handle) {
            const char* error = dlerror();
            throw std::runtime_error(std::string("Failed to load library '") + path + "': " + 
                                   (error ? error : "Unknown error"));
			return nullptr;
		}

		std::cout << "opened." << std::endl;

		Component* (*allocator)(Hexahedra&, InputState&);
		allocator = (Component*(*)(Hexahedra&, InputState&))dlsym(handle, "allocator");
		
		if (!allocator) {
			std::cout << "fail to allocator func." << std::endl;
			return nullptr;
		}
		
		std::cout << "func allocator created." << std::endl;

		Component* component = (Component*)allocator(hex, st);

		std::cout << "allocated." << std::endl;

		// dlclose(handle);

		return std::unique_ptr<Component>(component);
	}
	
};