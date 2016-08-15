#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Jopnal/Jopnal.hpp>
#include "CharacterHandler.hpp"
#include "Consts.hpp"

class Player : public Characters
{
    WeakReference<Object> m_crosshair;
    float m_jumpCD;

public:

    glm::vec3 m_crossHairLastDir;

    void aim(const float aimX, const float aimY);

    Player(Scene& sceneRef) :
        Characters(sceneRef, Characters::Type::playerC, s_shootPlayerCD, s_immuneCD, s_jumpCD, m_scene.createChild("player")),

        m_crosshair(m_scene.createChild("crosshair")),
        m_crossHairLastDir(1.f, 0.f, 0.f)
    {
        //Player
        {
            m_character->setPosition(s_playerStartPos);
            m_character->createComponent< jop::Drawable>(m_scene.getRenderer()).setModel(*c_playerModel);

            //Player animation
            {
                m_character->setScale(0.003f); //big tex
                m_character->createComponent<AnimatedSprite>(m_scene.getRenderer());
                m_character->getComponent<AnimatedSprite>()->setAnimationRange(0u, 11u).setFrameTime(1.f / 60.f).setAtlas(*c_animAtlasPlayer);
            }

            //Sounds
        {
            {
                m_character->createComponent<jop::SoundEffect>().setID(22u);
                static_cast<jop::SoundEffect&>(*m_character->getComponent<jop::SoundEffect>(22u)).setBuffer(ResourceManager::get<jop::SoundBuffer>("tehGame/end.wav")).setSpatialization(false);
            }
            {
                m_character->createComponent<jop::SoundEffect>().setID(23u);
                static_cast<jop::SoundEffect&>(*m_character->getComponent<jop::SoundEffect>(23u)).setBuffer(ResourceManager::get<jop::SoundBuffer>("tehGame/menu.wav")).setSpatialization(false);
            }
        }
        m_character->createComponent<RigidBody2D>(m_scene.getWorld<2>(), *c_playerInfo);
        }

        //Crosshair
    {
        auto& tex = ResourceManager::get<Texture2D>("tehGame/crosshair.png", true, false);
        auto& mat = ResourceManager::getEmpty<Material>("crosshairmat", true);
        auto& mesh = ResourceManager::getNamed<CircleMesh>("crosshairmesh", 0.5f, 15u);

        mat.setMap(Material::Map::Diffuse, tex);

        m_crosshair->createComponent<Drawable>(m_scene.getRenderer()).setModel(Model(mesh, mat));
        m_crosshair->setPosition(s_playerStartPos.x + s_limitCrossHair, s_playerStartPos.y, s_playerStartPos.z);
    }
    }



};

void Player::aim(const float aimX, const float aimY)
{
    if ((aimX > 0.f && aimX < g_screenSize.x) && (aimY > 0.f && aimY < g_screenSize.y))
        m_crossHairLastDir = glm::normalize(glm::vec3(
        (aimX / (g_screenSize.x * 0.5f)) - 1.f,
        (aimY / (g_screenSize.y * 0.5f)) - 1.f,
        m_crossHairLastDir.z))*s_limitCrossHair;

    m_crosshair->setPosition(m_character->getGlobalPosition() + m_crossHairLastDir * s_limitCrossHair);
}


#endif