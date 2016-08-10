
#include <Jopnal/Jopnal.hpp>

#define VERBOSE true

using namespace jop;

glm::vec2 screenSize(1024.f, 768.f);

const static glm::vec3 s_playerStartPos(0.f, 5.f, 0.f);
const static float s_limitCrossHair(4.f);
const static unsigned int s_maxBullets(50u);
const static float s_jumpCD(1.f);
const static float s_shootCD(0.05f);
const static float s_bulletSpeed(50.f);

class tehGame : public Scene
{
private:

    WeakReference<jop::Object> m_ground;
    WeakReference<jop::Object> m_player;
    WeakReference<jop::Object> m_crossHair;
    WeakReference<jop::Object> m_cam;
    WeakReference<jop::Object> m_bullet;

    Material* newMaterial;

    const RigidBody2D::ConstructInfo2D m_bulletInfo;

    unsigned long long int m_score;
    float m_endPoint;
    float m_jumpCD;
    float m_shootCD;
    glm::vec3 m_crossHairLastAngle;

    void end();
    void init();
    void destroy();
    void createWorld();
    void createPlayer();
    void createCamera();
    void createEnemies();
    void jump(RigidBody2D& body);
    void shoot(const RigidBody2D& body, const glm::vec2 direction = glm::vec2(-1.f, 0.f));
    void shoot(const RigidBody2D& bodyA, const RigidBody2D& bodyB);
    void shoot(const RigidBody2D& player, const bool thisIsPlayer);

public:

    tehGame()
        : Scene("MyScene"),

        m_ground(createChild("ground")),
        m_player(createChild("player")),
        m_crossHair(createChild("crosshair")),
        m_cam(createChild("cam")),
        m_bullet(createChild("bullet")),
        m_bulletInfo(ResourceManager::getNamed<ConeShape2D>("bullet", 0.2f, 0.6f), RigidBody2D::Type::Dynamic, 0.2f),
        m_score(0u),
        m_endPoint(),
        m_jumpCD(s_jumpCD),
        m_shootCD(s_shootCD),
        m_crossHairLastAngle(1.f, 0.f, 0.f)
    {
        getWorld<2>().setDebugMode(true);

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

            if ((aimX > 0.85f || aimX < -0.85f) || (aimY > 0.85f || aimY < -0.85f))
            {
                m_crossHairLastAngle = glm::vec3(aimX, aimY, m_crossHairLastAngle.z);
                m_crossHair->setPosition((m_crossHairLastAngle * s_limitCrossHair) + m_player->getGlobalPosition());
            }

            else
                m_crossHair->setPosition((m_crossHairLastAngle * s_limitCrossHair) + m_player->getGlobalPosition());
#else
            aimX = jm::getPosition().x;
            aimY = jm::getPosition().y;

            if ((aimX > 0.f && aimX < screenSize.x) && (aimY > 0.f && aimY < screenSize.y))
            {
                float y = (aimY / (screenSize.y * 0.5f));
                m_crossHairLastAngle = glm::vec3(
                    (aimX / (screenSize.x * 0.5f)) - 1.f,
                    y > 1.f ? (3.f - y) : (1.f + y), //...
                    m_crossHairLastAngle.z);
                m_crossHair->setPosition((m_crossHairLastAngle * s_limitCrossHair) + m_player->getGlobalPosition());
            }

            else
                m_crossHair->setPosition((m_crossHairLastAngle * s_limitCrossHair) + m_player->getGlobalPosition());
#endif
        }
#endif

        //Player stand up
#if 0
        if (m_player->getGlobalRotation().z < 0.f)
            pl_rb->applyCentralForce(glm::vec2(-5.f, 5.f));

        if (m_player->getGlobalRotation().z > 0.f)
            pl_rb->applyCentralForce(glm::vec2(5.f, 5.f));
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
        m_jumpCD -= deltaTime;
        m_shootCD -= deltaTime;

        using jc = Controller;
        using jk = Keyboard;

        auto pl_rb = m_player->getComponent<RigidBody2D>();

        if (m_bullet->childCount() > s_maxBullets)
            for (unsigned int i = 0; i < m_bullet->childCount() - s_maxBullets + 5u; ++i)
                m_bullet->getChildren()[i].removeSelf();

        for (auto itr = m_bullet->getChildren().begin(); itr != m_bullet->getChildren().end(); ++itr)
            if (itr->getGlobalPosition().y < -20.f)
                itr->removeSelf();

        if (m_player->getGlobalPosition().x >= m_endPoint)
            init();

        if (m_player->getGlobalPosition().x < -5.f)
            end();

        //Exit
#if 1
#ifdef JOP_OS_ANDROID
        if (jc::isButtonDown(0u, jc::XBox::Start))
            Engine::exit();
#else
        if (jk::isKeyDown(jk::Escape))
            Engine::exit();
#endif
#endif

        //Jump
#if 1
#ifdef JOP_OS_ANDROID
        if (jc::isButtonDown(0u, jc::XBox::A) && m_jumpCD <= 0.f)
            jump(*pl_rb);
#else
        if (jk::isKeyDown(jk::Space) && m_jumpCD <= 0.f)
            jump(*pl_rb);
#endif
#endif

