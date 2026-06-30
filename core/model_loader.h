#pragma once 

#include <ultimaille/all.h>
using namespace UM;

#include <string>

struct SceneNode;
struct Scene;
struct PolyLineGeometry;

struct ModelLoader {

	ModelLoader(Scene &scene) : _scene(scene) {}

	// Remove copy constructors, allow moves
	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;
	ModelLoader(ModelLoader&&) = default;
	ModelLoader& operator=(ModelLoader&&) = default;


	std::shared_ptr<SceneNode> load(const std::string filename, const std::string name);

	template<typename TGeometry>
	std::shared_ptr<TGeometry> loadGeometry(const std::string &filename) {

		auto geometry = std::make_shared<TGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if constexpr (std::is_same_v<TGeometry, PolyLineGeometry>) {
			if (geometry->_m.nedges() <= 0) {
				return nullptr;
			}
		} else {
			if (geometry->_m.nfacets() <= 0) {
				return nullptr;
			}
		}

		geometry->path = filename;

		return geometry;
	}


	private:
	Scene &_scene;

};