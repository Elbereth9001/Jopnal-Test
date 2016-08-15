#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP
#include <Jopnal/Jopnal.hpp>

using namespace jop;

class Characters
{
    class Player;
private:

    WeakReference<jop::Object> createEnemyStatic(const glm::vec2& pos);
    WeakReference<jop::Object> createEnemyTargeting(const glm::vec2& pos);

protected:

    enum Type
    {
        playerC,
        staticC,
        targetC
    };

    Scene& m_scene;
    Type m_type;
    float m_immuneCD;
    float m_shootCD;
    float m_jumpCD;
    

public:

    WeakReference<Object> m_character;
    unsigned int m_health;

    void aim(const float aimX, const float aimY);
    void shoot();

    void reduceCDs(const float deltaTime);

    Characters* createPlayer();
    std::vector<Characters*> createEnemies(const unsigned int density, const std::vector<glm::vec2>& worldPoints);

    Characters();
    Characters(Scene& sceneRef, Type type, const float immuneCD, const float shootCD, const float jumpCD, WeakReference<jop::Object> obj) :
        m_scene(sceneRef),
        m_type(type),
        m_immuneCD(immuneCD),
        m_shootCD(shootCD),
        m_jumpCD(jumpCD),
        m_character(obj)
    {
        m_health = m_type == Type::playerC ? g_healthPlayerStart : 5u;
    }
};

void Characters::shoot()
{
    if (m_shootCD > 0.f)
        return;

    glm::vec2 dir;

    auto bullet = m_scene.findChild("bullets")->createChild("");

    //Player
    switch (m_type)
    {

    case Type::playerC:
    {
        auto player = m_character->findChild("player");

        dir = glm::vec2(
            m_scene.findChild("crosshair")->getGlobalPosition().x - player->getGlobalPosition().x,
            m_scene.findChild("crosshair")->getGlobalPosition().y - player->getGlobalPosition().y);

        dir = glm::normalize(dir);

        bullet->setPosition(
            player->getGlobalPosition().x + dir.x * 1.2f,
            player->getGlobalPosition().y + dir.y * 2.f,
            0.f);

        m_shootCD = s_shootPlayerCD;
        break;
    }

    case Type::targetC:
    {
        auto player = m_character->findChild("player");

        dir = glm::vec2(
            player->getGlobalPosition().x - m_character->getGlobalPosition().x,
            player->getGlobalPosition().y - m_character->getGlobalPosition().y);

        m_shootCD = s_shootTargetCD;
        break;
    }

    case Type::staticC:
    {
        dir = glm::vec2(
            m_character->getGlobalPosition().x - 1.f - m_character->getGlobalPosition().x,
            m_character->getGlobalPosition().y - m_character->getGlobalPosition().y);

        m_shootCD = s_shootStaticCD;
        break;
    }
    default:
        return;
    }

    bullet->rotate(0.f, 0.f, glm::atan(dir.y, dir.x) - glm::half_pi<float>());
    bullet->createComponent<RigidBody2D>(m_scene.getWorld<2>(), *c_bulletInfo);
    bullet->createComponent<jop::Drawable>(m_scene.getRenderer()).setModel(*c_bulletModel);
    bullet->getComponent<RigidBody2D>()->applyCentralForce(dir * s_bulletSpeed);
}

std::vector<Characters*> Characters::createEnemies(const unsigned int density, const std::vector<glm::vec2>& worldPoints)
{
    std::vector<Characters*> enemies;

    const unsigned int amount = worldPoints.size() / density;
    unsigned int index(3u);

    m_scene.reserveChildren(amount);

    auto r = [density]()
    {
        static Randomizer rand;
        return rand.range(0u, density);
    };


    for (unsigned int i = 3u; i < amount; ++i)
    {
        m_scene.createChild("");

        index = i + density * i;

        if (index >= worldPoints.size())
            break;

        glm::vec2 newPos(
            worldPoints[index].x,
            worldPoints[index].y + s_maxWorldOffsetY + 1u + r()
            );

        if (r() <= 1.f)
            createEnemyTargeting(newPos);
        else
            createEnemyStatic(newPos);
    }

    return enemies;
}

WeakReference<jop::Object> Characters::createEnemyStatic(const glm::vec2& pos)
{
    //Enemy
    WeakReference<jop::Object> enemy = m_scene.findChild("enemies")->createChild("enemystatic");
    {
        enemy->setPosition(pos.x, pos.y, s_playerStartPos.z);

        auto& tex = ResourceManager::get<Texture2D>("tehGame/player.png", true, false);
        auto& mat = ResourceManager::getEmpty<Material>("enemymat", true);
        auto& mesh = ResourceManager::getNamed<RectangleMesh>("enemymesh", glm::vec2(1.f, 2.f));

        mat.setMap(Material::Map::Diffuse, tex);

        enemy->createComponent<RigidBody2D>(m_scene.getWorld<2>(), *c_playerInfo);
        enemy->createComponent< jop::Drawable>(m_scene.getRenderer()).setModel(Model(mesh, mat));
    }
    return enemy;
}

WeakReference<jop::Object> Characters::createEnemyTargeting(const glm::vec2& pos)
{
    //Enemy
    WeakReference<jop::Object> enemy = m_scene.findChild("enemies")->createChild("enemytargeting");
    {
        enemy->setPosition(pos.x, pos.y, s_playerStartPos.z);

        auto& tex = ResourceManager::get<Texture2D>("tehGame/player.png", true, false);
        auto& mat = ResourceManager::getEmpty<Material>("enemymat", true);
        auto& mesh = ResourceManager::getNamed<RectangleMesh>("enemymesh", glm::vec2(1.f, 2.f));

        mat.setMap(Material::Map::Diffuse, tex);

        enemy->createComponent<RigidBody2D>(m_scene.getWorld<2>(), *c_playerInfo);
        enemy->createComponent< jop::Drawable>(m_scene.getRenderer()).setModel(Model(mesh, mat));
    }
    return enemy;
}

void Characters::reduceCDs(const float deltaTime)
{
    m_immuneCD = m_immuneCD == 0.f ? 0.f : m_immuneCD -= deltaTime;
    m_shootCD = m_shootCD == 0.f ? 0.f : m_shootCD -= deltaTime;
    m_jumpCD = m_jumpCD == 0.f ? 0.f : m_jumpCD -= deltaTime;
}

Characters* Characters::createPlayer()
{
    return static_cast<Characters*>(&Player(m_scene));
}

#endif