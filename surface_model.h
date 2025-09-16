#pragma once

#include <json.hpp>
#include <ultimaille/all.h>
#include <string>
#include "core/model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "tri_renderer.h"

using namespace UM;
using json = nlohmann::json;

struct SurfaceModel : public Model {

	using Model::Model;

	bool load(const std::string path) override;
	void save() const override;
	void saveAs(const std::string path) const override;


	std::string save_state() const override {
		json j;
		j["name"] = _name;
		j["path"] = _path;
		j["position"] = { position.x, position.y, position.z };
		j["color_mode"] = colorMode;
		j["is_light_enabled"] = isLightEnabled;
		j["is_light_follow_view"] = isLightFollowView;
		j["is_clipping"] = isClipping;
		j["clipping_plane_point"] = { clippingPlanePoint.x, clippingPlanePoint.y, clippingPlanePoint.z };
		j["clipping_plane_normal"] = { clippingPlaneNormal.x, clippingPlaneNormal.y, clippingPlaneNormal.z };
		j["invert_clipping"] = invertClipping;
		j["mesh_size"] = meshSize;
		j["mesh_shrink"] = meshShrink;
		j["frag_render_mode"] = fragRenderMode;
		j["selected_colormap"] = selectedColormap;
		j["visible"] = visible;
		return j.dump(4);
	}

	void load_state(json model_state) {
		
		_name = model_state["name"].get<std::string>();
		_path = model_state["path"].get<std::string>();

		position = glm::vec3(
			model_state["position"][0].get<float>(),
			model_state["position"][1].get<float>(),
			model_state["position"][2].get<float>()
		);
		
		setColorMode((Model::ColorMode)model_state["color_mode"].get<int>());

		setLight(model_state["is_light_enabled"].get<bool>());
		setLightFollowView(model_state["is_light_follow_view"].get<bool>());
		setClipping(model_state["is_clipping"].get<bool>());

		setClippingPlanePoint(glm::vec3(
			model_state["clipping_plane_point"][0].get<float>(),
			model_state["clipping_plane_point"][1].get<float>(),
			model_state["clipping_plane_point"][2].get<float>()
		));

		setClippingPlaneNormal(glm::vec3(
			model_state["clipping_plane_normal"][0].get<float>(),
			model_state["clipping_plane_normal"][1].get<float>(),
			model_state["clipping_plane_normal"][2].get<float>()
		));

		setInvertClipping(model_state["invert_clipping"].get<bool>());

		setMeshSize(model_state["mesh_size"].get<float>());
		setMeshShrink(model_state["mesh_shrink"].get<float>());
		
		setSelectedColormap(model_state["selected_colormap"].get<int>());
		setVisible(model_state["visible"].get<bool>());

		load(_path);
	}

	void init() override {
		_surfaceRenderer->init();
		_pointSetRenderer.init();
		// _halfedgeRenderer.init();
	}

	void push() override;

	void render() override {
		if (!visible)
			return;
		
		glm::vec3 pos = getWorldPosition();

		_surfaceRenderer->render(pos);
		_pointSetRenderer.render(pos);
		// _halfedgeRenderer.render(pos);
	}
	
	void clean() override {
		_surfaceRenderer->clean();
		_pointSetRenderer.clean();
		// _halfedgeRenderer.clean();
	}

	SurfaceAttributes& getSurfaceAttributes() { return _surfaceAttributes; }

	int nverts() const override {
		return _surface->nverts();
	}

	int nfacets() const override {
		return _surface->nfacets();
	} 

	int ncells() const override {
		return 0;
	} 

	void setTexture(unsigned int tex) override {
		_surfaceRenderer->setTexture(tex);
		_pointSetRenderer.setTexture(tex);
		// _halfedgeRenderer.setTexture(tex);
	}

	// Just call underlying renderer methods
	int getColorMode() const override {
		return colorMode;
	}

	void setColorMode(Model::ColorMode mode) override {
		_surfaceRenderer->shader.setColorMode(mode);
		_pointSetRenderer.setColorMode(mode);
		// _halfedgeRenderer.setColorMode(mode);
		colorMode = mode;
	}

	glm::vec3 getColor() const override {
		return color;
	}

	void setColor(glm::vec3 c) override {
		_surfaceRenderer->shader.setColor(c);
		color = c;
	}

	bool getLight() const override {
		return isLightEnabled;
	}

	void setLight(bool enabled) override {
		_surfaceRenderer->shader.setLight(enabled);
		isLightEnabled = enabled;
	}

	bool getLightFollowView() const override {
		return isLightFollowView;
	}

	void setLightFollowView(bool follow) override {
		_surfaceRenderer->shader.setLightFollowView(follow);
		isLightFollowView = follow;
	}

	bool getClipping() const override {
		return isClipping;
	}

	void setClipping(bool enabled) override {
		_surfaceRenderer->shader.setClipping(enabled);
		_pointSetRenderer.setClipping(enabled);
		isClipping = enabled;
	}

	glm::vec3 getClippingPlanePoint() const override {
		return clippingPlanePoint;
	}

	void setClippingPlanePoint(glm::vec3 p) override {
		_surfaceRenderer->shader.setClippingPlanePoint(p);
		_pointSetRenderer.setClippingPlanePoint(p);
		clippingPlanePoint = p;
	}

	glm::vec3 getClippingPlaneNormal() const override {
		return clippingPlaneNormal;
	}

	void setClippingPlaneNormal(glm::vec3 n) override {
		_surfaceRenderer->shader.setClippingPlaneNormal(n);
		_pointSetRenderer.setClippingPlaneNormal(n);
		clippingPlaneNormal = n;
	}

