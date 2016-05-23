#pragma once
#include <Jopnal/Jopnal.hpp>


namespace jd
{
    class Spawner : public jop::Component
    {
    public:

        Spawner(jop::Object& obj, jop::Renderer& rend)
            : jop::Component(obj, 0)
        {
            using namespace jop;
            using RM = ResourceManager;

            auto root = getObject();
            root->setActive(false);

            auto& baseMat = RM::getEmptyResource<Material>("spawnbasemat", Material::Attribute::DefaultLighting | Material::Attribute::DiffuseMap);
            auto& baseMesh = RM::getNamedResource<SphereMesh>("spawnbasemesh", 0.35f, 20, 20);

            const glm::vec3 basePos(2.f, 0.f, 2.f);

            for (std::size_t i = 0; i < 4; ++i)
            {
                static const glm::vec2 posMult[] =
                {
                    glm::vec2(-1.f, -1.f),
                    glm::vec2(1.f, -1.f),
                    glm::vec2(1.f, 1.f),
                    glm::vec2(-1.f, 1.f)
                };

                auto child = root->createChild(std::to_string(i));

                child->setPosition(basePos.x * posMult[i].x, basePos.y, basePos.z * posMult[i].y);
                child->setIgnoreTransform(Object::Rotation);

                child->createComponent<GenericDrawable>(rend).setModel(Model(baseMesh, baseMat));
            }
        }

        void update(const float deltaTime) override
        {
            getObject()->rotate(0.f, deltaTime * 0.4f, 0.f);
        }
    };
}