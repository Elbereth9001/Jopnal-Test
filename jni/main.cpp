#include <Jopnal/Jopnal.hpp>
#include "Consts.hpp"
#include "CharacterHandler.hpp"

using namespace jop;

class tehGame : public Scene
{
private:

    WeakReference<jop::Object> m_ground;
    WeakReference<jop::Object> m_player;
    WeakReference<jop::Object> m_crossHair;
    WeakReference<jop::Object> m_cam;
    WeakReference<jop::Object> m_bullet;

    Characters m_char;
    const RigidBody2D::ConstructInfo2D m_bulletInfo;

    unsigned int m_levelLength;
    float m_endPoint;
    float m_jumpCD;
    float m_shootCD;
    glm::vec3 m_crossHairLastDir;
    glm::vec2 m_screenSize;

    void init();
    void destroy();
    void createWorld();
    void createPlayer();
    void createCamera();
    void createEnemies();
    void jump(RigidBody2D& body);
    void shoot(const RigidBody2D& body, const glm::vec2 direction = glm::vec2(-1.f, 0.f));
    void shoot(const RigidBody2D& bodyA, const RigidBody2D& bodyB);

public:

    bool hasEnded;
    void end();
    void shootPlayer();

    tehGame()
        : Scene("tehGame"),

        m_ground(createChild("ground")),
        m_player(),
        m_crossHair(),
        m_cam(createChild("cam")),
        m_bullet(createChild("bullet")),
        m_char(*this),
        m_bulletInfo(ResourceManager::getNamed<ConeShape2D>("bullet", 0.2f, 0.6f), RigidBody2D::Type::Dynamic, 0.2f),
        m_levelLength(0u),
        m_endPoint(0.f),
        m_jumpCD(s_jumpCD),
        m_shootCD(s_shootCD),
        m_crossHairLastDir(1.f, 0.f, 0.f),
        m_screenSize(Engine::getMainRenderTarget().getSize()),
        hasEnded(false)
    {
        getWorld<2>().setDebugMode(true);

        m_cam->createComponent<Camera>(getRenderer(), Camera::Projection::Orthographic).setSize(15.f, 15.f * (m_screenSize.y / m_screenSize.x));
        m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);


        init();
    }

    void preUpdate(const float deltaTime) override
    {
        using jc = Controller;
        using jk = Keyboard;
        using jm = Mouse;

        //Player move
#if 1
        {
            auto pl_rb = m_player->getComponent<RigidBody2D>();
            float moveX = 0.f;
            float moveY = 0.f;

#ifdef JOP_OS_ANDROID
            moveX = jc::getAxisOffset(0u, jc::XBox::Axis::LeftStickX);
            moveY = jc::getAxisOffset(0u, jc::XBox::Axis::LeftStickY);
#else
            if (jk::isKeyDown(jk::A))
                moveX = -1.f;
            else if (jk::isKeyDown(jk::D))
                moveX = 1.f;

            if (jk::isKeyDown(jk::W))
                moveY = 1.f;
            else if (jk::isKeyDown(jk::S))
                moveY = -1.f;
#endif

            pl_rb->applyCentralForce(glm::vec2(moveX * 25.f, moveY * 15.f));
            pl_rb->synchronizeTransform();
        }
#endif

        //Player aim
#if 1
        {
            float aimX = 0.f;
            float aimY = 0.f;

#ifdef JOP_OS_ANDROID
            aimX = jc::getAxisOffset(0u, jc::XBox::Axis::RightStickX);
            aimY = jc::getAxisOffset(0u, jc::XBox::Axis::RightStickY);

            if (aimX != 0.f || aimY != 0.f)
                m_crossHairLastDir = glm::normalize(glm::vec3(aimX, aimY, m_crossHairLastDir.z)) * s_limitCrossHair;

#else
            aimX = jm::getPosition().x;
            aimY = m_screenSize.y - jm::getPosition().y;

            if ((aimX > 0.f && aimX < m_screenSize.x) && (aimY > 0.f && aimY < m_screenSize.y))
                m_crossHairLastDir = glm::normalize(glm::vec3(
                (aimX / (m_screenSize.x * 0.5f)) - 1.f,
                (aimY / (m_screenSize.y * 0.5f)) - 1.f,
                m_crossHairLastDir.z))*s_limitCrossHair;

#endif
            m_crossHair->setPosition(m_player->getGlobalPosition() + m_crossHairLastDir * s_limitCrossHair);

        }
#endif

        //Player stand up
#if 1
        if (m_player->getGlobalRotation().z < 0.f)
            m_player->getComponent<RigidBody2D>()->applyTorque(15.f);

        if (m_player->getGlobalRotation().z > 0.f)
            m_player->getComponent<RigidBody2D>()->applyTorque(-15.f);
#endif

        //Camera follow
#if 1
        m_cam->setPosition(
            m_player->getGlobalPosition().x,
            m_player->getGlobalPosition().y,
            m_cam->getGlobalPosition().z
            );
#endif
    }

    void postUpdate(const float deltaTime) override
    {
        using c = Controller;
        using k = Keyboard;
        using m = Mouse;


        m_jumpCD -= deltaTime;
        m_shootCD -= deltaTime;

        auto pl_rb = m_player->getComponent<RigidBody2D>();

        if (m_bullet->childCount() > s_maxBullets)
            for (unsigned int i = 0; i < m_bullet->childCount() - s_maxBullets + 5u; ++i)
                m_bullet->getChildren()[i].removeSelf();

        for (auto itr = m_bullet->getChildren().begin(); itr != m_bullet->getChildren().end(); ++itr)
            if (itr->getGlobalPosition().y < -20.f)
                itr->removeSelf();

        //Jump
#if 1
#ifdef JOP_OS_ANDROID
        if (c::isButtonDown(0u, c::XBox::A) && m_jumpCD <= 0.f)
            jump(*pl_rb);
#else
        if (k::isKeyDown(k::Space) && m_jumpCD <= 0.f)
            jump(*pl_rb);
#endif
#endif

        //Shoot
#if 1
#ifdef JOP_OS_ANDROID
        if (c::getAxisOffset(0u, c::XBox::RTrigger))
            shootPlayer();
#else
        if (m::isButtonDown(m::Left))
            shootPlayer();
#endif
#endif

        if (m_player->getGlobalPosition().x >= m_endPoint)
        {
            score += m_levelLength;
            destroy();
        }

        if (m_player->getGlobalPosition().x < -5.f)
            end();
    }
};

