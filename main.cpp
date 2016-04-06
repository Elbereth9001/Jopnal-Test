
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
        //createChild("nano")->createComponent<jop::ModelLoader>().load("nanosuit2.3ds");

        getWorld().setDebugMode(true);

        auto attribs = jop::Material::Attribute::DefaultLighting | jop::Material::Attribute::SpecularMap | jop::Material::Attribute::EmissionMap | jop::Material::Attribute::DiffuseMap;

        jop::Engine::setDeltaScale(2.f);
        setDeltaScale(.5f);

        //if (false)
        {
            createChild("envmap")->setPosition(-4.5f, 0, -5);
            auto& record = findChild("envmap")->createComponent<jop::EnvironmentRecorder>(getRenderer());

            jop::Material& envMat = jop::ResourceManager::getEmptyResource<jop::Material>("envMat",
                                                                                          jop::Material::Attribute::EnvironmentMap |
                                                                                          jop::Material::Attribute::DefaultLighting);

            envMat.setMap(jop::Material::Map::Environment, *record.getTexture());
            //envMat.setMap(jop::Material::Map::Environment, jop::ResourceManager::getResource<jop::Cubemap>("drakeq_rt.tga", "drakeq_lf.tga", "drakeq_up.tga", "drakeq_dn.tga", "drakeq_bk.tga", "drakeq_ft.tga"));
            envMat.setReflectivity(1.f)
                  .setReflection(jop::Material::Reflection::Diffuse, jop::Color::Black)
                  .setShininess(512.f)
                  .removeAttributes(jop::Material::Attribute::AmbientConstant);

            findChild("envmap")->createComponent<jop::GenericDrawable>(getRenderer()).setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("mirrorb", 1.f, 40, 40), envMat));
        }

        createChild("pln")->setPosition(-2.5, -5, -5);
        findChild("pln")->createComponent<jop::RigidBody>(getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::InfinitePlaneShape>("bigbcoll")));

        jop::Material& def = jop::ResourceManager::getEmptyResource<jop::Material>("defmat", attribs);
        def.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("container2.png"));
        def.setMap(jop::Material::Map::Specular, jop::ResourceManager::getResource<jop::Texture2D>("container2_specular.png"));
        def.setMap(jop::Material::Map::Emission, jop::ResourceManager::getResource<jop::Texture2D>("matrix.jpg"));
        def.setShininess(64.f);
        def.setReflection(jop::Material::Reflection::Emission, jop::Color::Black);
        
        auto obj = createChild("Def");
        obj->createComponent<jop::GenericDrawable>(getRenderer())
           .setModel(jop::Model(jop::Mesh::getDefault(), def));
        obj->setPosition(0.5, -0.2f, -4);

        cloneChild("Def", "Def2")->setPosition(-5.f, 0, -8).rotate(-45, 45, 0);
        findChild("Def2")->createComponent<jop::RigidBody>(getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::BoxShape>("boxcoll", 1.f), jop::RigidBody::Type::Dynamic, 1.f));

        cloneChild("Def2", "Def3", jop::Transform(*findChild("Def2").get()).setPosition(-5, -2, -7.8f).rotate(54, 70, 1));
        
        auto sens = createChild("sensor");
        sens->setPosition(-5.f, -3.f, -8);
        sens->createComponent<jop::RigidBody>(getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::BoxShape>("asdfshape", 1.f), jop::RigidBody::Type::StaticSensor));

        createChild("LightCaster")/*->createComponent<jop::SoundEffect>().setBuffer(jop::ResourceManager::getResource<jop::SoundBuffer>("32.wav")).setLoop(true).setPitch(2.f).setAttenuation(5).setMinDistance(1.f);*/;
        //findChild("LightCaster")->getComponent<jop::SoundEffect>()->play();
        findChild("LightCaster")->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Point);
        findChild("LightCaster")->getComponent<jop::LightSource>()->setAttenuation(jop::LightSource::AttenuationPreset::_50).setCastShadows(true);
        findChild("LightCaster")->createComponent<jop::GenericDrawable>(getRenderer()).setCastShadows(true);
        findChild("LightCaster")->setPosition(-0.5f, 0.f, -3.f).setScale(0.3f);

        findChild("Def")->adoptChild(findChild("LightCaster"));
        findChild("LightCaster", true, true)->setParent(findChild("Def"));

        createChild("DirLight")->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Directional).setCastShadows(false);
        findChild("DirLight")->setActive(false);
        findChild("DirLight")->setPosition(-2.5f, 10.f, -5.f).setScale(30).setRotation(-glm::half_pi<float>(), 0.f, 0.f);

        createChild("SpotLight")->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Spot).setAttenuation(jop::LightSource::AttenuationPreset::_320).setCutoff(glm::radians(10.f), glm::radians(12.f)).setCastShadows(true);
        findChild("SpotLight")->rotate(0, glm::radians(5.f), 0);

        createChild("Cam")/*->createComponent<jop::LightSource>("LC2", getRenderer(), jop::LightSource::Type::Spot)->setAttenuation(jop::LightSource::AttenuationPreset::_50)*/;

        findChild("Cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective);
        findChild("Cam")->createComponent<jop::Listener>();

        // Ground
        {
            auto attr = jop::Material::Attribute::AmbientConstant
                | jop::Material::Attribute::DiffuseMap
                | jop::Material::Attribute::Phong;

            auto ground = createChild("grnd");
            auto& comp = ground->createComponent<jop::GenericDrawable>(getRenderer());
            comp.setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::BoxMesh>("rectasdf", 10.f, true), jop::ResourceManager::getEmptyResource<jop::Material>("grndmat", attr).setMap(jop::Material::Map::Diffuse, jop::Texture2D::getDefault()).setReflection(jop::Color::Black, jop::Color::Gray, jop::Color::Gray, jop::Color::Black)));

            comp.setReceiveShadows(true);

            ground->setPosition(-2.5f, -0.f, -5.f);
        }

        //if (!jop::StateLoader::saveState("Scene/test", true, true))
        //    jop::Engine::exit();
    }

    void preUpdate(const float dt) override
    {
        if (findChild("Def").expired())
            return;

        m_sine += dt;

        findChild("Def")->rotate(0.f, dt / 4, dt / 2);

        //getChild("DirLight")->rotate(dt, 0, 0.f);
        //findChild("SpotLight")->rotate(0.f, std::sin(m_sine * 5) * dt / 1.5f, 0.f);
        jop::broadcast("[=SpotLight] rotate 0 " + std::to_string(std::sin(m_sine * 5) * dt / 1.5f) + " 0");

        const jop::uint8 col = static_cast<jop::uint8>(200 * std::max(0.f, std::sin(m_sine)));

        jop::ResourceManager::getExistingResource<jop::Material>("defmat").setReflection(jop::Material::Reflection::Emission, jop::Color(col, col, col));

        findChild("LightCaster", true, true)->move(0.f, 2.f * dt * std::sin(8.f * m_sine), 2.f * dt * std::sin(4.f * m_sine));

    }

    void postUpdate(const float dt) override
    {
        using jop::Keyboard;
        auto& h = *jop::Engine::getSubsystem<jop::Window>()->getEventHandler();
        auto& cam = *findChild("Cam");

        const float speed = 4.f;

        if (h.keyDown(Keyboard::A) || h.keyDown(Keyboard::D))
        {
            cam.move((h.keyDown(Keyboard::D) ? 1.f : -1.f) * dt * speed * cam.getGlobalRight());
        }
        if (h.keyDown(Keyboard::W) || h.keyDown(Keyboard::S))
        {

            cam.move((h.keyDown(Keyboard::W) ? 1.f : -1.f) * dt * speed * cam.getGlobalFront());
        }
        if (h.keyDown(Keyboard::Space) || h.keyDown(Keyboard::LShift))
        {
            cam.move((h.keyDown(Keyboard::Space) ? 1.f : -1.f) * dt * speed * cam.getGlobalUp());
        }
    }
};

