#ifndef HOLD_HPP
#define HOLD_HPP
#include <Jopnal/Jopnal.hpp>

//Statics

//Cooldowns
const static float s_jumpCD(1.f);
const static float s_immuneCD(0.6f);
const static float s_shootPlayerCD(0.1f);
const static float s_shootStaticCD(0.4f);
const static float s_shootTargetCD(0.75f);

//Limits
const static float s_maxMoveSpeed(15.f);
const static float s_limitCrossHair(2.f);
const static float s_minWorldOffsetX(1.f);
const static float s_maxWorldOffsetX(5.f);
const static float s_minWorldOffsetY(-2.f);
const static float s_maxWorldOffsetY(2.f);
const static unsigned int s_maxBullets(50u);
const static unsigned int s_minWorldLength(40u);
const static unsigned int s_maxWorldLength(60u);
const static int g_healthPlayerStart(50);
const static int g_healthEnemyStart(5);
const static int g_jumpPower(20);

const static float s_colorLimitTop(s_maxWorldOffsetY * s_maxWorldLength * 0.2f);   //Max values at: offset * length * speed
const static float s_colorLimitBottom(s_minWorldOffsetY * s_maxWorldLength * 0.2f);
const static float s_bulletSpeed(50.f);
const static glm::vec3 s_playerStartPos(-1.f, 5.f, 0.f);

//Consts
const jop::Model* c_bulletModel;
const jop::Model* c_playerModel;
const jop::Model* c_crosshairModel;
const jop::Model* c_backgroundModel1;
const jop::Model* c_backgroundModel2;
const jop::AnimationAtlas* c_animAtlasPlayer;
const jop::AnimationAtlas* c_animAtlasEnemyStatic;
const jop::AnimationAtlas* c_animAtlasEnemyTarget;
const jop::RigidBody2D::ConstructInfo2D* c_bulletInfo;
const jop::RigidBody2D::ConstructInfo2D* c_playerInfo;
const jop::RigidBody2D::ConstructInfo2D* c_EnemyInfoStatic;
const jop::RigidBody2D::ConstructInfo2D* c_EnemyInfoTarget;

//Saved stats here too atm
int g_health(g_healthPlayerStart);
unsigned int g_score(0u);
unsigned int g_density(5u);
unsigned int g_level(1u);

//Stuff
glm::vec2 g_screenSize;

//Controller connected
bool g_cc(false);

#endif