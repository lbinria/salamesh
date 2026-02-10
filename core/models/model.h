#pragma once

#include "../element.h"
#include "../layer.h"
#include "../attribute.h"
#include "../renderers/renderer.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"

#include "../../include/glm/glm.hpp"
#include "../../include/json.hpp"

#include <ultimaille/all.h>

#include <optional>

using namespace UM;
using json = nlohmann::json;

struct Model {

	std::function<bool(Model& /*self*/, const std::string /*path*/)> loadCallback;

	Model (const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(std::map<std::string, std::shared_ptr<IRenderer>> renderers) :
	_name(""),
	_path(""),
	_renderers(std::move(renderers)) {}

	Model(std::map<std::string, std::shared_ptr<IRenderer>> renderers, std::string name) : 
	_name(name), 
	_path(""),
	_renderers(std::move(renderers)) {}

	template<typename T>
	T& as() {
		static_assert(std::is_base_of_v<Model, T>, "Model::as() can only be used with derived classes of Model");
		return static_cast<T&>(*this);
	}

	virtual ModelType getModelType() const = 0;

	virtual bool load(const std::string path) = 0;
	virtual bool saveAs(const std::string path, std::vector<Attribute> attrs) const = 0;

	bool saveAs(const std::string path) const {
		return saveAs(path, attrs);
	}


	bool save() const {
		return saveAs(_path);
	}

	void saveState(std::string dirPath, json &j) /*const*/;
	void loadState(json &j);

	virtual void doLoadState(json &j) {};
	virtual void doSaveState(json &j) const {};

	std::string getName() const { return _name; }
	void setName(std::string name) { _name = name; }
	std::string getPath() const { return _path; }

	static constexpr const char* clippingModeStrings[2] = {"Cell", "Std"};
	constexpr std::array<std::string_view, 2> getClippingModeStrings() {
		return {clippingModeStrings[0], clippingModeStrings[1]};
	}

	// Lifecycle functions
	void init() {
		for (auto const &[k, r] : _renderers)
			r->init();
	}

	void push() {
		for (auto const &[k, r] : _renderers)
			r->push();

		// Push highlight and filter attributes if they exist
		for (auto [k, isActivated] : activatedLayers) {
			if (!isActivated) 
				continue;

			auto [layer, kind] = k;
			updateLayer(layer, kind);
		}
	}

	void render() {
		if (!visible)
			return;
		
		glm::vec3 pos = getWorldPosition();

		for (auto const &[k, r] : _renderers)
			r->render(pos);
	}

	void clean() {
		for (auto const &[k, r] : _renderers)
			r->clean();
	}

	// Mesh info
	virtual int nverts() const = 0; 
	virtual int nfacets() const = 0; 
	virtual int ncells() const = 0; 
	virtual int ncorners() const = 0; 
	virtual int nhalfedges() const = 0;
	virtual std::tuple<glm::vec3, glm::vec3> bbox() = 0;

	glm::vec3 getCenter() {
		auto [bmin, bmax] = bbox();
		return (bmin + bmax) / 2.f;
	}

	double getRadius() {
		auto [bmin, bmax] = bbox();
		return glm::length(bmax - bmin) / 2.f;
	}


	void removeAttr(int idx);
	void removeAttr(ElementKind kind, std::string name);

	std::vector<Attribute> getAttrs() const {
		return attrs;
	}

	Attribute getAttr(int idx) const {
		return attrs[idx];
	}

	std::optional<Attribute> getAttr(ElementKind kind, std::string name) const {
		for (auto &a : attrs) {
			if (a.getName() == name && a.getKind() == kind)
				return a;
		}

		return std::nullopt;
	}

	void clearAttrs() {
		attrs.clear();
	}

	int getSelectedAttr(ColormapLayer colormapLayer) {
		return selectedAttr[static_cast<int>(colormapLayer)];
	}

	void setSelectedAttr(int idx, ColormapLayer layer);

	// void updateAttrs() {
	// 	for (int l = 0; l < 3; ++l)
	// 		setSelectedAttr(selectedAttr[l], static_cast<ColormapLayer>(l));
	// }

	// // TODO move out of this, in element.h
	// template<typename T>
	// ElementType deduceType(GenericAttribute<T> &attr) {
	// 	if constexpr (std::is_same_v<T, double>) {
	// 		return ElementType::DOUBLE_ELT;
	// 	} else if constexpr (std::is_same_v<T, int>) {
	// 		return ElementType::INT_ELT;
	// 	} else if constexpr (std::is_same_v<T, bool>) {
	// 		return ElementType::BOOL_ELT;
	// 	} else if constexpr (std::is_same_v<T, vec2>) {
	// 		return ElementType::VEC2_ELT;
	// 	} else if constexpr (std::is_same_v<T, vec3>) {
	// 		return ElementType::VEC3_ELT;
	// 	} else {
	// 		throw std::runtime_error("Unknown attribute type for container: " + attr.getName());
	// 	}
	// }
	// std::optional<Attribute> findAttr(std::string name, ElementKind kind) {
	// 	for (const auto &a : attrs) {
	// 		if (a.getName() == name && a.getKind() == kind) {
	// 			return a;
	// 		}
	// 	}
	// 	return std::nullopt;
	// }
	// ElementKind umAttributeKind2ElementKind(AttributeBase::TYPE kind) {
	// 	switch (kind) {
	// 		case AttributeBase::POINTS: return ElementKind::POINTS_ELT;
	// 		case AttributeBase::EDGES: return ElementKind::EDGES_ELT;
	// 		case AttributeBase::FACETS: return ElementKind::FACETS_ELT;
	// 		case AttributeBase::CORNERS: return ElementKind::CORNERS_ELT;
	// 		case AttributeBase::CELLS: return ElementKind::CELLS_ELT;
	// 		case AttributeBase::CELLFACETS: return ElementKind::CELL_FACETS_ELT;
	// 		case AttributeBase::CELLCORNERS: return ElementKind::CELL_CORNERS_ELT;
	// 		default: throw std::runtime_error("Unknown attribute kind for binding: " + std::to_string(kind));
	// 	}
	// }

	void setSelectedColormap(int idx, ColormapLayer layer) {
		selectedColormap[static_cast<int>(layer)] = idx;
	}

	int getSelectedColormap(ColormapLayer layer) const {
		return selectedColormap[static_cast<int>(layer)];
	}

	// Choose which attribute to bind to layer / kind
	void setLayerAttr(std::string name, Layer layer, ElementKind kind) {
		attrNameByLayerAndKind[{layer, kind}] = name;
	}

	std::string getLayerAttr(Layer layer, ElementKind kind) {
		std::tuple<Layer, ElementKind> k = {layer, kind};
		if (attrNameByLayerAndKind.contains(k))
			return attrNameByLayerAndKind[k];
		
		return defaultAttrName(layer);
	}

	inline std::string getColormapAttr(ElementKind kind, ColormapLayer layer) {
		return getLayerAttr(static_cast<Layer>(layer), kind);
	}

	inline void setColormapAttr(std::string name, ElementKind kind, ColormapLayer layer) {
		setLayerAttr(name, static_cast<Layer>(layer), kind);
	}

	inline std::string getHighlightAttr(ElementKind kind) {
		return getLayerAttr(Layer::HIGHLIGHT, kind);
	}

	inline void setHighlightAttr(std::string name, ElementKind kind) {
		setLayerAttr(name, Layer::HIGHLIGHT, kind);
	}

	inline std::string getFilterAttr(ElementKind kind) {
		return getLayerAttr(Layer::HIGHLIGHT, kind);
	}

	inline void setFilterAttr(std::string name, ElementKind kind) {
		setLayerAttr(name, Layer::FILTER, kind);
	}

	bool isLayerActivated(ElementKind kind, Layer layer) {
		return activatedLayers[{layer, kind}];
	}

	void resetLayer(ElementKind kind, Layer layer);

	void setLayer(ElementKind kind, Layer layer, bool update = true);
	// TODO maybe reset parameter won't be used, so layer never need to be reseted in this context
	void unsetLayer(ElementKind kind, Layer layer, bool reset = false);

	inline void setColormap(ElementKind kind, ColormapLayer layer, bool update = true) {
		setLayer(kind, static_cast<Layer>(layer), update);
	}

	void unsetColormap(ElementKind kind, ColormapLayer colormapLayer) {
		unsetLayer(kind, static_cast<Layer>(colormapLayer));
	}

	void unsetColormaps(ColormapLayer colormapLayer) {
		// Unset all
		for (int i = 0; i < 7; ++i) {
			int kind = 1 << i;
			unsetColormap((ElementKind)kind, colormapLayer);
		}
	}

	void unsetColormaps(ElementKind kind) {
		for (int l = 0; l < 3; ++l)
			unsetColormap(kind, static_cast<ColormapLayer>(l));
	}

	void unsetColormaps() {
		// Unset all
		for (int l = 0; l < 3; ++l) {
			for (int i = 0; i < 7; ++i) {
				int kind = 1 << i;
				unsetColormap((ElementKind)kind, static_cast<ColormapLayer>(l));
			}
		}
	}

	inline void setHighlight(ElementKind kind, bool update = true) {
		setLayer(kind, Layer::HIGHLIGHT, update);
	}

	inline void unsetHighlight(ElementKind kind) {
		unsetLayer(kind, Layer::HIGHLIGHT);
	}

	void unsetHighlights() {
		// Unset all
		for (int i = 0; i < 7; ++i) {
			int kind = 1 << i;
			unsetHighlight((ElementKind)kind);
		}
	}

	inline void setFilter(ElementKind kind, bool update = true) {
		setLayer(kind, Layer::FILTER, update);
	}

	inline void unsetFilter(ElementKind kind) {
		unsetLayer(kind, Layer::FILTER);
	}

	void unsetFilters() {
		// Unset all
		for (int i = 0; i < 7; ++i) {
			int kind = 1 << i;
			unsetFilter((ElementKind)kind);
		}
	}

	glm::vec3 getPosition() const {
		return position;
	}

	void setPosition(glm::vec3 p) {
		position = p;
	}

	bool getVisible() const {
		return visible;
	}

	void setVisible(bool v) {
		visible = v;
	}

	std::shared_ptr<Model> getParent() const {
		return parent;
	}

	void setParent(std::shared_ptr<Model> parentModel) {
			parent = parentModel;
	}

	glm::vec3 getWorldPosition() const {
		if (parent) {
			return parent->getWorldPosition() + position;
		} else {
			return position;
		}
	}

	// Renderer functions

	// TODO maybe protected
	void setColormap0Texture(unsigned int tex) {
		for (auto const &[k, r] : _renderers)
			r->setColormap0Texture(tex);
	}
	// TODO maybe protected
	void setColormap1Texture(unsigned int tex) {
		for (auto const &[k, r] : _renderers)
			r->setColormap1Texture(tex);
	}
	// TODO maybe protected
	void setColormap2Texture(unsigned int tex) {
		for (auto const &[k, r] : _renderers)
			r->setColormap2Texture(tex);
	}

	bool getLight() const {
		return isLightEnabled;
	}

	void setLight(bool enabled) {
		for (auto const &[k, r] : _renderers)
			r->setLight(enabled);

		isLightEnabled = enabled;
	}

	bool getLightFollowView() const {
		return isLightFollowView;
	}

	void setLightFollowView(bool follow) {
		for (auto const &[k, r] : _renderers)
			r->setLightFollowView(follow);

		isLightFollowView = follow;
	}

	IRenderer::ClippingMode getClippingMode() const {
		return clippingMode;
	}

	void setClippingMode(IRenderer::ClippingMode mode) {
		for (auto const &[k, r] : _renderers)
			r->setClippingMode(mode);

		clippingMode = mode;
	}

	bool getClipping() const {
		return isClipping;
	}

	void setClipping(bool enabled) {
		for (auto const &[k, r] : _renderers)
			r->setClipping(enabled);

		isClipping = enabled;
	}

	glm::vec3 getClippingPlanePoint() const {
		return clippingPlanePoint;
	}

	void setClippingPlanePoint(glm::vec3 p) {
		for (auto const &[k, r] : _renderers)
			r->setClippingPlanePoint(p);

		clippingPlanePoint = p;
	}

	glm::vec3 getClippingPlaneNormal() const {
		return clippingPlaneNormal;
	}

	void setClippingPlaneNormal(glm::vec3 n) {
		for (auto const &[k, r] : _renderers)
			r->setClippingPlaneNormal(n);

		clippingPlaneNormal = n;
	}

	bool getInvertClipping() const {
		return invertClipping;
	}

	void setInvertClipping(bool invert) {
		for (auto const &[k, r] : _renderers)
			r->setInvertClipping(invert);

		invertClipping = invert;
	}

	void setupClipping() {
		setClippingPlanePoint(getCenter());
		setClippingPlaneNormal({1, 0, 0});
	}

	void setMeshIndex(int index) {
		for (auto const &[k, r] : _renderers)
			r->setMeshIndex(index);
	}

	// Renderer getters
	PointSetRenderer& getPointsRenderer() {
		return *static_cast<PointSetRenderer*>(_renderers.at("point_renderer").get());
	}

	std::shared_ptr<HalfedgeRenderer> getEdgesRenderer() {
		if (_renderers.contains("edge_renderer"))
			return  std::static_pointer_cast<HalfedgeRenderer>(_renderers.at("edge_renderer"));
			
		return nullptr;
	}

	IRenderer& getMeshRenderer() const {
		// Warning, I do that here because I have the garantee that _meshRenderer is always initialized
		// If _meshRenderer is uninitialized, this will throw a segfault
		// (I don't want to transfer ownership)
		// Maybe there is a better way to do that
		return *_renderers.at("mesh_renderer");
	}

	bool hasRenderer(std::string name) {
		return _renderers.contains(name);
	}

	std::map<std::string, std::shared_ptr<IRenderer>> getRenderers() const {
		return _renderers;
	}

	std::shared_ptr<IRenderer> getRenderer(const std::string name) const {
		return _renderers.at(name);
	}

	// TODO maybe remove that later, using screen RBO to get edge ?
	virtual long pickEdge(glm::vec3 p0, int c) = 0;

	protected:
	std::string _name;
	std::string _path;

	glm::vec3 position{0, 0, 0};
	glm::vec3 rotation{0, 0, 0};
	bool visible = true;

	// Pointer to parent model, if there is one
	std::shared_ptr<Model> parent;

	std::vector<Attribute> attrs;

	int selectedAttr[3] = {-1, -1, -1};


	bool isLightEnabled = true;
	bool isLightFollowView = false;

	IRenderer::ClippingMode clippingMode = IRenderer::ClippingMode::STD;
	bool isClipping = false;
	glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
	glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
	bool invertClipping = false;
	

	int selectedColormap[3] = {0, 0, 0};

	// Renderers
	std::map<std::string, std::shared_ptr<IRenderer>> _renderers;

	virtual std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const = 0;
	
	void addAttr(ElementKind kind, NamedContainer &container);

	int getAttributeNDims(std::string attrName, ElementKind kind) {

		// TODO refactor, factorize that
		auto containers = getAttributeContainers();

		for (auto &[k, c] : containers) {
			if (k == kind && c.name == attrName) {
				auto cb = c.ptr.get();
				if (auto a = dynamic_cast<AttributeContainer<vec3>*>(cb)) {
					return 3;
				} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(cb)) {
					return 2;
				} else {
					return 1;
				}
			}
		}

		return -1;


	}

