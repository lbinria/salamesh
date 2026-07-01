#pragma once
#include <map>
#include <vector>
#include <optional>
#include "element_type.h"

enum PickElement {
	PICK_VERTEX,
	PICK_FACET,
	PICK_CELL,
	PICK_MESH,
	PICK_ELEMENT_COUNT
};

struct PickResult {

	void set(int x, int y, long id) {
		ids[{x, y}] = id;
	}

	long get(int x, int y, long defaultId = -1) const {
		auto it = ids.find({x, y});
		return (it != ids.end()) ? it->second : defaultId;
	}

	bool exists(int x, int y) const {
		return ids.find({x, y}) != ids.end();
	}

	void erase(int x, int y) {
		ids.erase({x, y});
	}

	auto begin() { return ids.begin(); }
	auto end() { return ids.end(); }
	auto begin() const { return ids.begin(); }
	auto end() const { return ids.end(); }

	long count() const{ return ids.size(); }

	std::vector<long> getIds() const {
		std::vector<long> result;
		result.reserve(ids.size());
		for (const auto& [coord, id] : ids) {
			result.push_back(id);
		}
		return result;
	}

	private:
		std::map<std::pair<int, int>, long> ids;
};

struct PickResultId {

	long id[PickElement::PICK_ELEMENT_COUNT];

	long get(PickElement kind) const {
		return id[kind];
	}

};

struct PickState {

	PickState(std::array<PickResult, PickElement::PICK_ELEMENT_COUNT> results) : _results(results) {

	}

	PickResult getResult(PickElement e) {
		return _results[e];
	}

	PickResult getResult(long meshId, PickElement e) {
		PickResult res;
		for (auto &[xy, id] : _results[PickElement::PICK_MESH]) {
			auto [x, y] = xy;
			
			if (id != meshId)
				continue;

			auto result = _results[e];

			if (!result.exists(x, y))
				continue;

			res.set(x, y, result.get(x, y));
		}

		return res;
	}

	std::vector<long> getIds(PickElement e) {
		std::vector<long> ids;
		auto result = getResult(e);
		ids.reserve(result.count());
		for (const auto& [coord, id] : result) {
			ids.push_back(id);
		}
		return ids;
	}

	std::vector<long> getIds(long meshId, PickElement e) {
		std::vector<long> ids;
		auto result = getResult(meshId, e);
		ids.reserve(result.count());
		for (const auto& [coord, id] : result) {
			ids.push_back(id);
		}
		return ids;
	}

	bool any(PickElement e) {
		return _results[e].count() > 0;
	}

	long count(PickElement e) {
		return _results[e].count();
	}

	private:
		std::array<PickResult, PickElement::PICK_ELEMENT_COUNT> _results;
};