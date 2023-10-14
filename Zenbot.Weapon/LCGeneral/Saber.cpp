
#include "Tools.h"

extern LC_SDK_STRUCT* SDK;
void Saber::MainExecuteCode() {

}
void Saber::FreeEvent() {

}
void Saber::LibraryInit() {

}
void Saber::ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	int WeaponID = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 119) {

		this->RunableLogic(LocalPlayer, Enemy);
	}
}
bool Saber::IsInAttackRange(float Distance, float ExtraRange) {
	return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
}
int Saber::GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack) {
	if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime()) {
		return 2;
	}
	if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel)) {
		return 2;
	}
	return 1;
}
bool Saber::IsFastStatck(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return (Name.find(skCrypt(L"_attack_light")) != std::wstring::npos || Name.find(skCrypt(L"_attack_heavy")) != std::wstring::npos) && (Name.find(skCrypt(L"_02")) != std::wstring::npos || Name.find(skCrypt(L"_03")) != std::wstring::npos || Name.find(skCrypt(L"_07")) != std::wstring::npos || Name.find(skCrypt(L"_04")) != std::wstring::npos || Name.find(skCrypt(L"_06")) != std::wstring::npos || Name.find(skCrypt(L"_05")) != std::wstring::npos) && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;;
	}
	return false;
}
void Saber::ResetCC() {
	SDK->KEYBOARD.SendKeyBoard('C', 0);
	SDK->KEYBOARD.Set_IsActiveCrouch(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	SDK->KEYBOARD.SendKeyBoard('C', 2);
	SDK->KEYBOARD.Set_IsActiveCrouch(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	SDK->KEYBOARD.SendKeyBoard('C', 0);
	SDK->KEYBOARD.Set_IsActiveCrouch(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	SDK->KEYBOARD.SendKeyBoard('C', 2);
	SDK->KEYBOARD.Set_IsActiveCrouch(false);
}
bool Saber::Get_IsCharge(LC_PLAYER_INFO* Player) {
	return SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
}
bool Saber::AttackShock(LC_PLAYER_INFO* LocalPlayer)
{
	float Tick = SDK->GAMEFUNC.GetGlobalTime() + 1.2f;
	while (Tick > SDK->GAMEFUNC.GetGlobalTime())
	{
		if (GetAsyncKeyState(VK_LBUTTON) >= 0)
		{
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
			SDK->KEYBOARD.Set_IsActiveLeft(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
			SDK->KEYBOARD.Set_IsActiveLeft(false);
		}
		float FirstTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
		while (FirstTime > SDK->GAMEFUNC.GetGlobalTime())
		{
			if (SDK->ANIMHASH.IS_ATTACK_SHOCK(LocalPlayer->ActorModel))
			{
				return true;
			}
			if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
				return false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	return false;
}
bool Saber::CBAB(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	//过滤掉pre动作
	std::wstring Dont_pre_name;
	float Tick = SDK->GAMEFUNC.GetGlobalTime() + 1.2f;
	while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		if (SDK->GAMEFUNC.GetActionName(Enemy->ActorModel, Dont_pre_name)) {
			if (Dont_pre_name.find(skCrypt(L"_pre")) == std::wstring::npos) {
				break;
			}
		}
		if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	Tick = SDK->GAMEFUNC.GetGlobalTime() + 3.5f;
	while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
			break;
		}
		if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (!SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) || !SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	//0.87是原地捏蓝 0.58是重复CC
	if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 1.00f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	NextInput = Control::LongFlash;
	return true;
}
bool Saber::ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer))
	{
		auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Blade_DuoHun);
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, SoulMode ? 0.53f : 0.87f)) {
			SDK->KEYBOARD.StopCombo();
			return false;
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(2, true)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		bool success_get_recover = false;
		std::wstring TempName;
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, TempName);
			if (TempName.find(L"heavy_01_recover") != std::wstring::npos) {
				//获取到recover;
				success_get_recover = true;
				break;
			}
			if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (success_get_recover) {
			SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, LocalPlayer->BasicPlayerInfo.HuWei_V ? 0.61f : 0.87f);
			NextInput = Control::DirectInput;
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			SDK->KEYBOARD.StopCombo();
			return true;
		}
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		return true;
	}
	NextInput = Control::LongFlash;
	SDK->KEYBOARD.StopCombo();
	return false;
}
void Saber::Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {

	static float Local_ChargeTime = 0.f;

	auto EnemyPosition = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);
	auto MyPosition = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel);
	float Distance = EnemyPosition.DistTo(MyPosition);

	bool IsCharge = Get_IsCharge(LocalPlayer);

	bool EnemyIsDontAttack = SDK->ANIMHASH.Action_HuoNan_IsF(Enemy->ActorModel) || Enemy->BasicPlayerInfo.GuQingHan_F || SDK->ANIMHASH.Is_CuiSanNiangV2(Enemy->ActorModel);

	if (DisableWeaponWhiteMode > SDK->GAMEFUNC.GetGlobalTime()) {
		DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 1.5f;
		DisableWeaponWhiteMode = 0.f;
	}
	if (AllowResetChargeTime > SDK->GAMEFUNC.GetGlobalTime()) {
		StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
		AllowResetChargeTime = 0.f;
	}

	if (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel) && ForceUseRight < SDK->GAMEFUNC.GetGlobalTime()) {

		//拿到当前动画时间
		bool SuccessCC = false;
		while (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
			float CurTime = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
			if (CurTime > 0.50f) {
				ResetCC();
				LastControl = Control::DirectInput;
				return;
			}
			if (CurTime >= 0.44f && Distance <= 6.f) {

				//>=0.31 证明可以释放鬼反断 判断敌人条件
				float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;
				float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;
				float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

				bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
					SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
				bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f;
				bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel);

				//能鬼反优先鬼反
				if ((isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.15f || FlashStepTime < 0.15f) && Distance <= 4.f) {
					//可以释放丙基~~~ 直接调用CALL
					//attach_3abcombo(LocalPlayer, Enemy, LastControl, false, true, HasJingKe, HasGuiKu);

					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
					SDK->KEYBOARD.Set_IsActiveLeft(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
					SDK->KEYBOARD.Set_IsActiveLeft(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.56f);
					ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					LastControl = Control::DirectInput;
					break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		int ChargeType = GetNextChargeType(Enemy, Distance);
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if (((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) && (Time - LocalPlayer->BasicActiontime.LastBladeParryTime > 2.f)) ||
			(Time - ChangeWeaponTime < 0.2f) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			LastControl = Control::DirectInput;
			ChargeType = 1;
		}
		else {
			if ((Time - LocalPlayer->BasicActiontime.LastHuWeiJumpAATime < 1.0f)) {
				if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					CBAB(LocalPlayer, Enemy, LastControl);
				}
				return;
			}
		}
		std::wstring Name;
		SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel,Name);
		//printf("状态进入 | %d %d %d %ls\n",IsCharge, SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel), SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel),Name.c_str());
		if (LastControl == Control::LongFlash)
		{
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.5f)
			{
				if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IsAllGetUp(LocalPlayer->ActorModel)&& !SDK->ANIMHASH.IS_PUTDOWN(LocalPlayer->ActorModel)) {

					if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(ChargeType, 'W', false, 0, 0.f)) {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					}
					else {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
						if (ChargeType == 1) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
							SDK->KEYBOARD.Set_IsActiveLeft(true);
							StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
						}
						else {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
							SDK->KEYBOARD.Set_IsActiveRight(true);

						}
					}

				}
				else
				{
					//printf("蓄力logic1\n");
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					if (ChargeType == 1) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
						SDK->KEYBOARD.Set_IsActiveLeft(true);
					}
					else {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
						SDK->KEYBOARD.Set_IsActiveRight(true);
					}
				}
			}
			else {
				//printf("原地蓄了\n");
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				if (ChargeType == 1) {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
					SDK->KEYBOARD.Set_IsActiveLeft(true);
				}
				else {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
				}
			}
		}
		if (LastControl == Control::CCReset) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.BasicLogic.ExecuteCC();
			return;
		}
		if (LastControl == Control::DirectInput) {
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			if (ChargeType == 1) {
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
				SDK->KEYBOARD.Set_IsActiveLeft(true);
			}
			else {
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
				SDK->KEYBOARD.Set_IsActiveRight(true);
			}
			LastControl = Control::LongFlash;
			return;
		}
		if (LastControl == Control::FlashStep) {
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				SDK->KEYBOARD.BasicLogic.ShortFlash('S', false);
			}
			LastControl = Control::CCReset;
			return;
		}
	}
	bool IntoCharge = SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel);
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	int EnemyRealHP = GetRealHealth(Enemy);
	int LocalRealHP = GetRealHealth(LocalPlayer);
	int StackValue = SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(LocalPlayer->ActorModel);
	BOOL CanToAttack = SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel) && StackValue != 2 && StackValue != 3 && StackValue != 7 && StackValue != 4 && StackValue != 6 && StackValue != 5 && ChargeTime < 0.49f;

	if (IsCharge) {



		int EnemyEndureLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel);
		int ChargeLevel = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel);
		bool hasChiYan = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Saber_ChiYanZhan);

		if (!EnemyIsDontAttack) {

			bool HuoNanMode = LocalPlayer->BasicPlayerInfo.HuoNan_V && LocalPlayer->BasicPlayerInfo.CanExecuteFlash;

			bool A3Mode = StackValue == 3 && !SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel);

			float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

			float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

			float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

			bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
				SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicPlayerInfo.IsCharm || HuoNanMode;
			bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f ||
				 Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.GuQingHan_Frost || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied || Enemy->BasicPlayerInfo.IsShaNanFHurt || Enemy->BasicPlayerInfo.IsStunned || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel);

			bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) /*|| SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)*/ || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel);

			float LastBladeParryTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastBladeParryTime;

			float LastCrouchTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastCrouchTime;

			bool CanCastCharge = Enemy->BasicPlayerInfo.WuTianF1Tick < SDK->GAMEFUNC.GetGlobalTime();

			HurtCast hurtExecuteMode = HurtCast::Null;

			auto CurrentHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);

			if (CurrentHurt.z > 0.f && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !Enemy->BasicPlayerInfo.IsBladeParry) {
				//当前玩家僵直 > 0.f 开始计算
				hurtExecuteMode = (CurrentHurt.z + ChargeTime > 0.49f) ? HurtCast::Charge : HurtCast::AA;
			}
			bool ShuiPaoMode = false;

			if (Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied) {
				float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 1.10f;
			}

			if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
				float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 1.10f;
			}

			if (Distance <= ChargeRange && (StackValue == 1 || StackValue == 7 || StackValue == 3 || StackValue == 5) && CanCastCharge && LastBladeParryTime > 1.2f) {
				bool CanNext = isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || hurtExecuteMode == HurtCast::Charge;
				if (ShuiPaoMode ? ChargeLevel == 2 :CanNext) {
					bool isLeft = false;
					float RealDelay = 0.f;
					if (IntoCharge) {
						if (StackValue == 1 || StackValue == 7) {
							//左蓄 第一段
							if (hasChiYan) {
								RealDelay = 0.75f;
								//delay == 0.75f
							}
							else {
								RealDelay = 1.09f;
								//delay == 1.09f
							}
							isLeft = true;
						}
					}
					bool chold = false;
					//3为A->B 无蓄力无敌磐石状态
					if (StackValue == 3) {
						RealDelay = 1.20f;
						chold = true;
						//0.84f
					}
					//5为A->B 蓄力无敌磐石状态
					if (StackValue == 5) {
						if (ChargeLevel == 1) {
							//0.73f
							RealDelay = 0.73f;
						}
						if (ChargeLevel == 2) {
							//1.35f
							RealDelay = 1.35f;
						}
						if (ChargeLevel == 3) {
							//1.999743f
							RealDelay = 2.00f;
						}
					}
					if (RealDelay > 0.f) {
						if (isLeft) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
							SDK->KEYBOARD.Set_IsActiveLeft(false);
						}
						else {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
							SDK->KEYBOARD.Set_IsActiveRight(false);
						}
						bool exc = false;
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + RealDelay;
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
							if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
								//自己被振刀就跳出循环
								againexecute = false;
								break;
							}
							if (!isLeft && LocalPlayer->BasicPlayerInfo.HuoNan_V && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
								if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) < 0.2f) {
									//printf("攻击成功 %f \n",SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel));
									exc = true;
									break;
								}
								if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel)) {
									break;
								}
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						if (exc) {
							LastControl = Control::LongFlash;
							return;
						}
						if (chold && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
							char InputKey = NULL;
							bool CanExecuteDoubleWeapon = false;
							auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
							auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
							bool TwinMode = WP && RealWeaponID == 118;
							bool DualMode = WP && RealWeaponID == 121;
							bool SwordMode = WP && RealWeaponID == 101;
							bool RodMode = WP && RealWeaponID == 120;
							bool SpearMode = WP && RealWeaponID == 105;
							bool KatanaMode = WP && RealWeaponID == 102;
							if (TwinMode || SwordMode || DualMode || RodMode || SpearMode || KatanaMode) {
								//DbgPrint("存在太刀.直接进行C切");
								auto isbool = SDK->KEYBOARD.BasicLogic.DodgeChangeWeapon(InputKey);
								if (!isbool) {
									return;
								}
								float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
								while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
									int id = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
									if ((TwinMode && id == 118) || (SwordMode && id == 101) || (SpearMode && id == 105) || (KatanaMode && id == 102) || (DualMode && id == 121) || (RodMode && id == 120)) {
										SDK->CORE.LockHuWeiJumpTime(1.5f);
										ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
										break;
									}
								}
								return;
							}
						}
						if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
							LastControl = Control::LongFlash;
							return;
						}
						else {
							LastControl = Control::DirectInput;
							return;
						}
					}
				}
			}

			bool ParryMode = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastParryTime <= 0.63f;

			if (CanToAttack && DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime()) {

				//printf("嘻嘻哈哈11\n");

				auto CurrentHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);

				float time = SDK->GAMEFUNC.GetChargeTime(Enemy->ActorModel);

				if (IsInAttackRange(Distance) && !LocalPlayer->BasicPlayerInfo.HuoNan_V) {
					bool sucg = time >= 0.04f && time <= 0.12f;
					if (sucg) {
						if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {
							if (StackValue == 1) {
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
								SDK->KEYBOARD.Set_IsActiveLeft(false);
								//这里延迟一会 怕动作未完全展开  
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
								if (SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f)) {
									ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f; //B进入无敌磐石
								}
								LastControl = Control::DirectInput;
								return;
							}
						}
					}
				}

				if (HuoNanMode || (!Enemy->BasicPlayerInfo.YinZiPing_F2 && EnemyEndureLevel != 20 && EnemyEndureLevel != 31 && EnemyEndureLevel != 50 &&
					(Enemy->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IS_IDLE(Enemy->ActorModel) || (hurtlandFlag && !SDK->KEYBOARD.IsPressKey(1)) || SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) ||
						SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || (hurtExecuteMode == HurtCast::AA && (!hurtlandFlag || SDK->KEYBOARD.IsPressKey(2))) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || 
						ParryMode || ChainShotTime < 1.0f || (FlashExecuteTime <= 0.1f && (LocalPlayer->BasicActiontime.LastFlashTime < Enemy->BasicActiontime.LastFlashTime)) || LastCrouchTime < 0.1f || SDK->ANIMHASH.IS_WUCHEN_teleported(Enemy->ActorModel)))) {

					if (IsInAttackRange(Distance)) {

						//(forceAttackTime > SDK->GAMEFUNC.GetGlobalTime() && SDK->ANIMHASH.IsHurt(Enemy->ActorModel))
						if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {

							if (StackValue == 1) {

								//printf("进入222 | %d | %d | %d | %f\n", StackValue, CanToAttack, IntoCharge,ChargeTime);

								//处于一段攻击时
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
								SDK->KEYBOARD.Set_IsActiveLeft(false);
								//这里延迟一会 怕动作未完全展开  
								if (LocalPlayer->BasicPlayerInfo.HuoNan_V) {
									//printf("开始计时\n");
									float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.60f;
									while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
										if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) < 0.2f) {
											//printf("攻击成功 %f \n",SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel));
											break;
										}
										if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel)) {
											break;
										}
									}
									//printf("完毕 %f\n",SDK->GAMEFUNC.GetGlobalTime());
									LastControl = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::LongFlash : Control::DirectInput;
									if (LastControl == Control::DirectInput) {
										ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
									}
									return;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
								if (SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f)) {
									ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f; //B进入无敌磐石
								}
								LastControl = Control::DirectInput;
								return;
							}
						}
						if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {

							if (StackValue == 1) {

								//处于一段攻击时
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
								SDK->KEYBOARD.Set_IsActiveRight(false);
								//这里延迟一会 怕动作未完全展开
								if (LocalPlayer->BasicPlayerInfo.HuoNan_V) {
									float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.84f;
									while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
										if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) < 0.2f) {
											break;
										}
										if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel)) {
											break;
										}
									}
									LastControl = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::LongFlash : Control::DirectInput;
									if (LastControl == Control::DirectInput) {
										ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
									}
									return;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
								if (SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.84f)) {
									ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f; //B进入无敌磐石
								}
								LastControl = Control::DirectInput;
								//printf("GO2\n");
								return;
							}
						}
					}
				}
			}
			if (!CanToAttack && LocalPlayer->BasicPlayerInfo.CanExecuteFlash && ParryMode) {
				//超出放蓄时间时. 例如对方在2.0时出振刀 脚本会来不及读取ParryBcakSwingTime. 所以判断超出时间后转中闪抓!

				float TransitionTime = Enemy->BasicHurtInfo.ParryBackSwingTransitionTime - SDK->GAMEFUNC.GetGlobalTime();

				if (MaxChargeTime < ChargeTime + TransitionTime) {

					//printf("中闪11\n");

					if (Distance <= LeftAARange + 3.f) {

						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0, 0.f)) {
							//中闪
							Distance = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));
							if (Distance <= LeftAARange) {
								//左A嫖
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
								SDK->KEYBOARD.Set_IsActiveLeft(false);
								//这里延迟一会 怕动作未完全展开  
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
								if (SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f)) {
									ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.2f; //B进入无敌磐石
								}
								LastControl = Control::DirectInput;
								return;
							}
						}
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
			//如果目标不在攻击范围内时
			bool x1flag = !IsInAttackRange(Distance,2.f);
			bool x2flag = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime <= 0.2f;
			if ((x1flag && ChargeTime > 0.24f) || x2flag) {
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				return;
			}
		}
		if (ChargeTime >= (EnemyIsDontAttack ? 0.7f : MaxChargeTime)) {
			//printf("重置\n");
			if (EnemyIsDontAttack && IsInAttackRange(Distance)) {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
				return;
			}
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash)
			{
				auto boolean = SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				return;
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
			}
		}
	}

	if (!IsCharge && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && SDK->GAMEFUNC.GetGlobalTime() - StartChargeTime > 1.2f) {
		//printf("asdasd1\n");
		if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
			SDK->KEYBOARD.Set_IsActiveLeft(false);
			return;
		}
		if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
			SDK->KEYBOARD.Set_IsActiveRight(false);
			return;
		}
	}
}
void Saber::RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	//拼刀事件
	auto timexx = GetTickCount64();
	if (LocalPlayer->BasicPlayerInfo.IsBladeParry) {

		//printf("BPT: %f", SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastBladeParryTime);

		std::wstring Name;
		SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name);
		SDK->KEYBOARD.StopCombo();
		//printf("PVVVVIDA\n");
		if (SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastBladeParryTime > 0.2f) {
			//printf("XVVVVV1 %ls | %f\n", Name.c_str(),SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel));
			LastControl = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::FlashStep : Control::CCReset;
		}
		else {
			return;
		}
	}
	if (LocalPlayer->BasicPlayerInfo.IsBounce) {
		//清理一切按键
		SDK->KEYBOARD.StopCombo();
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.70f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (!LocalPlayer->BasicPlayerInfo.IsBounce) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		LastControl = Control::DirectInput;
	}
	//振刀事件
	if (SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastParryTime < 0.3f) {
		//printf("error2\n");
		//清理一切按键
		SDK->KEYBOARD.StopCombo();
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.60f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (LocalPlayer->BasicPlayerInfo.IsShock) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		LastControl = Control::DirectInput;
	}
	//处决事件
	if (LocalPlayer->BasicPlayerInfo.IsShock || SDK->ANIMHASH.IS_SUCCESS_SHOCK(LocalPlayer->ActorModel)) {
		//清理一切按键
		SDK->KEYBOARD.StopCombo();
		ExecuteShock(LocalPlayer, Enemy, LastControl);
		return;
	}
	auto MyPosition = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel);
	auto EnemyPosition = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);

	if (SDK->UTILITY.GetCountEnemysHeroesInRange(EnemyPosition, 8.f) == 1 && //目标周围敌人数 == 1
		SDK->UTILITY.GetCountAllyHeroesInRange(MyPosition, 10.f) == 1)       //周围友军数量 == 1
	{
		//printf("代码时间:%d\n", GetTickCount64() - timexx);
		Combo_Solo(LocalPlayer, Enemy);
		return;
	}
	//获取离鼠标最近目标 如果没有就选距离最近目标
	auto ClosestMouseTarget = SDK->WORLD.GetPlayerDataByFID(SDK->GAMEFUNC.GetPlayerfid(SDK->UTILITY.GetNearMouseTarget(99999.f)));
	if (ClosestMouseTarget) {
		auto Temp_EnemyPos = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);
		auto Temp_Closepos = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
		if (Temp_EnemyPos.DistTo(MyPosition) < ChargeRange && Temp_Closepos.DistTo(MyPosition) > ChargeRange) {
			ClosestMouseTarget = Enemy;
		}
	}
	if (!ClosestMouseTarget) {
		ClosestMouseTarget = Enemy;
	}
	bool IsCharge = Get_IsCharge(LocalPlayer);
	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if (((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) && (Time - LocalPlayer->BasicActiontime.LastBladeParryTime > 2.f)) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			LastControl = Control::DirectInput;
		}
		else {
			if (Time - LocalPlayer->BasicActiontime.LastHuWeiJumpAATime < 1.0f) {
				General_HuWeiAttack(LocalPlayer, ClosestMouseTarget, LastControl);
				return;
			}
		}
		if (LastControl == Control::LongFlash) {
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.5f)
			{
				if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IsAllGetUp(LocalPlayer->ActorModel)&& !SDK->ANIMHASH.IS_PUTDOWN(LocalPlayer->ActorModel)) {
					if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0, 0.f)) {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					}
					else {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
						SDK->KEYBOARD.Set_IsActiveLeft(true);
					}
				}
				else {
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
					SDK->KEYBOARD.Set_IsActiveLeft(true);
				}
			}
			else {
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
				SDK->KEYBOARD.Set_IsActiveLeft(true);
			}
		}
		if (LastControl == Control::CCReset) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.BasicLogic.ExecuteCC();
			return;
		}
		if (LastControl == Control::DirectInput) {
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
			SDK->KEYBOARD.Set_IsActiveLeft(true);
			LastControl = Control::LongFlash;
			return;
		}
		if (LastControl == Control::FlashStep) {
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				SDK->KEYBOARD.BasicLogic.ShortFlash('S', false);
			}
			LastControl = Control::CCReset;
			return;
		}
	}
	//获取基础信息
	auto NearPosition = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
	float Distance = NearPosition.DistTo(MyPosition);
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	if (IsCharge) {
		bool HasChiYan = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Saber_ChiYanZhan);
		if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel)) {
			if (Distance <= ChargeRange) {
				if (CheckCanCastCharge(NearPosition, 4.f)) {
					if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
						SDK->KEYBOARD.Set_IsActiveLeft(false);
					}
					bool againexecute = true;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (HasChiYan ? 0.75f : 1.09f);
					while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
						if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
							//自己被振刀就跳出循环
							againexecute = false;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (!againexecute) {
						LastControl = Control::LongFlash;
						return;
					}
					if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						LastControl = Control::LongFlash;
						return;
					}
					else {
						LastControl = Control::DirectInput;
						return;
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && Distance > ChargeRange && SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel) > 0.36f) {
			SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0, 0.f);
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			return;
		}
		//重新蓄力阶段
		if (ChargeTime >= MaxChargeTime) {
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash)
			{
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
			}
		}
	}
	if (!IsCharge && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && SDK->GAMEFUNC.GetGlobalTime() - StartChargeTime > 1.2f) {
		if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
			SDK->KEYBOARD.Set_IsActiveLeft(false);
			return;
		}
		if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
			SDK->KEYBOARD.Set_IsActiveRight(false);
			return;
		}
	}
}