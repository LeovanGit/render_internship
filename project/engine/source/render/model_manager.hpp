#ifndef MODEL_MANAGER_HPP
#define MODEL_MANAGER_HPP

#include "spdlog.h"
#include <unordered_map>
#include <string>
#include <memory>

#include "model.hpp"

namespace engine
{
class ModelManager final
{
public:
    // deleted methods should be public for better error messages
    ModelManager(const ModelManager & other) = delete;
    void operator=(const ModelManager & other) = delete;

    static void init();

    static ModelManager * getInstance();

    static void del();

    std::shared_ptr<Model> getModel(const std::string & model_path);

    void bindModel(const std::string & key);

    std::shared_ptr<Model> getDefaultCube(const std::string & key);

    std::shared_ptr<Model> getDefaultPlane(const std::string & key);

private:
    ModelManager() = default;
    ~ModelManager() = default;

    static ModelManager * instance;

    std::unordered_map<std::string, std::shared_ptr<Model>> models;
};
} // namespace engine

#endif
