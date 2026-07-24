#pragma once
#include <map>
#include <vector>
#include <set>
#include <optional>
#include "element_type.h"

enum PickElement {
	PICK_VERTEX,
	PICK_HALFEDGE,
	PICK_FACET,
	PICK_CELL,
	PICK_MESH,
	PICK_ELEMENT_COUNT
};

static std::string pickElementToString(PickElement e) {
	switch (e) {
		case PickElement::PICK_VERTEX: return "vertices";
		case PickElement::PICK_HALFEDGE: return "edges";
		case PickElement::PICK_FACET: return "facets";
		case PickElement::PICK_CELL: return "cells";
		case PickElement::PICK_MESH: return "meshes";
		case PickElement::PICK_ELEMENT_COUNT: return "count";
		default: return "unknown";
	}
}

struct PickResult {

	PickResult() : PickResult(0, 0) {}
	PickResult(int w, int h) : _w(w), _h(h) {
		ids.resize(w * h, -1);
	}

	void set(int x, int y, long id) {
		ids[x * _w + y] = id;
	}

	long get(int x, int y) const {
		return ids[x * _w + y];
	}

	bool exists(int x, int y) const {
		return ids[x * _w + y] >= 0;
	}

	void erase(int x, int y) {
		ids[x * _w + y] = -1;
	}

	auto begin() { return ids.begin(); }
	auto end() { return ids.end(); }
	auto begin() const { return ids.begin(); }
	auto end() const { return ids.end(); }

	long count() const{ return ids.size(); }

	std::vector<long> getIndexedIds() const {
		return ids;
	}

	std::vector<long> getIds() const {
		std::set<long> id_set(ids.begin(), ids.end());
		id_set.erase(-1L);
		return std::vector<long>(id_set.begin(), id_set.end());
	}

	int getWidth() const { return _w; }
	int getHeight() const { return _h; }

	private:
		std::vector<long> ids;
		int _w, _h;
};

struct PickState {

	PickState() = default;

	PickState(std::array<PickResult, PickElement::PICK_ELEMENT_COUNT> results) : _results(results) {

	}

	PickResult& getResult(PickElement e) {
		return _results[e];
	}

	PickResult getResult(long meshId, PickElement e) {

		auto &meshResult = _results[PickElement::PICK_MESH];
		auto ids = meshResult.getIndexedIds();

		PickResult res(meshResult.getWidth(), meshResult.getHeight());

		for (int x = 0; x < meshResult.getWidth(); ++x) {
			for (int y = 0; y < meshResult.getHeight(); ++y) {
				int i = x * meshResult.getWidth() + y;
				int id = ids[i];

				if (id != meshId)
					continue;
				
				auto &result = _results[e];
				if (!result.exists(x, y))
					continue;
				
				res.set(x, y, result.get(x, y));
			}
		}

		return res;
	}

	std::vector<long> getIds(PickElement e) {
		auto ids = getResult(e).getIndexedIds();
		std::set<long> id_set(ids.begin(), ids.end());
		id_set.erase(-1L);
		return std::vector<long>(id_set.begin(), id_set.end());
	}

	std::vector<long> getIds(long meshId, PickElement e) {
		auto ids = getResult(meshId, e).getIndexedIds();
		std::set<long> id_set(ids.begin(), ids.end());
		id_set.erase(-1L);
		return std::vector<long>(id_set.begin(), id_set.end());
	}

	// std::vector<long> getIds(long meshId, PickElement e) {
	// 	auto ids = getResult(meshId, e).getIds();
	// 	// std::sort(ids.begin(), ids.end());
	// 	// auto it = std::unique(ids.begin(), ids.end());
	// 	// ids.erase(it, ids.end());
	// 	// it = ids.begin();
	// 	// if (*it == -1) {
	// 	// 	ids.erase(it);
	// 	// }
	// 	// return ids;
	// 	return ids;
	// }

	bool any(PickElement e) {
		return count(e) > 0;
	}

	bool any(long meshId, PickElement e) {
		return count(meshId, e) > 0;
	}

	long count(PickElement e) {
		return _results[e].count();
	}

	long count(long meshId, PickElement e) {
		auto result = getResult(meshId, e);
		return result.count();
	}

	private:
		std::array<PickResult, PickElement::PICK_ELEMENT_COUNT> _results;
};