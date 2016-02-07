#include <Jopnal/Jopnal.hpp>


class EventHandler : public jop::WindowEventHandler
{
public:

    EventHandler(jop::Window& w)
        : jop::WindowEventHandler(w)
    {}

    void closed() override
    {
        jop::broadcast("[En-c] exit");
    }

    void keyPressed(const int key, const int, const int) override
    {
        if (key == jop::Keyboard::Escape)
            closed();
    }
};

class SomeScene : public jop::Scene
{
public:

    SomeScene()
        : jop::Scene("SomeScene")
    {}

    void initialize() override
    {
        auto& obj = createObject("Def");
        obj.createComponent<jop::DefaultDrawable>(getDefaultLayer(), "DefDrawable");
        obj.getComponent<jop::DefaultDrawable>().lock()->setModel(*jop::ResourceManager::getNamedResource<jop::SphereModel>("Ball", 0.5f, 20, 20).lock());
        obj.setPosition(0, 0, -3);

        auto& rotator = obj.createChild("Rotator");

        auto& left = rotator.createChild("Left");
        auto& right = rotator.createChild("Right");
        left.createComponent<jop::DefaultDrawable>("DefDrawable");

        right.createComponent<jop::DefaultDrawable>("DefDrawable");
        left.setPosition(-1.5, 0, 0).setScale(0.5f);
        right.setPosition(1.5, 0, 0).setScale(0.5f);
    }

    void preUpdate(const double dt) override
    {
        static float sine = 0;
        sine += dt * 4;

        getObject("Def").lock()->rotate(dt, dt, 0);
        getObject("Def").lock()->getChild("Rotator").lock()->rotate(0, 0, -dt * 2);
        getObject("Def").lock()->getChild("Rotator").lock()->getChild("Left").lock()->setScale(0.3 * std::abs(std::sin(sine)) + 0.2);
        getObject("Def").lock()->getChild("Rotator").lock()->getChild("Right").lock()->setScale(0.3 * std::abs(std::sin(sine)) + 0.2);


    }
};

int main(int c, char* v[])
{
    jop::Engine e("JopTestProject", c , v);
    e.loadDefaultConfiguration();
    e.getSubsystem<jop::Window>()->setEventHandler<EventHandler>();
    

    e.createScene<SomeScene>();

    return e.runMainLoop();
}