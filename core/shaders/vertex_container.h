#pragma once

class VertexContainer {
public:
	virtual ~VertexContainer() = default;
};

class EmptyVertexContainer : public VertexContainer {};

template<typename TVertex>
class ConcreteVertexContainer : public VertexContainer {
public:
	std::vector<TVertex> vertices;

	const std::vector<TVertex>& get() const {
		return vertices;
	}

	std::vector<TVertex>& get() {
		return vertices;
	}

};