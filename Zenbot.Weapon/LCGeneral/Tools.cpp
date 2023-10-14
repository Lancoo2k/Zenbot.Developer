#include "Tools.h"

extern LC_SDK_STRUCT* SDK;

bool CheckCanCastCharge(Vector3 Position,float Range) {
	//获取准星目标->敌人坐标->获取敌人周围身边的角色 判断释放条件
	std::vector<LC_PLAYER_INFO> Enemys;
	SDK->UTILITY.GetEnemysHeroesListInRange(Enemys, Position,Range);
	if (!SDK->UTILITY.CanCastChargeByList(Enemys)) {
		//不能放蓄力 直接返回假
		return false;
	}
	for (const auto& j : Enemys) {
		bool isbigMode = j.BasicPlayerInfo.Dafo_V || j.BasicPlayerInfo.ShenMiao_V || j.BasicPlayerInfo.YueShan_V || j.BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(j.ActorModel) == 6 ||
			SDK->GAMEFUNC.GetHeroBattleStatus(j.ActorModel) == 1 || (j.BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime());
		bool isHurtMode = SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(j.ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(j.ActorModel) || j.BasicPlayerInfo.IsBounce || SDK->GAMEFUNC.GetPlayerEndureLevel(j.ActorModel) == 6 || j.BasicPlayerInfo.CuiSanNiang_F || j.BasicPlayerInfo.GuQingHan_Frost || j.BasicPlayerInfo.CuiSanNiang_V_tied || j.BasicPlayerInfo.IsShaNanFHurt || j.BasicPlayerInfo.IsStunned || SDK->ANIMHASH.IS_WUCHEN_teleported(j.ActorModel) || j.BasicPlayerInfo.IsCharm;
		bool isHitMode = j.BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || j.BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() ||
			(j.BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime()) || SDK->ANIMHASH.IsHurtFlayingToLand(j.ActorModel) ||
			SDK->ANIMHASH.IS_IDLE(j.ActorModel) || SDK->ANIMHASH.IS_RUNNING(j.ActorModel) || SDK->ANIMHASH.IsInjuredLie(j.ActorModel) || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(j.ActorModel));
		if (isbigMode || isHurtMode || isHitMode) {
			return true;
		}
	}
	return false;
}
bool AttackBlade(LC_PLAYER_INFO* Enemy,bool normal) {
	int wId = SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel);
	if (wId == 119 || wId == 103) {
		auto Stack = SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(Enemy->ActorModel);
		if (normal) {
			return Stack == 1 || (SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20 && Stack != 4);
		}
		return wId == 103 ? (Stack == 2 || Stack == 3 || Stack == 4) : (Stack == 3 || Stack == 5 || Stack == 2);
	}
}
bool TargetIsFlee(LC_PLAYER_INFO* Player,LC_PLAYER_INFO* Enemy) {
	float yaw = fabsf(SDK->MATH.AngleDifference(SDK->MATH.get_enityangle(Enemy->ActorModel), SDK->MATH.VectorToRotationYaw(SDK->MATH.FindLookAtVector(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel), SDK->GAMEFUNC.GetPlayerPosition(Player->ActorModel)))));//敌人面向 自己 的 角度
	if (yaw >= 80 && yaw <= 180) {

		auto LocalH = GetRealHealth(Player);
		auto EnemyH = GetRealHealth(Enemy);
		//我一共 2500血 他 1000血 HP Rate 都是100%
		if (LocalH - EnemyH > 300) {
			return true;
		}
	}
	return false;
}
void CheckCastSkill(LC_PLAYER_INFO* Player, Control& LastControl) {
	auto Data = SDK->GAMEFUNC.GetAppointSkill(Player->ActorModel, 1);
	if (Data.OutputKey == 'F') {
		SDK->KEYBOARD.SendKeyBoard(Data.OutputKey, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		SDK->KEYBOARD.SendKeyBoard(Data.OutputKey, 2);
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.1f;
		bool IsSuccess = false;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == Data.EndureLevel) {
				IsSuccess = true;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		//if (IsSuccess) {
		//	Tick = SDK->GAMEFUNC.GetGlobalTime() + Data.ExecuteAnimTime;
		//	while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//	}
		//}
	}
	LastControl = Control::LongFlash;
}
bool General_HuWeiAttack(LC_PLAYER_INFO* Player, LC_PLAYER_INFO* Enemy,Control& LastControl) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		LastControl = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (!SDK->KEYBOARD.IsPressKey('W')) {
		SDK->KEYBOARD.SendKeyBoard('W', 0);
	}
	if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, 5.f) && SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, false)) {
		//先打一次跳A 判断敌军情况
		bool logic1 = false;
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.60f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
				//目标被浮空 接着跳A 先等待落地
				logic1 = true;
				break;
			}
		}
		if (logic1) {
			//这里 自己落地了
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 1.0f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Player->ActorModel) || SDK->ANIMHASH.IS_RUNNING(Player->ActorModel)) {
					break;
				}
				if (SDK->GAMEFUNC.IsHiting(Player->ActorModel) || Player->BasicPlayerInfo.IsBounce) {
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			do {
				if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, 5.f) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, false)) {
					if (SDK->KEYBOARD.IsPressKey('W')) {
						SDK->KEYBOARD.SendKeyBoard('W', 2);
					}
					SDK->KEYBOARD.StopCombo();
					LastControl = Control::LongFlash;
					return false;
				}
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.6f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Player->ActorModel) || SDK->ANIMHASH.IS_RUNNING(Player->ActorModel)) {
						break;
					}
					if (SDK->GAMEFUNC.IsHiting(Player->ActorModel) || Player->BasicPlayerInfo.IsBounce) {
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

			} while (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel));
		}
		if (SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 2);
		}
		if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, 5.f) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(2,0)) {
				if (SDK->KEYBOARD.IsPressKey('W')) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
				}
				SDK->KEYBOARD.StopCombo();
				LastControl = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(Player, Enemy, 0.21f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				if (SDK->KEYBOARD.IsPressKey('W')) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
				}
				SDK->KEYBOARD.StopCombo();
				LastControl = Control::DirectInput;
				return false;
			}
			bool success_get_recover = false;
			std::wstring TempName;
			Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				SDK->GAMEFUNC.GetActionName(Player->ActorModel, TempName);
				if (TempName.find(L"heavy_01_recover") != std::wstring::npos) {
					//获取到recover;
					success_get_recover = true;
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (success_get_recover) {
				SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(Player, Enemy, 0.61f);
				LastControl = Control::LongFlash;
				SDK->KEYBOARD.Set_IsExecuteCombo(false);
				SDK->KEYBOARD.StopCombo();
				return true;
			}
		}
	}
	if (SDK->KEYBOARD.IsPressKey('W')) {
		SDK->KEYBOARD.SendKeyBoard('W', 2);
	}
	LastControl = Control::LongFlash;
	SDK->KEYBOARD.StopCombo();
	return false;
}
int superstoi(const std::string& name) {
	if (name.empty()) {
		return 0;
	}
	return std::stoi(name);
}
int GetRealHealth(LC_PLAYER_INFO* Player) {
	return Player->BasicPlayerInfo.Property.CurHp + Player->BasicPlayerInfo.Property.Curshield;
}
int GetHpRate(LC_PLAYER_INFO* Player) {
	auto Max = Player->BasicPlayerInfo.Property.HpMax + Player->BasicPlayerInfo.Property.shieldMax;
	return (GetRealHealth(Player) / Max) * 100;
}
bool CheckQuality(int LocalWeapon, int HopeWeapon) {
	int level1 = (LocalWeapon % 100) / 10;
	int level2 = (HopeWeapon % 100) / 10;
	if (level1 <= level2) {
		return true;
	}
	return false;
}