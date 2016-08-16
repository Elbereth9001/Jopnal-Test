#include <Jopnal/Jopnal.hpp>
#include "Consts.hpp"
#include "CharacterHandler.hpp"
#include "Player.hpp"
#include "World.hpp"

class tehGame : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_bullets;
    jop::WeakReference<jop::Object> m_cam;
    Player* m_playerPtr;

    std::vector<Characters*> m_chars;

    SceneWorld m_world;

    void init();
    void createUI();
    void jump(jop::RigidBody2D& body);

public:

    bool hasEnded;

    void destroy();
    void end();

    tehGame()
        : Scene("tehGame"),

        m_bullets(createChild("bullets")->reserveChildren((unsigned int)(s_maxBullets * 1.1f))),
        m_cam(createChild("cam")),
        m_playerPtr(nullptr),
        m_world(*this),
        m_chars(),
        hasEnded(false)
    {
        getWorld<2>().setDebugMode(true);

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
        {
            auto score = findChild("scoreText");
            score->setPosition(
                p.x + 5.f,
                p.y + 5.f,
                p.z
                );
            score->getComponent<jop::Text>()->setString(std::to_string(g_score)).setColor(jop::Color::Orange);
        }
        {
            auto health = findChild("healthText");
            health->setPosition(
                p.x - 5.f,
                p.y - 5.f,
                p.z
                );
            health->getComponent<jop::Text>()->setString(std::to_string(m_playerPtr->getHealth())).setColor(jop::Color::Green);
        }

#endif
    }

    void postUpdate(const float deltaTime) override
    {
        using c = jop::Controller;
        using k = jop::Keyboard;
        using m = jop::Mouse;

        std::for_each(m_chars.begin(), m_chars.end(), [deltaTime](Characters* c)
        {
            c->reduceCDs(deltaTime);
        });

        //Jump
#if 1
        if (g_cc)
            if (c::isButtonDown(0u, c::XBox::A))
                m_playerPtr->jump();

        if (k::isKeyDown(k::Space))
            m_playerPtr->jump();
#endif

        //Shoot
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
        {
            for (auto& bulletsItr : m_bullets->getChildren())
            {
                //Not immune
                if (!charsItr->isImmune())
                {
                    //If there is a contact
                    if (charsItr->m_character->getComponent<jop::RigidBody2D>()->checkContact(*bulletsItr.getComponent<jop::Collider2D>()))
                    {
                        charsItr->dealDamage(1);
                        charsItr->applyImmuneCD();

                        //This character has been wounded, end bulletsItr
                        break;
                    }
                }
            }
        }
#endif

        //Remove dead
#if 1
        for (unsigned int i = 0; i < m_chars.size(); ++i)
        {
            if (m_chars[i]->getHealth() <= 0)
            {
                if (m_chars[i] == m_playerPtr)
                    end();
                else
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
                    m_chars[i]->m_character->removeSelf();
                    delete m_chars[i];
                    m_chars.erase(m_chars.begin() + i);
                }
            }
        }

#endif

        //Remove bullets
#if 1
        if (m_bullets->childCount() > s_maxBullets)
            for (unsigned int i = 0; i < m_bullets->childCount() - (s_maxBullets * 1.1f); ++i) //<<< Remove some extra -> don't have to remove all the time
                m_bullets->getChildren()[i].removeSelf();

        std::for_each(m_bullets->getChildren().begin(), m_bullets->getChildren().end(), [](jop::Object& obj)
        {
            if (obj.getGlobalPosition().y < s_maxWorldLength * s_minWorldOffsetY)
                obj.removeSelf();
        });

#endif


        //NextLevel
#if 1
        if (findChild("player")->getGlobalPosition().x >= m_world.m_endPoint)
        {
            g_score += m_world.m_levelLength;
            g_healthPlayerStart = m_playerPtr->getHealth();
            if (g_density != 1u)
                --g_density;
            destroy();
        }
#endif

        //EndGame
#if 1
        if (findChild("player")->getGlobalPosition().x < -5.f)
            end();
