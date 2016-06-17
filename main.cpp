#include "DemoScene.hpp"
#include "LoadingScene.hpp"

int main(int c, char* v[])
{
    JOP_ENGINE_INIT("JopTestProject", c, v);
    
    jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);

    jop::Engine::createScene<jd::LoadingScene>();
    jop::Engine::createScene<jd::DemoScene, true, true>();

    return JOP_MAIN_LOOP;
}