#ifndef HOLD_HPP
#define HOLD_HPP
#include <Jopnal/Jopnal.hpp>

const static glm::vec3 s_playerStartPos(0.f, 5.f, 0.f);
const static float s_limitCrossHair(2.f);
const static unsigned int s_maxBullets(50u);
const static float s_jumpCD(1.f);
const static float s_shootCD(0.1f);
const static float s_bulletSpeed(50.f);

unsigned long long int score(0u);

#endif