#endif

    }
};

void tehGame::end()
{
    jop::Vibrator::vibrate(1000u);
    jop::Engine::setState(jop::Engine::State::RenderOnly);

    static_cast<jop::SoundEffect&>(*m_playerPtr->m_character->getComponent<jop::SoundEffect>(23u)).play();

    m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);
    m_cam->getComponent<jop::Camera>()->setRenderMask(2u);
    {
        jop::WeakReference<jop::Object> overText = createChild("overText");
        overText->setScale(0.01f).setPosition(s_playerStartPos.x, s_playerStartPos.y + 2.f, s_playerStartPos.z);
        overText->createComponent<jop::Text>(getRenderer()).setRenderGroup(1u);
        overText->getComponent<jop::Text>()->setString("Game over!").setColor(jop::Color::Purple);
    }
    {
        jop::WeakReference<jop::Object> scoreText = createChild("scoreText");
        scoreText->setScale(0.01f).setPosition(s_playerStartPos);
        scoreText->createComponent<jop::Text>(getRenderer()).setRenderGroup(1u);
        scoreText->getComponent<jop::Text>()->setString("Your score: " + std::to_string(g_score)).setColor(jop::Color::Orange);
    }
    hasEnded = true;
}

void tehGame::init()
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
}

void tehGame::createUI()
{
    auto p = m_cam->getGlobalPosition();
    {
        jop::WeakReference<jop::Object> score = createChild("scoreText");
        score->setScale(0.02f);
        score->createComponent<jop::Text>(getRenderer());
        score->getComponent<jop::Text>()->setString(std::to_string(g_score)).setColor(jop::Color::Purple);
    }
    {
        jop::WeakReference<jop::Object> health = createChild("healthText");
        health->setScale(0.02f);
        health->createComponent<jop::Text>(getRenderer());
        health->getComponent<jop::Text>()->setString(std::to_string(m_playerPtr->getHealth())).setColor(jop::Color::Green);
    }
}

void tehGame::destroy()
{
    jop::ResourceManager::unload("ground");
    jop::Engine::createScene<tehGame>();
}

void getResources()
{
    using namespace jop;

    static const jop::RigidBody2D::ConstructInfo2D bulletInfo(jop::ResourceManager::getNamed<jop::ConeShape2D>("bullet", 0.2f, 0.6f), jop::RigidBody2D::Type::Dynamic, 0.2f);
    c_bulletInfo = &bulletInfo;

    static const RigidBody2D::ConstructInfo2D playerInfo(ResourceManager::getNamed<CapsuleShape2D>("player", 1.f, 2.f), RigidBody2D::Type::Dynamic, 1.2f);
    c_playerInfo = &playerInfo;

    c_animAtlasPlayer = &ResourceManager::get<AnimationAtlas>("tehGame/art_spsh_small.png", glm::uvec2(4u, 3u), glm::uvec2(0u, 0u), glm::uvec2(3399u, 3487u)); //glm::uvec2(7650u, 8134u));

    static const Model bulletmodel(
        ResourceManager::getNamed<RectangleMesh>("bulletmesh", glm::vec2(0.35f, 0.75f)),
        ResourceManager::getEmpty<Material>("bulletmat", true).setMap(Material::Map::Diffuse, ResourceManager::get<Texture2D>("tehGame/bullet.png")));
    c_bulletModel = &bulletmodel;

    static const Model playermodel(
        ResourceManager::getNamed<RectangleMesh>("playermesh", glm::vec2(1.f, 2.f)),
        ResourceManager::getEmpty<Material>("playermat", true).setMap(Material::Map::Diffuse, ResourceManager::get<Texture2D>("tehGame/player.png")));
    c_playerModel = &playermodel;

    static const Model crosshairmodel(
        ResourceManager::getNamed<CircleMesh>("crosshairmesh", 0.5f, 15u),
        ResourceManager::getEmpty<Material>("crosshairmat", true).setMap(Material::Map::Diffuse, ResourceManager::get<Texture2D>("tehGame/crosshair.png")));
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