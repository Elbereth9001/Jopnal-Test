#pragma once
#include "EventHandler.hpp"


namespace jd
{
    class DemoScene : public jop::Scene
    {
    private:

        JOP_DISALLOW_COPY_MOVE(DemoScene);

    public:

        DemoScene()
            : jop::Scene(typeid(DemoScene).name())
        {
            std::this_thread::sleep_for(std::chrono::seconds(7));

            // Camera & audio listener
            {
                createChild("Cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective)
                      .getObject()->createComponent<jop::Listener>();
            }

            // Skysphere
            {
                createChild("sky")->createComponent<jop::SkySphere>(getRenderer()).setMap(jop::ResourceManager::getResource<jop::Texture2D>("starmap.jpg", true));
            }

            jop::Engine::getSubsystem<jop::Window>()->setEventHandler<EventHandler>();
        }

        void preUpdate(const float /*dt*/) override
        {

        }

        void postUpdate(const float dt) override
        {
            using jop::Keyboard;
            auto& h = *jop::Engine::getSubsystem<jop::Window>()->getEventHandler();

            auto cam = findChild("Cam");

            if (!cam.expired())
            {
                const float speed = 4.f;

                if (h.keyDown(Keyboard::A) || h.keyDown(Keyboard::D))
                    cam->move((h.keyDown(Keyboard::D) ? 1.f : -1.f) * dt * speed * cam->getGlobalRight());

                if (h.keyDown(Keyboard::W) || h.keyDown(Keyboard::S))
                    cam->move((h.keyDown(Keyboard::W) ? 1.f : -1.f) * dt * speed * cam->getGlobalFront());

                if (h.keyDown(Keyboard::Space) || h.keyDown(Keyboard::LShift))
                    cam->move((h.keyDown(Keyboard::Space) ? 1.f : -1.f) * dt * speed * cam->getGlobalUp());
            }
        }
    };
}