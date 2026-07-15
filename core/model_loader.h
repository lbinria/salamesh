#pragma once 

#include <ultimaille/all.h>
using namespace UM;

#include <string>

struct SceneModel;
struct Scene;
struct PolyLineMesh;

struct ModelLoader {

	ModelLoader(Scene &scene) : _scene(scene) {}

	// Remove copy constructors, allow moves
	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;
	ModelLoader(ModelLoader&&) = default;
	ModelLoader& operator=(ModelLoader&&) = default;


	std::shared_ptr<SceneModel> load(const std::string filename, const std::string name);

	template<typename TMesh>
	std::shared_ptr<TMesh> loadMesh(const std::string &filename) {

		auto mesh = std::make_shared<TMesh>();

		mesh->_attributes = read_by_extension(filename, mesh->_m);
		if constexpr (std::is_same_v<TMesh, PolyLineMesh>) {
			if (mesh->_m.nedges() <= 0) {
				return nullptr;
			}
		} else {
			if (mesh->_m.nfacets() <= 0) {
				return nullptr;
			}
		}

		mesh->path = filename;

		return mesh;
	}


	private:
	Scene &_scene;

};