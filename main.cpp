#include <Jopnal/Jopnal.hpp>


class SomeScene : public jop::Scene
{
public:

    float m_sine;

    SomeScene()
        : jop::Scene("SomeScene"),
          m_sine(0.f)
    {}

    void initialize() override
    {
        auto attribs = jop::Material::DefaultAttributes;

        jop::Material& def = jop::ResourceManager::getEmptyResource<jop::Material>("defmat", attribs);
        def.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture>("container2.png"));
        def.setMap(jop::Material::Map::Specular, jop::ResourceManager::getResource<jop::Texture>("container2_specular.png"));
        def.setMap(jop::Material::Map::Emission, jop::ResourceManager::getResource<jop::Texture>("matrix.jpg"));
        def.setShininess(128.f);
        def.setReflection(jop::Material::Reflection::Emission, jop::Color::Black);
        
        auto obj = createObject("Def");
        obj->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "BoxDrawable")
           ->setShader(jop::ShaderManager::getShader(attribs))
           .setModel(jop::Model(jop::Mesh::getDefault(), def));
        obj->setPosition(0.5, -0.2f, -4);

        cloneObject("Def", "Def2")->setPosition(-5.f, 0, -10).rotate(-45, 45, 0);
        cloneObject("Def2", "Def3")->setPosition(- 5, -2, -10).rotate(54, 70, 1);
        
        createObject("LightCaster");
        getObject("LightCaster")->createComponent<jop::LightSource>(*getDefaultLayer(), "LC");
        getObject("LightCaster")->getComponent<jop::LightSource>()->setAttenuation(0.1f, 0.3f, 0.3f, 10);
        getObject("LightCaster")->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "ASDF");
        getObject("LightCaster")->setPosition(1.5f, 0.f, -3.f).setScale(0.3f);

        createObject("DirLight")->createComponent<jop::LightSource>(*getDefaultLayer(), "LS")->setType(jop::LightSource::Type::Directional);
        getObject("DirLight")->setActive(false);

        createObject("SpotLight")->createComponent<jop::LightSource>(*getDefaultLayer(), "SP")->setType(jop::LightSource::Type::Spot).setAttenuation(jop::LightSource::AttenuationPreset::_320).setCutoff(glm::radians(10.f), glm::radians(12.f));
        getObject("SpotLight")->rotate(0, glm::radians(5.f), 0);

        jop::Camera::getDefault().getObject()->createComponent<jop::LightSource>(*getDefaultLayer(), "LC2")->setAttenuation(jop::LightSource::AttenuationPreset::_50);

        if (!jop::StateLoader::saveState("Scene/test", true, true, true))
            jop::Engine::exit();
    }

    void preUpdate(const float dt) override
    {
        if (getObject("Def").expired())
            return;

        m_sine += dt;

        getObject("Def")->rotate(0.f, dt / 4, dt / 2);

        getObject("DirLight")->rotate(0.f, dt, 0.f);
        getObject("SpotLight")->rotate(0.f, std::sin(m_sine * 5) * dt / 2, 0.f);

        const jop::uint8 col = static_cast<jop::uint8>(200 * std::max(0.f, std::sin(m_sine)));

        jop::ResourceManager::getExistingResource<jop::Material>("defmat").setReflection(jop::Material::Reflection::Emission, jop::Color(col, col, col));

        getObject("LightCaster")->move(0.f, 2.f * dt * std::sin(8.f * m_sine), 2.f * dt * std::sin(4.f * m_sine));
    }

    void postUpdate(const float dt) override
    {
        using jop::Keyboard;
        auto& h = *jop::Engine::getSubsystem<jop::Window>()->getEventHandler();
        auto& cam = *jop::Camera::getDefault().getObject();

        const float speed = 4.f;

        if (h.keyDown(Keyboard::A) || h.keyDown(Keyboard::D))
        {
            cam.move((h.keyDown(Keyboard::D) ? 1.f : -1.f) * dt * speed * cam.getGlobalRight());
        }
        if (h.keyDown(Keyboard::W) || h.keyDown(Keyboard::S))
        {

            cam.move((h.keyDown(Keyboard::W) ? 1.f : -1.f) * dt * speed * cam.getGlobalFront());
        }
        if (h.keyDown(Keyboard::Space) || h.keyDown(Keyboard::LControl))
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
                jop::StateLoader::getInstance().loadState("Scene/test", true, true, true);
            else if (key == jop::Keyboard::K)
                jop::StateLoader::getInstance().saveState("Scene/test", true, true, true);

            if (key == jop::Keyboard::Escape)
                closed();
        }

        void mouseMoved(const float x, const float y) override
        {
            auto& cam = *jop::Camera::getDefault().getObject();

            static float mx = 0.f;
            static float my = 0.f;
            mx += x;
            my = glm::clamp(my + y, -85.f, 85.f);

            cam.setRotation(glm::radians(-my), glm::radians(-mx), 0.f);
        }
    };

    jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);
    jop::Engine::getSubsystem<jop::Window>()->setEventHandler<EventHandler>();

    //jop::Engine::createScene<SomeScene>();

    /*for (int i = 1; i <= jop::Material::DefaultAttributes; ++i)
    {
        JOP_DEBUG_INFO("Compiling shader: " << i);
        if (&jop::ShaderManager::getShader(i) == &jop::Shader::getDefault())
            return EXIT_FAILURE;

        jop::ResourceManager::unloadResource("jop_shader_" + std::to_string(i));
    }*/

    if (&jop::ShaderManager::getShader(jop::Material::DefaultAttributes) == &jop::Shader::getDefault())
        return EXIT_FAILURE;

    return JOP_MAIN_LOOP;
}