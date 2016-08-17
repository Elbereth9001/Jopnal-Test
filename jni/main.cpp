#include <Jopnal/Jopnal.hpp>
#include "Consts.hpp"
#include "CharacterHandler.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "UserInterfaceHandler.hpp"

class tehGame : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_bullets;
    jop::WeakReference<jop::Object> m_cam;
    UI m_ui;
    Player* m_playerPtr;

    std::vector<Characters*> m_chars;
    SceneWorld m_world;

    /////////////////////////////////////////////////

    void init()
    {
        m_world.createWorld();
        m_chars.push_back(CharacterFactory::createCharacter(*this, CharacterType::playerC, s_playerStartPos));
        m_playerPtr = static_cast<Player*>(m_chars[0]);
        m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);

        auto r = []()
        {
            static jop::Randomizer rand;
            return rand.range(0.f, 1.f);
        };

        unsigned int amount = m_world.m_groundP.size() / g_density;
        auto& w = m_world.m_groundP;
        unsigned int index;

        for (unsigned int i = 3u; i < amount; ++i)
        {
            index = i + g_density * i;

            if (index >= w.size())
                break;

            m_chars.push_back(CharacterFactory::createCharacter(
                *this,
                r() < 0.5f ? CharacterType::staticC : CharacterType::targetC,
                glm::vec3(
                w[index].x + (r() * 1.5f),
                w[index].y + (r() * 1.5f),
                s_playerStartPos.z)));
        }

        JOP_DEBUG_INFO("Health: " << g_health);
        JOP_DEBUG_INFO("Score: " << g_score);
    }

    /////////////////////////////////////////////////

    void createUI()
    {
        auto p = m_cam->getGlobalPosition();

        {
            std::vector<glm::vec3> positions({ glm::vec3(p.x + 200.f, p.y + 200.f, p.z + 6.f), glm::vec3(p.x - 200.f, p.y - 200.f, p.z + 6.f) });
            std::vector<std::string> names({ std::string("score"), std::string("health") });
            std::vector<int> values({ (int)g_score, g_health });

            m_ui.createScreen(std::string("HUD"), positions, names, 0u, values);
        }

        {
            std::vector<glm::vec3> positions({ glm::vec3(p.x, p.y - 20.f, p.z + 6.f), glm::vec3(p.x, p.y + 20.f, p.z + 6.f) });
            std::vector<std::string> names({ std::string("resume"), std::string("options") });

            m_ui.createScreen(std::string("PAUSEMENU"), positions, names, 1u);
        }

    }

    /////////////////////////////////////////////////

    void removeDeadEnemies()
    {
        for (unsigned int i = 1u; i < m_chars.size(); ++i)
        {
            if (m_chars[i]->getHealth() <= 0)
            {
                switch (m_chars[i]->getType())
                {
                case CharacterType::staticC:
                    g_score += 1u;
                    break;
                case CharacterType::targetC:
                    g_score += 5u;
                    break;
                }
                removeNews(i);
            }
        }
    }

    /////////////////////////////////////////////////

    void removeBullets()
    {
        //Too many bullets in game
        if (m_bullets->childCount() > s_maxBullets)
            for (unsigned int i = 0; i < m_bullets->childCount() - (s_maxBullets * 1.1f); ++i) //<<< Remove some extra -> don't have to remove all the time
                m_bullets->getChildren()[i].removeSelf();

        //Bullet out of game area
        std::for_each(m_bullets->getChildren().begin(), m_bullets->getChildren().end(), [](jop::Object& obj)
        {
            if (obj.getGlobalPosition().y < s_maxWorldLength * s_minWorldOffsetY)
                obj.removeSelf();
        });
    }

    /////////////////////////////////////////////////

    void removeNews(const unsigned int index)
    {
        m_chars[index]->m_character->removeSelf();
        delete m_chars[index];
        m_chars.erase(m_chars.begin() + index);
    }

