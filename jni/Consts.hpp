#ifndef HOLD_HPP
#define HOLD_HPP
#include <Jopnal/Jopnal.hpp>

//Statics
const static float s_limitCrossHair(2.f);
const static float s_jumpCD(1.f);
const static float s_shootCD(0.1f);
const static float s_immuneCD(0.6f);
const static float s_bulletSpeed(50.f);
const static float s_maxMoveSpeed(15.f);
const static float s_minWorldOffsetX(1.f);
const static float s_maxWorldOffsetX(5.f);
const static float s_minWorldOffsetY(-2.f);
const static float s_maxWorldOffsetY(2.f);
const static unsigned int s_maxBullets(50u);
const static unsigned int s_minWorldLength(40u);
const static unsigned int s_maxWorldLength(60u);
const static float s_colorLimitTop(s_maxWorldOffsetY * s_maxWorldLength * 0.2f);   //Max values at: offset * length * speed
const static float s_colorLimitBottom(s_minWorldOffsetY * s_maxWorldLength * 0.2f);
const static glm::vec3 s_playerStartPos(-1.f, 5.f, 0.f);

//Consts
const jop::AnimationAtlas* c_animAtlas;
const jop::RigidBody2D::ConstructInfo2D* c_bulletInfo;
const jop::RigidBody2D::ConstructInfo2D* c_playerInfo;

//Saved stats here too atm
unsigned long long int score(0u);
unsigned int healthPlayer(50u);

//Stuff

//Controller connected
bool g_cc(false);

#endif