#ifndef ENEMIES_HPP
#define ENEMIES_HPP

#include <Jopnal/Jopnal.hpp>

class Enemies : public Characters
{
protected:
    glm::vec3 m_playerPos;
    glm::vec3 m_position;

public:
    Enemies(jop::Scene& scene, const glm::vec3& pos, const CharacterType type, float immuneCD, float shootCD, float jumpCD, int health, jop::WeakReference<jop::Object> obj) :
        Characters(scene, type, immuneCD, shootCD, jumpCD, health, obj),
        m_position(pos),
        m_playerPos(scene.findChild("player")->getGlobalPosition())
    {}

    virtual ~Enemies(){}

};

#endif