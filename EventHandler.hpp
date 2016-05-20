#pragma once
#include <Jopnal/Jopnal.hpp>


namespace jd
{
    class EventHandler : public jop::WindowEventHandler
    {
    public:

        EventHandler(jop::Window& window)
            : jop::WindowEventHandler(window)
        {}

        void closed() override
        {
            jop::Engine::exit();
        }

        void keyPressed(const int key, const int, const int mods) override
        {
            using jop::Keyboard;

            if (key == Keyboard::Escape)
                closed();

            else if (jop::Engine::hasCurrentScene() && key == Keyboard::T)
                jop::Engine::getCurrentScene().printDebugTree();

            else if ((mods & Keyboard::Modifier::Control) != 0 && key == Keyboard::S)
            {
                jop::SettingManager::save();
            }
        }

        void mouseMoved(const float x, const float y) override
        {
            using jop::Engine;

            if (!Engine::hasCurrentScene())
                return;

            auto cam = Engine::getCurrentScene().findChild("cam");

            if (!cam.expired())
            {
                static glm::vec2 totals(0.f);
                const glm::vec2 delta = glm::vec2(x, y) * 0.5f;

                totals.x += delta.x;
                totals.y = glm::clamp(totals.y + delta.y, -85.f, 85.f);

                cam->setRotation(glm::radians(-totals.y), glm::radians(-totals.x), 0.f);
            }
        }
    };
}