#include <ultimaille/all.h>
#include <string>
#include "core/renderer.h"
#include "hex_renderer.h"

using namespace UM;

struct HexModel : public Renderer {

	// Mesh + Renderer

	HexModel() : _name(""), _path(""), _hex(), _hex_renderer(_hex) {
		
	}

	HexModel(std::string name) : 
		_name(name), 
		_path(""), 
		_hex(),
		_hex_renderer(_hex) {}



	void load(const std::string path) {
		// Load the mesh
		VolumeAttributes attributes = read_by_extension(path, _hex);
		_path = path;
		// Extract name
		if (_name.empty()) {

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

    void update() {
		_hex_renderer.update();
		// TODO _ps_renderer.update();
	}
    
	void init() final override {
		_hex_renderer.init();
		// TODO _ps_renderer.init();
	}
    
	void push() final override {
		_hex_renderer.push();
		// TODO _ps_renderer.push();

		if (colorMode == Renderer::ColorMode::ATTRIBUTE) {
			updateAttr();
		}
	}

    void render() {
		_hex_renderer.render(position);
		// TODO _ps_renderer.render();
	}
    

    void bind() {
		_hex_renderer.bind();
	}
    void clean() {
		_hex_renderer.clean();
	}

	std::string getName() const { return _name; }
	std::string getPath() const { return _path; }


	Hexahedra& getHexahedra() { return _hex; }
	HexRenderer& getRenderer() { return _hex_renderer; }


	// Just call underlying renderer methods
    int getColorMode() {
        return colorMode;
    }

    void setColorMode(Renderer::ColorMode mode) {
        _hex_renderer.setColorMode(mode);
        colorMode = mode;
    }

    bool getLight() {
        return isLightEnabled;
    }

    void setLight(bool enabled) {
        _hex_renderer.setLight(enabled);
        isLightEnabled = enabled;
    }

    bool getLightFollowView() {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) {
		_hex_renderer.setLightFollowView(follow);
        isLightFollowView = follow;
    }

    bool getClipping() {
        return isClipping;
    }

    void setClipping(bool enabled) {
        _hex_renderer.setClipping(enabled);
        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) {
        _hex_renderer.setClippingPlanePoint(p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() {
		_hex_renderer.setClippingPlaneNormal(clippingPlaneNormal);
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) {
		_hex_renderer.setClippingPlaneNormal(n);
        clippingPlaneNormal = n;
    }

    void setInvertClipping(bool invert) {
        _hex_renderer.setInvertClipping(invert);
        invertClipping = invert;
    }

    bool getInvertClipping() {
        return invertClipping;
    }

    float getMeshSize() {
        return meshSize;
    }

    void setMeshSize(float val) {
        _hex_renderer.setMeshSize(val);
        meshSize = val;
    }

    float getMeshShrink() {
        return meshShrink;
    }

    void setMeshShrink(float val) {
		_hex_renderer.setMeshShrink(val);
        meshShrink = val;
    }

    int getFragRenderMode() {
        return fragRenderMode;
    }

    void setFragRenderMode(Renderer::RenderMode mode) {
        _hex_renderer.setFragRenderMode(mode);
        fragRenderMode = mode;
    }

    int getSelectedColormap() const {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) {
		_hex_renderer.setSelectedColormap(idx);
        selectedColormap = idx;
    }

    glm::vec3 getPosition() {
        return position;
    }

    void setPosition(glm::vec3 p) {
        position = p;
    }

    void setHighlight(int idx, bool highlighted) {
        _hex_renderer.setHighlight(idx, highlighted);
    }

    void setHighlight(int idx, float highlight) {
        _hex_renderer.setHighlight(idx, highlight);
    }

    void setHighlight(std::vector<float> highlights) {
        _hex_renderer.setHighlight(highlights);
    }

    void setFilter(int idx, bool filter) {
        _hex_renderer.setFilter(idx, filter);
    }

    std::vector<std::tuple<std::string, int>> getAttrs() {
        std::vector<std::tuple<std::string, int>> result;
        for (const auto& attr : attrs) {
            result.emplace_back(std::get<0>(attr), std::get<1>(attr));
        }
        return result;
    }

    std::tuple<std::string, int> getAttr(int idx) {
        return std::make_tuple(std::get<0>(attrs[idx - 1]), std::get<1>(attrs[idx - 1]));
    }

    void addAttr(Element element, NamedContainer &container) {
        attrs.emplace_back(container.first, element, container.second);
    }

    void removeAttr(const std::string& name, Element element) {
        attrs.erase(std::remove_if(attrs.begin(), attrs.end(),
            [&name, &element](const auto& attr) {
                return std::get<0>(attr) == name && std::get<1>(attr) == element;
            }), attrs.end());
    }

    void clearAttrs() {
        attrs.clear();
    }

    int getSelectedAttr() const {
        return selectedAttr;
    }

    void setSelectedAttr(int idx) {
        selectedAttr = idx;
        int attr_element = std::get<1>(attrs[idx]);
		_hex_renderer.changeAttribute(std::get<2>(attrs[idx]).get(), attr_element);
    }

    void updateAttr() {
        setSelectedAttr(selectedAttr);
    }

    void setView(glm::mat4 &view) {
        _hex_renderer.setView(view);
    }

    void setProjection(glm::mat4 &projection) {
        _hex_renderer.setProjection(projection);
    }

	private:
	std::string _name;
	std::string _path;

    glm::vec3 position{0, 0, 0};

    bool isLightEnabled = true;
    bool isLightFollowView = false;

    bool isClipping = false;
    glm::vec3 clippingPlanePoint;
    glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
    bool invertClipping = false;
    float meshSize = 0.01f;
    float meshShrink = 0.f;
    Renderer::RenderMode fragRenderMode;
    Renderer::ColorMode colorMode = Renderer::ColorMode::COLOR;
    
    int selectedAttr = 0;
    int selectedColormap = 0;

	// Mesh
	Hexahedra _hex;

	// Renderers
	HexRenderer _hex_renderer;
	// PointRenderer _point_renderer;

    std::vector<std::tuple<std::string, Element, std::shared_ptr<GenericAttributeContainer>>> attrs;

};