public:

    bool hasEnded;
    bool m_isPaused;

    /////////////////////////////////////////////////

    void showMenu(const bool show)
    {
        using je = jop::Engine;

        JOP_DEBUG_INFO("Health: " << g_health);
        JOP_DEBUG_INFO("Score: " << g_score);

        je::setState(show ? je::State::RenderOnly : je::State::Running);

        show ? m_cam->getComponent<jop::Camera>()->setRenderMask(7u) : m_cam->getComponent<jop::Camera>()->setRenderMask(1u);
    }

    /////////////////////////////////////////////////

    void destroy()
    {
        for (unsigned int i = 0; i < m_chars.size(); ++i)
        {
            removeNews(i);
        }
        jop::ResourceManager::unload("ground");
        jop::Engine::createScene<tehGame>();
    }

    /////////////////////////////////////////////////

    void end()
    {
        jop::Vibrator::vibrate(1000u);
        jop::Engine::setState(jop::Engine::State::RenderOnly);

        static_cast<jop::SoundEffect&>(*m_playerPtr->m_character->getComponent<jop::SoundEffect>(23u)).play();

        m_cam->getComponent<jop::Camera>()->setRenderMask(2u);

        m_ui.setNewMask("HUD", 3u);
        m_ui.setNewMask("PAUSEMENU", 3u);

        auto p = m_cam->getGlobalPosition();
        {
            std::vector<glm::vec3> positions({ glm::vec3(p.x, p.y - 20.f, p.z + 6.f), glm::vec3(p.x, p.y + 20.f, p.z + 6.f) });
            std::vector<std::string> names({ std::string("final score"), std::string("health") });
            std::vector<int> values({ (int)g_score, g_health });

            m_ui.createScreen(std::string("GAMEOVER"), positions, names, 0u, values);
        }

        hasEnded = true;

        JOP_DEBUG_INFO("Health: " << g_health);
        JOP_DEBUG_INFO("Score: " << g_score);

        //std::this_thread::sleep_for(std::chrono::milliseconds(1500u));
        for (unsigned int i = 0; i < m_chars.size(); ++i)
        {
            removeNews(i);
        }
    }

    /////////////////////////////////////////////////

    tehGame()
        : Scene("tehGame"),

        m_bullets(createChild("bullets")->reserveChildren((unsigned int)(s_maxBullets * 1.1f))),
        m_cam(createChild("cam")),
        m_ui(*this, m_cam),
        m_playerPtr(nullptr),
        m_world(*this),
        m_chars(),
        m_isPaused(false),
        hasEnded(false)
    {
        //getWorld<2>().setDebugMode(true);

        m_cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic).setSize(15.f, 15.f * (g_screenSize.y / g_screenSize.x));
        m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);

        init();

        createUI();

        jop::Engine::setState(jop::Engine::State::Running);
    }

    void preUpdate(const float deltaTime) override
    {
        using jc = jop::Controller;
        using jk = jop::Keyboard;
        using jm = jop::Mouse;

        g_cc = jc::isControllerPresent(0u);


        //Player move
#if 1
        {
            auto pl = findChild("player");
            auto pl_rb = pl->getComponent<jop::RigidBody2D>();
            float moveX = 0.f;
            float moveY = 0.f;

            if (g_cc)
            {
                moveX = jc::getAxisOffset(0u, jc::XBox::Axis::LeftStickX);
                moveY = jc::getAxisOffset(0u, jc::XBox::Axis::LeftStickY);
            }

            if (jk::isKeyDown(jk::A))
                moveX = -1.f;
            else if (jk::isKeyDown(jk::D))
                moveX = 1.f;

            if (jk::isKeyDown(jk::W))
                moveY = 1.f;
            else if (jk::isKeyDown(jk::S))
                moveY = -1.f;

            pl_rb->applyCentralForce(glm::vec2(moveX * 25.f, moveY * 15.f));
            pl_rb->synchronizeTransform();
            pl->getComponent<jop::AnimatedSprite>()->play();

            glm::vec2 vel = pl_rb->getLinearVelocity();

            //No animation when moving slowly
            if ((vel.x < 1.f && vel.x > -1.f) && (vel.y < 1.f && vel.y > -1.f))
                pl->getComponent<jop::AnimatedSprite>()->pause();

            //Speedlimits
            pl_rb->setLinearVelocity(glm::vec2(
                std::min(std::max(vel.x, -s_maxMoveSpeed), s_maxMoveSpeed),
                std::min(std::max(vel.y, -s_maxMoveSpeed), s_maxMoveSpeed)
                ));

        }
#endif

        //Player aim
#if 1
        {
            float aimX = 0.f;
            float aimY = 0.f;

            if (g_cc)
            {
                aimX = jc::getAxisOffset(0u, jc::XBox::Axis::RightStickX);
                aimY = jc::getAxisOffset(0u, jc::XBox::Axis::RightStickY);

                if (aimX != 0.f || aimY != 0.f)
                    m_playerPtr->m_crossHairLastDir = glm::normalize(glm::vec3(aimX, aimY, m_playerPtr->m_crossHairLastDir.z)) * s_limitCrossHair;
            }

            aimX = jm::getPosition().x;
            aimY = g_screenSize.y - jm::getPosition().y;

            m_playerPtr->aim(aimX, aimY);
        }
#endif

        //Player stand up
#if 1
        if (m_playerPtr->m_character->getGlobalRotation().z < -0.05f)
            m_playerPtr->m_character->getComponent<jop::RigidBody2D>()->applyTorque(9.f);

        if (m_playerPtr->m_character->getGlobalRotation().z > 0.05f)
            m_playerPtr->m_character->getComponent<jop::RigidBody2D>()->applyTorque(-9.f);
#endif

        //Camera & UI follow
#if 1
        m_cam->setPosition(
            m_playerPtr->m_character->getGlobalPosition().x,
            m_playerPtr->m_character->getGlobalPosition().y,
            m_cam->getGlobalPosition().z
            );

        auto p = m_cam->getGlobalPosition();

        m_ui.updateScreenValues(std::string("HUD"), std::vector<int>({ (int)g_score, g_health }));
        m_ui.updateScreenPosition("HUD", p);
        m_ui.updateScreenPosition("PAUSEMENU", p);


#endif
    }

    void postUpdate(const float deltaTime) override
    {
        using c = jop::Controller;
        using k = jop::Keyboard;
        using m = jop::Mouse;

        //Reduce cooldowns
#if 1
        std::for_each(m_chars.begin(), m_chars.end(), [deltaTime](Characters* c)
        {
            c->reduceCDs(deltaTime);
        });
#endif

        //Jump
#if 1
        if (g_cc)
            if (c::isButtonDown(0u, c::XBox::A))
                m_playerPtr->jump();

        if (k::isKeyDown(k::Space))
            m_playerPtr->jump();
#endif

        //Everyone shooting
#if 1
        if (g_cc)
            if ((c::getAxisOffset(0u, c::XBox::RightStickX) != 0.f) || (c::getAxisOffset(0u, c::XBox::RightStickY) != 0.f))
                m_playerPtr->shoot();

        if (m::isButtonDown(m::Left))
            m_playerPtr->shoot();

        std::for_each(m_chars.begin() + 1u, m_chars.end(), [](Characters* c){c->shoot(); });

#endif

        //Deal damage
#if 1

        // A character receives only one wound per turn, no matter how many bullets are touching
        // A single bullet can harm many characters at once

        for (auto& charsItr : m_chars)
            for (auto& bulletsItr : m_bullets->getChildren())
                //Not immune
                if (!charsItr->isImmune())
                    //If there is a contact
                    if (charsItr->m_character->getComponent<jop::RigidBody2D>()->checkContact(*bulletsItr.getComponent<jop::Collider2D>()))
                    {
                        charsItr->dealDamage(1);
                        charsItr->applyImmuneCD();

                        //This character has been wounded, end bulletsItr
                        break;
                    }
#endif

        //Remove excess objects
#if 1
        removeDeadEnemies();
        removeBullets();
#endif

        //Endgame / NextLevel
#if 1
        if ((m_playerPtr->getHealth() <= 0u) ||
            (m_playerPtr->m_character->getGlobalPosition().x < -5.f))
            end();

        else if (m_playerPtr->m_character->getGlobalPosition().x >= m_world.m_endPoint)
        {
            ++g_level;
            g_score += (m_world.m_levelLength - s_minWorldLength);
            g_health = m_playerPtr->getHealth();
            g_density != 1u ? --g_density : g_density;
            destroy();
        }
#endif

    }
};

