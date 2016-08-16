#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Jopnal/Jopnal.hpp>

class Player : public Characters
{
    jop::WeakReference<jop::Object> m_crosshair;

public:

    glm::vec3 m_crossHairLastDir;

    ////////////////////////////////

    void aim(const float aimX, const float aimY)
    {
        if ((aimX > 0.f && aimX < g_screenSize.x) && (aimY > 0.f && aimY < g_screenSize.y))
            m_crossHairLastDir = glm::normalize(glm::vec3(
            (aimX / (g_screenSize.x * 0.5f)) - 1.f,
            (aimY / (g_screenSize.y * 0.5f)) - 1.f,
            m_crossHairLastDir.z))*s_limitCrossHair;

        m_crosshair->setPosition(m_character->getGlobalPosition() + m_crossHairLastDir * s_limitCrossHair);
    }

    ////////////////////////////////

    Player(jop::Scene& scene) :
        Characters(scene, CharacterType::playerC, s_immuneCD, s_shootPlayerCD, s_jumpCD, g_healthPlayerStart, scene.createChild("player")),

        m_crosshair(scene.createChild("crosshair")),
        m_crossHairLastDir(1.f, 0.f, 0.f)
    {
        //Player
        {
            m_character->setPosition(s_playerStartPos);
            m_character->createComponent< jop::Drawable>(m_scene.getRenderer()).setModel(*c_playerModel);

            //Player animation
            {
                m_character->setScale(0.003f); //big tex
                m_character->createComponent<jop::AnimatedSprite>(m_scene.getRenderer());
                m_character->getComponent<jop::AnimatedSprite>()->setAnimationRange(0u, 11u).setFrameTime(1.f / 60.f).setAtlas(*c_animAtlasPlayer);
            }

            //Sounds
        {
            {
                m_character->createComponent<jop::SoundEffect>().setID(22u);
                static_cast<jop::SoundEffect&>(*m_character->getComponent<jop::SoundEffect>(22u)).setBuffer(jop::ResourceManager::get<jop::SoundBuffer>("tehGame/end.wav")).setSpatialization(false);
            }
            {
                m_character->createComponent<jop::SoundEffect>().setID(23u);
                static_cast<jop::SoundEffect&>(*m_character->getComponent<jop::SoundEffect>(23u)).setBuffer(jop::ResourceManager::get<jop::SoundBuffer>("tehGame/menu.wav")).setSpatialization(false);
            }
        }
        m_character->createComponent<jop::RigidBody2D>(m_scene.getWorld<2>(), *c_playerInfo);
        }

        //Crosshair
    {
        m_crosshair->createComponent<jop::Drawable>(m_scene.getRenderer()).setModel(*c_crosshairModel);
        m_crosshair->setPosition(s_playerStartPos.x + s_limitCrossHair, s_playerStartPos.y, s_playerStartPos.z);
    }
    }

    virtual ~Player(){}

};



#endif