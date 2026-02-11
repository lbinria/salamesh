// #pragma once

// #include "../../include/json.hpp"
// #include <ultimaille/all.h>
// #include <string>
// #include "model.h"
// #include "../renderers/point_set_renderer.h"
// #include "../renderers/halfedge_renderer.h"
// #include "../renderers/tet_renderer.h"
// #include "../renderers/bbox_renderer/bbox_renderer.h"
// #include "../renderers/clipping_renderer.h"

// #include "../layer.h"
// #include "../helpers.h"


// using namespace UM;
// using json = nlohmann::json;

// struct TetModel final : public Model {

//     TetModel() : 
//         _m(), 
//         Model::Model({
//             {"mesh_renderer", std::make_shared<TetRenderer>(_m)}, 
//             {"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
//             {"edge_renderer", std::make_shared<VolumeHalfedgeRenderer>(_m) },
//             {"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
//             {"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
//         })
//         {}

//     ModelType getModelType() const override {
//         return ModelType::TET_MODEL;
//     }


// 	bool load(const std::string path) override;
// 	bool saveAs(const std::string path, std::vector<Attribute> attrs) const override;

// 	Tetrahedra& getTetrahedra() { return _m; }
// 	VolumeAttributes& getVolumeAttributes() { return _volumeAttributes; }

//     int nverts() const override {
//         return _m.nverts();
//     }

//     int nfacets() const override {
//         return _m.nfacets();
//     } 

//     int ncells() const override {
//         return _m.ncells();
//     } 

//     int ncorners() const override {
//         return _m.ncorners();
//     }

//     int nhalfedges() const override {
//         return _m.ncells() * 12;
//     }

//     std::tuple<glm::vec3, glm::vec3> bbox() override {
//         glm::vec3 min = glm::vec3(FLT_MAX);
//         glm::vec3 max = glm::vec3(-FLT_MAX);

//         for (auto &v : _m.iter_vertices()) {
//             glm::vec3 p = sl::um2glm(v.pos());
//             min = glm::min(min, p);
//             max = glm::max(max, p);
//         }

//         return {min, max};
//     }

//     long pickEdge(glm::vec3 p0, int c) override {
//         // Search nearest edge
//         double min_d = std::numeric_limits<double>().max();
//         long found_e = -1;
        
//         for (int le = 0; le < 12; ++le) {
//             long e = c * 12 + le;

//             // Get local points indices of edge extremities
//             int lv0 = reference_cells[0].facets[le % 3 + (le / 3 * 3)];
//             int lv1 = reference_cells[0].facets[(le + 1) % 3 + (le / 3 * 3)];

//             // Get global index of points
//             int v0 = _m.vert(c, lv0);
//             int v1 = _m.vert(c, lv1);

//             // Get points from current edge
//             vec3 p1 = _m.points[v0];
//             vec3 p2 = _m.points[v1];
//             vec3 b = (p1 + p2) * .5;
//             // Compute dist from picked point to bary of edge points
//             double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

//             // Keep min dist
//             if (d < min_d) {
//                 min_d = d;
//                 found_e = e;
//             }
//         }

//         return found_e;
//     }

//     protected:

//     // TODO refactor into volume_model
// 	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
// 		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
// 		for (auto &c : _volumeAttributes.points)
// 			containers.push_back({ElementKind::POINTS_ELT, c});
// 		for (auto &c : _volumeAttributes.cell_corners)
// 			containers.push_back({ElementKind::CELL_CORNERS_ELT, c});
// 		for (auto &c : _volumeAttributes.cell_facets)
// 			containers.push_back({ElementKind::CELL_FACETS_ELT, c});
// 		for (auto &c : _volumeAttributes.cells)
// 			containers.push_back({ElementKind::CELLS_ELT, c});

// 		return containers;
// 	}

//     private:

//     // Mesh
//     Tetrahedra _m;
//     VolumeAttributes _volumeAttributes;

// };