	// TODO maybe move into helper
	std::optional<std::vector<float>> getAttrData(std::string attrName, ElementKind kind, int selectedDim = -1) {


		auto containers = getAttributeContainers();

		for (auto &[k, c] : containers) {
			if (k == kind && c.name == attrName) {
				return sl::getContainerData(c.ptr.get(), selectedDim);
			}
		}

		std::cerr << "Error: Model::getAttrData(). Attribute "
			<< attrName 
			<< " of kind "
			<< elementKindToString(kind) 
			<< " is not supported on "
			<< modelTypeToString(getModelType())
			<< std::endl;
		return std::nullopt;
	}

	void updateLayer(Layer layer, ElementKind kind) {

		auto attrName = getLayerAttr(layer, kind);

		// TODO refactor, maybe move getNDims and ndim hacks out of updateLayer & pass as parameters
		int nDims = getAttributeNDims(attrName, kind);

		// Extract selectedDim (hack here... extract from string)
		int selectedDim = -1;
		auto lbrPos = attrName.find('[');
		auto rbrPos = attrName.find(']');
		// if attr[0] for example, we would like to see selectedDim=0, so nDims of attribute = 1
		if (lbrPos != std::string::npos && rbrPos != std::string::npos) {
			selectedDim = std::stoi(attrName.substr(lbrPos + 1, rbrPos - lbrPos));
			attrName = attrName.substr(0, lbrPos);
			nDims = 1;
		}

		auto data_opt = getAttrData(attrName, kind, selectedDim);

		// Silent when no data ?
		// If no data => it means that attr name wasn't found
		if (!data_opt.has_value())
			return;

		auto data = data_opt.value();

		auto [min, max] = sl::getRange(data);

		for (auto const &[k, r] : _renderers) {
			if (r->isRenderElement(kind)) {
				r->setLayerRange(layer, min, max);
				r->setLayerNDims(layer, nDims);
				r->setLayer(data, layer);
			}
		}
	}

	private:
	
	std::map<std::tuple<Layer, ElementKind>, std::string> attrNameByLayerAndKind;
	std::map<std::tuple<Layer, ElementKind>, bool> activatedLayers;
};