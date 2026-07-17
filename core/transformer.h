#pragma once

#include <memory>
#include <map>
#include <functional>
#include <typeinfo>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "shader_base.h"
#include "mesh.h"

// ============ Type-Erased Transform Function ============
class TransformFunction {
public:
    virtual ~TransformFunction() = default;
    virtual std::unique_ptr<VertexContainer> execute(ShaderBase& shader, Mesh& mesh) = 0;
};

template<typename TShader, typename TMesh>
class ConcreteTransformFunction : public TransformFunction {
public:
    using VertexType = typename TShader::Vertex;
    using TransformFunc = std::function<std::vector<VertexType>(TShader&, const TMesh&)>;

private:
    TransformFunc func;

public:
    ConcreteTransformFunction(TransformFunc f) : func(f) {}

    std::unique_ptr<VertexContainer> execute(ShaderBase& shader, Mesh& mesh) override {
        auto& typed_shader = static_cast<TShader&>(shader);
        auto& typed_mesh = static_cast<TMesh&>(mesh);

        auto result = std::make_unique<ConcreteVertexContainer<VertexType>>();
        result->vertices = func(typed_shader, typed_mesh);
        return result;
    }
};

// ============ Transform Registry ============
class TransformRegistry {
private:
    struct Key {
        const std::type_info* shader_type;
        const std::type_info* mesh_type;

        bool operator<(const Key& other) const {
            if (shader_type->before(*other.shader_type))
                return true;
            if (other.shader_type->before(*shader_type))
                return false;
            return mesh_type->before(*other.mesh_type);
        }
    };

    std::map<Key, std::unique_ptr<TransformFunction>> functions;

public:
    template<typename TShader, typename TMesh>
    void registerTransform(
        std::function<std::vector<typename TShader::Vertex>(TShader&, const TMesh&)> func
    ) {
        Key key{&typeid(TShader), &typeid(TMesh)};
        functions[key] = std::make_unique<ConcreteTransformFunction<TShader, TMesh>>(func);
    }

    // Return type-erased results directly!
    std::unique_ptr<VertexContainer> transform(ShaderBase& shader, Mesh& mesh) {
        Key key{&typeid(shader), &typeid(mesh)};

        auto it = functions.find(key);
        if (it == functions.end()) {
            return std::make_unique<EmptyVertexContainer>();
        }

        return it->second->execute(shader, mesh);
    }
};