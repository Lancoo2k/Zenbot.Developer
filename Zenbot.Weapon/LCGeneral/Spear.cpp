
#include "Tools.h"

extern LC_SDK_STRUCT* SDK;
void Spear::MainExecuteCode() {

}
void Spear::FreeEvent() {

}
void Spear::LibraryInit() {

}
void Spear::ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	int WeaponID = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 105) {

		this->RunableLogic(LocalPlayer, Enemy);
	}
}
int  Spear::GetNextChargeType(LC_PLAYER_INFO* Enemy, float Distance) {
	//如果追击距离太远就左键去追
	if (Distance > RightAARange + 2.f) {
		return 2;
	}
	if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime()) {
		return 2;
	}
	if (SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel)) {
		return 2;
	}
	return 1;
}
bool Spear::IsInAttackRange(float Distance, float ExtraRange) {
	return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
}
bool Spear::IsFastStatck(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return (Name.find(skCrypt(L"attack_light")) != std::wstring::npos || Name.find(skCrypt(L"attack_heavy")) != std::wstring::npos) && Name.find(skCrypt(L"_03")) != std::wstring::npos && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;
	}
	return false;
}
bool Spear::IsNaoHai(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return Name.find(skCrypt(L"attack_light")) != std::wstring::npos && (Name.find(skCrypt(L"_05")) != std::wstring::npos || Name.find(skCrypt(L"_06")) != std::wstring::npos && Name.find(skCrypt(L"_07")) != std::wstring::npos);
	}
	return false;
}
bool Spear::WillEnd(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return Name.find(skCrypt(L"attack_light")) != std::wstring::npos && Name.find(skCrypt(L"_07")) != std::wstring::npos;

	}
	return false;
}
bool Spear::Get_IsCharge(LC_PLAYER_INFO* Player) {
	return SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
}
bool Spear::AttackShock(LC_PLAYER_INFO* LocalPlayer)
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
bool Spear::ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool LongWangMode, bool ShockAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		//	printf("combo back 0\n");
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (ShockAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer)))
	{
		//如果有精力 那么第一段戳之后转升龙985接b. 不打处决  共0.92f时间
		//如果有夺魂 那么就打完夺魂+跳b
		auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_Spear_DuoHun);
		if (SoulMode && !LongWangMode) {
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.75f)) {
				SDK->KEYBOARD.StopCombo();
				return false;
			}
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::LongFlash;
					return false;
				}
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.4f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
						//printf("落地11\n");
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
						break;
					}
					if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

			} while ((SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && !LocalPlayer->BasicPlayerInfo.IsBounce);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
			SDK->KEYBOARD.Set_IsActiveLeft(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
			SDK->KEYBOARD.Set_IsActiveLeft(false);
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			bool suc = false;
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 4.f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
				if (SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel) == 30) {
					if (WillEnd(LocalPlayer)) {
						//即将结束
						std::this_thread::sleep_for(std::chrono::milliseconds(300));
						suc = true;
						break;
					}
				}
				else {
					suc = false;
					break;
				}
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
				SDK->KEYBOARD.Set_IsActiveLeft(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
				SDK->KEYBOARD.Set_IsActiveLeft(false);
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}
		}
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		return true;
	}
	//printf("faild back 0\n");
	NextInput = Control::LongFlash;
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Spear::AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool attachMode) {
	if (attachMode) {
		if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
			NextInput = Control::DirectInput;
			return false;
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(true);
	}
	if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	char InputKey = NULL;
	auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
	auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
	bool BladeMode = WP && RealWeaponID == 103;
	bool SaberMode = WP && RealWeaponID == 119;
	//过滤掉pre动作
	std::wstring Dont_pre_name;
	float Tick = SDK->GAMEFUNC.GetGlobalTime() + 1.2f;
	bool JumpOut = false;
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
	bool wpMode = (BladeMode || SaberMode);
	if (SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.50f) && wpMode && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
		//此时可以闪避 判断一下当前状态
		SDK->KEYBOARD.SendKeyBoard(InputKey, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		SDK->KEYBOARD.SendKeyBoard(InputKey, 2);
		bool SuccessQQ = false;
		float QQ_Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
		while (QQ_Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			int id = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
			if (id == 103 ||
				id == 119) {
				SuccessQQ = true;

				ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
				break;
			}
		}
		if (!SuccessQQ) {

			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (SuccessQQ) {
			////DbgPrint("输入闪避信息");
			if (SaberMode) {
				if (!SDK->KEYBOARD.IsPressKey('A')) {
					SDK->KEYBOARD.SendKeyBoard('A', 0);
				}
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'S', false, 20, 0.f);
				if (SDK->KEYBOARD.IsPressKey('A')) {
					SDK->KEYBOARD.SendKeyBoard('A', 2);
				}
			}
			else {
				//if (!SDK->KEYBOARD.IsPressKey('W')) {
				//	SDK->KEYBOARD.SendKeyBoard('W', 0);
				//}
				bool cs = SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'A', false, 0, 0.f);
				//if (SDK->KEYBOARD.IsPressKey('W')) {
				//	SDK->KEYBOARD.SendKeyBoard('W', 2);
				//}
			}
		}
		AllowResetChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		return true;
	}
	else {
		//if (LocalPlayer->BasicPlayerInfo.HuoNan_V && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
	//	float realT = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
	//	while (realT > SDK->GAMEFUNC.GetGlobalTime()) {
	//		auto LastHitTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
	//		if (LastHitTime <= 0.1f) {
	//			JumpOut = true;
	//			break;
	//		}
	//		else {
	//			printf("%f\n", LastHitTime);
	//		}
	//	}
	//}
	//if (JumpOut) {
	//	printf("跳出逻辑1\n");
	//	SDK->KEYBOARD.StopCombo();
	//	NextInput = Control::LongFlash;
	//	return true;
	//}
		Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (!SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) || !SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.43f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		//if (!SDK->KEYBOARD.IsPressKey('W')) {
		//	SDK->KEYBOARD.SendKeyBoard('W', 0);
		//}
		//if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, false)) {
		//	if (SDK->KEYBOARD.IsPressKey('W')) {
		//		SDK->KEYBOARD.SendKeyBoard('W', 2);
		//	}
		//	SDK->KEYBOARD.StopCombo();
		//	NextInput = Control::DirectInput;
		//	return false;
		//}
		//if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
		//	if (SDK->KEYBOARD.IsPressKey('W')) {
		//		SDK->KEYBOARD.SendKeyBoard('W', 2);
		//	}
		//	SDK->KEYBOARD.StopCombo();
		//	NextInput = Control::DirectInput;
		//	return false;
		//}
		//Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
		//while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		//	if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
		//		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		//		break;
		//	}
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//}
		if (SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 2);
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.77f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		//判断目标是否在浮空 就打CA
		if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CARange) || !SDK->KEYBOARD.BasicLogic.ExecuteCA()) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.73f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	NextInput = Control::LongFlash;
	return true;
}
bool Spear::Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0))) {
		//跑B的出招速度有所不同
		float RealDelay = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel) ? 0.53f : 0.43f;
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			//如果这里没打中 判断下对面是闪了还是怎样.防止被闪A抓A闪
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
		if (SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y < 0.75f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) && !SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange)) {
				do {
					if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.4f;
					while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
						if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
							//printf("落地11\n");
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							break;
						}
						if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}

				} while ((SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && !LocalPlayer->BasicPlayerInfo.IsBounce);
			}
			else {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.97f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
			}
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
bool Spear::Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0))) {
		float RealDelay = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel) ? 0.75f : 0.57f;
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
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
		if (SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y < 0.85f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) ||  (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::DirectInput;
			return true;
		}
		return AllUse_AfterCombo(LocalPlayer, Enemy, NextInput);
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
void Spear::Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
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
	//TARGET
	if (!EnemyIsDontAttack && LocalPlayer->BasicPlayerInfo.HuoNan_V && (!IsCharge || (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) && !SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel))) && Distance <= JumpAARange && LocalPlayer->BasicPlayerInfo.CanExecuteFlash && HNDontJA < SDK->GAMEFUNC.GetGlobalTime() && !SDK->ANIMHASH.IS_PRE_ATTACK(LocalPlayer->ActorModel)) {
		//是火男状态 而且没有蓄力中
		if (SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(2, false)) {
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.35f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) < 0.1f) {
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', false, 0, 0.f)) {
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			}
		}
	}
	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		int ChargeType = GetNextChargeType(Enemy, Distance);
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if ((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) ||
			(Time - ChangeWeaponTime < 0.2f) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f) ) {
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
		if (LastControl == Control::LongFlash)
		{
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
		int EnemyEndureLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel);
		bool IsDragonBreakMode = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
		bool HasSoul = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Spear_DaShengYou);
		if (!EnemyIsDontAttack) {

			float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

			float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

			float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

			bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
				SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
			bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) 
				|| Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.GuQingHan_Frost || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied || Enemy->BasicPlayerInfo.IsShaNanFHurt || Enemy->BasicPlayerInfo.IsStunned || ChainShotTime < 1.0f || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel);

			bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) /*|| SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)*/ || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel);

			//废弃A3 B3 前摇过大了.
			float LastBladeParryTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastBladeParryTime;

			float LastCrouchTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastCrouchTime;

			bool CanCastCharge = Enemy->BasicPlayerInfo.WuTianF1Tick < SDK->GAMEFUNC.GetGlobalTime();

			bool ShuiPaoMode = false;

			HurtCast hurtExecuteMode = HurtCast::Null;

			auto CurrentHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);

			if (CurrentHurt.z > 0.f && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !Enemy->BasicPlayerInfo.IsBladeParry) {
				//当前玩家僵直 > 0.f 开始计算
				hurtExecuteMode = (CurrentHurt.z + ChargeTime > 0.49f) ? HurtCast::Charge : HurtCast::AA;
			}

			//打龙王破
			if (Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied) {
				float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 0.90f;
			}
			if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
				float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 0.90f;
			}


			if (Distance <= (IsDragonBreakMode ? 7.f : ChargeRange) && IntoCharge && CanCastCharge && LastBladeParryTime > 1.2f) {
				//如果巨大化 / 落地状态 / 抓闪 /必中状态下 进入了蓄力层级就释放.
				if (ShuiPaoMode ? IsDragonBreakMode : (isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || hurtExecuteMode == HurtCast::Charge)) {
					bool useLeft = false;
					if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
						SDK->KEYBOARD.Set_IsActiveLeft(false);
						useLeft = true;
					}
					if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
					}
					bool againexecute = true;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsDragonBreakMode ? 0.85f : (!useLeft ? (HasSoul ? 1.80f : 0.95f) : 1.00f));
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
					LastControl = (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && !IsDragonBreakMode) ? Control::LongFlash : Control::DirectInput;
					if (IsDragonBreakMode) {
						HNDontJA = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
					}
					return;
				}
			}
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
					bool Execute1 = Enemy->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IS_IDLE(Enemy->ActorModel) ||
						(hurtlandFlag && !SDK->KEYBOARD.IsPressKey(1)) || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) ||
						SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || (hurtExecuteMode == HurtCast::AA && (!hurtlandFlag || SDK->KEYBOARD.IsPressKey(2))) || ParryMode || LastCrouchTime < 0.1f || SDK->ANIMHASH.IS_WUCHEN_teleported(Enemy->ActorModel) || AirFlag || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel);
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
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f);
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
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.97f);
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
			if ((x1flag && ChargeTime > 0.24f) || x2flag) {
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
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
void Spear::RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	//拼刀事件
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
		ExecuteShock(LocalPlayer, Enemy, LastControl, false, false);
		NextNaoHai = SDK->GAMEFUNC.GetGlobalTime() + 1.f;
		return;
	}
	if (IsNaoHai(LocalPlayer) && SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel) == 30 && SDK->GAMEFUNC.GetGlobalTime() - StartChargeTime < 1.f && SDK->GAMEFUNC.GetGlobalTime() > NextNaoHai) {
		//清理一切按键
		SDK->KEYBOARD.StopCombo();
		ExecuteShock(LocalPlayer, Enemy, LastControl, true, true);
		NextNaoHai = SDK->GAMEFUNC.GetGlobalTime() + 1.f;
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
	//进入蓄力阶段
	bool IsCharge = Get_IsCharge(LocalPlayer);
	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if ((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) ||
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
				if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IsAllGetUp(LocalPlayer->ActorModel)&& !SDK->ANIMHASH.IS_PUTDOWN(LocalPlayer->ActorModel) ) {
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();

					if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', false, 0, 0.f)) {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					}
					else {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
						SDK->KEYBOARD.Set_IsActiveRight(true);
					}
				}
				else {
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
				}
			}
			else {
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
				SDK->KEYBOARD.Set_IsActiveRight(true);
			}
		}
		if (LastControl == Control::CCReset) {
			LastControl = Control::DirectInput;
			SDK->KEYBOARD.BasicLogic.ExecuteCC();
			return;
		}
		if (LastControl == Control::DirectInput) {
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
			SDK->KEYBOARD.Set_IsActiveRight(true);
			LastControl = Control::LongFlash;
			return;
		}
	}
	//获取基础信息
	auto NearPosition = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
	float Distance = NearPosition.DistTo(MyPosition);
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	if (IsCharge) {
		bool HasSoul = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Spear_DaShengYou);
		bool IsDragonBreakMode = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
		bool LWPMode = SDK->KEYBOARD.ActiveCombo(ActiveMode::SoulMode);
		if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel)) {
			if (Distance <= (IsDragonBreakMode ? 7.f : ChargeRange)) {
				if (CheckCanCastCharge(NearPosition, 4.f)) {
					if (!LWPMode || IsDragonBreakMode) {
						bool useLeft = false;
						if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
							SDK->KEYBOARD.Set_IsActiveLeft(false);
							useLeft = true;
						}
						if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
							SDK->KEYBOARD.Set_IsActiveRight(false);
						}
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsDragonBreakMode ? 0.85f : (!useLeft ? (HasSoul ? 1.80f : 0.95f) : 1.00f));
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
						LastControl = (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && !IsDragonBreakMode) ? Control::LongFlash : Control::DirectInput;
						return;
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && Distance > ChargeRange && SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel) > 0.36f) {
			SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', false, 0, 0.f);
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			return;
		}
		//重新蓄力阶段
		if (ChargeTime >= MaxChargeTime) {
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash)
			{
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', false, 0, 0.f);
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