        //Shoot
#if 1
#ifdef JOP_OS_ANDROID
        //if (jc::getAxisOffset(0u, jc::XBox::RTrigger))
        if (jc::isButtonDown(0u, jc::XBox::RTrigger) && m_shootCD <= 0.f)
            shoot(*pl_rb, true);
#else
        if (Mouse::isButtonDown(Mouse::Button::Left) && m_shootCD <= 0.f)
            shoot(*pl_rb, true);
#endif
#endif

    }
};

void tehGame::end()
{
    //Vibrator::vibrate(1000u);

    destroy();
    createCamera();

    WeakReference<jop::Object> endText;
    endText->createComponent<jop::Text>(getRenderer());
    auto* txt = endText->getComponent<Text>();

    endText->setPosition(s_playerStartPos.x, s_playerStartPos.y + 5.f, s_playerStartPos.z);
    txt->setString("Game over!").setColor(Color::Purple);

    endText->setPosition(s_playerStartPos);
    txt->setString("Your score: " + m_score).setColor(Color::Orange);
}

void tehGame::init()
{
    destroy();
    createWorld();
    createPlayer();
    createCamera();
    createEnemies();
}

void tehGame::destroy()
{
    //m_score += m_ground->getComponent<jop::RigidBody2D>()->
    ResourceManager::unload("ground");
    m_ground->clearComponents();
    m_player->clearComponents();
    m_crossHair->clearComponents();
    m_cam->clearComponents();
    m_bullet->clearComponents();
}

void tehGame::createWorld()
{
    Randomizer r;

    std::vector<glm::vec2> ground;

    float pointX = -5.f;
    float pointY = 0.f;
    ground.emplace_back(pointX, pointY);
    pointX = 0.f;
    pointY = 0.f;
    ground.emplace_back(pointX, pointY);

    for (unsigned int i = 0; i < r.range<unsigned int>(40u, 60u); ++i)
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
    RigidBody2D::ConstructInfo2D playerInfo(ResourceManager::getNamed<CapsuleShape2D>("player", 1.f, 2.f), RigidBody2D::Type::Dynamic, 1.2f);
    m_player->createComponent<RigidBody2D>(getWorld<2>(), playerInfo);
    m_player->setPosition(s_playerStartPos);

    m_crossHair->createComponent<Drawable>(getRenderer());
    m_crossHair->setPosition(s_playerStartPos.x + s_limitCrossHair, s_playerStartPos.y, s_playerStartPos.z);
}

void tehGame::createCamera()
{
    m_cam->createComponent<Camera>(getRenderer(), Camera::Projection::Perspective);
    m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z + 7.f);
}

void tehGame::createEnemies()
{
    if (VERBOSE)
    {
        JOP_DEBUG_INFO("Made an enemy");
    }
}

void tehGame::jump(RigidBody2D& body)
{
    if (VERBOSE)
    {
        JOP_DEBUG_INFO("BOING")
    }
    body.applyCentralForce(glm::vec2(0.f, 500.f));
    body.synchronizeTransform();
    m_jumpCD = s_jumpCD;
}

//Static guns
void tehGame::shoot(const RigidBody2D& body, const glm::vec2 dir)
{
    JOP_ASSERT((-1.f <= dir.x && dir.x <= 1.f) && (-1.f <= dir.y && dir.y <= 1.f), "Shooting direction must be between -1.f - +1.f.");

    if (VERBOSE)
    {
        JOP_DEBUG_INFO("BANG")
    }

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
void tehGame::shoot(const RigidBody2D& player, const bool thisIsPlayer)
{
    JOP_ASSERT(thisIsPlayer && player == m_player->getComponent<RigidBody2D>(), "Shooter is not player.");

    if (VERBOSE)
    {
        JOP_DEBUG_INFO("BANG")
    }

    glm::vec2 dir(
        (m_crossHair->getGlobalPosition().x - m_player->getGlobalPosition().x) / s_limitCrossHair,
        (m_crossHair->getGlobalPosition().y - m_player->getGlobalPosition().y) / s_limitCrossHair);
    JOP_ASSERT((-1.f <= dir.x && dir.x <= 1.f) && (-1.f <= dir.y && dir.y <= 1.f), "Shooting direction must be between -1.f - +1.f.");

    auto bullet = m_bullet->createChild("bullet");
    bullet->setPosition(
        player.getObject()->getGlobalPosition().x + dir.x,
        player.getObject()->getGlobalPosition().y + dir.y,
        0.f);
    bullet->createComponent<RigidBody2D>(getWorld<2>(), m_bulletInfo);
    bullet->getComponent<RigidBody2D>()->applyCentralForce(dir * s_bulletSpeed);
    m_shootCD = s_shootCD;
}

int main(int argc, char* argv[])
{
#ifdef JOP_OS_ANDROID
    SettingManager::setDefaultDirectory("defconf");
    SettingManager::setOverrideWithDefaults();
#endif

    JOP_ENGINE_INIT("MyProject", argc, argv);

    Engine::createScene<tehGame>();

    return JOP_MAIN_LOOP;
}
