
#include "Tools.h"

extern LC_SDK_STRUCT* SDK;

void Sword::MainExecuteCode() {

}
void Sword::FreeEvent() {

}
void Sword::LibraryInit() {

}
void Sword::ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	int WeaponID = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 101) {

		this->RunableLogic(LocalPlayer, Enemy);
	}
}
int  Sword::GetNextChargeType(LC_PLAYER_INFO* Enemy, float Distance) {
	//如果追击距离太远就左键去追
	if (Distance > RightAARange + 2.f) {
		return 1;
	}
	if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime()) {
		return 2;
	}
	if (SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(SDK->GAMEFUNC.GetLocalPlayer()) == 1) {
		return 2;
	}
	auto WepId = SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel);
	//二段捏用左A | 除了太刀和匕首 有30%概率用左键蓄力去试嫖刀.
	if (((WepId != 102 && WepId != 106) && TryStealAA && SDK->UTILITY.GetRandomIntEx(1, 10) <= 3) && Distance <= LeftAARange) {
		return 1;
	}
	return 2;
}
bool Sword::IsInAttackRange(float Distance, float ExtraRange) {
	return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
}
bool Sword::IsFastStatck(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return (Name.find(skCrypt(L"_attack_light")) != std::wstring::npos || Name.find(skCrypt(L"_attack_heavy")) != std::wstring::npos) && (Name.find(skCrypt(L"_03")) != std::wstring::npos || Name.find(skCrypt(L"_05")) != std::wstring::npos || Name.find(skCrypt(L"_09")) != std::wstring::npos) && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;
	}
	return false;
}
bool Sword::Get_IsCharge(LC_PLAYER_INFO* Player) {
	return  SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
}
bool Sword::AttackShock(LC_PLAYER_INFO* LocalPlayer)
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
			if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) 
			{
				return false;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	return false;
}
bool Sword::ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer))
	{
		auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Sword_DuoHun);
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, SoulMode ? 0.75f : 0.95f)) {
			SDK->KEYBOARD.StopCombo();
			return false;
		}
		if (SoulMode) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::LongFlash;
				return false;
			}
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.64f;
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
		}
		else {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CARange) || !SDK->KEYBOARD.BasicLogic.ExecuteCA()) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		return true;
	}
	NextInput = Control::LongFlash;
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Sword::AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	//这里直接转为升龙 如果ACCA僵直在0.77~1.00时 正好可以接一个
	if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::DirectInput;
		return false;
	}
	//拿到可双刀武器
	char InputKey = NULL;
	auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
	auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
	bool BladeMode = WP && RealWeaponID == 103;
	bool TwinMode = WP && RealWeaponID == 118;
	bool DualMode = WP && RealWeaponID == 121 && CheckQuality(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel), WP);
	bool SaberMode = WP && RealWeaponID == 119;
	bool KatanaMode = WP && RealWeaponID == 102 && CheckQuality(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel),WP);
	bool ActiveShuangDaoLiu = BladeMode || TwinMode || KatanaMode || SaberMode || DualMode;
	//过滤掉pre动作
	std::wstring Dont_pre_name;
	float Tick = SDK->GAMEFUNC.GetGlobalTime() + 1.0f;
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
	/*bool JumpOut
	if (LocalPlayer->BasicPlayerInfo.HuoNan_V && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
		float realT = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
		while (realT > SDK->GAMEFUNC.GetGlobalTime()) {
			auto LastHitTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
			if (LastHitTime <= 0.1f) {
				JumpOut = true;
				break;
			}
			else {
				printf("%f\n", LastHitTime);
			}
		}
	}
	if (JumpOut) {
		printf("跳出逻辑1\n");
		SDK->KEYBOARD.StopCombo();
		NextInput = Control::LongFlash;
		return true;
	}*/
	if (SDK->KEYBOARD.ActiveCombo(ActiveMode::ChainShootMode) && SDK->GAMEFUNC.GetItemCount(ItemId::ChainShoot) > 0) {
		SDK->KEYBOARD.SendKeyBoard('Q', 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		SDK->KEYBOARD.SendKeyBoard('Q', 2);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (SaberMode) {
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
			SDK->KEYBOARD.Set_IsActiveLeft(true);
		}
		else {
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
			SDK->KEYBOARD.Set_IsActiveRight(true);
		}
		if (ActiveShuangDaoLiu) {
			std::this_thread::sleep_for(std::chrono::milliseconds(600));
			SDK->KEYBOARD.SendKeyBoard(InputKey, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			SDK->KEYBOARD.SendKeyBoard(InputKey, 2);
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				int id = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
				if (id == 103 || id == 118 || id == 102 || id == 121 || id == 119) {
					//printf("success\n");
					SDK->CORE.LockHuWeiJumpTime(1.5f);
					ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					DisableWeaponWhiteMode = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					AllowResetChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					break; 
				}
			}
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			return true;
		}
		StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
		float MaxTick = SDK->GAMEFUNC.GetGlobalTime() + 5.f;
		bool successflag = false;
		while (SDK->GAMEFUNC.GetGlobalTime() < MaxTick) {
			if (Get_IsCharge(LocalPlayer)) {
				DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 0.7f;
				SDK->CORE.LockHuWeiJumpTime(1.5f);
				SDK->KEYBOARD.Set_IsExecuteCombo(false);
				return true;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
		SDK->KEYBOARD.Set_IsActiveRight(false);
		SDK->KEYBOARD.StopCombo();
		return false;
	}
	else {
		if (SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Sword_FuTian)) {
			//如果能闪 使用拳切技巧 提前0.2s预输入切拳
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.6f)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			int localweapon = SDK->GAMEFUNC.GetWeaponIndex() == 1 ? '1' : '2';
			//预输入一个切拳
			SDK->KEYBOARD.SendKeyBoard('0', 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			SDK->KEYBOARD.SendKeyBoard('0', 2);
			bool SuccessQQ = false;
			float QQ_Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.6f;
			while (QQ_Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel) == 3209901) {
					SuccessQQ = true;
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (!SuccessQQ) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//执行一次闪避 并按住W 防止B1空了
			SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 2);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			if (ActiveShuangDaoLiu) {
				SDK->KEYBOARD.SendKeyBoard(InputKey, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				SDK->KEYBOARD.SendKeyBoard(InputKey, 2);
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
				SDK->KEYBOARD.Set_IsActiveRight(true);
				ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
				DisableWeaponWhiteMode = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
				AllowResetChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
				SDK->KEYBOARD.Set_IsExecuteCombo(false);
				return true;
			}
			SDK->KEYBOARD.SendKeyBoard(localweapon, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			SDK->KEYBOARD.SendKeyBoard(localweapon, 2);
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
			SDK->KEYBOARD.Set_IsActiveRight(true);
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			float MaxChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
			while (MaxChargeTime > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel)) {
					//printf("DONT WHITE!\n");
					DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 1.f;
					SDK->CORE.LockHuWeiJumpTime(1.5f);
					SDK->KEYBOARD.Set_IsExecuteCombo(false);
					return true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
		else
		{
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//按下W键
			if (!SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 0);
			}
			//等待0.60s后 左键打出去
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 450)) {
				if (SDK->KEYBOARD.IsPressKey('W')) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
				}
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
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, false)) {
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
				Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
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

			} while ((SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && !LocalPlayer->BasicPlayerInfo.IsBounce);

			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::DirectInput;
			return true;
		}
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	NextInput = Control::DirectInput;
	return true;
}
bool Sword::Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	//printf("JOIN 1\n");
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0))) {
		//跑B的出招速度有所不同
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			//printf("ERROR1\n");
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
		char InputKey = NULL;
		bool CanExecuteDoubleWeapon = false;
		auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
		auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
		bool KatanaMode = WP && RealWeaponID == 102;
		auto Ack = SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel);
		bool sdMode = KatanaMode && (!SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Sword_FuTian) || CheckQuality(Ack, WP));
		if ((SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y < 0.75f && !sdMode) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
					//printf("FAILED\n");
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.6f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
						break;
					}
					if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

			} while ((SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && !LocalPlayer->BasicPlayerInfo.IsBounce);

			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::DirectInput;
			return true;
		}
		if (sdMode) {
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
				if ((KatanaMode && id == 102)) {
					SDK->CORE.LockHuWeiJumpTime(1.5f);
					ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					break;
				}
			}
			SDK->KEYBOARD.StopCombo();
			return true;
		}
		auto MaxHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y;
		if (!(MaxHurt >= 0.74f && MaxHurt <= 0.76f)) {
			//中闪过去抓
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				if (!SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'S', false, 0, 0.f)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				//正常应该右键是按下的 直接松开
				if (SDK->KEYBOARD.Get_IsActiveRight()) {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
				else {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
			}
			else {
				//没精力用CC;
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				//直接左A.如果没打中或者僵直不够则直接返回假.继续写升龙就好了
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
			}
		}
		return AllUse_AfterCombo(LocalPlayer, Enemy, NextInput);
	}
	//printf("JOIN 2\n");
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Sword::Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0))) {
		float RealDelay = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel) ? 0.34f : 0.36f;
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
		char InputKey = NULL;
		bool CanExecuteDoubleWeapon = false;
		auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
		auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
		bool KatanaMode = WP && RealWeaponID == 102;
		bool TwinMode = WP && RealWeaponID == 118;
		bool DualMode = WP && RealWeaponID == 121;
		auto Ack = SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel);
		bool sdMode = (KatanaMode && (!SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Sword_FuTian) || CheckQuality(Ack, WP))) || DualMode || TwinMode;
		//这里判断敌方僵直情况
		if ((SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y < 0.63f && !sdMode) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.6f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
						break;
					}
					if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

			} while ((SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && !LocalPlayer->BasicPlayerInfo.IsBounce);
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::DirectInput;
			return true;
		}
		if (sdMode) {
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
				if ((KatanaMode && id == 102) || (DualMode && id == 121) || (TwinMode && id == 118)) {
					SDK->CORE.LockHuWeiJumpTime(1.5f);
					ChangeWeaponTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
					break;
				}
			}
			SDK->KEYBOARD.StopCombo();
			return true;
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && (GetRealHealth(Enemy) >= 380 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) && SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y >= 0.61f) {
			//如果可以闪避就ASB连招
			if (!SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'S', false, 0, 0.f)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//正常应该右键是按下的 直接松开
			if (SDK->KEYBOARD.Get_IsActiveRight()) {
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
				SDK->KEYBOARD.Set_IsActiveRight(false);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			else {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			return AllUse_AfterCombo(LocalPlayer, Enemy, NextInput);
		}
		else {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.75f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::DirectInput;
			return true;
		}
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
void Sword::Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
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
		int ChargeType = GetNextChargeType(Enemy, Distance);
		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if ((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) ||
			(Time - ChangeWeaponTime < 0.2f) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			//printf("进入受击判断\n");
			//这里是 进入连招之前 自身受到僵直时间 < 0.1f.  刚从被打到可以捏蓝.
			LastControl = Control::DirectInput;
			ChargeType = 2;
		}
		else {
			if (Time - LocalPlayer->BasicActiontime.LastHuWeiJumpAATime < 1.0f) {
				if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					AllUse_AfterCombo(LocalPlayer, Enemy, LastControl);
				}
				return;
			}
		}
		if (LastControl == Control::LongFlash)
		{
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.5f)
			{
				if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IsAllGetUp(LocalPlayer->ActorModel)&& !SDK->ANIMHASH.IS_PUTDOWN(LocalPlayer->ActorModel)) {

					//printf("cast 2222222!\n");
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
	//printf("开始执行222 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
	if (IsCharge) {
		int EnemyEndureLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel);
		int level = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel);

		if (!EnemyIsDontAttack) {

			bool A3Mode = StackValue == 3 && !SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel);

			float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

			float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

			float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

			bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
				SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
			bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f
				|| Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.GuQingHan_Frost || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied || Enemy->BasicPlayerInfo.IsShaNanFHurt || Enemy->BasicPlayerInfo.IsStunned || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel);

			bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) /*|| SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)*/ || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel);

			bool CanCastCharge = Enemy->BasicPlayerInfo.WuTianF1Tick < SDK->GAMEFUNC.GetGlobalTime();

			float LastBladeParryTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastBladeParryTime;

			float LastCrouchTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastCrouchTime;

			HurtCast hurtExecuteMode = HurtCast::Null;

			auto CurrentHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);

			if (CurrentHurt.z > 0.f && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !Enemy->BasicPlayerInfo.IsBladeParry) {
				//当前玩家僵直 > 0.f 开始计算
				hurtExecuteMode = (CurrentHurt.z + ChargeTime > 0.49f) ? HurtCast::Charge : HurtCast::AA;
			}


			bool ShuiPaoMode = false;
			//打2.0
			if (Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied) {
				float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = (ChargeTime + FewTime) >= 1.10f;
			}
			if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
				float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = (ChargeTime + FewTime) >= 1.10f;
			}

			bool AirFlag = ChainShotTime < 1.0f || SDK->ANIMHASH.PLAYER_INAIR(Enemy->ActorModel) || (SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel) && (SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) != 20));

			//printf("开始执行333 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
			if (Distance <= (A3Mode ? 5.5f : ChargeRange) && CanCastCharge && LastBladeParryTime > 1.2f) {
				//如果巨大化 / 落地状态 / 抓闪 /必中状态下 进入了蓄力层级就释放. 或者在落地状态且不是左键蓄力而且进入A3状态时释放右键.
				bool CanNext = false;
				if (A3Mode) {
					CanNext = isHitMode;
				}
				else if (IntoCharge) {
					CanNext = isbigMode || isHitMode || hurtlandFlag || hurtExecuteMode == HurtCast::Charge || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || (AirFlag && SDK->KEYBOARD.IsPressKey(2));
				}
				if (ShuiPaoMode ? level == 2 : CanNext) {
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
					//printf("cast charge!\n");
					bool againexecute = true;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (A3Mode ? (IsLeftA3 ? 1.20f : 1.26f) : 0.80f);
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
					//printf("cast 1111111!\n");
					if (!A3Mode && SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Sword_QiXingDuoQiao) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
						//释放惊雷十劫
						//DbgPrint("释放惊雷十劫 按住右键");
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
						SDK->KEYBOARD.Set_IsActiveLeft(true);
						std::wstring Name;

						bool success = false;

						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
							if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
								if (Name.find(skCrypt(L"attack_light_soul_04")) != std::wstring::npos) { //太刀右键_惊雷十劫(太刀蓄力之后的右键追击魂玉)
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									success = true;
									break;
								}
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						if (success) {
							bool b2_success = false;
							//如果打出了 先给0.62f后判断目标是否受击
							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.62f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
								if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
									//自己被振刀就跳出循环
									break;
								}
								if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
									if (Name.find(skCrypt(L"soul_04_execute")) != std::wstring::npos) { //太刀右键_惊雷十劫(太刀蓄力之后的右键追击魂玉)
										b2_success = true;
										break;
									}
								}
							}
							if (b2_success) {
								float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 1.95f;
								while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
									if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
										//自己被振刀就跳出循环
										break;
									}
									std::this_thread::sleep_for(std::chrono::milliseconds(1));
								}
							}
						}
						else {
							if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
								//DbgPrint("没释放出惊雷十劫.松开鼠标右键");
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
								SDK->KEYBOARD.Set_IsActiveLeft(false);
							}
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

			bool ParryMode = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastParryTime <= 0.63f;

			if (DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime() && !Enemy->BasicPlayerInfo.YinZiPing_F2 && !Enemy->BasicPlayerInfo.JiYingYing_Ghost) {

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

							float RealDelay = SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) ? 0.34f : 0.36f;

							float StealOffset = 0.51f - RealDelay;

							if (time >= StealOffset && time <= (StealOffset + 0.02f)) { //0.33f 

								//0.175 > 0.18 > 0.17
								if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {

									if (StackValue == 1) {

										//处于一段攻击时
										std::wstring name;
										SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, name);
										float tick = SDK->GAMEFUNC.GetGlobalTime();
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
						SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || (hurtExecuteMode == HurtCast::AA && (!hurtlandFlag || SDK->KEYBOARD.IsPressKey(2))) || sdMode || ParryMode  || LastCrouchTime < 0.1f || SDK->ANIMHASH.IS_WUCHEN_teleported(Enemy->ActorModel) || Enemy->BasicPlayerInfo.IsCharm || AirFlag || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel);
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
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.75f);
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
								if (StackValue == 2 && Disable2A < SDK->GAMEFUNC.GetGlobalTime()) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//这里延迟一会 怕动作未完全展开
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.55f);
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

					if (Distance <= RightAARange + 3.f) {

						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', true, 0, 0.f)) {
							//中闪
							Distance = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));
							if (Distance <= RightAARange) {

								Right_ComboRun(LocalPlayer, Enemy, LastControl, false);
								return;
							}
						}
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime()) {
			//如果目标不在攻击范围内时
			bool x1flag = !IsInAttackRange(Distance);
			bool x2flag = EnableForceFlashWithFlashStep && SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime <= 0.2f;
			if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {
				if (CanToAttack && StackValue == 1 && SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) && !IsInAttackRange(Distance, 3.5f)) {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
					SDK->KEYBOARD.Set_IsActiveLeft(false);
					//这里延迟一会 怕动作未完全展开
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.34f);
					SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, Distance), 'W', false, 0, 0.f);
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					return;
				}
			}
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
		//printf("cast 33333333!\n");
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
void Sword::RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
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
				if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IsAllGetUp(LocalPlayer->ActorModel)&& !SDK->ANIMHASH.IS_PUTDOWN(LocalPlayer->ActorModel)) {
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
		if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel)) {
			if (Distance <= ChargeRange) {
				if (CheckCanCastCharge(NearPosition, 4.f)) {
					if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
						SDK->KEYBOARD.Set_IsActiveLeft(false);
					}
					if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
					}
					bool againexecute = true;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.8f;
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
					if (SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Sword_QiXingDuoQiao) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
						//释放惊雷十劫
						//DbgPrint("释放惊雷十劫 按住右键");
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
						SDK->KEYBOARD.Set_IsActiveLeft(true);
						std::wstring Name;

						bool success = false;

						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
							if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
								if (Name.find(skCrypt(L"attack_light_soul_04")) != std::wstring::npos) { //太刀右键_惊雷十劫(太刀蓄力之后的右键追击魂玉)
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									success = true;
									break;
								}
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						if (success) {
							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 1.85f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
								if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
									//自己被振刀就跳出循环
									break;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(1));
							}
						}
						else {
							if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
								//DbgPrint("没释放出惊雷十劫.松开鼠标右键");
								SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
								SDK->KEYBOARD.Set_IsActiveLeft(false);
							}
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