#include <Jopnal/Jopnal.hpp>


class SomeComponent : public jop::Component
{
public:

    SomeComponent(jop::Object& obj)
        : jop::Component(obj, "")
    {}

    JOP_GENERIC_CLONE(SomeComponent)
};

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
        obj.createComponent<jop::GenericDrawable>("defc");
        obj.getComponent<jop::GenericDrawable>().lock()->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("Ball", 1.f, 20, 20), def));


        obj.setPosition(0, 0, -3);
        obj.createComponent<SomeComponent>();
        auto& rotator = obj.createChild("Rotator");

        auto& left = rotator.createChild("Left");
        auto& right = rotator.createChild("Right");
        left.createComponent<jop::GenericDrawable>("DefDrawable");
        
        right.createComponent<jop::GenericDrawable>("DefDrawable");
        left.setPosition(-1.5, 0, 0).setScale(0.5f);
        right.setPosition(1.5, 0, 0).setScale(0.5f);
    }

    void preUpdate(const float dt) override
    {
        const float sine = static_cast<float>(jop::Engine::getTotalTime()) * 4.f;

        getObject("Def").lock()->rotate(0.f, dt, 0.f);
        getObject("Def").lock()->getChild("Rotator").lock()->rotate(0.f, 0.f, -dt * 2.f);
        getObject("Def").lock()->getChild("Rotator").lock()->getChild("Left").lock()->setScale(0.3f * std::abs(std::sin(sine)) + 0.2f);
        getObject("Def").lock()->getChild("Rotator").lock()->getChild("Right").lock()->setScale(0.3f * std::abs(std::sin(sine)) + 0.2f);
    }
};

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
    };

    e.getSubsystem<jop::Window>()->setEventHandler<EventHandler>();

    e.createScene<SomeScene>();

    return e.runMainLoop();
}