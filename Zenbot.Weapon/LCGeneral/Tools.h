#pragma once
#include <Windows.h>
#include <thread>
#include <random>
#include "../../Zenbot_SDK/LC_SDK.h"
#include "Weapons.h"

void CheckCastSkill(LC_PLAYER_INFO* Player, Control& LastControl);
int superstoi(const std::string& name);
int GetRealHealth(LC_PLAYER_INFO* Player);
int GetHpRate(LC_PLAYER_INFO* Player);
bool CheckQuality(int LocalWeapon, int HopeWeapon);
bool General_HuWeiAttack(LC_PLAYER_INFO* Player, LC_PLAYER_INFO* Enemy, Control& LastControl);
bool TargetIsFlee(LC_PLAYER_INFO* Player, LC_PLAYER_INFO* Enemy);
bool AttackBlade(LC_PLAYER_INFO* Enemy, bool normal);
bool CheckCanCastCharge(Vector3 Position, float Range);