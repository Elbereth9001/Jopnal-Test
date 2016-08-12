#include <Jopnal/Jopnal.hpp>
#include "Consts.hpp"
#include "CharacterHandler.hpp"


class tehGame : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_ground;
    jop::WeakReference<jop::Object> m_player;
    jop::WeakReference<jop::Object> m_crossHair;
    jop::WeakReference<jop::Object> m_cam;
    jop::WeakReference<jop::Object> m_bullet;

    Characters m_char;

    float m_jumpCD;
    float m_shootCD;
    float m_immuneCD;

    unsigned int m_levelLength;
    float m_endPoint;

    glm::vec3 m_crossHairLastDir;
    glm::vec2 m_screenSize;

    void init();
    void destroy();
    void createWorld();
    void createPlayer();
    void createCamera();
    void createEnemies();
    void jump(jop::RigidBody2D& body);
    void shoot(const jop::RigidBody2D& body, const glm::vec2 direction = glm::vec2(-1.f, 0.f));
    void shoot(const jop::RigidBody2D& bodyA, const jop::RigidBody2D& bodyB);

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
        m_bullet(createChild("bullet")->reserveChildren((unsigned int)(s_maxBullets * 1.1f))),
        m_char(*this),
        m_jumpCD(s_jumpCD),
        m_shootCD(s_shootCD),
        m_immuneCD(s_immuneCD),
        m_levelLength(0u),
        m_endPoint(0.f),
        m_crossHairLastDir(1.f, 0.f, 0.f),
        m_screenSize(jop::Engine::getMainRenderTarget().getSize()),
        hasEnded(false)
    {
        getWorld<2>().setDebugMode(true);

        m_cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic).setSize(15.f, 15.f * (m_screenSize.y / m_screenSize.x));
        m_cam->setPosition(s_playerStartPos.x, s_playerStartPos.y, s_playerStartPos.z);

        init();
    }

    void preUpdate(const float deltaTime) override
    {
        using jc = jop::Controller;
        using jk = jop::Keyboard;
        using jm = jop::Mouse;

        if (jc::isControllerPresent(0u))
            g_cc = true;
        else
            g_cc = false;

        //Player move
#if 1
        {
            auto pl_rb = m_player->getComponent<jop::RigidBody2D>();
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
            m_player->getComponent<jop::AnimatedSprite>()->play();

            glm::vec2 vel = pl_rb->getLinearVelocity();

            //No animation when moving slowly
            if ((vel.x < 1.f && vel.x > -1.f) && (vel.y < 1.f && vel.y > -1.f))
                m_player->getComponent<jop::AnimatedSprite>()->pause();

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
                    m_crossHairLastDir = glm::normalize(glm::vec3(aimX, aimY, m_crossHairLastDir.z)) * s_limitCrossHair;
            }

            aimX = jm::getPosition().x;
            aimY = m_screenSize.y - jm::getPosition().y;

            if ((aimX > 0.f && aimX < m_screenSize.x) && (aimY > 0.f && aimY < m_screenSize.y))
                m_crossHairLastDir = glm::normalize(glm::vec3(
                (aimX / (m_screenSize.x * 0.5f)) - 1.f,
                (aimY / (m_screenSize.y * 0.5f)) - 1.f,
                m_crossHairLastDir.z))*s_limitCrossHair;

            m_crossHair->setPosition(m_player->getGlobalPosition() + m_crossHairLastDir * s_limitCrossHair);
        }
#endif

        //Player stand up
#if 1
        if (m_player->getGlobalRotation().z < -0.05f)
            m_player->getComponent<RigidBody2D>()->applyTorque(9.f);

        if (m_player->getGlobalRotation().z > 0.05f)
            m_player->getComponent<RigidBody2D>()->applyTorque(-9.f);
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
        m_immuneCD -= deltaTime;

        auto pl_rb = m_player->getComponent<RigidBody2D>();

        JOP_DEBUG_INFO(healthPlayer);

        //Deal damage
#if 1
        if (m_immuneCD <= 0.f)
        {
            for (auto itr = m_bullet->getChildren().begin(); itr != m_bullet->getChildren().end(); ++itr)
                if (pl_rb->checkContact(*itr->getComponent<Collider2D>()))
                {
                    JOP_DEBUG_INFO("AUTS");
                    if (--healthPlayer == 0u)
                        end();
                    m_immuneCD = s_immuneCD;
                    break;
                }
        }
#endif

        //Remove bullets
#if 1
        if (m_bullet->childCount() > s_maxBullets)
            for (unsigned int i = 0; i < m_bullet->childCount() - (s_maxBullets * 1.1f); ++i) //<<< Remove some extra -> don't have to remove all the time
                m_bullet->getChildren()[i].removeSelf();

        for (auto itr = m_bullet->getChildren().begin(); itr != m_bullet->getChildren().end(); ++itr)
            if (itr->getGlobalPosition().y < s_maxWorldLength * s_minWorldOffsetY)
                itr->removeSelf();
#endif

        //Jump
#if 1
        if (g_cc)
            if (c::isButtonDown(0u, c::XBox::A) && m_jumpCD <= 0.f)
                jump(*pl_rb);

        if (k::isKeyDown(k::Space) && m_jumpCD <= 0.f)
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
        if (m_player->getGlobalPosition().x >= m_endPoint)
        {
            score += m_levelLength;
            destroy();
        }
