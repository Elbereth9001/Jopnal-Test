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
    void createCamera();
    void jump(jop::RigidBody2D& body);

public:

    bool hasEnded;

    void destroy();
    void end();
    void shootPlayer();

    bool rot;

    tehGame()
        : Scene("tehGame"),

        m_bullets(createChild("bullets")->reserveChildren((unsigned int)(s_maxBullets * 1.1f))),
        m_cam(createChild("cam")),
        m_playerPtr(nullptr),
        m_world(*this),
        m_chars(),
        hasEnded(false),
        rot(false)
    {
        getWorld<2>().setDebugMode(true);

        m_cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic).setSize(15.f, 15.f * (g_screenSize.y / g_screenSize.x));
        m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);

        init();

        Engine::setState(Engine::State::Running);
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
            m_playerPtr->m_character->getComponent<RigidBody2D>()->applyTorque(9.f);

        if (m_playerPtr->m_character->getGlobalRotation().z > 0.05f)
            m_playerPtr->m_character->getComponent<RigidBody2D>()->applyTorque(-9.f);
#endif

        //Camera follow
#if 1
        m_cam->setPosition(
            m_playerPtr->m_character->getGlobalPosition().x,
            m_playerPtr->m_character->getGlobalPosition().y,
            m_cam->getGlobalPosition().z
            );
#endif
    }

    void postUpdate(const float deltaTime) override
    {
        using c = Controller;
        using k = Keyboard;
        using m = Mouse;

        std::for_each(m_chars.begin(), m_chars.end(), [deltaTime](Characters* c)
        {
            c->reduceCDs(deltaTime);
        });

        auto pl_rb = findChild("player")->getComponent<RigidBody2D>();

        //Deal damage
#if 1
        
        //Only one bullet wound to char, one bullet can harm many chars
        for (auto& charsItr : m_chars)
        {
            for (auto& bulletsItr : m_bullets->getChildren())
            {
                if (charsItr->m_character->getComponent<jop::RigidBody2D>()->checkContact(bulletsItr))

            }
        }

        std::for_each(m_chars.begin(), m_chars.end(), [&m_playerPtr, m_bullets](Characters* c){

            if (std::any_of(m_bullets->getChildren().begin(), m_bullets->getChildren().end(), [&pl_rb](jop::Object& obj)
            {
                return (pl_rb->checkContact(*obj.getComponent<Collider2D>()));
            }))
            {
                --c->m_health;
                if (-- m_health == 0u)
                    end();
                m_immuneCD = s_immuneCD;
            }
        });
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

        //Jump
#if 1
        if (g_cc)
            if (c::isButtonDown(0u, c::XBox::A))
                jump(*pl_rb);

        if (k::isKeyDown(k::Space))
            jump(*pl_rb);
#endif

        //Shoot
#if 1
        if (g_cc)
            if (c::getAxisOffset(0u, c::XBox::RTrigger))
                shootPlayer();

        if (m::isButtonDown(m::Left))
            shootPlayer();
#endif

        //NextLevel
#if 1
        if (findChild("player")->getGlobalPosition().x >= m_world.m_endPoint)
        {
            g_score += m_world.m_levelLength;
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
    Vibrator::vibrate(1000u);
    Engine::setState(Engine::State::RenderOnly);

    static_cast<jop::SoundEffect&>(*m_playerPtr->m_character->getComponent<jop::SoundEffect>(23u)).play();

    m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);
    m_cam->getComponent<Camera>()->setRenderMask(2u);

    WeakReference<jop::Object> overText = createChild("overText");
    overText->setScale(0.01f).setPosition(s_playerStartPos.x, s_playerStartPos.y + 5.f, s_playerStartPos.z);
    overText->createComponent<Text>(getRenderer()).setRenderGroup(1u);
    overText->getComponent<Text>()->setString("Game over!").setColor(Color::Purple);

    WeakReference<jop::Object> scoreText = createChild("scoreText");
    scoreText->setScale(0.01f).setPosition(s_playerStartPos);
    scoreText->createComponent<Text>(getRenderer()).setRenderGroup(1u);
    scoreText->getComponent<Text>()->setString("Your score: " + std::to_string(g_score)).setColor(Color::Orange);

    hasEnded = true;
}

void tehGame::init()
{
    Characters* c;

    m_world.createWorld();
    m_chars = c->createEnemies(g_density, m_world.m_groundP);
    m_chars.insert(m_chars.begin(), c->createPlayer());
    m_playerPtr = static_cast<Player*>(m_chars[0]);
    m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);
}

void tehGame::destroy()
{
    ResourceManager::unload("ground");
    Engine::createScene<tehGame>();
}

void getResources()
{
    static const jop::RigidBody2D::ConstructInfo2D bulletInfo(jop::ResourceManager::getNamed<jop::ConeShape2D>("bullet", 0.2f, 0.6f), jop::RigidBody2D::Type::Dynamic, 0.2f);
    c_bulletInfo = &bulletInfo;

    static const RigidBody2D::ConstructInfo2D playerInfo(ResourceManager::getNamed<CapsuleShape2D>("player", 1.f, 2.f), RigidBody2D::Type::Dynamic, 1.2f);
    c_playerInfo = &playerInfo;

    c_animAtlasPlayer = &ResourceManager::get<AnimationAtlas>("tehGame/art_spsh_small.png", glm::uvec2(4u, 3u), glm::uvec2(0u, 0u), glm::uvec2(3399u, 3487u)); //glm::uvec2(7650u, 8134u));

    c_bulletModel = &Model(
        ResourceManager::getNamed<RectangleMesh>("bulletmesh", glm::vec2(0.35f, 0.75f)),
        ResourceManager::getEmpty<Material>("bulletmat", true).setMap(Material::Map::Diffuse, ResourceManager::get<Texture2D>("tehGame/bullet.png", true, false)));

    c_playerModel = &Model(
        ResourceManager::getNamed<RectangleMesh>("playermesh", glm::vec2(1.f, 2.f)),
        ResourceManager::getEmpty<Material>("playermat", true).setMap(Material::Map::Diffuse, ResourceManager::get<Texture2D>("tehGame/player.png", true, false)));

}

#include "EventHandler.hpp"
int main(int argc, char* argv[])
{
#ifdef JOP_OS_ANDROID
    SettingManager::setDefaultDirectory("defconf");
    SettingManager::setOverrideWithDefaults();
#endif

    JOP_ENGINE_INIT("MyProject", argc, argv);

#ifdef JOP_OS_ANDROID
    JOP_ASSERT(Controller::isControllerPresent(0u), "System: Android && no controller detected, exiting.");
#endif

    getResources();

    Engine::getSubsystem<Window>()->setEventHandler<jd::EventHandler>();

    g_screenSize = jop::Engine::getMainRenderTarget().getSize();

    Engine::createScene<tehGame>();

    return JOP_MAIN_LOOP;
}