void tehGame::end()
{
    Vibrator::vibrate(1000u);
    Engine::setState(Engine::State::RenderOnly);

    m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);
    m_cam->getComponent<Camera>()->setRenderMask(2u);

    WeakReference<jop::Object> overText = createChild("overText");
    overText->setScale(0.01f).setPosition(s_playerStartPos.x, s_playerStartPos.y + 5.f, s_playerStartPos.z);
    overText->createComponent<Text>(getRenderer()).setRenderGroup(1u);
    overText->getComponent<Text>()->setString("Game over!").setColor(Color::Purple);

    WeakReference<jop::Object> scoreText = createChild("scoreText");
    scoreText->setScale(0.01f).setPosition(s_playerStartPos);
    scoreText->createComponent<Text>(getRenderer()).setRenderGroup(1u);
    scoreText->getComponent<Text>()->setString("Your score: " + std::to_string(score)).setColor(Color::Orange);

    hasEnded = true;
}

void tehGame::init()
{
    createWorld();
    createPlayer();
    m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);
    createEnemies();
}

void tehGame::destroy()
{
    ResourceManager::unload("ground");
    Engine::createScene<tehGame>();
}

void tehGame::createWorld()
{
    Randomizer r;
    std::vector<glm::vec2> ground;

    m_levelLength = r.range<unsigned int>(40u, 60u);

    float pointX = -5.f;
    float pointY = 0.f;
    ground.emplace_back(pointX, pointY);
    pointX = 0.f;
    pointY = 0.f;
    ground.emplace_back(pointX, pointY);

    for (unsigned int i = 0; i < m_levelLength; ++i)
    {
        pointX = r.range<float>(pointX + 1.f, pointX + 5.f);
        pointY = r.range<float>(pointY - 2.f, pointY + 2.f);

        ground.emplace_back(pointX, pointY);
    }

    m_endPoint = pointX;

    RigidBody2D::ConstructInfo2D groundInfo(ResourceManager::getNamed<TerrainShape2D>("ground", ground));

    m_ground->createComponent<RigidBody2D>(getWorld<2>(), groundInfo);
}

void tehGame::createPlayer()
{
    auto pData = m_char.createPlayer();
    m_player = pData.first;
    m_crossHair = pData.second;

    //    RigidBody2D::ConstructInfo2D playerInfo(ResourceManager::getNamed<CapsuleShape2D>("player", 1.f, 2.f), RigidBody2D::Type::Dynamic, 1.2f);
    //    m_player->createComponent<RigidBody2D>(getWorld<2>(), playerInfo);
    //    m_player->setPosition(s_playerStartPos);
    //
    //    m_crossHair->createComponent<Drawable>(getRenderer());
    //    m_crossHair->setPosition(s_playerStartPos.x + s_limitCrossHair, s_playerStartPos.y, s_playerStartPos.z);
}

