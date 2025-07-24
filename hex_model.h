#pragma once

#include <json.hpp>
#include <ultimaille/all.h>
#include <string>
#include "core/model.h"
#include "hex_renderer.h"
#include "point_set_renderer.h"

using namespace UM;
using json = nlohmann::json;

struct HexModel final : public Model {

	// Mesh + Renderer

	HexModel() : 
        _name(""), 
        _path(""), 
        _hex(), 
        _hexRenderer(_hex), 
        _pointSetRenderer(_hex.points) {}

	HexModel(std::string name) : 
		_name(name), 
		_path(""), 
		_hex(),
		_hexRenderer(_hex),
        _pointSetRenderer(_hex.points) {}


	void load(const std::string path) override {
		// Load the mesh
		VolumeAttributes attributes = read_by_extension(path, _hex);
		_path = path;
		// Extract name
		if (_name.empty()) {
            std::filesystem::path p(path);
            _name = p.stem().string();
		}

		// Add attribute to renderer
		clearAttrs();

		for (auto &a : attributes.points) {
			addAttr(Element::POINTS, a);
		}
		for (auto a : attributes.cell_corners) {
			addAttr(Element::CORNERS, a);
		}
		for (auto a : attributes.cell_facets) {
			addAttr(Element::FACETS, a);
		}
		for (auto a : attributes.cells) {
			addAttr(Element::CELLS, a);
		}
	}

	void save() const override {
		// Save the mesh
		if (_path.empty()) {
			std::cerr << "Error: No path specified for saving the mesh." << std::endl;
			return;
		}
        // TODO save attributes
		write_by_extension(_path, _hex);
	}

	void save_as(const std::string path) override {
		// Save the mesh to a new path
        // TODO save attributes
		write_by_extension(path, _hex);
		_path = path;
	}

