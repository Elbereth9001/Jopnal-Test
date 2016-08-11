#ifndef EVENTS_HPP
#define EVENT_HPP

#include <Jopnal/Jopnal.hpp>

namespace jd
{
    using namespace jop;

    class EventHandler : public WindowEventHandler
    {
    public:

        EventHandler(Window& window)
            : WindowEventHandler(window)
        {}

        void closed() override
        {
            Engine::exit();
        }

        void controllerButtonPressed(const int index, const int button) override
        {
            using c = Controller::XBox;
            using k = Keyboard;

            if (button == c::Back)
                keyPressed(k::Escape, 0u, 0u);

            if (button == Controller::XBox::Start)
                keyPressed(k::P, 0u, 0u);
        }

        void keyPressed(const int key, const int, const int mods) override
        {
            using k = Keyboard;

            if (key == Keyboard::Escape)
            {
                auto& s = static_cast<tehGame&>(Engine::getCurrentScene());
                s.hasEnded ? closed() : s.end();
            }
            if (Engine::getState() == Engine::State::Running)
            {
                if (key == Keyboard::Key::P || key == Keyboard::Key::Pause)
                    Engine::setState(Engine::getState() == Engine::State::Running ? Engine::State::Frozen : Engine::State::Running);
            }
        }
    };
}
#endif