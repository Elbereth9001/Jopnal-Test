#include <Jopnal/Jopnal.hpp>


class EventHandler : public jop::WindowEventHandler
{
public:

    EventHandler(jop::Window& w)
        : jop::WindowEventHandler(w)
    {}

    void closed() override
    {
        jop::Engine::exit();
    }

    void keyPressed(const int key, const int, const int) override
    {
        if (key == jop::Keyboard::Escape)
            closed();
        else if (key == jop::Keyboard::Enter)
            jop::broadcast("[SuSc] print", nullptr);
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

        jop::MessageResult sendMessage(const std::string& message, void*) override
        {
            if (message.find("print") != std::string::npos)
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

void printer(char c,
             unsigned char uc,
             unsigned int ui,
             int i,
             unsigned long ul,
             long l,
             unsigned long long ull,
             long long ll,
             float f,
             double d,
             long double ld,
             const std::string& str,
             std::reference_wrapper<int> ref)
{
    JOP_DEBUG_INFO(c << uc << ui << i << ul << l << ull << ll << f << d << ld << str << ref.get());
    ref.get() = 1;
    //return 999;
}

class Class
{
public:

    int print(int i, long l, const std::string& str, std::reference_wrapper<int> ptr)
    {
        JOP_DEBUG_INFO(i << l << str);
        ptr.get() = 123;
        return 0;
    }

    void otherPrint()
    {
        //JOP_DEBUG_INFO(a << b);
    }

    static void useless()
    {
        //return 0;
    }

};

JOP_REGISTER_COMMAND_HANDLER(Class)

    JOP_BIND_MEMBER_COMMAND(Class::print, "print")
    JOP_BIND_MEMBER_COMMAND(Class::otherPrint, "print2")
    JOP_BIND_COMMAND(Class::useless, "useless")

JOP_END_COMMAND_HANDLER(Class)

int main(int c, char** v)
{
    jop::Engine e("JopTestProject", c , v);
    e.loadDefaultConfiguration();

    int iref = 0;
    std::stringstream ss;
    ss << std::hex << &iref;
    jop::CommandHandler h;

    {
        h.bind("print", printer);

        int ret = 0;

        h.execute("print a b 123 123 123 123 123 123 123.0 123.0 123.0 asdf " + ss.str(), &ret);
        //printer('a', 'b', 123, 123, 123, 123, 123, 123, 123.0, 123.0, 123.0, "asdf", iref);
    }

    {
        
        Class test;
        //jop::detail::VoidWrapper wrap(&test);
        //h.bindMember("print", &Class::print);
        h.executeMember("print 1 2 asdf " + ss.str(), &test, nullptr);
        JOP_EXECUTE_MEMBER_COMMAND(Class, "print 1 2 asdf " + ss.str(), test, &iref);
        JOP_EXECUTE_MEMBER_COMMAND(Class, "print2 1 3", test, &iref);
    }

    auto& w = *e.getSubsystem<jop::Window>();
    w.setEventHandler<EventHandler>();

    e.createScene<SomeScene>();

    return e.runMainLoop();
}