    std::string save_state() override {
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

    std::string getName() const override { return _name; }
    void setName(std::string name) override { _name = name; }
    std::string getPath() const override { return _path; }

	void init() override {
		_hexRenderer.init();
		_pointSetRenderer.init();
	}

    void push() override;

    void render() override {
        if (!visible)
            return;
        
        glm::vec3 pos = getWorldPosition();

        _hexRenderer.render(pos);
        _pointSetRenderer.render(pos);
	}
    
    void clean() override {
		_hexRenderer.clean();
	}

	Hexahedra& getHexahedra() override { return _hex; }
	// HexRenderer& getRenderer() { return _hexRenderer; }

    void setTexture(unsigned int tex) override {
        _hexRenderer.setTexture(tex);
        _pointSetRenderer.setTexture(tex);
    }

	// Just call underlying renderer methods
    int getColorMode() const override {
        return colorMode;
    }

    void setColorMode(Model::ColorMode mode) override {
        _hexRenderer.setColorMode(mode);
        _pointSetRenderer.setColorMode(mode);
        colorMode = mode;
    }

    bool getLight() const override {
        return isLightEnabled;
    }

    void setLight(bool enabled) override {
        _hexRenderer.setLight(enabled);
        isLightEnabled = enabled;
    }

    bool getLightFollowView() const override {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) override {
		_hexRenderer.setLightFollowView(follow);
        isLightFollowView = follow;
    }

    bool getClipping() const override {
        return isClipping;
    }

    void setClipping(bool enabled) override {
        _hexRenderer.setClipping(enabled);
        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() const override {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) override {
        _hexRenderer.setClippingPlanePoint(p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() const override {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) override {
		_hexRenderer.setClippingPlaneNormal(n);
        clippingPlaneNormal = n;
    }

    bool getInvertClipping() const override {
        return invertClipping;
    }

    void setInvertClipping(bool invert) override {
        _hexRenderer.setInvertClipping(invert);
        invertClipping = invert;
    }

    float getMeshSize() const override {
        return meshSize;
    }

    void setMeshSize(float val) override {
        _hexRenderer.setMeshSize(val);
        meshSize = val;
    }

    float getMeshShrink() const override {
        return meshShrink;
    }

    void setMeshShrink(float val) override {
		_hexRenderer.setMeshShrink(val);
        meshShrink = val;
    }

    int getFragRenderMode() const override {
        return fragRenderMode;
    }

    void setFragRenderMode(Model::RenderMode mode) override {
        _hexRenderer.setFragRenderMode(mode);
        fragRenderMode = mode;
    }

    int getSelectedColormap() const override {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) override {
		_hexRenderer.setSelectedColormap(idx);
        selectedColormap = idx;
    }

    glm::vec3 getPointColor() const override {
        return _pointSetRenderer.getPointColor();
    }

    void setPointColor(glm::vec3 color) override {
        _pointSetRenderer.setPointColor(color);
    }

    float getPointSize() const override {
        return _pointSetRenderer.getPointSize();
    }

    void setPointSize(float size) override {
        _pointSetRenderer.setPointSize(size);
    }

    glm::vec3 getWorldPosition() const override {
        if (parent) {
            return parent->getWorldPosition() + position;
        } else {
            return position;
        }
    }

    glm::vec3 getPosition() const override {
        return position;
    }

    void setPosition(glm::vec3 p) override {
        position = p;
    }

    bool getVisible() const override {
        return visible;
    }

    void setVisible(bool v) override {
        visible = v;
        _hexRenderer.setVisible(v);
    }

    void setHighlight(int idx, bool highlighted) override {
        _hexRenderer.setHighlight(idx, highlighted);
    }

    void setHighlight(int idx, float highlight) override {
        _hexRenderer.setHighlight(idx, highlight);
    }

    void setHighlight(std::vector<float> highlights) override {
        _hexRenderer.setHighlight(highlights);
    }

    // TODO filter anything else than cell !
    void setFilter(int idx, bool filter) override {
        _hexRenderer.setFilter(idx, filter);

        // TODO it works but... not very efficient !
        Volume::Cell c(_hex, idx);
        for (int lc = 0; lc < 8; ++lc) {
            auto corner = c.corner(lc);
            auto v = corner.vertex();
            // Retrieve all cells attached to this point to see whether filtered
            bool allFiltered = true;
            for (int i = 0; i < _hex.cells.size(); ++i) {
                if (_hex.cells[i] != v)
                    continue;
                
                int ci = i / 8;
                if (_hexRenderer.getFilterPtr()[ci] <= 0) {
                    allFiltered = false;
                    break;
                }
            }

            _pointSetRenderer.setFilter(v, allFiltered);
        }

        
    }

    std::vector<std::tuple<std::string, int>> getAttrs() const override {
        std::vector<std::tuple<std::string, int>> result;
        for (const auto& attr : attrs) {
            result.emplace_back(std::get<0>(attr), std::get<1>(attr));
        }
        return result;
    }

    std::tuple<std::string, int> getAttr(int idx) const override {
        return std::make_tuple(std::get<0>(attrs[idx - 1]), std::get<1>(attrs[idx - 1]));
    }

    void addAttr(Element element, NamedContainer &container) override {
        attrs.emplace_back(container.first, element, container.second);
    }

    void removeAttr(const std::string& name, Element element) override {
        attrs.erase(std::remove_if(attrs.begin(), attrs.end(),
            [&name, &element](const auto& attr) {
                return std::get<0>(attr) == name && std::get<1>(attr) == element;
            }), attrs.end());
    }

    void clearAttrs() override {
        attrs.clear();
    }

    int getSelectedAttr() const override {
        return selectedAttr;
    }

    void setSelectedAttr(int idx) override {
        selectedAttr = idx;
        int attr_element = std::get<1>(attrs[idx]);
        // TODO see condition here not very smart maybe abstract renderers ?
        if (attr_element == Element::POINTS) {
            _pointSetRenderer.changeAttribute(std::get<2>(attrs[idx]).get());
        } else 
		    _hexRenderer.changeAttribute(std::get<2>(attrs[idx]).get(), attr_element);
    }

    void updateAttr() {
        setSelectedAttr(selectedAttr);
    }

    std::shared_ptr<Model> getParent() const override {
        return parent;
    }

    void setParent(std::shared_ptr<Model> parentModel) override {
            parent = parentModel;
    }

	private:
	std::string _name;
	std::string _path;

    glm::vec3 position{0, 0, 0};

    bool visible = true;

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
    
    int selectedAttr = 0;
    int selectedColormap = 0;

	// Mesh
	Hexahedra _hex;

	// Renderers
	HexRenderer _hexRenderer;
	PointSetRenderer _pointSetRenderer;

    std::vector<std::tuple<std::string, Element, std::shared_ptr<GenericAttributeContainer>>> attrs;
    // Pointer to parent model, if there is one
    std::shared_ptr<Model> parent;

};