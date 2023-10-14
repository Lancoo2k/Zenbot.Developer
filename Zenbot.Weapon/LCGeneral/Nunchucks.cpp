
#include "Tools.h"

extern LC_SDK_STRUCT* SDK;
void Nunchucks::MainExecuteCode() {

}
void Nunchucks::FreeEvent() {

}
void Nunchucks::LibraryInit() {

}
void Nunchucks::ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	int WeaponID = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 116) {

		this->RunableLogic(LocalPlayer, Enemy);
	}
}

int Nunchucks::GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack, float Distance) {
	if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime())
		return 2;
	if (ForceUseLeft > SDK->GAMEFUNC.GetGlobalTime())
		return 1;
	if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel) || Distance > LeftAARange) {
		return 2;
	}
	return 1;
}
bool Nunchucks::IsSoulParry(ULONG64 Obj) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Obj, Name)) {
		return Name.find(skCrypt(L"_nunchucks_parry_0")) != std::wstring::npos;
	}
	return false;
}
int Nunchucks::GetfewPlayerChargeType(LC_PLAYER_INFO* Enemy, float Distance) {
	auto MyPointer = SDK->GAMEFUNC.GetLocalPlayer();
	int drg = SDK->GAMEFUNC.GetDragonBlockStack(MyPointer);
	if (drg > 2) {
		//龙虎功>=2
		return 2;
	}
	if (Distance > ChargeRange) {
		return 2;
	}
	return 1;
}
bool Nunchucks::IsInAttackRange(float Distance, float ExtraRange) {
	return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
}
bool Nunchucks::IsFastStatck(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return (Name.find(skCrypt(L"_attack_light")) != std::wstring::npos || Name.find(skCrypt(L"_attack_heavy")) != std::wstring::npos) && Name.find(skCrypt(L"_03")) != std::wstring::npos && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;
	}
	return false;
}
bool Nunchucks::Get_IsCharge(LC_PLAYER_INFO* Player) {
	return  SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
}
bool Nunchucks::AttackShock(LC_PLAYER_INFO* LocalPlayer)
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
bool Nunchucks::ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer))
	{
		auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Nunchuck_DuoHun);
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, SoulMode ? 0.55f : 1.07f)) {
			SDK->KEYBOARD.StopCombo();
			return false;
		}
		if (!SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 0);
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, false)) {
			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.36f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.21f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 2);
		}
		bool success_get_recover = false;
		std::wstring TempName;
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, TempName);
			if (TempName.find(skCrypt(L"heavy_01_recover")) != std::wstring::npos) {
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
bool Nunchucks::AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput,bool attachMode) {
	if (attachMode) {
		if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
			NextInput = Control::DirectInput;
			return false;
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(true);
	}
	//这里直接转为升龙 如果ACCA僵直在0.77~1.00时 正好可以接一个
	do {

		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
			//等待0.60s后 左键打出去
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 450)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
					std::this_thread::sleep_for(std::chrono::milliseconds(45));
					break;
				}
				if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.37f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//0.87是原地捏蓝 0.58是重复CC
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.58f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}

	} while (SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y >= 0.70f && SDK->KEYBOARD.ActiveCombo(ActiveMode::All));
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	NextInput = Control::DirectInput;
	return true;
}
bool Nunchucks::Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0))) {
		//跑B的出招速度有所不同
		bool IsRunningB = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel);
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.37f);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			//如果这里没打中 判断下对面是闪了还是怎样.防止被闪A抓A闪 闪b偷刀是0.33f
			if (!Logic2) {
				if ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) ||
					(SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) {
					//对面闪了 就原地S闪，如果没精力了就关闭2A选项并原地捏蓝
					SDK->KEYBOARD.StopCombo();
					float Dist = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));

					if (Dist <= 5.f || !LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						//距离小于5米就原地捏蓝
						Disable2A = SDK->GAMEFUNC.GetGlobalTime() + 0.80f;
						NextInput = Control::DirectInput;
						return false;
					}
					else {
						if (!TargetIsFlee(LocalPlayer, Enemy)) {
							ForceUseSLongShift = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
						}
						NextInput = Control::LongFlash;
						return false;
					}
				}
			}
			SDK->KEYBOARD.StopCombo();
			ForceUseWLongShiftKeep = SDK->GAMEFUNC.GetGlobalTime() + 0.3f;
			NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20 ? Control::UseSkill : Control::LongFlash;
			return false;
		}
		char InputKey = NULL;
		bool CanExecuteDoubleWeapon = false;
		auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
		auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
		bool KatanaMode = WP && RealWeaponID == 102;
		bool SwordMode = WP && RealWeaponID == 101;
		if (KatanaMode || SwordMode) {
			//双刀为连招启动器 不使用品质检查

			//DbgPrint("存在太刀.直接进行C切");
			auto isbool = SDK->KEYBOARD.BasicLogic.CrouchChangeWeapon(InputKey);
			if (!isbool) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				int id = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
				if ((KatanaMode && id == 102) || (SwordMode && id == 101)) {
					SDK->CORE.LockHuWeiJumpTime(1.5f);
					ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					break;
				}
			}
			SDK->KEYBOARD.StopCombo();
			return true;
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.58f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.StopCombo();
		return true;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Nunchucks::Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0))) {
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.42f);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			if (!Logic2) {
				if ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) ||
					(SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) {
					//对面闪了 就原地S闪，如果没精力了就关闭2A选项并原地捏蓝
					SDK->KEYBOARD.StopCombo();
					float Dist = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));

					if (Dist <= 5.f || !LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						//距离小于5米就原地捏蓝
						Disable2A = SDK->GAMEFUNC.GetGlobalTime() + 0.80f;
						NextInput = Control::DirectInput;
						return false;
					}
					else {
						if (!TargetIsFlee(LocalPlayer, Enemy)) {
							ForceUseSLongShift = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
						}
						NextInput = Control::LongFlash;
						return false;
					}
				}
			}
			SDK->KEYBOARD.StopCombo();
			ForceUseWLongShiftKeep = SDK->GAMEFUNC.GetGlobalTime() + 0.3f;
			NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20 ? Control::UseSkill : Control::LongFlash;
			return false;
		}
		//这里判断敌方僵直情况
		if (SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y < 0.76f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//连招被躲 或 对方开出技能 此时原地捏蓝返回.
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.58f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//这里直接2B结束连招
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::LongFlash;
			return true;
		}
		return AllUse_AfterCombo(LocalPlayer, Enemy, NextInput);
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
void Nunchucks::Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {

	//printf("COMBO _SOLO INJECT %d\n",SDK->KEYBOARD.Get_IsActiveLeft());
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
	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		int ChargeType = GetNextChargeType(Enemy, EnemyIsDontAttack, Distance);
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if (((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) && (BreakTime < Time)) ||
			(Time - ChangeWeaponTime < 0.2f) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			//printf("进入受击判断\n");
			//这里是 进入连招之前 自身受到僵直时间 < 0.1f.  刚从被打到可以捏蓝.
			//printf("GOGOGOG111dasdasdasdasd\n");
			LastControl = Control::DirectInput;
			ChargeType = 1;
		}
		else {
			if ((Time - LocalPlayer->BasicActiontime.LastHuWeiJumpAATime < 1.0f)) {
				//直接转升龙
				if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					AllUse_AfterCombo(LocalPlayer, Enemy, LastControl, true);
				}
				return;
			}
		}
		std::wstring bb;
		SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, bb);
		//printf("%f %ls 捏蓝ID:%d\n",SDK->GAMEFUNC.GetGlobalTime(),bb.c_str(),ChargeType);
		if (LastControl == Control::LongFlash)
		{
			//printf("长闪进入\n");

			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.5f)
			{
				if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IsAllGetUp(LocalPlayer->ActorModel)&& !SDK->ANIMHASH.IS_PUTDOWN(LocalPlayer->ActorModel)) {
					if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(ChargeType, ForceUseSLongShift > SDK->GAMEFUNC.GetGlobalTime() ? 'S' : 'W', false, 0, ForceUseWLongShiftKeep > 0.f ? 0.15f : 0.0f)) {
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
				//printf("捏蓝开始\n");
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
	}
	bool IntoCharge = SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel);
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	int EnemyRealHP = GetRealHealth(Enemy);
	int LocalRealHP = GetRealHealth(LocalPlayer);
	int StackValue = SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(LocalPlayer->ActorModel);
	BOOL CanToAttack = SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel) && StackValue != 3 && ChargeTime < 0.49f;
	if (IsCharge) {
		//printf("IS CHARGE\n");
		bool HasFeiTi = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Nunchuck_DuoHun);
		bool HasYangBian = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Nunchuck_YangBianJin);
		int EnemyEndureLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel);
		bool ActiveLongHu = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
		int DragonPause = SDK->GAMEFUNC.GetDragonBlockStack(LocalPlayer->ActorModel);
		bool HasHuXiao = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Nunchuck_HuXiaoLongPao);

		if (!EnemyIsDontAttack) {
			bool A3Mode = StackValue == 3 && !SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel);

			float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

			float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

			float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

			bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
				SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
			bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f || Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.GuQingHan_Frost || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied || Enemy->BasicPlayerInfo.IsShaNanFHurt || Enemy->BasicPlayerInfo.IsStunned || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel);

			bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) /*|| SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)*/;

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


			//2.0
			if (Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied) {
				float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 1.10f;
			}
			if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
				float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 1.10f;
			}

			if (Distance <= (SDK->KEYBOARD.Get_IsActiveLeft() ? 3.f : ChargeRange) && CanCastCharge && LastBladeParryTime > 1.2f) {
				bool CanNext = false;
				if (A3Mode) {
					CanNext = (hurtlandFlag && SDK->KEYBOARD.IsPressKey(2)) || isHitMode || (FlashExecuteTime < 0.1f && SDK->KEYBOARD.IsPressKey(1));
				}
				else if (IntoCharge) {
					CanNext = isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || hurtExecuteMode == HurtCast::Charge;
				}
				//如果巨大化 / 落地状态 / 抓闪 /必中状态下 进入了蓄力层级就释放. 或者在落地状态且不是左键蓄力而且进入A3状态时释放右键.
				if (ShuiPaoMode && DragonPause > 0 && SDK->KEYBOARD.Get_IsActiveRight() ? ActiveLongHu : CanNext) {
					bool IsLeftA3 = false;
					if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
						SDK->KEYBOARD.Set_IsActiveLeft(false);
						IsLeftA3 = true;
					}
					if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
					}
					if (ActiveLongHu && DragonPause > 0) {
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.3f;
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
						std::wstring localName;
						AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 6.f;
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
							int Level = SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel);
							SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, localName);
							if (Level == 30 || Level == 20) {

								if (DragonPause == 1) {
									//1层时
									if (localName.find(skCrypt(L"hold_heavy_02")) != std::wstring::npos) {
										//0.62f //0.73f
										if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) > (HasHuXiao ? 0.73f : 0.65f)) {
											break;
										}
									}
								}
								if (DragonPause == 2) {
									//1层时
									if (localName.find(skCrypt(L"hold_heavy_04")) != std::wstring::npos) {
										//0.62f //0.73f
										if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) > (HasHuXiao ? 0.50f : 0.40f)) {
											break;
										}
									}
								}
								if (DragonPause >= 3) {
									//1层时
									if (localName.find(skCrypt(L"hold_heavy_06")) != std::wstring::npos) {
										//0.62f //0.73f
										if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) > (HasHuXiao ? 0.82f : 0.90f)) {
											break;
										}
									}
								}
								SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, localName);
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
								SDK->KEYBOARD.Set_IsActiveRight(true);
								std::this_thread::sleep_for(std::chrono::milliseconds(25));
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
								SDK->KEYBOARD.Set_IsActiveRight(false);
							}
							else {
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
						}
					}
					else
					{
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (A3Mode ? (IsLeftA3 ? (HasFeiTi ? 0.77f : 1.21f) : 1.21f) : (IsLeftA3 ? (HasYangBian ? 0.42f : 1.08f) : 0.61f));
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
					}

					if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						LastControl = Control::LongFlash;
						return;
					}
					else {
						LastControl = Control::CCReset;
						return;
					}
				}

			}
			//printf("开始执行444 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
			//快捷振刀0.7f 蓄转振 0.63f 
			bool ParryMode = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastParryTime <= 0.63f;

			if (DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime() && !Enemy->BasicPlayerInfo.YinZiPing_F2 && !Enemy->BasicPlayerInfo.JiYingYing_Ghost) {

				bool AirFlag = ChainShotTime < 1.0f || SDK->ANIMHASH.PLAYER_INAIR(Enemy->ActorModel) || (SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel) && (SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) != 20));

				if (AirFlag) {
					float yyy = EnemyPosition.y - MyPosition.y;
					if (Distance <= 3.f && yyy > 0 && yyy < 6.f) {
						//转升龙抓
						if (SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
							std::wstring nnn;
							float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
							while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
								if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
									break;
								}
								if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
									break;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(1));
							}
							LastControl = Control::DirectInput;
							return;
						}
					}
				}
				if (CanToAttack) {
					if (TryStealAA && StackValue == 1) {

						bool CanTwoExecuteFlash = LocalPlayer->BasicPlayerInfo.Property.CurEnergy - (LocalPlayer->BasicPlayerInfo.Property.ConsumEnergy * 2.f) > -200;

						if (CanTwoExecuteFlash && IsInAttackRange(Distance) && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20) {

							float time = SDK->GAMEFUNC.GetChargeTime(Enemy->ActorModel);

							float RealDelay = SDK->KEYBOARD.IsPressKey(1) ? (SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) ? 0.42f : 0.38f) : (SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) ? 0.36f : 0.33f);

							float StealOffset = 0.51f - RealDelay;

							if (time >= StealOffset && time <= (StealOffset + 0.02f)) { //0.33f 

								if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {

									if (StackValue == 1) {

										//处于一段攻击时
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
										SDK->KEYBOARD.Set_IsActiveRight(false);
										std::this_thread::sleep_for(std::chrono::milliseconds(50));
										SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
										ForceUseWLongShiftKeep = SDK->GAMEFUNC.GetGlobalTime() + 0.3f;
										LastControl = ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) || (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) ? Control::DirectInput : Control::LongFlash;
										return;
									}
								}
								if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {

									if (StackValue == 1) {

										//处于一段攻击时
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
										SDK->KEYBOARD.Set_IsActiveLeft(false);
										std::this_thread::sleep_for(std::chrono::milliseconds(50));
										SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
										ForceUseWLongShiftKeep = SDK->GAMEFUNC.GetGlobalTime() + 0.3f;
										LastControl = ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) || (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) ? Control::DirectInput : Control::LongFlash;
										return;
									}
								}
							}
						}
					}
					bool sdMode = (SDK->GAMEFUNC.GetGlobalTime() - ChangeWeaponTime < 1.0f) && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);

					bool Execute1 = Enemy->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IS_IDLE(Enemy->ActorModel) ||
						(hurtlandFlag && !SDK->KEYBOARD.IsPressKey(1)) || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) ||
						SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || (hurtExecuteMode == HurtCast::AA && (!hurtlandFlag || SDK->KEYBOARD.IsPressKey(2))) || sdMode || ParryMode || LastCrouchTime < 0.1f || SDK->ANIMHASH.IS_WUCHEN_teleported(Enemy->ActorModel) || AirFlag || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel);
					bool Execute2 = (FlashExecuteTime <= 0.1f && (!SDK->KEYBOARD.IsPressKey(1) || Enemy->BasicActiontime.LastFlashType != EVADE_TYPE::EVADE_Double) && (LocalPlayer->BasicActiontime.LastFlashTime < Enemy->BasicActiontime.LastFlashTime)) || (SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) != 20);
					bool Execute3 = AttackBlade(Enemy, false);

					if (IsInAttackRange(Distance)) {

						if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {

							if (Execute1 || Execute2 || Execute3) {
								if (StackValue == 1) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									//这里延迟一会 怕动作未完全展开
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									if (!Left_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer, LastControl);
									}
									return;
								}
							}
							if (Execute1 || Execute3) {
								if (StackValue == 2 && Disable2A < SDK->GAMEFUNC.GetGlobalTime()) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.87f);
									LastControl = Control::DirectInput;
									return;
								}
							}

						}
						if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
							if (Execute1 || Execute2 || Execute3) {
								if (StackValue == 1) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//这里延迟一会 怕动作未完全展开
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									if (!Right_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer, LastControl);
									}
									return;
								}
							}
							if (Execute1 || Execute3) {
								if (StackValue == 2 && Disable2A < SDK->GAMEFUNC.GetGlobalTime()) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//这里延迟一会 怕动作未完全展开
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.58f);
									LastControl = Control::DirectInput;
									return;
								}
							}
						}
					}
				}
			}
			if (!CanToAttack && LocalPlayer->BasicPlayerInfo.CanExecuteFlash && ParryMode) {


				//超出放蓄时间时. 例如对方在2.0时出振刀 脚本会来不及读取ParryBcakSwingTime. 所以判断超出时间后转中闪抓!

				float TransitionTime = Enemy->BasicHurtInfo.ParryBackSwingTransitionTime - SDK->GAMEFUNC.GetGlobalTime();

				if (MaxChargeTime < ChargeTime + TransitionTime) {

					if (Distance <= LeftAARange + 3.f) {

						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0, 0.f)) {
							//中闪
							Distance = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));
							if (Distance <= LeftAARange) {

								Left_ComboRun(LocalPlayer, Enemy, LastControl, false);
								return;
							}
						}
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
			//如果目标不在攻击范围内时
			bool x1flag = !IsInAttackRange(Distance);
			bool x2flag = EnableForceFlashWithFlashStep && SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime <= 0.2f;
			if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
				if (CanToAttack && StackValue == 1 && SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) && !IsInAttackRange(Distance, 3.5f)) {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					//这里延迟一会 怕动作未完全展开
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.35f);
					SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, EnemyIsDontAttack, Distance), 'W', false, 0, 0.f);
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					return;
				}
			}
			if ((x1flag && ChargeTime > 0.24f) || x2flag) {
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				return;
			}
		}
		if (ChargeTime >= (EnemyIsDontAttack ? 0.7f : MaxChargeTime)) {
			if (EnemyIsDontAttack && IsInAttackRange(Distance)) {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
				return;
			}
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash)
			{
				//printf("RESET FLASH\n");
				auto boolean = SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, EnemyIsDontAttack, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				//printf("GOB FLASH\n");
				return;
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
			}
		}
	}

	if (!IsCharge && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && SDK->GAMEFUNC.GetGlobalTime() - StartChargeTime > 1.2f) {
		//printf("放弃捏蓝\n");
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
void Nunchucks::RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	//拼刀事件
	if (IsSoulParry(LocalPlayer->ActorModel) && SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel) == 30 && BreakTime < SDK->GAMEFUNC.GetGlobalTime()) {
		//printf("检测到横栏\n");
		//检测横栏
		SDK->KEYBOARD.StopCombo();
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 1.35f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) >= 0.75f) {
				LastControl = (LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::LongFlash : Control::CCReset);
				BreakTime = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
				//printf("LastControl:%d\n", LastControl);
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	if (LocalPlayer->BasicPlayerInfo.IsBounce) {
		//printf("111\n");
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
	auto NearPosition = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
	float Distance = NearPosition.DistTo(MyPosition);
	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		int ChargeType = GetfewPlayerChargeType(ClosestMouseTarget, Distance);
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if (((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) && (BreakTime < Time)) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			//printf("进入受击判断\n");
			//这里是 进入连招之前 自身受到僵直时间 < 0.1f.  刚从被打到可以捏蓝.
			LastControl = Control::DirectInput;
		}
		else {
			if (Time - LocalPlayer->BasicActiontime.LastHuWeiJumpAATime < 1.0f) {
				General_HuWeiAttack(LocalPlayer, ClosestMouseTarget, LastControl);
				return;
			}
		}
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
	}
	//获取基础信息
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	if (IsCharge) {
		int ChargeLevel = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel);
		int DragonPause = SDK->GAMEFUNC.GetDragonBlockStack(LocalPlayer->ActorModel);
		bool ForceSoulMode = SDK->KEYBOARD.ActiveCombo(ActiveMode::SoulMode) && DragonPause > 0;
		bool HasHuXiao = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Nunchuck_HuXiaoLongPao);
		bool HasYangBian = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Nunchuck_YangBianJin);
		if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel)) {
			if (Distance <= ChargeRange) {
				if (CheckCanCastCharge(NearPosition, 4.f)){
					if (!ForceSoulMode || ChargeLevel == 2) {
						bool IsLeftA3 = false;
						if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
							SDK->KEYBOARD.Set_IsActiveLeft(false);
							IsLeftA3 = true;
						}
						if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
							SDK->KEYBOARD.Set_IsActiveRight(false);
						}
						if (ChargeLevel == 2 && DragonPause > 0) {
							bool againexecute = true;
							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.3f;
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
							std::wstring localName;
							AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 6.f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
								int Level = SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel);
								SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, localName);
								if (Level == 30 || Level == 20) {

									if (DragonPause == 1) {
										//1层时
										if (localName.find(skCrypt(L"hold_heavy_02")) != std::wstring::npos) {
											//0.62f //0.73f
											if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) > (HasHuXiao ? 0.73f : 0.65f)) {
												break;
											}
										}
									}
									if (DragonPause == 2) {
										//1层时
										if (localName.find(skCrypt(L"hold_heavy_04")) != std::wstring::npos) {
											//0.62f //0.73f
											if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) > (HasHuXiao ? 0.50f : 0.40f)) {
												break;
											}
										}
									}
									if (DragonPause >= 3) {
										//1层时
										if (localName.find(skCrypt(L"hold_heavy_06")) != std::wstring::npos) {
											//0.62f //0.73f
											if (SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel) > (HasHuXiao ? 0.82f : 0.90f)) {
												break;
											}
										}
									}
									SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, localName);
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
									SDK->KEYBOARD.Set_IsActiveRight(true);
									std::this_thread::sleep_for(std::chrono::milliseconds(25));
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
								}
								else {
									break;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
							}
						}
						else {
							bool againexecute = true;
							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsLeftA3 ? (HasYangBian ? 0.42f : 1.08f) : 0.61f);
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
						}
						if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
							LastControl = Control::LongFlash;
							return;
						}
						else {
							LastControl = Control::CCReset;
							return;
						}
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && Distance > ChargeRange && SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel) > 0.36f) {
			SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetfewPlayerChargeType(ClosestMouseTarget, Distance), 'W', false, 0, 0.f);
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			return;
		}
		//重新蓄力阶段
		if (ChargeTime >= MaxChargeTime) {
			//printf("RESET\n");
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash)
			{
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetfewPlayerChargeType(ClosestMouseTarget, Distance), 'W', false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				//printf("beo1\n");
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
				//printf("CC\n");
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