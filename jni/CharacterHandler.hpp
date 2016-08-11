#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP
#include <Jopnal/Jopnal.hpp>

using namespace jop;

class Characters
{

private:
    Scene& m_scene;

public:
    std::pair<WeakReference<jop::Object>, WeakReference<jop::Object>> createPlayer();

    Characters(Scene& sceneRef) :
        m_scene(sceneRef)

    {

    }

};

std::pair<WeakReference<jop::Object>, WeakReference<jop::Object>> Characters::createPlayer()
{



    WeakReference<jop::Object> player = m_scene.createChild("player");
    {
        RigidBody2D::ConstructInfo2D playerInfo(ResourceManager::getNamed<CapsuleShape2D>("player", 1.f, 2.f), RigidBody2D::Type::Dynamic, 1.2f);
        player->createComponent<RigidBody2D>(m_scene.getWorld<2>(), playerInfo);
        player->setPosition(s_playerStartPos);

        auto& tex = ResourceManager::get<Texture2D>("tehGame/player.png", true, false);
        auto& mat = ResourceManager::getEmpty<Material>("playermat", true);
        auto& mesh = ResourceManager::getNamed<RectangleMesh>("playermesh", glm::vec2(1.f, 2.f));

        mat.setMap(Material::Map::Diffuse, tex);

        player->createComponent< jop::Drawable>(m_scene.getRenderer()).setModel(Model(mesh, mat));
    }

    WeakReference<jop::Object> crossHair = m_scene.createChild("crossHair");
    {
        auto& tex = ResourceManager::get<Texture2D>("tehGame/crosshair.png", true, false);
        auto& mat = ResourceManager::getEmpty<Material>("crosshairmat", true);
        auto& mesh = ResourceManager::getNamed<CircleMesh>("crosshairmesh", 0.5f, 15u);

        mat.setMap(Material::Map::Diffuse, tex);

        crossHair->createComponent<Drawable>(m_scene.getRenderer()).setModel(Model(mesh, mat));
        crossHair->setPosition(s_playerStartPos.x + s_limitCrossHair, s_playerStartPos.y, s_playerStartPos.z);
    }
    return std::make_pair(player, crossHair);
}

#endif