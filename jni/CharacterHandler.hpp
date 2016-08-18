#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP
#include <Jopnal/Jopnal.hpp>
#include "Consts.hpp"

enum class CharacterType
{
    playerC,
    staticC,
    targetC
};


class Characters
{

protected:

    jop::Scene& m_scene;
    const CharacterType m_type;
    int m_health;
    float m_immuneCD;
    float m_shootCD;
    float m_jumpCD;

public:

    jop::WeakReference<jop::Object> m_character;

    ////////////////////////////////////////////

    void shoot()
    {
        if (m_shootCD > 0.f)
            return;

        glm::vec2 dir;

        jop::WeakReference<jop::Object> bullet; 

        //Player
        switch (m_type)
        {

        case CharacterType::playerC:
        {
            auto player = m_scene.findChild("player");
            bullet = m_scene.findChild("bullets")->createChild("");

            dir = glm::vec2(
                m_scene.findChild("crosshair")->getGlobalPosition().x - player->getGlobalPosition().x,
                m_scene.findChild("crosshair")->getGlobalPosition().y - player->getGlobalPosition().y);

            dir = glm::normalize(dir);

            bullet->setPosition(
                player->getGlobalPosition().x + dir.x * 1.2f,
                player->getGlobalPosition().y + dir.y * 2.f,
                s_playerStartPos.z);

            m_shootCD = s_shootPlayerCD;
            break;
        }

        case CharacterType::targetC:
        {
            dir = glm::vec2(
                m_scene.findChild("player")->getGlobalPosition().x - m_character->getGlobalPosition().x,
                m_scene.findChild("player")->getGlobalPosition().y - m_character->getGlobalPosition().y);

            if (dir.length() > 6.f)
                return;

            bullet = m_scene.findChild("bullets")->createChild("");

            bullet->setPosition(
                m_character->getGlobalPosition().x + dir.x * 0.3f,
                m_character->getGlobalPosition().y + dir.y * 1.f,
                s_playerStartPos.z
                );

            m_shootCD = s_shootTargetCD;
            break;
        }

        case CharacterType::staticC:
        {
            dir = glm::vec2(
                m_character->getGlobalPosition().x - 1.f - m_character->getGlobalPosition().x,
                m_character->getGlobalPosition().y - m_character->getGlobalPosition().y);

            if (dir.length() > 8.f)
                return;

            bullet = m_scene.findChild("bullets")->createChild("");

            bullet->setPosition(
                m_character->getGlobalPosition().x + dir.x * 1.2f,
                m_character->getGlobalPosition().y + dir.y * 2.f,
                s_playerStartPos.z
                );

            m_shootCD = s_shootStaticCD;
            break;
        }
        default:
            return;
        }

        bullet->rotate(0.f, 0.f, glm::atan(dir.y, dir.x) - glm::half_pi<float>());
        bullet->createComponent<jop::RigidBody2D>(m_scene.getWorld<2>(), *c_bulletInfo);
        bullet->createComponent<jop::Drawable>(m_scene.getRenderer()).setModel(*c_bulletModel);
        bullet->getComponent<jop::RigidBody2D>()->applyCentralForce(glm::normalize(dir) * s_bulletSpeed);
    }

    ////////////////////////////////////////////

    void reduceCDs(const float deltaTime)
    {
        m_immuneCD = m_immuneCD == 0.f ? 0.f : m_immuneCD -= deltaTime;
        m_shootCD = m_shootCD == 0.f ? 0.f : m_shootCD -= deltaTime;
        m_jumpCD = m_jumpCD == 0.f ? 0.f : m_jumpCD -= deltaTime;
    }

    ////////////////////////////////////////////

    void applyImmuneCD()
    {
        m_immuneCD = s_immuneCD;
    }

    ////////////////////////////////////////////

    bool isImmune()
    {
        return m_immuneCD > 0.f;
    }

    ////////////////////////////////////////////

    void dealDamage(const int dmg)
    {
        m_health -= dmg;
    }

    ////////////////////////////////////////////

    int getHealth() const
    {
        return m_health;
    }

    ////////////////////////////////////////////

    void jump()
    {
        m_character->getComponent<jop::RigidBody2D>()->applyCentralForce(glm::vec2(0.f,g_jumpPower));
    }

    ////////////////////////////////////////////
    
    CharacterType getType() const
    {
        return m_type;
    }

    ////////////////////////////////////////////

    Characters(jop::Scene& sceneRef, const CharacterType type, float immuneCD, float shootCD, float jumpCD, int health, jop::WeakReference<jop::Object> obj) :
        m_scene(sceneRef),
        m_type(type),
        m_immuneCD(immuneCD),
        m_shootCD(shootCD),
        m_jumpCD(jumpCD),
        m_health(health),
        m_character(obj)
    {}

    virtual ~Characters(){}
};

#include "StaticEnemy.hpp"
#include "TargetEnemy.hpp"
#include "Player.hpp"

class CharacterFactory
{
public:
    static Characters* createCharacter(jop::Scene& scene, const CharacterType type, const glm::vec3& pos)
    {
        switch (type)
        {
        case CharacterType::playerC:
            return new Player(scene);

        case CharacterType::staticC:
            return new StaticEnemy(scene, pos);

        case CharacterType::targetC:
            return new TargetEnemy(scene, pos);

        default:
            return nullptr;
        }
    }
};


#endif