JOP_REGISTER_LOADABLE(a, SomeScene)[](std::unique_ptr<jop::Scene>& s, const jop::json::Value& val)
{
    auto sc = std::make_unique<SomeScene>();
    sc->m_sine = static_cast<float>(val["sine"].GetDouble());

    s = std::move(sc);
    return true;
}
JOP_END_LOADABLE_REGISTRATION(SomeScene)

JOP_REGISTER_SAVEABLE(a, SomeScene)[](const jop::Scene& s, jop::json::Value& v, jop::json::Value::AllocatorType& a) -> bool
{
    v.AddMember(jop::json::StringRef("id"), jop::json::StringRef(s.getID().c_str()), a)
     .AddMember(jop::json::StringRef("active"), s.isActive(), a)
     .AddMember(jop::json::StringRef("sine"), static_cast<const SomeScene&>(s).m_sine, a);

    return true;
}
JOP_END_SAVEABLE_REGISTRATION(SomeScene)

int main(int c, char* v[])
{
    JOP_ENGINE_INIT("JopTestProject", c , v);

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
            if (key == jop::Keyboard::L)
                jop::StateLoader::getInstance().loadState("Scene/test", true, true);
            else if (key == jop::Keyboard::K)
                jop::StateLoader::getInstance().saveState("Scene/test", true, true);
            else if (key == jop::Keyboard::C)
            {
                static unsigned int created = 0;
                JOP_DEBUG_INFO("Objects created: " << created++);

                jop::Engine::getCurrentScene().cloneChild("Def2", "def", jop::Transform().setPosition(-2.5f, 1.f, -5.f));
            }
            else if (key == jop::Keyboard::Comma)
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
            auto& cam = *jop::Engine::getCurrentScene().findChild("Cam");

            static float mx = 0.f;
            static float my = 0.f;
            mx += x;
            my = glm::clamp(my + y, -85.f, 85.f);

            cam.setRotation(glm::radians(-my), glm::radians(-mx), 0.f);
        }
    };

    jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);
    jop::Engine::getSubsystem<jop::Window>()->setEventHandler<EventHandler>();

    if (&jop::ShaderManager::getShader(jop::Material::Attribute::Default) == &jop::Shader::getError())
        return EXIT_FAILURE;

    jop::Engine::createScene<SomeScene>();

    /*for (int i = 1; i <= jop::Material::DefaultAttributes; ++i)
    {
        JOP_DEBUG_INFO("Compiling shader: " << i);
        if (&jop::ShaderManager::getShader(i) == &jop::Shader::getDefault())
            return EXIT_FAILURE;

        jop::ResourceManager::unloadResource("jop_shader_" + std::to_string(i));
    }*/

    return JOP_MAIN_LOOP;
}