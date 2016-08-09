#include <Jopnal/Jopnal.hpp>

namespace jd
{
    class Object
    {
    public:
        Object(const glm::vec2& spawnPos):
            m_position(spawnPos)
        {};

        virtual ~Object() = 0;

        glm::vec2 m_position;

        void move(const glm::vec2& dir, const float force)
        {
            m_position += dir * force;
        }

    private:


    };
}
