#pragma once
#include <Jopnal/Jopnal.hpp>


namespace jd
{
    class MirrorBall : public jop::Component
    {
    public:

        MirrorBall(jop::Object& obj, jop::Renderer& rend)
            : jop::Component(obj, 0)
        {
            using namespace jop;
            using MA = Material::Attribute;
            using RM = ResourceManager;

            auto& mat = RM::getEmptyResource<Material>("mirrballmat", MA::DefaultLighting | MA::EnvironmentMap);
            auto& mesh = RM::getNamedResource<SphereMesh>("mirrorballmesh", 2.5f, 30, 30);

            auto ballObj = getObject()->createChild("ball");
            ballObj->setPosition(10.f, 0.f, 0.f);

            mat.setMap(Material::Map::Environment, *ballObj->createComponent<EnvironmentRecorder>(rend).getTexture());

            mat.setReflectivity(1.f)
               .setShininess(256.f)
               .setReflection(Color::Black, Color::Black, Color::Black, Color::Black);

            ballObj->createComponent<GenericDrawable>(rend).setModel(Model(mesh, mat));
        }

        void update(const float deltaTime) override
        {
            getObject()->rotate(0.f, deltaTime * 0.1f, 0.f);
        }
    };
}