void tehGame::createEnemies()
{
    JOP_DEBUG_INFO("Made an enemy");
}

void tehGame::jump(RigidBody2D& body)
{
    JOP_DEBUG_INFO("BOING")
        body.applyCentralForce(glm::vec2(0.f, 500.f));
    body.synchronizeTransform();
    m_jumpCD = s_jumpCD;
}

//Static guns
void tehGame::shoot(const RigidBody2D& body, const glm::vec2 dir)
{
    JOP_ASSERT((-1.f <= dir.x && dir.x <= 1.f) && (-1.f <= dir.y && dir.y <= 1.f), "Shooting direction must be between -1.f - +1.f.");

    JOP_DEBUG_INFO("BANG");

    auto bullet = m_bullet->createChild("bullet");
    bullet->setPosition(
        body.getObject()->getGlobalPosition().x + dir.x,
        body.getObject()->getGlobalPosition().y + dir.y,
        0.f);
    bullet->createComponent<RigidBody2D>(getWorld<2>(), m_bulletInfo);
    bullet->getComponent<RigidBody2D>()->applyCentralForce(dir * s_bulletSpeed);
    m_shootCD = s_shootCD;
}

//Targeting shooters
void tehGame::shoot(const RigidBody2D& bodyA, const RigidBody2D& bodyB)
{
    JOP_ASSERT(bodyA != bodyB, "Body tried to commit suicide!");

    glm::vec2 dir(
        (bodyA.getObject()->getGlobalPosition().x - bodyB.getObject()->getGlobalPosition().x),
        (bodyA.getObject()->getGlobalPosition().y - bodyB.getObject()->getGlobalPosition().y));
    JOP_ASSERT((-1.f <= dir.x && dir.x <= 1.f) && (-1.f <= dir.y && dir.y <= 1.f), "Shooting direction must be between -1.f - +1.f.");

    auto bullet = m_bullet->createChild("bullet");
    bullet->setPosition(
        bodyA.getObject()->getGlobalPosition().x + dir.x,
        bodyA.getObject()->getGlobalPosition().y + dir.y,
        0.f);
    bullet->createComponent<RigidBody2D>(getWorld<2>(), m_bulletInfo);
    bullet->getComponent<RigidBody2D>()->applyCentralForce(dir * s_bulletSpeed);
    m_shootCD = s_shootCD;
}

//Player
void tehGame::shootPlayer()
{
    if (!(m_shootCD <= 0.f))
        return;

    JOP_ASSERT(thisIsPlayer && player == m_player->getComponent<RigidBody2D>(), "Shooter is not player.");

    JOP_DEBUG_INFO("BANG");

    glm::vec2 dir(
        (m_crossHair->getGlobalPosition().x - m_player->getGlobalPosition().x),
        (m_crossHair->getGlobalPosition().y - m_player->getGlobalPosition().y));

    dir = glm::normalize(dir);

    JOP_DEBUG_INFO("Bullet speed: " << glm::length(dir*s_bulletSpeed));

    auto bullet = m_bullet->createChild("bullet");

    //RigidBody
    {
        bullet->setPosition(
            m_player->getGlobalPosition().x + dir.x,
            m_player->getGlobalPosition().y + dir.y,
            0.f);
        bullet->rotate(0.f, 0.f, glm::atan(dir.y, dir.x) - glm::half_pi<float>());
        bullet->createComponent<RigidBody2D>(getWorld<2>(), m_bulletInfo);
    }

    //Texture
    {
        auto& tex = ResourceManager::get<Texture2D>("tehGame/bullet.png", true, false);
        auto& mat = ResourceManager::getEmpty<Material>("bulletmat", true);
        auto& mesh = ResourceManager::getNamed<RectangleMesh>("bulletmesh", glm::vec2(0.35f, 0.75f));

        mat.setMap(Material::Map::Diffuse, tex);

        bullet->createComponent< jop::Drawable>(getRenderer()).setModel(Model(mesh, mat));
    }
    bullet->getComponent<RigidBody2D>()->applyCentralForce(dir * s_bulletSpeed);
    m_shootCD = s_shootCD;
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
    JOP_ASSERT(Controller::isControllerPresent(0u), "No controller detected, exiting.");
#endif

    Engine::getSubsystem<Window>()->setEventHandler<jd::EventHandler>();

    Engine::createScene<tehGame>();

    return JOP_MAIN_LOOP;
}
