#ifndef TARGET_ENEMY_HPP
#define TARGET_ENEMY_HPP

#include <Jopnal/Jopnal.hpp>
#include "Enemies.hpp"

class TargetEnemy : public Enemies
{

public:

    TargetEnemy(jop::Scene& scene, const glm::vec3& pos) :
        Enemies(scene, pos, CharacterType::targetC, s_immuneCD, s_shootTargetCD, s_jumpCD, g_healthEnemyStart, scene.createChild(""))
    {
        m_character->setPosition(pos);
        m_character->createComponent< jop::Drawable>(scene.getRenderer()).setModel(*c_playerModel);
        m_character->createComponent<jop::RigidBody2D>(scene.getWorld<2>(), *c_playerInfo);
    }
    virtual ~TargetEnemy(){}

};

#endif