
#include <Jopnal/Jopnal.hpp>


class SomeScene : public jop::Scene
{
private:

    JOP_DISALLOW_COPY_MOVE(SomeScene);

public:

    float m_sine;

    SomeScene()
        : jop::Scene("SomeScene"),
          m_sine(0.f)
    {
        auto mod = createChild("");
        mod->createComponent<jop::ModelLoader>().load("nanosuit2.3ds");
        mod->scale(0.25f).setPosition(-2.5f, -4.f, -4.f);

        auto attribs = jop::Material::Attribute::DefaultLighting | jop::Material::Attribute::SpecularMap | jop::Material::Attribute::EmissionMap | jop::Material::Attribute::DiffuseMap;
        jop::Material::getDefault().setReflection(jop::Material::Reflection::Emission, jop::Color(10.f, 10.f, 10.f));

        {
            createChild("envmap")->setPosition(-4.5f, 0, -5);
            auto& record = findChild("envmap")->createComponent<jop::EnvironmentRecorder>(getRenderer());

            jop::Material& envMat = jop::ResourceManager::getEmptyResource<jop::Material>("envMat",
                                                                                          jop::Material::Attribute::EnvironmentMap |
                                                                                          jop::Material::Attribute::Phong, false);
            
            envMat.setMap(jop::Material::Map::Environment, *record.getTexture());
            envMat.setReflectivity(1.f)
                  .setReflection(jop::Material::Reflection::Diffuse, jop::Color::Black).setReflection(jop::Material::Reflection::Specular, jop::Color::White)
                  .setShininess(512.f);

            findChild("envmap")->createComponent<jop::GenericDrawable>(getRenderer()).setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("mirrorb", 1.f, 40, 40), envMat));
        }

        createChild("pln")->setPosition(-2.5, -5, -5);
        findChild("pln")->createComponent<jop::RigidBody>(getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::InfinitePlaneShape>("bigbcoll")));

        {
            jop::Material& def = jop::ResourceManager::getEmptyResource<jop::Material>("defmat", attribs);
            def.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("container2.png", true));
            def.setMap(jop::Material::Map::Specular, jop::ResourceManager::getResource<jop::Texture2D>("container2_specular.png", false));
            def.setMap(jop::Material::Map::Emission, jop::ResourceManager::getResource<jop::Texture2D>("matrix.jpg", true));
            def.setShininess(512);

            auto obj = createChild("Def");
            obj->createComponent<jop::GenericDrawable>(getRenderer())
                .setModel(jop::Model(jop::Mesh::getDefault(), def));
            obj->setPosition(0.5, -0.2f, -4);
        }

        createChild("LightCaster")->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Point);
        findChild("LightCaster")->getComponent<jop::LightSource>()->setAttenuation(10).setCastShadows(true);
        findChild("LightCaster")->createComponent<jop::GenericDrawable>(getRenderer()).setCastShadows(true);

        using Res = jop::Object::TransformRestriction;
        findChild("LightCaster")->setPosition(-0.5f, 0.f, -3.f).setScale(0.3f).setIgnoreTransform(TransformRestriction::Rotation | Res::TranslationX | Res::TranslationY);

        findChild("Def")->adoptChild(findChild("LightCaster"));
        findChild("LightCaster", true, true)->setParent(findChild("Def"));

        createChild("SpotLight")->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Spot).setAttenuation(30).setCutoff(glm::radians(10.f), glm::radians(12.f)).setCastShadows(true);
        findChild("SpotLight")->rotate(0, glm::radians(5.f), 0).move(0, 0, -0.5f);

        createChild("Cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective);
        findChild("Cam")->createComponent<jop::Listener>();

        // Ground
        {
            auto attr = jop::Material::Attribute::DiffuseMap
                | jop::Material::Attribute::Phong;

            auto ground = createChild("grnd");
            auto& comp = ground->createComponent<jop::GenericDrawable>(getRenderer());
            comp.setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::BoxMesh>("rectasdf", 10.f, true), jop::ResourceManager::getEmptyResource<jop::Material>("grndmat", attr).setMap(jop::Material::Map::Diffuse, jop::Texture2D::getDefault()).setReflection(jop::Color::Black, jop::Color::Gray, jop::Color::Gray, jop::Color::Black)));

            comp.setReceiveShadows(true);

            ground->setPosition(-2.5f, -0.f, -5.f);
        }

        // Skybox
        {
            createChild("sky")->createComponent<jop::SkySphere>(getRenderer()).setMap(jop::ResourceManager::getResource<jop::Texture2D>("starmap.jpg", true));
        }
        
        struct EventHandler : jop::WindowEventHandler
        {
            EventHandler(jop::Window& w)
                : jop::WindowEventHandler(w)
            {}

            void closed() override
            {
                jop::Engine::exit();
            }
            void keyPressed(const int key, const int, const int) override
            {
                if (key == jop::Keyboard::Comma)
                    jop::Engine::getCurrentScene().getWorld().setDebugMode(!jop::Engine::getCurrentScene().getWorld().debugMode());

                else if (key == jop::Keyboard::Period)
                {
                    auto& obj = *jop::Engine::getCurrentScene().findChild("envmap");
                    obj.setActive(!obj.isActive());
                }
                else if (key == jop::Keyboard::P)
                    jop::Engine::setState(jop::Engine::getState() == jop::Engine::State::Running ? jop::Engine::State::RenderOnly : jop::Engine::State::Running);

                else if (key == jop::Keyboard::F)
                    jop::Engine::advanceFrame();

                if (key == jop::Keyboard::Escape)
                    closed();

                if (key == jop::Keyboard::R)
                    jop::Engine::getCurrentScene().findChild("def")->removeSelf();
            }

            void mouseMoved(const float x, const float y) override
            {
                if (!jop::Engine::hasCurrentScene())
                    return;

                auto& cam = *jop::Engine::getCurrentScene().findChild("Cam");

                static float mx = 0.f;
                static float my = 0.f;
                mx += x;
                my = glm::clamp(my + y, -85.f, 85.f);

                cam.setRotation(glm::radians(-my), glm::radians(-mx), 0.f);
            }
        };

        jop::Engine::getSubsystem<jop::Window>()->setEventHandler<EventHandler>();
    }

    void preUpdate(const float dt) override
    {
        if (findChild("Def").expired())
            return;

        m_sine += dt;

        findChild("Def")->rotate(0.f, dt / 4, dt / 2);

        jop::broadcast("[=SpotLight] rotate 0 " + std::to_string(std::sin(m_sine * 4) * dt / 1.f) + " 0");

        const jop::uint8 col = static_cast<jop::uint8>(200 * std::max(0.f, std::sin(m_sine)));

        jop::ResourceManager::getExistingResource<jop::Material>("defmat").setReflection(jop::Material::Reflection::Emission, jop::Color(col, col, col));

        findChild("LightCaster", true, true)->move(0.f, 2.f * dt * std::sin(8.f * m_sine), 2.f * dt * std::sin(4.f * m_sine)).lookAt(findChild("Def")->getGlobalPosition());
    }

    void postUpdate(const float dt) override
    {
        using jop::Keyboard;
        auto& h = *jop::Engine::getSubsystem<jop::Window>()->getEventHandler();
        auto& cam = *findChild("Cam");

        const float speed = 4.f;

        if (h.keyDown(Keyboard::A) || h.keyDown(Keyboard::D))
            cam.move((h.keyDown(Keyboard::D) ? 1.f : -1.f) * dt * speed * cam.getGlobalRight());

        if (h.keyDown(Keyboard::W) || h.keyDown(Keyboard::S))
            cam.move((h.keyDown(Keyboard::W) ? 1.f : -1.f) * dt * speed * cam.getGlobalFront());

        if (h.keyDown(Keyboard::Space) || h.keyDown(Keyboard::LShift))
            cam.move((h.keyDown(Keyboard::Space) ? 1.f : -1.f) * dt * speed * cam.getGlobalUp());
    }
};

int main(int c, char* v[])
{
    JOP_ENGINE_INIT("JopTestProject", c , v);
    
    jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);

    class LoadingScene : public jop::Scene
    {
    public:

        LoadingScene()
            : jop::Scene("loadScene")
        {
            createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic);

            createChild("rect")->createComponent<jop::GenericDrawable>(getRenderer()).getModel().setMesh(jop::ResourceManager::getNamedResource<jop::RectangleMesh>("rectmsha", 100.f));

            createChild("text")->createComponent<jop::Text>(getRenderer()).setString("Loading...").setColor(jop::Color::White).getObject()->move(-88, -120, 0).setScale(800, 800, 1);
        }

        void preUpdate(const float dt) override
        {
            findChild("rect")->rotate(0, 0, dt);
        }
    };

    jop::Engine::createScene<LoadingScene>();
    jop::Engine::createScene<SomeScene, true>();

    return JOP_MAIN_LOOP;
}