#endif

        //EndGame
#if 1
        if (m_player->getGlobalPosition().x < -5.f)
            end();
#endif

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
    using namespace jop;

    Randomizer r;
    std::vector<glm::vec2> ground;

    //Ground hitbox
#if 1
    {
        m_levelLength = r.range<unsigned int>(s_minWorldLength, s_maxWorldLength);
        ground.reserve(m_levelLength);

        //Small flat area at the beginning
        float pointX = -5.f;
        float pointY = 0.f;
        ground.emplace_back(pointX, pointY);
        pointX = 0.f;
        pointY = 0.f;
        ground.emplace_back(pointX, pointY);

        for (unsigned int i = 0; i < m_levelLength; ++i)
        {
            pointX = r.range<float>(pointX + s_minWorldOffsetX, pointX + s_maxWorldOffsetX);
            pointY = r.range<float>(pointY + s_minWorldOffsetY, pointY + s_maxWorldOffsetY);

            ground.emplace_back(pointX, pointY);
        }

        m_endPoint = pointX;

        RigidBody2D::ConstructInfo2D groundInfo(ResourceManager::getNamed<TerrainShape2D>("ground", ground));

        m_ground->createComponent<RigidBody2D>(getWorld<2>(), groundInfo);
    }
#endif

    //Ground coloring: gradient: top = White, bottom = green
#if 1
    {
        std::vector<std::pair< glm::vec3, glm::vec4>> meshPoints;
        std::vector<unsigned int> meshIndices;
        uint32 base(0u);
        float colorHere(0.f);
        float colorNext(0.f);
        const glm::vec4 colorBottom(0.f, 1.f, 0.f, 1.f);  //RGBA


        meshPoints.reserve(ground.size() * 4u);
        meshIndices.reserve(ground.size() * 6u);

        for (unsigned int i = 0u; i < ground.size() - 1u; ++i)
        {
            base = i * 4u;

            meshIndices.push_back(base + 3u);
            meshIndices.push_back(base + 0u);
            meshIndices.push_back(base + 1u);
            meshIndices.push_back(base + 1u);
            meshIndices.push_back(base + 2u);
            meshIndices.push_back(base + 3u);

            //Both = 0.f - +1.f
            colorHere = std::max(0.f, std::min(1.f, 1.f + (((ground[i].y + s_colorLimitBottom) / s_colorLimitTop) * 0.5f)));
            colorNext = std::max(0.f, std::min(1.f, 1.f + (((ground[i + 1u].y + s_colorLimitBottom) / s_colorLimitTop) * 0.5f)));

            meshPoints.emplace_back(glm::vec3(ground[i].x, ground[i].y, 0.f), glm::vec4(colorHere, 1.f, colorHere, 1.f));               // current point = left
            meshPoints.emplace_back(glm::vec3(ground[i].x, s_minWorldOffsetY * s_maxWorldLength, 0.f), colorBottom);                    // bottom left
            meshPoints.emplace_back(glm::vec3(ground[i + 1u].x, s_minWorldOffsetY * s_maxWorldLength, 0.f), colorBottom);               // bottom right
            meshPoints.emplace_back(glm::vec3(ground[i + 1u].x, ground[i + 1u].y, 0.f), glm::vec4(colorNext, 1.f, colorNext, 1.f));     // next point = right

        }
        auto& mesh = ResourceManager::getEmpty<jop::Mesh>("groundMesh");
        mesh.load(meshPoints.data(), meshPoints.size()*sizeof(std::pair< glm::vec3, glm::vec4 >), Mesh::Position | Mesh::Color, meshIndices.data(), sizeof(unsigned int), meshIndices.size());
        auto& mat = ResourceManager::getEmpty<jop::Material>("groundmat", false);

        m_ground->createComponent<jop::Drawable>(getRenderer()).setModel(Model(mesh, mat));
    }
#endif

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
    bullet->createComponent<RigidBody2D>(getWorld<2>(), *c_bulletInfo);
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
    bullet->createComponent<RigidBody2D>(getWorld<2>(), *c_bulletInfo);
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
        bullet->createComponent<RigidBody2D>(getWorld<2>(), *c_bulletInfo);
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

void getResources()
{
    static const jop::RigidBody2D::ConstructInfo2D bulletInfo(jop::ResourceManager::getNamed<jop::ConeShape2D>("bullet", 0.2f, 0.6f), jop::RigidBody2D::Type::Dynamic, 0.2f);
    c_bulletInfo = &bulletInfo;

    static const RigidBody2D::ConstructInfo2D playerInfo(ResourceManager::getNamed<CapsuleShape2D>("player", 1.f, 2.f), RigidBody2D::Type::Dynamic, 1.2f);
    c_playerInfo = &playerInfo;

    c_animAtlas = &ResourceManager::get<AnimationAtlas>("tehGame/art_spsh_small.png", glm::uvec2(4u, 3u), glm::uvec2(0u, 0u), glm::uvec2(3399u, 3487u)); //glm::uvec2(7650u, 8134u));
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

    Engine::createScene<tehGame>();

    return JOP_MAIN_LOOP;
}
