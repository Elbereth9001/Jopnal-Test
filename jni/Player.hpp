#include <Jopnal/Jopnal.hpp>

namespace jd
{
    class Player : public Object
    {
    public:
        Player(const glm::vec2& spawnPos) :
            Object(spawnPos)
        {};

        ~Player();

    private:


    };
}
