#include <ultimaille/all.h>
#include <string>
#include "hex_renderer.h"

using namespace UM;

struct HexModel {

	// Mesh + Renderer

	HexModel() : _name(""), _path(""), _hex(), _hex_renderer() {
		
	}

	HexModel(std::string &name) : 
		_name(name), 
		_path(""), 
		_hex() {}

	void load(const std::string &path) {
		// Load the mesh
		read_by_extension(path, _hex);
		_path = path;
		// Extract name
	}

	void save() {
		// Save the mesh
		if (_path.empty()) {
			std::cerr << "Error: No path specified for saving the mesh." << std::endl;
			return;
		}
		write_by_extension(_path, _hex);
	}

	void save_as(const std::string &path) {
		// Save the mesh to a new path
		write_by_extension(path, _hex);
		_path = path;
	}
	
	std::string getName() const { return _name; }
	std::string getPath() const { return _path; }

	Hexahedra& getHexahedra() { return _hex; }
	HexRenderer& getRenderer() { return _hex_renderer; }

	private:
	std::string _name;
	std::string _path;

	// Mesh
	Hexahedra _hex;

	// Renderers
	HexRenderer _hex_renderer;
	// PointRenderer _point_renderer;

    glm::vec3 position{0, 0, 0};

    std::vector<std::tuple<std::string, Element, std::shared_ptr<GenericAttributeContainer>>> attrs;

};