	bool getInvertClipping() const override {
		return invertClipping;
	}

	void setInvertClipping(bool invert) override {
		_surfaceRenderer->shader.setInvertClipping(invert);
		_pointSetRenderer.setInvertClipping(invert);
		invertClipping = invert;
	}

	float getMeshSize() const override {
		return meshSize;
	}

	void setMeshSize(float val) override {
		_surfaceRenderer->shader.setMeshSize(val);
		meshSize = val;
	}

	float getMeshShrink() const override {
		return meshShrink;
	}

	void setMeshShrink(float val) override {
		_surfaceRenderer->shader.setMeshShrink(val);
		meshShrink = val;
	}

	bool getMeshVisible() const override {
		return _surfaceRenderer->getVisible();
	}

	void setMeshVisible(bool visible) override {
		return _surfaceRenderer->setVisible(visible);
	}

	int getFragRenderMode() const override {
		return fragRenderMode;
	}

	void setFragRenderMode(Model::RenderMode mode) override {
		_surfaceRenderer->shader.setFragRenderMode(mode);
		fragRenderMode = mode;
	}

	int getSelectedColormap() const override {
		return selectedColormap;
	}

	void setSelectedColormap(int idx) override {
		_surfaceRenderer->shader.setSelectedColormap(idx);
		selectedColormap = idx;
	}

	glm::vec3 getPointColor() const override {
		return _pointSetRenderer.getColor();
	}

	void setPointColor(glm::vec3 color) override {
		_pointSetRenderer.setColor(color);
	}

	float getPointSize() const override {
		return _pointSetRenderer.getPointSize();
	}

	void setPointSize(float size) override {
		_pointSetRenderer.setPointSize(size);
	}

	float getEdgeSize() const override {
		return 0;
		// return _halfedgeRenderer.getEdgeSize();
	}

	void setEdgeSize(float size) override {
		// _halfedgeRenderer.setEdgeSize(size);
	}

	glm::vec3 getEdgeInsideColor() const override {
		return glm::vec3(0);
		// return _halfedgeRenderer.getEdgeInsideColor();
	}

	void setEdgeInsideColor(glm::vec3 color) override {
		// _halfedgeRenderer.setEdgeInsideColor(color);
	}

	glm::vec3 getEdgeOutsideColor() const override {
		return glm::vec3(0);

		// return _halfedgeRenderer.getEdgeOutsideColor();
	}

	void setEdgeOutsideColor(glm::vec3 color) override {
		// _halfedgeRenderer.setEdgeOutsideColor(color);
	}

	void setMeshIndex(int index) override {
		_surfaceRenderer->shader.setMeshIndex(index);
	}

	bool getPointVisible() const override {
		return _pointSetRenderer.getVisible();
	}

	void setPointVisible(bool v) override {
		_pointSetRenderer.setVisible(v);
	}

	bool getEdgeVisible() const override {
		return false;
		// return _halfedgeRenderer.getVisible();
	}

	void setEdgeVisible(bool v) override {
		// _halfedgeRenderer.setVisible(v);
	}

	void setHighlight(int idx, float highlight) override {
		_surfaceRenderer->setHighlight(idx, highlight);
	}

	void setHighlight(std::vector<float> highlights) override {
		_surfaceRenderer->setHighlight(highlights);
	}

	void setPointHighlight(int idx, float highlight) override {
		_pointSetRenderer.setHighlight(idx, highlight);
	}

	void setPointHighlight(std::vector<float> highlights) override {
		_pointSetRenderer.setHighlight(highlights);
	}

	void setFilter(int idx, bool filter) override {

		_surfaceRenderer->setFilter(idx, filter);
		Surface::Facet f(*_surface, idx);
		for (int lv = 0; lv < 3; ++lv) {
			auto v = f.vertex(lv);

			bool allFiltered = true;

			// Iterate on corners
			for (int c = 0; c < _surface->facets.size(); ++c) {
				// If iterated corner is not current vertex, skip !
				if (_surface->facets[c] != v)
					continue;

				int fi = c / 3;
				if (_surfaceRenderer->getFilterPtr()[fi] <= 0) {
					allFiltered = false;
					break;
				}
			}

			// Only filter point when all attached facets are filtered
			_pointSetRenderer.setFilter(v, allFiltered);

		}
		
	}

	void setSelectedAttr(int idx) override;
	void setSelectedAttr(std::string name, ElementKind kind) override;


	private: 

	bool isLightEnabled = true;
	bool isLightFollowView = false;

	bool isClipping = false;
	glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
	glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
	bool invertClipping = false;
	float meshSize = 0.01f;
	float meshShrink = 0.f;
	Model::RenderMode fragRenderMode = Model::RenderMode::Color;
	Model::ColorMode colorMode = Model::ColorMode::COLOR;
	glm::vec3 color{0.8f, 0.f, 0.2f};
	
	int selectedColormap = 0;

	// Mesh
	std::unique_ptr<Surface> _surface;
	SurfaceAttributes _surfaceAttributes;

	// Renderers
	PointSetRenderer _pointSetRenderer;
	//HalfedgeRenderer _halfedgeRenderer;
	std::unique_ptr<SurfaceRenderer> _surfaceRenderer;


	void addAttr(ElementKind kind, NamedContainer &container) {
		
		// Get the type of the container
		ElementType type = ElementType::DOUBLE; // Default type
		if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
			type = ElementType::DOUBLE;
		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
			type = ElementType::INT;
		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
			type = ElementType::BOOL;
		} else {
			throw std::runtime_error("Unknown attribute type for container: " + container.name);
		}

		attrs.emplace_back(container.name, kind, type, container.ptr);
	}
};