void getResources()
{
    using namespace jop;
    using jr = jop::ResourceManager;
    using jb = jop::RigidBody2D;

    static const jb::ConstructInfo2D bulletInfo(jr::getNamed<ConeShape2D>("bullet", 0.2f, 0.6f), jb::Type::Dynamic, 0.2f);
    c_bulletInfo = &bulletInfo;

    static const jb::ConstructInfo2D playerInfo(jr::getNamed<CapsuleShape2D>("player", 1.f, 2.f), jb::Type::Dynamic, 1.2f);
    c_playerInfo = &playerInfo;

    c_animAtlasPlayer = &jr::get<AnimationAtlas>("tehGame/art_spsh_small.png", glm::uvec2(4u, 3u), glm::uvec2(0u, 0u), glm::uvec2(3399u, 3487u)); //glm::uvec2(7650u, 8134u));

    static const Model background1(
        jr::getNamed<RectangleMesh>("bg1mesh", glm::vec2(
        (s_minWorldOffsetX + s_maxWorldOffsetX)*s_maxWorldLength*0.5f,
        (-s_minWorldOffsetY + s_maxWorldOffsetY)*s_maxWorldLength*0.5f
        )),
        jr::getEmpty<Material>("bg1mat", true).setMap(Material::Map::Diffuse, jr::get<Texture2D>("tehGame/starmap.jpg")));
    c_backgroundModel1 = &background1;

    static const Model background2(
        jr::getNamed<RectangleMesh>("bg2mesh", glm::vec2(
        (s_minWorldOffsetX + s_maxWorldOffsetX)*s_maxWorldLength*0.5f,
        (-s_minWorldOffsetY + s_maxWorldOffsetY)*s_maxWorldLength*0.5f
        )),
        jr::getEmpty<Material>("bg2mat", true).setMap(Material::Map::Diffuse, jr::get<Texture2D>("tehGame/bg.png")));
    c_backgroundModel2 = &background2;

    //c_bgTexture = &jr::get<Texture2D>("tehGame/bg.jpg", glm::uvec2(1920u, 1080u), Texture2D::Format::RGB_UB_8, nullptr);

    static const Model bulletmodel(
        jr::getNamed<RectangleMesh>("bulletmesh", glm::vec2(0.35f, 0.75f)),
        jr::getEmpty<Material>("bulletmat", true).setMap(Material::Map::Diffuse, jr::get<Texture2D>("tehGame/bullet.png")));
    c_bulletModel = &bulletmodel;

    static const Model playermodel(
        jr::getNamed<RectangleMesh>("playermesh", glm::vec2(1.f, 2.f)),
        jr::getEmpty<Material>("playermat", true).setMap(Material::Map::Diffuse, jr::get<Texture2D>("tehGame/player.png")));
    c_playerModel = &playermodel;

    static const Model crosshairmodel(
        jr::getNamed<CircleMesh>("crosshairmesh", 0.5f, 15u),
        jr::getEmpty<Material>("crosshairmat", true).setMap(Material::Map::Diffuse, jr::get<Texture2D>("tehGame/crosshair.png")));
    c_crosshairModel = &crosshairmodel;

}

#include "EventHandler.hpp"
int main(int argc, char* argv[])
{
#ifdef JOP_OS_ANDROID
    jop::SettingManager::setDefaultDirectory("defconf");
    jop::SettingManager::setOverrideWithDefaults();
#endif

    JOP_ENGINE_INIT("MyProject", argc, argv);

#ifdef JOP_OS_ANDROID
    JOP_ASSERT(Controller::isControllerPresent(0u), "System: Android && no controller detected, exiting.");
#endif

    getResources();

    jop::Engine::getSubsystem<jop::Window>()->setEventHandler<jd::EventHandler>();

    g_screenSize = jop::Engine::getMainRenderTarget().getSize();

    jop::Engine::createScene<tehGame>();

    return JOP_MAIN_LOOP;
}