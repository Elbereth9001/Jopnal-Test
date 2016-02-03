#include <Jopnal/Jopnal.hpp>


class EventHandler : public jop::WindowEventHandler
{
public:

    EventHandler(jop::Window& w)
        : jop::WindowEventHandler(w)
    {}

    void closed() override
    {
        jop::broadcast("[En-c] exit", nullptr);
    }

    void keyPressed(const int key, const int, const int) override
    {
        if (key == jop::Keyboard::Escape)
            closed();
        else if (key == jop::Keyboard::Enter)
            jop::broadcast("[CoOb=SomeComponent]    print", nullptr);
    }

    void controllerConnected(const int index, const std::string& name) override
    {
        JOP_DEBUG_INFO("Controller connected: " << index << ", " << name);
    }

    void controllerDisconnected(const int index) override
    {
        JOP_DEBUG_INFO("Controller disconnected: " << index);
    }

    void controllerAxisShifted(const int, const int axisIndex, const float shift) override
    {
        JOP_DEBUG_INFO("Axis " << axisIndex << ": " << shift);
    }

    void controllerButtonPressed(const int, const int button) override
    {
        JOP_DEBUG_INFO("Button " << button);
    }

};

class SomeScene : public jop::Scene
{
private:

    class SomeComponent : public jop::Component
    {
    public:

        SomeComponent(jop::Object& object, const std::string& ID)
            : jop::Component(object, ID)
        {}

        SomeComponent(const SomeComponent& other)
            : jop::Component(other)
        {}

        SomeComponent* clone() const override
        {
            auto ptr = std::make_unique<SomeComponent>(*this);
            return ptr.release();
        }

        jop::MessageResult sendMessageImpl(const jop::Message& message) override
        {
            if (message.getString().find("print") != std::string::npos)
                JOP_DEBUG_INFO("Message received!");

            return jop::MessageResult::Escape;
        }

    };

public:

    SomeScene()
        : jop::Scene("SomeScene"),
          m_counter(0.0)
    {
        createLayer<jop::Layer>("asdf");
        getLayer<jop::Layer>();
        auto& o = createObject("SomeObject");
        o.createComponent<SomeComponent>("SomeComponent");
        o.getComponent<SomeComponent>();
    }


    void postFixedUpdate(const double timeStep) override
    {
        m_counter += timeStep;

        if (m_counter >= 2.0)
        {
            //JOP_DEBUG_INFO("2 seconds");
            m_counter -= 2.0;
        }
    }

private:

    double m_counter;
};

struct Test
{
    int mem()
    {
        JOP_DEBUG_INFO("mem");
        return 1;
    }
};

int freef()
{
    JOP_DEBUG_INFO("freef");
    return 0;
}

int main(int c, char* v[])
{

    jop::Engine e("JopTestProject", c , v);
    e.loadDefaultConfiguration();
    int ret;
    jop::CommandHandler h;
    h.bind("freef", &freef);
    h.bindMember("mem", &Test::mem);
    h.execute("freef", nullptr, &ret);
    Test t;
    h.execute("mem", &t, &ret);

    auto& w = *e.getSubsystem<jop::Window>();
    w.setEventHandler<EventHandler>();

    //auto& r = *e.getSubsystem<jop::ResourceManager>();
    //r.getResource<jop::Shader>("v;Shaders/v.vert|f;Shaders/f.frag");

    e.createScene<SomeScene>();

    return e.runMainLoop();
}