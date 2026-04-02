#pragma once

#include "../element.h"
#include "../layer.h"
#include "../attribute.h"
#include "../renderers/renderer.h"
#include "../renderers/mesh_renderer.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"

// #include "../models/model_view.h"

#include "../../include/glm/glm.hpp"
#include "../../include/json.hpp"

#include <ultimaille/all.h>

#include <optional>

using namespace UM;
using json = nlohmann::json;

struct ModelView;

struct Model {

	Model (const Model&) = delete;
	Model& operator= (const Model&) = delete;

	Model(std::string name, std::map<std::string, std::shared_ptr<Renderer>> renderers) :  
	name(name.empty() ? sl::generateGuid() : name),
	_path(""),
	_renderers(std::move(renderers)) {
		index = maxIndex;
		++maxIndex;
	}

	template<class T>
	T& as() {
		static_assert(std::is_base_of_v<Model, T>, "Model::as() can only be used with derived classes of Model");
		return static_cast<T&>(*this);
	}

	virtual ModelType getModelType() const = 0; 
	virtual int getDim() const = 0; 

	virtual bool load(const std::string path) = 0;
	virtual bool saveAs(const std::string path, std::vector<Attribute> attrs) const = 0;

	bool saveAs(const std::string path) const {
		return saveAs(path, attrs);
	}


	bool save() const {
		return saveAs(_path);
	}

	bool saveState(std::string dirPath, json &j) /*const*/;
	void loadState(json &j);

	virtual void doLoadState(json &j) {};
	virtual void doSaveState(json &j) const {};

	std::string getPath() const { return _path; }

	static constexpr const char* clippingModeStrings[2] = {"Cell", "Std"};
	constexpr std::array<std::string_view, 2> getClippingModeStrings() {
		return {clippingModeStrings[0], clippingModeStrings[1]};
	}

	// Lifecycle functions
	void init() {
		for (auto const &[k, r] : _renderers) {
			r->init();
		}
	}

	void push();



	ModelView getDefaultView(std::string viewName);
	ModelView& getView(std::string viewName);

	void render(ModelView &modelView);


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
	// Mesh manipulations (just simple ones)
	virtual void vert(int i, int lv, int vi) = 0;
	virtual int createElements(int n, int size = 0) = 0;
	virtual void deleteElements(std::vector<int> indexes) = 0;
	virtual void deleteIsolatedVertices() = 0;
	virtual void deleteVertices(std::vector<int> indexes) = 0;

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
			<< " was not found on "
			<< modelTypeToString(getModelType())
			<< std::endl;
		return std::nullopt;
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






	Renderer::ClippingMode getClippingMode() const {
		return clippingMode;
	}

	void setClippingMode(Renderer::ClippingMode mode) {
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

	int getIndex() {
		return index;
	}

	static inline int getMaxIndex() {
		return maxIndex;
	}

	static void clearIndex() {
		maxIndex = 0;
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

	std::shared_ptr<MeshRenderer> getMeshRenderer() const {
		if(_renderers.contains("mesh_renderer"))
			return std::static_pointer_cast<MeshRenderer>(_renderers.at("mesh_renderer"));
		
		return nullptr;
	}

	bool hasRenderer(std::string name) {
		return _renderers.contains(name);
	}

	std::map<std::string, std::shared_ptr<Renderer>> getRenderers() const {
		return _renderers;
	}

	std::shared_ptr<Renderer> getRenderer(const std::string name) const {
		return _renderers.at(name);
	}

	virtual long pickEdge(glm::vec3 p0, int c) = 0;

	NamedContainer getAttributeContainer(std::string name, ElementKind kind) const {
		auto containers = getAttributeContainers();
		for (auto &c : containers) {
			if (c.first ==  kind && c.second.name == name) {
				return c.second;
			}
		}

		throw std::runtime_error("Container " + name + " of kind " + elementKindToString(kind) + " not found.");
	}

	virtual Attribute bindAttr(std::string attrName, ElementKind kind, ElementType type) = 0;

	std::string getName() { return name; }

	protected:
	std::string _path;

	glm::vec3 position{0, 0, 0};
	glm::vec3 rotation{0, 0, 0};
	bool visible = true;

	// Pointer to parent model, if there is one
	std::shared_ptr<Model> parent;

	std::vector<Attribute> attrs;

	Renderer::ClippingMode clippingMode = Renderer::ClippingMode::STD;
	bool isClipping = false;
	glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
	glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
	bool invertClipping = false;
	


	// Renderers
	std::map<std::string, std::shared_ptr<Renderer>> _renderers;

	virtual std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const = 0;


	
	void addAttr(ElementKind kind, NamedContainer &container);



	private:
	std::string name;
	static inline int maxIndex = 0;
	int index;



	std::map<std::string, ModelView> views;

};