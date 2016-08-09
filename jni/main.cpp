
#include <Jopnal/Jopnal.hpp>

#include "EventHandler.hpp"
#include "Object.hpp"
#include "Player.hpp"

class MyScene : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_player;
    jop::WeakReference<jop::Object> m_ground;
    jop::Material* newMaterial;

    std::pair<float, float> m_endPoint;

public:

    MyScene()
        : jop::Scene("MyScene"),
        m_player()
    {

        getWorld<2>().setDebugMode(true);

        auto m_cam = createChild("cam");
        m_cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective);
        m_cam->setPosition(0.f, 0.f, 5.f);

        //Player
        {
            m_player = createChild("player");

            jop::RigidBody2D::ConstructInfo2D playerInfo(jop::ResourceManager::getNamed<jop::RectangleShape2D >("player", 1.f, 2.f), jop::RigidBody2D::Type::Dynamic, 1.2f);
            m_player->createComponent<jop::RigidBody2D>(getWorld<2>(), playerInfo);
            //m_player->createComponent<jop::Drawable>(getRenderer());
            m_player->setPosition(15.f, 15.f, 0.f);

            auto drawable = m_player->getComponent<jop::Drawable>();
        }

        //Ground
#if 1
        {
            jop::Randomizer r;
            std::vector<glm::vec2> ground;

            float firstPointX = 0.f;
            float secondPointX;
            float firstPointY = 0.f;
            float secondPointY;

            ground.emplace_back(firstPointX, secondPointX);

            for (unsigned int i = 0; i < r.range<unsigned int>(0u, 6u); ++i)
            {
                secondPointX = r.range<float>(firstPointX, firstPointX + 5.f);
                ground.emplace_back(firstPointX, secondPointX);
                firstPointX = secondPointX;

                secondPointY = r.range<float>(firstPointY - 2.f, firstPointY + 2.f);
                ground.emplace_back(firstPointY, secondPointY);
                firstPointY = secondPointY;
            }

            m_endPoint = std::make_pair(secondPointX, secondPointY);

            jop::RigidBody2D::ConstructInfo2D groundInfo(jop::ResourceManager::getNamed<jop::TerrainShape2D>("ground", ground));

            m_ground = createChild("ground");
            m_ground->createComponent<jop::RigidBody2D>(getWorld<2>(), groundInfo);

        }
#endif
        // To modify the drawable's material, we must create a new one to replace the default
        //newMaterial = &jop::ResourceManager::getEmpty<jop::Material>("newMaterial", true);
        //drawable->getModel().setMaterial(*newMaterial);//.setMesh(jop::ResourceManager::getNamed<jop::SphereMesh>("ball", 0.5f, 20));

    }

    void preUpdate(const float deltaTime) override
    {
        using jc = jop::Controller;

        float moveX = jc::getAxisOffset(0u, jc::Playstation::Axis::LeftStickX);
        float moveY = jc::getAxisOffset(0u, jc::Playstation::Axis::LeftStickY);

        m_player->move(moveX, moveY, 0.f);
        m_player->getComponent<jop::RigidBody2D>()->synchronizeTransform();

        JOP_DEBUG_INFO("move X: " << moveX);
        JOP_DEBUG_INFO("move Y: " << moveY);

    }
};

int main(int argc, char* argv[])
{
#ifdef JOP_OS_ANDROID
    jop::SettingManager::setDefaultDirectory("defconf");
    jop::SettingManager::setOverrideWithDefaults();
#endif

    JOP_ENGINE_INIT("MyProject", argc, argv);

    jop::Engine::createScene<MyScene>();

    return JOP_MAIN_LOOP;
}
