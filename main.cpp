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

JOP_REGISTER_SAVEABLE(a, SomeComponent)[](const jop::Component& c, jop::json::Value& v, jop::json::Value::AllocatorType& a) -> bool
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
        jop::Material def = jop::Material::getDefault();
        def.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture>("asdf"));

        auto& obj = createObject("Def");
        obj.createComponent<jop::GenericDrawable>(getDefaultLayer(), "defc");
        obj.getComponent<jop::GenericDrawable>().lock()->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("Ball", 1.f, 20, 20), def));

        obj.setPosition(0, 0, -3);
        obj.createComponent<SomeComponent>();
        auto& rotator = obj.createChild("Rotator");

        auto& left = rotator.createChild("Left");
        auto& right = rotator.createChild("Right");
        left.createComponent<jop::GenericDrawable>(getDefaultLayer(), "DefDrawable");

        right.createComponent<jop::GenericDrawable>(getDefaultLayer(), "DefDrawable");
        left.setPosition(-1.5, 0, 0).setScale(0.5f);
        right.setPosition(1.5, 0, 0).setScale(0.5f);

        jop::Object().createComponent<jop::GenericDrawable>("a");

        /*if (!jop::StateLoader::saveState("Scene/test", true, true, true))
            jop::Engine::exit();*/

            if (!jop::StateLoader::getInstance().loadState("Scene/test", true, true, true))
            jop::Engine::exit();
    }

    void preUpdate(const float dt) override
    {
        const float sine = static_cast<float>(jop::Engine::getTotalTime()) * 4.f;

        getObject("Def").lock()->rotate(0.f, dt, 0.f);
        getObject("Def").lock()->getChild("Rotator").lock()->rotate(0.f, 0.f, -dt * 2.f);
        getObject("Def").lock()->getChild("Rotator").lock()->getChild("Left").lock()->setScale(0.3f * std::abs(std::sin(sine)) + 0.2f);
        getObject("Def").lock()->getChild("Rotator").lock()->getChild("Right").lock()->setScale(0.3f * std::abs(std::sin(sine)) + 0.2f);

    }

    int getInt(float& a, float b) const
    {
        a = 0.f;
        return 1;
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

JOP_REGISTER_COMMAND_HANDLER(SomeScene)

    JOP_BIND_MEMBER_COMMAND(&SomeScene::getInt, "getInt");

JOP_END_COMMAND_HANDLER(SomeScene)

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
        }
    };

    jop::ResourceManager::getResource<jop::Texture>("asdf");

    e.getSubsystem<jop::Window>()->setEventHandler<EventHandler>();

    auto& s = e.createScene<SomeScene>();

    float a = 1.f;
    std::stringstream ss;
    ss << std::hex << &a;

    jop::Any wrap(0);
    jop::Any inst(&s);
    JOP_EXECUTE_COMMAND(SomeScene, "getInt " + ss.str(), inst, wrap);

    return e.runMainLoop();
}