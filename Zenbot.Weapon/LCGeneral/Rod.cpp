
#include "Tools.h"

extern LC_SDK_STRUCT* SDK;
void Rod::MainExecuteCode() {

}
void Rod::FreeEvent() {

}
void Rod::LibraryInit() {

}
void Rod::ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	int WeaponID = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 120) {

		this->RunableLogic(LocalPlayer, Enemy);
	}
}
int Rod::GetNextChargeType(LC_PLAYER_INFO* Enemy, float Distance) {
	//���׷������̫Զ�����ȥ׷
	if (Distance > RightAARange + 2.f) {
		return 2;
	}
	if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime()) {
		return 2;
	}
	if (SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel)) {
		return 2;
	}
	//printf();
	return 1;
}
bool Rod::IsInAttackRange(float Distance, float ExtraRange) {
	return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
}
bool Rod::IsFastStatck(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return (Name.find(skCrypt(L"_attack_light")) != std::wstring::npos || Name.find(skCrypt(L"_attack_heavy")) != std::wstring::npos) && Name.find(skCrypt(L"_03")) != std::wstring::npos && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;;
	}
	return false;
}
bool Rod::Get_IsCharge(LC_PLAYER_INFO* Player) {
	return SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
}
bool Rod::AttackShock(LC_PLAYER_INFO* LocalPlayer)
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
bool Rod::AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool attachMode) {
	if (attachMode) {
		if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
			NextInput = Control::DirectInput;
			return false;
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(true);
	}
	//ֱ������ �жϾ����㹻 ��985 + ����A
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
	//���˵�pre����
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
	bool wpMode = (BladeMode || SaberMode);
	if (SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.50f) && ((wpMode || SDK->KEYBOARD.ActiveCombo(ActiveMode::ChainShootMode)) && LocalPlayer->BasicPlayerInfo.CanExecuteFlash)) {
		//��ʱ�������� �ж�һ�µ�ǰ״̬
		if (wpMode) {
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
				////DbgPrint("����������Ϣ");
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
					bool cs = SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'A', false,0, 0.f);
					//if (SDK->KEYBOARD.IsPressKey('W')) {
					//	SDK->KEYBOARD.SendKeyBoard('W', 2);
					//}
				}
			}
			SDK->CORE.LockHuWeiJumpTime(1.5f);
			AllowResetChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			return true;
		}
		if (SDK->KEYBOARD.ActiveCombo(ActiveMode::ChainShootMode)) {
			//����211����
			int localweapon = SDK->GAMEFUNC.GetWeaponIndex() == 1 ? '1' : '2';
			//Ԥ����һ����ȭ
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
			//ִ��һ������ ����סW ��ֹB1����
			SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 2);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			SDK->KEYBOARD.SendKeyBoard(localweapon, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			SDK->KEYBOARD.SendKeyBoard(localweapon, 2);
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
			SDK->KEYBOARD.Set_IsActiveRight(true);
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			float MaxChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
			while (MaxChargeTime > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel)) {
					DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 1.f;
					SDK->CORE.LockHuWeiJumpTime(1.5f);
					SDK->KEYBOARD.Set_IsExecuteCombo(false);
					return true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}
	else {
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
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
		int MaxTick = 0;
		do {
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
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					break;
				}
				if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			MaxTick++;
		} while (MaxTick < 2 && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All));

	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	NextInput = Control::DirectInput;
	return true;
}
bool Rod::ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer))
	{
		//����о��� ��ô��һ�δ�֮��ת����985��b. ���򴦾�  ��0.92fʱ��
		//����ж�� ��ô�ʹ�����+��b
		auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_Spear_DuoHun);
		if (!SoulMode) {
			bool p_success = false;
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.55f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				//һ�δ�����
				if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) <= 0.05f) {
					p_success = true;
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			//δ��ȡ ֱ�����¶�ʱ�� ������Ϻ����A
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.92f)) {
				SDK->KEYBOARD.StopCombo();
				return false;
			}
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.4f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
						//printf("���11\n");
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
						//printf("���11\n");
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
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		return true;
	}
	NextInput = Control::LongFlash;
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Rod::Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0))) {
		//��B�ĳ����ٶ�������ͬ
		float RealDelay = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel) ? 0.53f : 0.43f;
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			//�������û���� �ж��¶��������˻�������.��ֹ����AץA��
			if (!Logic2) {
				if ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) ||
					(SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) {
					//�������� ��ԭ��S�������û�����˾͹ر�2Aѡ�ԭ������
					SDK->KEYBOARD.StopCombo();
					float Dist = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));

					if (Dist <= 5.f || !LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						//����С��5�׾�ԭ������
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
		//�����жϵз���ֱ��� ����B���޷�������
		if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)  || (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
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
							//printf("���11\n");
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
			NextInput = Control::DirectInput;
			return true;
		}
		char InputKey = NULL;
		bool CanExecuteDoubleWeapon = false;
		auto WP = SDK->GAMEFUNC.GetDoubleWeaponID(&InputKey);
		auto RealWeaponID = SDK->UTILITY.RealWeaponToWeaponID(WP);
		bool KatanaMode = WP && RealWeaponID == 102;
		bool SwordMode = WP && RealWeaponID == 101;
		if (KatanaMode || SwordMode) {
			//˫��Ϊ���������� ��ʹ��Ʒ�ʼ��

			//DbgPrint("����̫��.ֱ�ӽ���C��");
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
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		NextInput = Control::DirectInput;
		return true;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Rod::Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
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
					//�������� ��ԭ��S�������û�����˾͹ر�2Aѡ�ԭ������
					SDK->KEYBOARD.StopCombo();
					float Dist = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));

					if (Dist <= 5.f || !LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						//����С��5�׾�ԭ������
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
		//�����жϵз���ֱ���
		if (SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y < 0.85f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || (GetRealHealth(Enemy) < 200 && !Enemy->BasicPlayerInfo.CuiSanNiang_V) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.4f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
						//printf("���11\n");
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
		return AllUse_AfterCombo(LocalPlayer, Enemy, NextInput);
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
void Rod::Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
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
		//�ǻ���״̬ ����û��������
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
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			//printf("�����ܻ��ж�\n");
			//������ ��������֮ǰ �����ܵ���ֱʱ�� < 0.1f.  �մӱ��򵽿�������.
			LastControl = Control::DirectInput;
			ChargeType = 1;
			//if (MyHurtData.ActionDuration <= 0.3f || !LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
			//	LastControl = Control::DirectInput;
			//	//0.3f���ͽ�ֱʱ��ԭ������
			//	printf("����ԭ������\n");
			//}
			//else {
			//	if (SDK->KEYBOARD.SendDodgeEvent('S')) {
			//		//����һ�������¼�.
			//		printf("��������¼�\n");
			//		return;
			//	}
			//}
		}
		else {
			if ((Time - LocalPlayer->BasicActiontime.LastHuWeiJumpAATime < 1.0f)) {
				//ֱ��ת����
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

			if (SDK->GAMEFUNC.GetGlobalTime() - LastDragonDash <= 0.1f) {
				//ֱ��ת����

				if (SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y >= 0.75f && DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime()) {
					//ֱ��ת����
					AllUse_AfterCombo(LocalPlayer, Enemy, LastControl, true);
					return;
				}
			}
			std::wstring Hold03Name;
			SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Hold03Name);

			//�����ﴦ����������Ϣ
			if (Hold03Name.find(skCrypt(L"male_rod_attack_hold_light_03")) != std::wstring::npos) { //����ƪ

				if (Distance <= 5.f) {
					//�Ҽ� ����������
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					bool againexecute = true;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.25f;
					while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
						if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
							//�Լ����񵶾�����ѭ��
							againexecute = false;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (againexecute) {
						float Tick = 0;
						while (SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !LocalPlayer->BasicPlayerInfo.IsBounce && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel)) {
							SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Hold03Name);
							int LocalLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel);
							if (LocalLevel == 20 || LocalLevel == 30) {
								if (SDK->GAMEFUNC.GetGlobalTime() > Tick && Hold03Name.find(L"_05_02") == std::wstring::npos) {
									//�� or �ϰ���
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
									SDK->KEYBOARD.Set_IsActiveRight(true);
									std::this_thread::sleep_for(std::chrono::milliseconds(25));
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);

									Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
								}
							}
							else {
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
					}
					LastControl = Control::LongFlash;
					return;
				}
				else {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
					SDK->KEYBOARD.Set_IsActiveLeft(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
					SDK->KEYBOARD.Set_IsActiveLeft(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					bool HasDaoHaiGun = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_DaoHaiGun);
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, HasDaoHaiGun ? 0.90f : 0.60f);
					LastControl = HasDaoHaiGun ? Control::LongFlash : Control::DirectInput;
					LastDragonDash = SDK->GAMEFUNC.GetGlobalTime();
					return;
				}
			}
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
		bool HasSoul = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_TongJin);
		bool HasDingHaiZhen = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_DingHaiZhen);
		if (!EnemyIsDontAttack) {

			float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

			float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

			float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

			bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
				SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
			bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f || Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.GuQingHan_Frost || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied || Enemy->BasicPlayerInfo.IsShaNanFHurt || Enemy->BasicPlayerInfo.IsStunned || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel);

			bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) /*|| SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)*/ || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel);

			//����A3 B3 ǰҡ������.
			float LastBladeParryTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastBladeParryTime;

			bool CanCastCharge = Enemy->BasicPlayerInfo.WuTianF1Tick < SDK->GAMEFUNC.GetGlobalTime();

			float LastCrouchTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastCrouchTime;

			bool ShuiPaoMode = false;

			HurtCast hurtExecuteMode = HurtCast::Null;

			auto CurrentHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);

			if (CurrentHurt.z > 0.f && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !Enemy->BasicPlayerInfo.IsBladeParry) {
				//��ǰ��ҽ�ֱ > 0.f ��ʼ����
				hurtExecuteMode = (CurrentHurt.z + ChargeTime > 0.49f) ? HurtCast::Charge : HurtCast::AA;
			}

			//������
			if (Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied) {
				float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 0.90f;
				//����������ģʽ
			}
			if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
				float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
				ShuiPaoMode = ChargeTime + FewTime >= 0.90f;
			}
			if (Distance <= (IsDragonBreakMode && !HasDingHaiZhen ? 7.f : ChargeRange) && IntoCharge && LastBladeParryTime > 1.2f && CanCastCharge) {
				//����޴� / ���״̬ / ץ�� /����״̬�� �����������㼶���ͷ�.
				if (ShuiPaoMode ? IsDragonBreakMode : isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || hurtExecuteMode == HurtCast::Charge || AttackBlade(Enemy, false)) {
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
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsDragonBreakMode ? 0.83f : (useLeft ? (HasSoul ? 0.60f : 1.45f) : 1.34f));
					while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
						if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
							//�Լ����񵶾�����ѭ��
							againexecute = false;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (!useLeft && !IsDragonBreakMode && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', true, 0, 0.f)) {
							//����BB
							LastControl = Control::DirectInput;
							Right_ComboRun(LocalPlayer, Enemy, LastControl, false);
							return;
						}
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
						//ת����ץ
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
				if (CanToAttack) 
				{
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

									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									if (!Left_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer, LastControl);
									}
									return;
								}
							}
							if (Execute1 || Execute3) {
								if (StackValue == 2 && Disable2A < SDK->GAMEFUNC.GetGlobalTime()) {
									//����һ�ι���ʱ
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

									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									if (!Right_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer, LastControl);
									}
									return;
								}
							}
							if (Execute1 || Execute3) {
								if (StackValue == 2 && Disable2A < SDK->GAMEFUNC.GetGlobalTime()) {
									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
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
				//��������ʱ��ʱ. ����Է���2.0ʱ���� �ű�����������ȡParryBcakSwingTime. �����жϳ���ʱ���ת����ץ!

				float TransitionTime = Enemy->BasicHurtInfo.ParryBackSwingTransitionTime - SDK->GAMEFUNC.GetGlobalTime();

				if (MaxChargeTime < ChargeTime + TransitionTime) {

					if (Distance <= RightAARange + 3.f) {

						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0, 0.f)) {
							//����
							Distance = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel).DistTo(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel));

							if (Distance <= RightAARange) {

								Left_ComboRun(LocalPlayer, Enemy, LastControl, false);
								return;
							}
						}
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
			//���Ŀ�겻�ڹ�����Χ��ʱ
			bool x1flag = !IsInAttackRange(Distance);
			bool x2flag = EnableForceFlashWithFlashStep && SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime <= 0.2f;
			if ((x1flag && ChargeTime > 0.24f) || x2flag) {
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				//printf("1�ɹ�ô:%d\n", execute);
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
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				//printf("2�ɹ�ô:%d\n", boolean);
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
void Rod::RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	//ƴ���¼�
	if (LocalPlayer->BasicPlayerInfo.IsBounce) {
		//����һ�а���
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
	//���¼�
	if (SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastParryTime < 0.3f) {
		//printf("error2\n");
		//����һ�а���
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
	//�����¼�
	if (LocalPlayer->BasicPlayerInfo.IsShock || SDK->ANIMHASH.IS_SUCCESS_SHOCK(LocalPlayer->ActorModel)) {
		//����һ�а���
		SDK->KEYBOARD.StopCombo();
		ExecuteShock(LocalPlayer, Enemy, LastControl);
		return;
	}
	auto MyPosition = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel);
	auto EnemyPosition = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);

	if (SDK->UTILITY.GetCountEnemysHeroesInRange(EnemyPosition, 8.f) == 1 && //Ŀ����Χ������ == 1
		SDK->UTILITY.GetCountAllyHeroesInRange(MyPosition, 10.f) == 1)       //��Χ�Ѿ����� == 1
	{
		Combo_Solo(LocalPlayer, Enemy);
		return;
	}
	//��ȡ��������Ŀ�� ���û�о�ѡ�������Ŀ��
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
	//���������׶�
	bool IsCharge = Get_IsCharge(LocalPlayer);
	//��ȡ������Ϣ
	auto NearPosition = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
	float Distance = NearPosition.DistTo(MyPosition);
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
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();

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
			std::wstring Hold03Name;
			SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Hold03Name);

			//�����ﴦ����������Ϣ
			if (Hold03Name.find(skCrypt(L"male_rod_attack_hold_light_03")) != std::wstring::npos) { //����ƪ

				if (Distance <= 5.f) {
					//�Ҽ� ����������
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					bool againexecute = true;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.25f;
					while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
						if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
							//�Լ����񵶾�����ѭ��
							againexecute = false;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (againexecute) {
						float Tick = 0;
						while (SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !LocalPlayer->BasicPlayerInfo.IsBounce && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel)) {
							SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Hold03Name);
							int LocalLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel);
							if (LocalLevel == 20 || LocalLevel == 30) {
								if (SDK->GAMEFUNC.GetGlobalTime() > Tick && Hold03Name.find(L"_05_02") == std::wstring::npos) {
									//�� or �ϰ���
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
									SDK->KEYBOARD.Set_IsActiveRight(true);
									std::this_thread::sleep_for(std::chrono::milliseconds(25));
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);

									Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
								}
							}
							else {
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
					}
					LastControl = Control::LongFlash;
					return;
				}
				else {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
					SDK->KEYBOARD.Set_IsActiveLeft(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
					SDK->KEYBOARD.Set_IsActiveLeft(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					bool HasDaoHaiGun = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_DaoHaiGun);
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, ClosestMouseTarget, HasDaoHaiGun ? 0.90f : 0.60f);
					LastControl = HasDaoHaiGun ? Control::LongFlash : Control::DirectInput;
					LastDragonDash = SDK->GAMEFUNC.GetGlobalTime();
					return;
				}
			}

			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
			SDK->KEYBOARD.Set_IsActiveLeft(true);
			LastControl = Control::LongFlash;
			return;
		}
	}
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	if (IsCharge) {
		bool IsDragonBreakMode = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
		bool HasDingHaiZhen = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_DingHaiZhen);
		bool HasSoul = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Rod_TongJin);
		bool LWPMode = SDK->KEYBOARD.ActiveCombo(ActiveMode::SoulMode);
		if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel) ) {
			if (Distance <= ((IsDragonBreakMode && !HasDingHaiZhen) ? 7.f : ChargeRange)) {
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
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsDragonBreakMode ? 0.83f : (useLeft ? (HasSoul ? 0.60f : 1.45f) : 1.34f));
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
							if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
								//�Լ����񵶾�����ѭ��
								againexecute = false;
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						if (!useLeft && !IsDragonBreakMode && SDK->GAMEFUNC.IsHiting(ClosestMouseTarget->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
							if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', true, 0, 0.f)) {
								//����BB
								LastControl = Control::DirectInput;
								Right_ComboRun(LocalPlayer, ClosestMouseTarget, LastControl, false);
								return;
							}
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
			SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0, 0.f);
			StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
			return;
		}
		//���������׶�
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