#pragma once
#include <Jopnal/Graphics.hpp>


namespace jd
{
    class LoadingScene : public jop::Scene
    {
    public:

        LoadingScene()
            : jop::Scene(typeid(LoadingScene).name())
        {
            using RM = jop::ResourceManager;
            using MAT = jop::Material::Attribute;

            // Camera
            auto& camComponent = createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic);
            camComponent.setSize(camComponent.getSize() * 0.005f);

            // Logo
            createChild("logo")->move(-2.f, 0.f, 0.f).createComponent<jop::GenericDrawable>(getRenderer()).getModel()
                
                .setMesh        (RM::getNamedResource<jop::RectangleMesh>("logoMesh", 2.f))
                .setMaterial    (RM::getEmptyResource<jop::Material>("logoMat", MAT::DefaultLighting)
                .setMap         (jop::Material::Map::Diffuse, RM::getResource<jop::Texture2D>("jop.png", true))
                .setReflection  (jop::Color(0x222222FF), jop::Color::White, jop::Color::Black, jop::Color::Black));

            // Lights
            auto rotator = findChild("logo")->createChild("lightrot");

            rotator->createChild("left")->setPosition(-5.f, 0.f, 0.f).createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Point)
                .setAttenuation(30.f)
                .setIntensity(jop::Color::White, jop::Color::White, jop::Color::Black);

            rotator->createChild("right")->setPosition(5.f, 0.f, 0.f).createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Point)
                .setAttenuation(10.f)
                .setIntensity(jop::Color::White, jop::Color(50.f, 50.f, 50.f), jop::Color::Black);

            // Text
            createChild("text")->move(0.f, 0.f, 0.f).setScale(8.f).createComponent<jop::Text>(getRenderer())

                .setFont(RM::getResource<jop::Font>("Furore.otf", 64))
                .setString("please stand by")
                .setColor(jop::Color(0.05f, 0.05f, 1.f));
        }

        ~LoadingScene() override
        {
            using RM = jop::ResourceManager;

            RM::unloadResource<jop::RectangleMesh>("logoMesh");
            RM::unloadResource<jop::Material>("logoMat");
            RM::unloadResource<jop::Texture2D>("jop.png");
        }

        void postUpdate(const float deltaTime) override
        {
            if (jop::Engine::newSceneReady())
            {
                auto text = findChild("text")->getComponent<jop::Text>();

                const jop::Color newColor(text->getColor().colors, text->getColor().alpha - deltaTime * 1.5f);

                if (newColor.alpha <= 0.f)
                {
                    jop::Engine::signalNewScene();
                    return;
                }

                text->setColor(newColor);
                auto& mat = jop::ResourceManager::getExistingResource<jop::Material>("logoMat");
                mat.setReflection(jop::Material::Reflection::Diffuse, jop::Color(mat.getReflection(jop::Material::Reflection::Diffuse).colors, newColor.alpha));
            }

            findChild("lightrot", true)->rotate(0.f, deltaTime * glm::half_pi<float>(), 0.f);
        }
    };
}