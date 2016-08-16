#ifndef MORDOR_HPP
#define MORDOR_HPP

#include "Consts.hpp"
#include <Jopnal/Jopnal.hpp>

class SceneWorld
{
    jop::Scene& m_scene;
    jop::WeakReference<jop::Object> m_ground;

public:
    
    std::vector<glm::vec2> m_groundP;
    unsigned int m_levelLength;
    float m_endPoint;

    SceneWorld(jop::Scene& sceneRef):
        m_scene(sceneRef),
        m_ground(m_scene.createChild("ground")),
        m_groundP(),
        m_levelLength(1u),
        m_endPoint(0.f)
    {}
    
    void createWorld();

};

void SceneWorld::createWorld()
{

    jop::Randomizer r;

    //Ground hitbox
#if 1
    {
        m_levelLength = r.range<unsigned int>(s_minWorldLength, s_maxWorldLength);
        m_groundP.reserve(m_levelLength);

        //Small flat area at the beginning
        float pointX = -5.f;
        float pointY = 0.f;
        m_groundP.emplace_back(pointX, pointY);
        pointX = 0.f;
        pointY = 0.f;
        m_groundP.emplace_back(pointX, pointY);

        for (unsigned int i = 0; i < m_levelLength; ++i)
        {
            pointX = r.range<float>(pointX + s_minWorldOffsetX, pointX + s_maxWorldOffsetX);
            pointY = r.range<float>(pointY + s_minWorldOffsetY, pointY + s_maxWorldOffsetY);

            m_groundP.emplace_back(pointX, pointY);
        }

        m_endPoint = pointX;

        jop::RigidBody2D::ConstructInfo2D groundInfo(jop::ResourceManager::getNamed<jop::TerrainShape2D>("ground", m_groundP));

        m_ground->createComponent<jop::RigidBody2D>(m_scene.getWorld<2>(), groundInfo);

    }
#endif

    //Ground coloring: gradient: top = White, bottom = green
#if 1
    {
        std::vector<std::pair< glm::vec3, glm::vec4>> meshPoints;
        std::vector<unsigned int> meshIndices;
        jop::uint32 base(0u);
        float colorHere(0.f);
        float colorNext(0.f);
        const glm::vec4 colorBottom(0.f, 1.f, 0.f, 1.f);  //RGBA


        meshPoints.reserve(m_groundP.size() * 4u);
        meshIndices.reserve(m_groundP.size() * 6u);

        for (unsigned int i = 0u; i < m_groundP.size() - 1u; ++i)
        {
            base = i * 4u;

            meshIndices.push_back(base + 3u);
            meshIndices.push_back(base + 0u);
            meshIndices.push_back(base + 1u);
            meshIndices.push_back(base + 1u);
            meshIndices.push_back(base + 2u);
            meshIndices.push_back(base + 3u);

            //Both = 0.f - +1.f
            colorHere = std::max(0.f, std::min(1.f, 1.f + (((m_groundP[i].y + s_colorLimitBottom) / s_colorLimitTop) * 0.5f)));
            colorNext = std::max(0.f, std::min(1.f, 1.f + (((m_groundP[i + 1u].y + s_colorLimitBottom) / s_colorLimitTop) * 0.5f)));

            meshPoints.emplace_back(glm::vec3(m_groundP[i].x, m_groundP[i].y, 0.f), glm::vec4(colorHere, 1.f, colorHere, 1.f));                 // current point = left
            meshPoints.emplace_back(glm::vec3(m_groundP[i].x, s_minWorldOffsetY * s_maxWorldLength, 0.f), colorBottom);                         // bottom left
            meshPoints.emplace_back(glm::vec3(m_groundP[i + 1u].x, s_minWorldOffsetY * s_maxWorldLength, 0.f), colorBottom);                    // bottom right
            meshPoints.emplace_back(glm::vec3(m_groundP[i + 1u].x, m_groundP[i + 1u].y, 0.f), glm::vec4(colorNext, 1.f, colorNext, 1.f));       // next point = right

        }
        auto& mesh = jop::ResourceManager::getEmpty<jop::Mesh>("groundMesh");
        mesh.load(meshPoints.data(), meshPoints.size()*sizeof(std::pair< glm::vec3, glm::vec4 >), jop::Mesh::Position | jop::Mesh::Color, meshIndices.data(), sizeof(unsigned int), meshIndices.size());
        auto& mat = jop::ResourceManager::getEmpty<jop::Material>("groundmat", false);

        m_ground->createComponent<jop::Drawable>(m_scene.getRenderer()).setModel(jop::Model(mesh, mat));
    }
#endif

}

#endif