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
        //auto attribs = jop::Material::DefaultAttributes;
        auto attribs = jop::Material::Attribute::AmbientConstant | jop::Material::Attribute::Diffusemap | jop::Material::Attribute::Phong;
        jop::Material def(attribs);
        def.setShininess(5.f);
        //def.setReflection(jop::Material::Reflection::Ambient, jop::Color::Green);
        //def.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture>("asdf"));
        
        auto obj = createObject("Def");
        obj->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "defc")->setShader(jop::ShaderManager::getShader(attribs));
        obj->getComponent<jop::GenericDrawable>()->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("Ball", 1.f, 20, 20), def));

        obj->setPosition(0, 0, -3);
        obj->createComponent<SomeComponent>();
        auto rotator = obj->createChild("Rotator");

        auto left = rotator->createChild("Left")->createComponent<jop::LightSource>("Light");
        left->setAttenuation(1.f, 0.7f, 1.8f, 5.f);
        //left->setType(jop::LightSource::Type::Point);
        //rotator->createChild("Left")->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "DefDrawable");
        rotator->createChild("Left")->setPosition(-10000.5, 0, -3);
        
        getDefaultLayer()->addLight(*left);

        //auto right = rotator->createChild("Right");
        //right->createComponent<jop::GenericDrawable>(*getDefaultLayer(), "DefDrawable");
        //right->setPosition(1.5, 0, 0).setScale(0.5f);

        if (!jop::StateLoader::saveState("Scene/test", true, true, true))
            jop::Engine::exit();
    }

    void preUpdate(const float dt) override
    {
        const float sine = static_cast<float>(jop::Engine::getTotalTime()) * 4.f;

        getObject("Def")->rotate(0.f, dt, 0.f);
        //getObject("Def")->getChild("Rotator")->rotate(0.f, 0.f, -dt * 2.f);

        ///*auto l = */getObject("Def")->getChild("Rotator")->getChild("Left")->move(2.f * dt * std::sin(sine), 2.f * dt * std::sin(sine), 0.f);
        //auto r = getObject("Def")->getChild("Rotator")->getChild("Right");

        //l->setScale(0.3f * std::abs(std::sin(sine)) + 0.2f);
        //r->setScale(0.3f * std::abs(std::sin(sine)) + 0.2f);
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

    return e.runMainLoop();
}