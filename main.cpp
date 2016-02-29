#include <Jopnal/Jopnal.hpp>


class SomeComponent : public jop::Component
{
public:

    SomeComponent(jop::Object& obj)
        : jop::Component(obj, "")
    {}

    JOP_GENERIC_CLONE(SomeComponent)
};

JOP_REGISTER_LOADABLE(a, SomeComponent)[](jop::Object& o, const jop::Scene&, const jop::json::Value&)
{
    o.createComponent<SomeComponent>();
    return true;
}
JOP_END_LOADABLE_REGISTRATION(SomeComponent)

JOP_REGISTER_SAVEABLE(a, SomeComponent)[](const jop::Component&, jop::json::Value&, jop::json::Value::AllocatorType&) -> bool
{
    return true;
}
JOP_END_SAVEABLE_REGISTRATION(SomeComponent)

class SomeScene : public jop::Scene
{
public:

    SomeScene()
        : jop::Scene("SomeScene")
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
           .setModel(jop::Model(/*jop::ResourceManager::getNamedResource<jop::SphereMesh>("Ball", 1.f, 30, 30)*/jop::Mesh::getDefault(), def));
        obj->setPosition(0.5, -0.2f, 4);

        cloneObject("Def", "Def2")->setPosition(-5.f, 0, 10).rotate(-45, 45, 0);
        cloneObject("Def2", "Def3")->setPosition(- 5, -2, 10).rotate(54, 70, 1);
        
        createObject("LightCaster");
        getObject("LightCaster")->createComponent<jop::LightSource>(*getDefaultLayer(), "LC");
        getObject("LightCaster")->getComponent<jop::LightSource>()->setAttenuation(0.1f, 0.3f, 0.3f, 10);
        getObject("LightCaster")->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "ASDF");
        getObject("LightCaster")->setPosition(1.5f, 0.f, 3.f).setScale(0.3f);

        createObject("DirLight")->createComponent<jop::LightSource>(*getDefaultLayer(), "LS")->setType(jop::LightSource::Type::Directional);
        getObject("DirLight")->setActive(false);

        createObject("SpotLight")->createComponent<jop::LightSource>(*getDefaultLayer(), "SP")->setType(jop::LightSource::Type::Spot).setAttenuation(jop::LightSource::AttenuationPreset::_320).setCutoff(glm::radians(10.f), glm::radians(12.f));
        //getObject("SpotLight")->rotate(0, glm::radians(-200.f), 0);



        if (!jop::StateLoader::saveState("Scene/test", true, true, true))
            jop::Engine::exit();
    }

    void preUpdate(const float dt) override
    {
        getObject("Def")->rotate(0.f, dt / 8, dt / 4);

        getObject("DirLight")->rotate(0.f, dt, 0.f);
        getObject("SpotLight")->rotate(0.f, std::sin(jop::Engine::getTotalTime() * 5) * dt / 2, 0.f);

        const jop::uint8 col = static_cast<jop::uint8>(200 * std::max(0.0, std::sin(jop::Engine::getTotalTime())));

        jop::ResourceManager::getExistingResource<jop::Material>("defmat").setReflection(jop::Material::Reflection::Emission, jop::Color(col, col, col));

        getObject("LightCaster")->move(0.f, 2 * dt * std::sin(8.f * static_cast<float>(jop::Engine::getTotalTime())), 2* dt * std::sin(4.f * static_cast<float>(jop::Engine::getTotalTime())));
    }

    void postUpdate(const float dt) override
    {
        using jop::Keyboard;
        auto& h = *jop::Engine::getSubsystem<jop::Window>()->getEventHandler();
        auto& cam = *jop::Camera::getDefault().getObject();

        const float speed = 4.f;


        if (h.keyDown(Keyboard::A) || h.keyDown(Keyboard::D))
        {
            cam.move((h.keyDown(Keyboard::D) ? -1.f : 1.f) * dt * speed * cam.getRight());
        }
        if (h.keyDown(Keyboard::W) || h.keyDown(Keyboard::S))
        {
            //auto f = cam.getFront();
            //f.z = -f.z;

            auto f = cam.getFront();
            JOP_DEBUG_INFO("Front: " << f.x << ", " << f.y << ", " << f.z);

            cam.move((h.keyDown(Keyboard::W) ? -1.f : 1.f) * dt * speed * cam.getFront());
        }
    }
};

JOP_REGISTER_LOADABLE(a, SomeScene)[](std::unique_ptr<jop::Scene>& s, const jop::json::Value&)
{
    s = std::make_unique<SomeScene>();
    return true;
}
JOP_END_LOADABLE_REGISTRATION(SomeScene)

JOP_REGISTER_SAVEABLE(a, SomeScene)[](const jop::Scene& s, jop::json::Value& v, jop::json::Value::AllocatorType& a) -> bool
{
    v.AddMember(jop::json::StringRef("id"), jop::json::StringRef(s.getID().c_str()), a)
     .AddMember(jop::json::StringRef("active"), s.isActive(), a);

    return true;
}
JOP_END_SAVEABLE_REGISTRATION(SomeScene)

int main(int c, char* v[])
{
    jop::Engine e("JopTestProject", c , v);
    e.loadDefaultConfiguration();

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
                jop::StateLoader::getInstance().loadState("Scene/test");
            if (key == jop::Keyboard::Escape)
                closed();
        }

        void mouseMoved(const float x, const float y) override
        {
            static int count = 0;
            if (count++ < 5) return;

            auto& cam = *jop::Camera::getDefault().getObject();

            JOP_DEBUG_INFO("Mouse: " << x << ", " << y);


            cam.rotate(-glm::radians(y), -glm::radians(x), 0.f);
        }
    };

    e.getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);
    e.getSubsystem<jop::Window>()->setEventHandler<EventHandler>();

    e.createScene<SomeScene>();

    //for (int i = 1; i <= jop::Material::DefaultAttributes; ++i)
    //    JOP_ASSERT(&jop::ShaderManager::getShader(i) != &jop::Shader::getDefault(), "aaa");
    if (&jop::ShaderManager::getShader(jop::Material::DefaultAttributes) == &jop::Shader::getDefault())
        return EXIT_FAILURE;

    return e.runMainLoop();
}