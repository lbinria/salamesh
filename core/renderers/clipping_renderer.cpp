#include "clipping_renderer.h"
#include "../helpers.h"

struct Result {
    bool valid;                       // true if intersection polygon found
    std::vector<glm::dvec3> points;   // ordered CCW on the plane (3..6 typically)
};

// epsilon helpers
static double eps = 1e-9;
static bool nearlyEqual(double a, double b) { return std::abs(a - b) <= eps; }
static bool vecNearlyEqual(const glm::dvec3 &u, const glm::dvec3 &v) {
    return glm::length(u - v) <= eps;
}

// intersect segment (p0->p1) with plane (planePoint, planeNormal).
// returns (hit, t, point) where point = mix(p0,p1,t)
static bool segmentPlaneIntersect(const glm::dvec3 &p0, const glm::dvec3 &p1,
                                  const glm::dvec3 &planePoint, const glm::dvec3 &planeNormal,
                                  double &outT, glm::dvec3 &outP)
{
    glm::dvec3 u = p1 - p0;
    double denom = glm::dot(planeNormal, u);
    double numer = glm::dot(planeNormal, planePoint - p0);
    if (std::abs(denom) < eps) {
        // segment parallel to plane: either no intersection or infinite (co-planar)
        return false;
    }
    double t = numer / denom;
    if (t < -eps || t > 1.0 + eps) return false;
    outT = glm::clamp(t, 0.0, 1.0);
    outP = p0 + outT * u;
    return true;
}

// compute 2D basis on plane (u,v) given normal n. u is arbitrary orthonormal.
static void planeBasis(const glm::dvec3 &n, glm::dvec3 &u, glm::dvec3 &v)
{
    glm::dvec3 nrm = glm::normalize(n);
    // pick a vector not parallel to nrm
    glm::dvec3 tmp = (std::abs(nrm.x) > 0.9) ? glm::dvec3(0.0,1.0,0.0) : glm::dvec3(1.0,0.0,0.0);
    u = glm::normalize(glm::cross(nrm, tmp));
    v = glm::normalize(glm::cross(nrm, u));
}

// convex hull in 2D (Monotone chain) for points in 2D
static std::vector<int> convexHull2D(const std::vector<glm::dvec2> &pts)
{
    int n = (int)pts.size();
    std::vector<int> idx(n);
    for (int i=0;i<n;++i) idx[i]=i;
    std::sort(idx.begin(), idx.end(), [&](int a, int b){
        if (!nearlyEqual(pts[a].x, pts[b].x)) return pts[a].x < pts[b].x;
        return pts[a].y < pts[b].y;
    });
    auto cross = [&](const glm::dvec2 &o, const glm::dvec2 &a, const glm::dvec2 &b){
        return (a.x - o.x)*(b.y - o.y) - (a.y - o.y)*(b.x - o.x);
    };
    std::vector<int> hull;
    // lower
    for (int i=0;i<n;++i) {
        while (hull.size() >= 2 &&
               cross(pts[hull[hull.size()-2]], pts[hull.back()], pts[idx[i]]) <= eps)
            hull.pop_back();
        hull.push_back(idx[i]);
    }
    // upper
    size_t lower_size = hull.size();
    for (int i = n-1; i>=0; --i) {
        while (hull.size() > lower_size &&
               cross(pts[hull[hull.size()-2]], pts[hull.back()], pts[idx[i]]) <= eps)
            hull.pop_back();
        hull.push_back(idx[i]);
        if (i==0) break;
    }
    if (!hull.empty()) hull.pop_back();
    return hull;
}

// Main function
Result intersectPlaneAABB(const glm::dvec3 &a, const glm::dvec3 &b,
                          const glm::dvec3 &planePoint, const glm::dvec3 &planeNormal)
{
    Result res;
    res.valid = false;
    res.points.clear();

    // build 8 corners of AABB
    glm::dvec3 mins = glm::min(a,b);
    glm::dvec3 maxs = glm::max(a,b);
    glm::dvec3 corners[8];
    int k=0;
    for (int xi=0; xi<2; ++xi)
    for (int yi=0; yi<2; ++yi)
    for (int zi=0; zi<2; ++zi) {
        corners[k++] = glm::dvec3( xi ? maxs.x : mins.x,
                                   yi ? maxs.y : mins.y,
                                   zi ? maxs.z : mins.z );
    }

    // edges as index pairs (12 edges)
    const int edges[12][2] = {
        {0,1},{0,2},{0,4},{1,3},{1,5},{2,3},
        {2,6},{3,7},{4,5},{4,6},{5,7},{6,7}
    };

    std::vector<glm::dvec3> hits;
    for (int i=0;i<12;++i) {
        glm::dvec3 p0 = corners[edges[i][0]];
        glm::dvec3 p1 = corners[edges[i][1]];
        double t; glm::dvec3 ip;
        if (segmentPlaneIntersect(p0,p1,planePoint,planeNormal,t,ip)) {
            // dedupe
            bool dup = false;
            for (auto &h : hits) if (vecNearlyEqual(h, ip)) { dup = true; break; }
            if (!dup) hits.push_back(ip);
        }
    }

    if (hits.empty()) return res;

    // project hits to 2D coordinates on plane
    glm::dvec3 u, v;
    planeBasis(planeNormal, u, v);
    std::vector<glm::dvec2> pts2;
    pts2.reserve(hits.size());
    for (auto &h : hits) {
        glm::dvec3 d = h - planePoint;
        pts2.emplace_back(glm::dot(d, u), glm::dot(d, v));
    }

    // compute convex hull of 2D points (indexes)
    std::vector<int> hullIdx = convexHull2D(pts2);
    if (hullIdx.empty()) return res;

    // produce ordered 3D points along hull
    for (int idx : hullIdx) {
        res.points.push_back(hits[idx]);
    }
    res.valid = true;
    return res;
}

void ClippingRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	shader.use();

	// VBO
	GLuint pLoc = glGetAttribLocation(shader.id, "p");
	glEnableVertexAttribArray(pLoc);
	glVertexAttribPointer(pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p));

}

void ClippingRenderer::push() {

	std::vector<Vertex> vertices;

	// Search bbox
	glm::vec3 pMin{FLT_MAX, FLT_MAX, FLT_MAX};
	glm::vec3 pMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};
	for (auto &p : ps) {
		glm::vec3 glmP = sl::um2glm(p);
		pMin = glm::min(pMin, glmP);
		pMax = glm::max(pMax, glmP);
	}

	auto res = intersectPlaneAABB(
		glm::dvec3(pMin), glm::dvec3(pMax),
		glm::dvec3(clippingPlanePoint),
		glm::dvec3(clippingPlaneNormal)
	);

	if (res.valid) {
		if (res.points.size() == 3) {
			for (auto &p : res.points) {
				vertices.push_back({ glm::vec3(p) });
			}
		} else if (res.points.size() >= 4) {
			// quad -> 2 triangles
			vertices.push_back({ glm::vec3(res.points[0]) });
			vertices.push_back({ glm::vec3(res.points[1]) });
			vertices.push_back({ glm::vec3(res.points[2]) });
			vertices.push_back({ glm::vec3(res.points[0]) });
			vertices.push_back({ glm::vec3(res.points[2]) });
			vertices.push_back({ glm::vec3(res.points[3]) });
		}
	}

	nverts = vertices.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
}

void ClippingRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	setPosition(position);

	glDrawArrays(GL_TRIANGLES, 0, nverts);
}

void ClippingRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	nverts = 0;
}

void ClippingRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader.clean();
}