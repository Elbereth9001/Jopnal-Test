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

            if ((mods & Keyboard::Modifier::Control) != 0 && key == Keyboard::S)
            {
                jop::SettingManager::save();
            }
        }

        void mouseMoved(const float x, const float y) override
        {
            using jop::Engine;

            if (!Engine::hasCurrentScene())
                return;

            auto cam = Engine::getCurrentScene().findChild("Cam");

            if (!cam.expired())
            {
                static float mx = 0.f;
                static float my = 0.f;
                mx += x;
                my = glm::clamp(my + y, -85.f, 85.f);

                cam->setRotation(glm::radians(-my), glm::radians(-mx), 0.f);
            }
        }
    };
}