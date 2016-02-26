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
        jop::Material def(attribs);
        def.setShininess(128.f);
        
        auto obj = createObject("Def");
        obj->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "BoxDrawable")
           ->setShader(jop::ShaderManager::getShader(attribs))
           .setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("Ball", 1.f, 30, 30)/*jop::Mesh::getDefault()*/, def));
        obj->setPosition(0, -0.2f, -4);
        
        createObject("LightCaster");
        getDefaultLayer()->addLight(*getObject("LightCaster")->createComponent<jop::LightSource>("LC")/*->setIntensity(jop::LightSource::Intensity::Diffuse, jop::Color(0.f, 128.f, 200.f))*/);
        getObject("LightCaster")->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "ASDF");
        getObject("LightCaster")->setPosition(1.5f, 0.f, -3.f).setScale(0.3f);

        if (!jop::StateLoader::saveState("Scene/test", true, true, true))
            jop::Engine::exit();
    }

    void preUpdate(const float dt) override
    {
        getObject("Def")->rotate(0.f, dt, 0.f);

        getObject("LightCaster")->move(0.f, 2 * dt * std::sin(8.f * static_cast<float>(jop::Engine::getTotalTime())), 2* dt * std::sin(4.f * static_cast<float>(jop::Engine::getTotalTime())));
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
    };

    e.getSubsystem<jop::Window>()->setEventHandler<EventHandler>();

    e.createScene<SomeScene>();

    //for (int i = 1; i <= jop::Material::DefaultAttributes; ++i)
    //    JOP_ASSERT(&jop::ShaderManager::getShader(i) != &jop::Shader::getDefault(), "aaa");
    if (&jop::ShaderManager::getShader(jop::Material::DefaultAttributes) == &jop::Shader::getDefault())
        return EXIT_FAILURE;

    return e.runMainLoop();
}