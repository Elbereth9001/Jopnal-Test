#pragma once
#include "EventHandler.hpp"
#include "Spawner.hpp"
#include "MirrorBall.hpp"
#include "LightBall.hpp"


namespace jd
{
    class WaveTranslator : public jop::Component
    {
        JOP_GENERIC_COMPONENT_CLONE(WaveTranslator);

        float m_sine;
        const float m_scale;

    public:

        WaveTranslator(jop::Object& obj, const float scale)
            : jop::Component(obj, 0),
              m_sine(glm::half_pi<float>()),
              m_scale(scale)
        {}

        WaveTranslator(const WaveTranslator& other, jop::Object& newObj)
            : jop::Component(other, newObj),
              m_scale(other.m_scale)
        {}

        void update(const float deltaTime) override
        {
            m_sine += deltaTime;

            getObject()->move(std::sin(m_sine) * deltaTime * 1.2f, std::sin(m_sine) * m_scale * deltaTime, std::cos(m_sine) * deltaTime * 1.2f);
        }
    };

    class DemoScene : public jop::Scene
    {
    private:

        JOP_DISALLOW_COPY_MOVE(DemoScene);

    public:

        DemoScene()
            : jop::Scene(typeid(DemoScene).name())
        {
            using namespace jop;
            using RM = ResourceManager;
            using MA = Material::Attribute;

            // Lights
            {
                using LS = LightSource;
                createChild("dirlight")->setPosition(0.f, 100.f, 0.f).lookAt(0.f, -1.f, 0.f).createComponent<LS>(getRenderer(), LS::Type::Directional)
                    .setIntensity(Color::Black, Color::Black, Color::Black);

                createChild("pointlight1")->setPosition(0.f, -10.f, 0.f)
                    .createComponent<LS>(getRenderer(), LS::Type::Point)
                    .setIntensity(Color::Black, Color::White, Color::White)
                    .setAttenuation(75.f)
                    .setCastShadows(true).

                getObject()->createComponent<GenericDrawable>(getRenderer())
                    .setModel(Model(RM::getNamed<SphereMesh>("spotbulb", 0.075f, 10, 10),
                                    RM::getEmpty<Material>("spotbulbmat", false)
                                    .setReflection(Material::Reflection::Solid, Color::White * 10.f)));

                //auto& stream = findChild("pointlight1")->createComponent<SoundStream>();
                //stream.setPath("pulse.wav");
                //stream.setLoop(true).setSpatialization(true).setAttenuation(3.5f).setVolume(0.f).setMinDistance(5.f);
                //stream.play();
            }

            // Spinning spawner
            {
                createChild("spawn")->setPosition(0.f, 4.f, 0.f).createComponent<Spawner>(getRenderer());
            }

            // Mirror ball
            {
                createChild("mirror")->setPosition(0.f, 4.f, 0.f).createComponent<MirrorBall>(getRenderer());
            }

            // Light ball
            {
                createChild("lball")->setPosition(0.f, 15.f, 0.f).createComponent<LightBall>(getRenderer());
            }

            // Ground
            {
                auto& mat = RM::getEmpty<Material>("groundmat", MA::DefaultLighting | MA::DiffuseMap)
                    .setReflection(Material::Reflection::Ambient, Color(0x666666FF))
                    .setReflection(Material::Reflection::Specular, Color::Black);

                createChild("ground")->createComponent<GenericDrawable>(getRenderer())
                    .setCastShadows(false)
                    .setModel(Model(RM::getNamed<RectangleMesh>("groundmesh", 50.f), mat));

                RigidBody::ConstructInfo info(RM::getNamed<InfinitePlaneShape>("groundshape"));
                info.restitution = 1.f;

                findChild("ground")->createComponent<RigidBody>(getWorld<3>(), info).

                    getObject()->rotate(-glm::half_pi<float>(), 0.f, 0.f);
            }

            // Camera & audio listener
            {
                createChild("cam")->setPosition(0.f, 2.5f, 24.f)
                    .createComponent<Camera>(getRenderer(), Camera::Projection::Perspective)
                    /*.getObject()->createComponent<Listener>()*/;
            }

            // Sky sphere
            {
                auto& map = ResourceManager::get<Texture2D>("starmap.jpg", true, false);
                map.getSampler().setFilterMode(TextureSampler::Filter::Bilinear);

                createChild("sky")->createComponent<SkySphere>(getRenderer())
                    .setMap(map).getModel();
            }
            
            Engine::getSubsystem<Window>()->setEventHandler<EventHandler>();
        }

        ~DemoScene() override
        {
            jop::Engine::getSubsystem<jop::Window>()->setDefaultEventHandler();
        }

        void preUpdate(const float dt) override
        {
            // Point light 1
            {
                const float speed = 5.f;
                auto obj = findChild("pointlight1");

                if (obj->getGlobalPosition().y > -0.1f)
                {
                    findChild("spawn")->setActive(true);
                    findChild("mirror")->findChild("ball")->getComponent<jop::GenericDrawable>()
                        ->getModel().getMaterial()->setReflection(jop::Material::Reflection::Specular, jop::Color::White);

                    findChild("lball")->setActive(true);
                }

                if (!obj->getComponent<WaveTranslator>() && obj->move(0.f, speed * dt, 0.f).getGlobalPosition().y > 7.f)
                {
                    obj->createComponent<WaveTranslator>(3.5f);

                    //auto& stream = createComponent<jop::SoundStream>();
                    //stream.setID(1);
                    //stream.setOffset(62.f);
                    //stream.setPath("music.wav");
                    //stream.setLoop(true).setVolume(0.f);
                    //stream.play();
                }

                //auto streamComp = getComponent<jop::SoundStream>(1);
                //if (streamComp)
                //    streamComp->setVolume(std::min(7.5f, streamComp->getVolume() + dt * 1.f));

                //obj->getComponent<jop::SoundStream>()->setVolume(obj->getGlobalPosition().y * 10.f);
            }
        }

        void postUpdate(const float dt) override
        {
            using jop::Keyboard;
            auto& h = *jop::Engine::getSubsystem<jop::Window>()->getEventHandler();

            auto cam = findChild("cam");

            if (!cam.expired())
            {
                const float speed = 4.f + 6.f * h.keyDown(Keyboard::LControl);

                if (h.keyDown(Keyboard::A) || h.keyDown(Keyboard::D))
                    cam->move((h.keyDown(Keyboard::D) ? 1.f : -1.f) * dt * speed * cam->getGlobalRight());

                if (h.keyDown(Keyboard::W) || h.keyDown(Keyboard::S))
                    cam->move((h.keyDown(Keyboard::W) ? 1.f : -1.f) * dt * speed * cam->getGlobalFront());

                if (h.keyDown(Keyboard::Space) || h.keyDown(Keyboard::LShift))
                    cam->move((h.keyDown(Keyboard::Space) ? 1.f : -1.f) * dt * speed * cam->getGlobalUp());
            }
        }
    };
}