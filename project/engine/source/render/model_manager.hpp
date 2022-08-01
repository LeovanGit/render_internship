#ifndef MODEL_MANAGER_HPP
#define MODEL_MANAGER_HPP

#include "spdlog.h"
#include <unordered_map>
#include <string>

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

    void registerModel(const std::string & key,
                       const Model & model);

    void registerModel(const std::string & key,
                       const std::string & model_filename);

    void useModel(const std::string & key);

    Model & getModel(const std::string & key);

    void registerDefaultCube(const std::string & key);

private:
    ModelManager() = default;
    ~ModelManager() = default;

    static ModelManager * instance;

    std::unordered_map<std::string, Model> models;
};
} // namespace engine

#endif
