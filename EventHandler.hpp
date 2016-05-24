#pragma once
#include "Spawner.hpp"
#include "LightBall.hpp"


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
            using jop::Engine;

            if (key == Keyboard::Escape)
                closed();

            if (Engine::hasCurrentScene())
            {
                auto& scene = Engine::getCurrentScene();

                if (key == Keyboard::T)
                    scene.printDebugTree();

                if (key == Keyboard::O)
                {
                    auto spawn = scene.findChild("spawn");
                    if (!spawn.expired())
                        spawn->getComponent<Spawner>()->toggleSpawning();
                }

                if (key == Keyboard::I)
                {
                    auto spawn = scene.findChild("lball");
                    if (!spawn.expired())
                        spawn->getComponent<LightBall>()->toggleSpawning();
                }

                if (key == Keyboard::M)
                {
                    auto mus = Engine::getCurrentScene().getComponent<jop::SoundStream>();

                    mus->getStatus() == jop::SoundSource::Status::Paused ? mus->play(false) : mus->pause();
                }
            }

            if (key == Keyboard::Key::P)
                Engine::setState(Engine::getState() == Engine::State::Running ? Engine::State::RenderOnly : Engine::State::Running);

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

            auto cam = Engine::getCurrentScene().findChild("cam");

            if (!cam.expired())
            {
                static glm::vec2 totals(0.f);
                const glm::vec2 delta = glm::vec2(x, y) * 0.1f;

                totals.x += delta.x;
                totals.y = glm::clamp(totals.y + delta.y, -85.f, 85.f);

                cam->setRotation(glm::radians(-totals.y), glm::radians(-totals.x), 0.f);
            }
        }
    };
}