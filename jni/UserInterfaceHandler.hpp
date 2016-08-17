#ifndef UI_HPP
#define UI_HPP
#include <Jopnal/Jopnal.hpp>

class UI
{
    struct Screen
    {
        //First: showMask, Second: hiddenMask
        unsigned int mask;
        jop::WeakReference<jop::Object> obj;
        std::vector<std::string> names;
    };

    std::vector<Screen> m_screens;
    jop::Scene& m_scene;
    const jop::WeakReference<jop::Object>& m_camRef;

    ///////////////////////////////////////////////////////////////////////////////////////////

    int findScreen(const std::string& root)
    {
        for (unsigned int i = 0u; i < m_screens.size(); ++i)
        {
            if (m_screens[i].obj->getID() == root)
            {
                return i;
            }
        }
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

public:

    ///////////////////////////////////////////////////////////////////////////////////////////

    UI(jop::Scene& scene, jop::WeakReference<jop::Object>& camObj) :
        m_scene(scene),
        m_screens(),
        m_camRef(camObj)
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////

    //TODO make template
    jop::WeakReference<jop::Object>& UI::createScreen(const std::string& root, const std::vector<glm::vec3>& positions, const std::vector<std::string>& names,
        const unsigned int mask = 0u,
        const std::vector<int>& values = std::vector<int>(),
        const std::vector<jop::Color>& colors = std::vector<jop::Color>())
    {
        JOP_ASSERT(names.size() > 1u, "Too small element");

        bool useValues(!values.empty());
        if (useValues)
            JOP_ASSERT(names.size() == values.size() && names.size() == positions.size(), "createScreen sizes don't match");

        Screen newScreen;

        //Root
        newScreen.obj = m_scene.createChild(root);
        newScreen.obj->setScale(0.02f);
        newScreen.mask = mask;

        //Names
        newScreen.names = names;

        //Children
        for (unsigned int i = 0u; i < names.size(); ++i)
        {
            newScreen.obj->createChild(names[i].c_str());

            auto& o = newScreen.obj->findChild(names[i].c_str());

            o->setPosition(positions[i]);
            o->createComponent<jop::Text>(m_scene.getRenderer()).setRenderGroup(mask);

            auto* t = o->getComponent<jop::Text>();

            if (useValues)
                t->setString(names[i] + std::to_string(values[i]));
            else
                t->setString(names[i]);

            if (i >= colors.size())
                t->setColor(jop::Color::Green);
            else
                t->setColor(colors[i]);
        }

        m_screens.push_back(newScreen);
        return newScreen.obj;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void updateScreenValues(const std::string& root, const std::vector<int>& newValues)
    {
        int index = findScreen(root);

        if (index == -1)
        {
            JOP_DEBUG_INFO("updateScreen not found: " << root);
            return;
        }

        auto& s = m_screens[index];

        JOP_ASSERT(s.names.size() == newValues.size(), "updateScreen newValues size not matching with old.");

        for (unsigned int i = 0u; i < s.obj->getChildren().size() - 1u; ++i)
        {
            s.obj->findChild(s.names[i])->getComponent<jop::Text>()->setString(std::to_string(newValues[i]));
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void updateScreenPosition(const std::string& root, const glm::vec3& newPos)
    {
        int index = findScreen(root);

        if (index == -1)
        {
            JOP_DEBUG_INFO("updateScreen not found: " << root);
            return;
        }

        m_screens[index].obj->setPosition(newPos);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void setNewMask(const std::string& root, const unsigned int mask)
    {
        int index = findScreen(root);

        if (index == -1)
        {
            JOP_DEBUG_INFO("updateScreen not found: " << root);
            return;
        }

        auto s = m_screens[index];

        for (unsigned int i = 0u; i < s.obj->getChildren().size() - 1u; ++i)
        {
            s.obj->findChild(s.names[i])->getComponent<jop::Text>()->setRenderGroup(mask);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

};

#endif