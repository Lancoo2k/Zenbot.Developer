
#include "Tools.h"

extern LC_SDK_STRUCT* SDK;

void Dagger::MainExecuteCode() {

}
void Dagger::FreeEvent() {

}
void Dagger::LibraryInit() {

}
void Dagger::ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	int WeaponID = SDK->UTILITY.RealWeaponToWeaponID(SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 106) {

		this->RunableLogic(LocalPlayer, Enemy);
	}
}

int Dagger::GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack, float Distance) {
	if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime())
		return 2;
	if (ForceUseLeft > SDK->GAMEFUNC.GetGlobalTime())
		return 1;
	if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel) || SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(SDK->GAMEFUNC.GetLocalPlayer()) == 1) {
		return 2;
	}
	return 1;
}

bool Dagger::IsInAttackRange(float Distance, float ExtraRange) {
	return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
}

bool Dagger::IsFastStatck(LC_PLAYER_INFO* Obj) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Obj->ActorModel, Name)) {
		return (Name.find(skCrypt(L"attack_light")) != std::wstring::npos || Name.find(skCrypt(L"attack_heavy")) != std::wstring::npos) && Name.find(skCrypt(L"_03")) != std::wstring::npos && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;
	}
	return false;
}

bool Dagger::Get_IsCharge(LC_PLAYER_INFO* Player) {

	return SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
}

bool Dagger::IsCrouchCharge(LC_PLAYER_INFO* Player) {
	std::wstring Name;
	if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
		return Name.find(skCrypt(L"_crouch_attack_")) != std::wstring::npos && Name.find(skCrypt(L"_charge")) != std::wstring::npos;
	}
	return false;
}

void Dagger::ResetCC() {
	SDK->KEYBOARD.SendKeyBoard('C', 0);
	SDK->KEYBOARD.Set_IsActiveCrouch(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	SDK->KEYBOARD.SendKeyBoard('C', 2);
	SDK->KEYBOARD.Set_IsActiveCrouch(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	SDK->KEYBOARD.SendKeyBoard('C', 0);
	SDK->KEYBOARD.Set_IsActiveCrouch(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	SDK->KEYBOARD.SendKeyBoard('C', 2);
	SDK->KEYBOARD.Set_IsActiveCrouch(false);
}

bool Dagger::J2aJb(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (!SDK->KEYBOARD.IsPressKey('W')) {
		SDK->KEYBOARD.SendKeyBoard('W', 0);
	}
	//Ŀ����Ȼ�ܻ�״̬ ��ʱ���¿ո�� ���2��A
	if (!SDK->KEYBOARD.IsPressKey(VK_SPACE))
	{
		SDK->KEYBOARD.SendKeyBoard(VK_SPACE, 0);
		SDK->KEYBOARD.Set_IsActiveSpace(true);
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
		SDK->KEYBOARD.SendKeyBoard(VK_SPACE, 2);
		SDK->KEYBOARD.Set_IsActiveSpace(false);
	}
	bool JumpSuccess = false;
	float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
	while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		if (SDK->ANIMHASH.PLAYER_INAIR(LocalPlayer->ActorModel)) {
			JumpSuccess = true;
			break;
		}
		if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (!JumpSuccess) {
		if (SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 2);
		}
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		SDK->KEYBOARD.StopCombo();
		return false;
	}
	for (int i = 0; i < 2; i++) {
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.32f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
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
	Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
	while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, TempName);
		if (TempName.find(L"heavy_01_recover") != std::wstring::npos) {
			//��ȡ��recover;
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
	if (SDK->KEYBOARD.IsPressKey('W')) {
		SDK->KEYBOARD.SendKeyBoard('W', 2);
	}
	return true;
}

bool Dagger::attach_cab(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if ((SDK->UTILITY.CheckInRange(Enemy->ActorModel, CARange) && (SDK->KEYBOARD.BasicLogic.ExecuteCA() || IsAfter))) {
		bool TwoCAMode = GetRealHealth(Enemy) > 500;
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, TwoCAMode ? 0.58f : 0.48f);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		if (TwoCAMode) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CARange) || !SDK->KEYBOARD.BasicLogic.ExecuteCA()) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//0.58f
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.48f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}
		float delay = 0.f;
		auto Datas = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);
		if (Datas.y > 0.22f) {
			delay = (Datas.z - 0.22f) * 1000.f;
			delay = delay <= 0 ? 0.f : delay;
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, (int)delay)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		//0.58f
		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			SDK->KEYBOARD.StopCombo();
			NextInput = Control::DirectInput;
			return false;
		}
		NextInput = Control::DirectInput;
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		SDK->KEYBOARD.StopCombo();
		return true;
	}
	NextInput = Control::DirectInput;
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Dagger::attach_3abcombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsSky, bool guifan, bool jingke, bool guiku, bool dontb) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsSky && guifan) {
		//����ģʽ
		//���Ҷ���
		//printf("����1111");
		bool isdgMode = SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel);
		if (isdgMode || SDK->KEYBOARD.BasicLogic.ShortFlash('D', false)) {
			//0.31f�� ���������� �����
			if (guiku)
			{
				while (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
					float CurTime = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
					if (CurTime > 0.31f) {
						//ӵ�й����ʱ 70%���������Ծ
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
						SDK->KEYBOARD.Set_IsActiveRight(true);
						std::this_thread::sleep_for(std::chrono::milliseconds(25));
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
						float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
						while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
							if (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel)) {
								//0.20 ~ 0.54f �ڴ������жϵ��˶���
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
					}
				}
				NextInput = Control::DirectInput;
				SDK->KEYBOARD.Set_IsExecuteCombo(false);
				SDK->KEYBOARD.StopCombo();
				return true;
			}
			bool z1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.31f);
			bool z2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
			if (!isdgMode && (!z1 || !z2)) {
				//printf("Error1  %d   %d\n",z1,z2);
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::LongFlash;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				//printf("Error2\n");
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::LongFlash;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, jingke ? 0.81 : 0.47f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				//printf("Error3\n");
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::LongFlash;
				return false;
			}
			//��һ��B�ٴ��ȥ
			//std::wstring asdb;
			//SDK->GAMEFUNC.GetActionName(Enemy->ActorModel, asdb);
			//printf("name:%ls\n",asdb.c_str());
			if (jingke && dontb && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				NextInput = Control::FlashStep;
				SDK->KEYBOARD.Set_IsExecuteCombo(false);
				SDK->KEYBOARD.StopCombo();
				return true;
			}

			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				//printf("Error4\n");
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.57f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				//printf("Error5\n");
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//printf("Error6\n");
			NextInput = Control::DirectInput;
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			SDK->KEYBOARD.StopCombo();
			return true;
		}
	}
	if (IsSky || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) && SDK->KEYBOARD.BasicLogic.ExecuteCB())) {
		//����
		if (!IsSky) {
			bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.65f);
			bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
			if (!Logic1 || !Logic2) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}
		//����W��
		if (!SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 0);
		}
		//���һ����������A
		for (int i = 0; i < 2; i++) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				if (SDK->KEYBOARD.IsPressKey('W')) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
				}
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.32f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				if (SDK->KEYBOARD.IsPressKey('W')) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
				}
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}
		//�ȴ����
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 5.f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
				break;
			}
			if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			//Ŀ����Ȼ�ܻ�״̬ ��ʱ���¿ո�� ���2��A
			if (!SDK->KEYBOARD.IsPressKey(VK_SPACE))
			{
				SDK->KEYBOARD.SendKeyBoard(VK_SPACE, 0);
				SDK->KEYBOARD.Set_IsActiveSpace(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(25));
				SDK->KEYBOARD.SendKeyBoard(VK_SPACE, 2);
				SDK->KEYBOARD.Set_IsActiveSpace(false);
			}
			bool JumpSuccess = false;
			Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				if (SDK->ANIMHASH.PLAYER_INAIR(LocalPlayer->ActorModel)) {
					JumpSuccess = true;
					break;
				}
				if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (!JumpSuccess) {
				if (SDK->KEYBOARD.IsPressKey('W')) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
				}
				NextInput = Control::DirectInput;
				SDK->KEYBOARD.Set_IsExecuteCombo(false);
				SDK->KEYBOARD.StopCombo();
				return false;
			}
			for (int i = 0; i < 2; i++) {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
					if (SDK->KEYBOARD.IsPressKey('W')) {
						SDK->KEYBOARD.SendKeyBoard('W', 2);
					}
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.32f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					if (SDK->KEYBOARD.IsPressKey('W')) {
						SDK->KEYBOARD.SendKeyBoard('W', 2);
					}
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
			}
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
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
			Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, TempName);
				if (TempName.find(L"heavy_01_recover") != std::wstring::npos) {
					//��ȡ��recover;
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
		}
		if (SDK->KEYBOARD.IsPressKey('W')) {
			SDK->KEYBOARD.SendKeyBoard('W', 2);
		}
		NextInput = Control::DirectInput;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Dagger::AttackShock(LC_PLAYER_INFO* LocalPlayer)
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
bool Dagger::ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer))
	{
		auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_DuoHun);

		if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, SoulMode ? 0.51f : 0.71f)) {
			SDK->KEYBOARD.StopCombo();
			return false;
		}
		if (!SoulMode) {

			bool Success = J2aJb(LocalPlayer, Enemy, NextInput);
			NextInput = Control::DirectInput;
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			return Success;
		}
		else {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(2, true)) {
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
			bool success_get_recover = false;
			std::wstring TempName;
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
				SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, TempName);
				if (TempName.find(L"heavy_01_recover") != std::wstring::npos) {
					//��ȡ��recover;
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
	}
	NextInput = Control::LongFlash;
	SDK->KEYBOARD.StopCombo();
	return false;
}

bool Dagger::Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0))) {
		//��B�ĳ����ٶ�������ͬ
		float RealDelay = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel) ? 0.37f : 0.33f;
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			//�������û���� �ж��¶��������˻�������.��ֹ����AץA��

			if (!Logic2) {
				if ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) ||
					(SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) {
					//�������� ��ԭ��������ֹ�ٱ���ץ
					SDK->KEYBOARD.StopCombo();
					NextInput = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::FlashStep : Control::DirectInput;
					return false;
				}
			}
			//printf("LOGIN1 ERROR! %d  %d", Logic1, Logic2);
			SDK->KEYBOARD.StopCombo();
			NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20 ? Control::UseSkill : Control::LongFlash;
			return false;
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
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
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
bool Dagger::Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter) {
	if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
		NextInput = Control::DirectInput;
		return false;
	}
	SDK->KEYBOARD.Set_IsExecuteCombo(true);
	if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0))) {
		auto EnemyPos = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);
		auto LocalPos = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel);
		auto Distance = EnemyPos.DistTo(LocalPos);

		float RealDelay = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel) ? 0.37f : 0.31f;
		bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
		bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
		if (!Logic1 || !Logic2) {
			if (!Logic2) {
				if ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) ||
					(SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) {
					//�������� ��ԭ��������ֹ�ٱ���ץ
					SDK->KEYBOARD.StopCombo();
					NextInput = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::FlashStep : Control::DirectInput;
					return false;
				}
			}
			//printf("LOGIN1 ERROR! %d  %d", Logic1, Logic2);
			SDK->KEYBOARD.StopCombo();
			NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20 ? Control::UseSkill : Control::LongFlash;
			return false;
		}
		//���������A��Է������˾Ͳ���A ��ȻB�򲻵���

		if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.57f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			NextInput = Control::DirectInput;
			return true;
		}

		if (Distance > 1.8f) { //2M���CA + B
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CARange) || !SDK->KEYBOARD.BasicLogic.ExecuteCA()) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.48f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
		}
		if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
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
	SDK->KEYBOARD.Set_IsExecuteCombo(false);
	SDK->KEYBOARD.StopCombo();
	return false;
}
void Dagger::Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	auto EnemyPosition = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);
	auto MyPosition = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel);
	float Distance = EnemyPosition.DistTo(MyPosition);

	bool IsCharge = Get_IsCharge(LocalPlayer);

	bool EnemyIsDontAttack = SDK->ANIMHASH.Action_HuoNan_IsF(Enemy->ActorModel) || Enemy->BasicPlayerInfo.GuQingHan_F || SDK->ANIMHASH.Is_CuiSanNiangV2(Enemy->ActorModel);

	bool HasJingKe = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_JingKeXianBi);
	bool HasGuiKu = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_GuiKuShenHao);
	if (DisableWeaponWhiteMode > SDK->GAMEFUNC.GetGlobalTime()) {
		DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 1.5f;
		DisableWeaponWhiteMode = 0.f;
	}

	if (AllowResetChargeTime > SDK->GAMEFUNC.GetGlobalTime()) {
		StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
		AllowResetChargeTime = 0.f;
	}

	if (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel) && ForceUseRight < SDK->GAMEFUNC.GetGlobalTime()) {

		//�õ���ǰ����ʱ��
		bool SuccessCC = false;
		while (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
			float CurTime = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
			//�ε� �Եз��������м��
			if (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.15f ||
				SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.15f) {
				//�Է����� ����Ҫ��ץ ֱ�ӽ�������
				ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 1.5f;
				LastControl = Control::DirectInput;
				return;
			}
			if (CurTime > 0.36f) {
				ResetCC();
				LastControl = Control::DirectInput;
				return;
			}
			if (CurTime > 0.31f) {

				//>=0.31 ֤�������ͷŹ��� �жϵ�������
				float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;
				float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;
				float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

				bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
					SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
				bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f;
				bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel);

				//�ܹ����ȹ�
				if (isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.15f || FlashStepTime < 0.15f) {
					//�����ͷŹ���~~~ ֱ�ӵ���CALL
					attach_3abcombo(LocalPlayer, Enemy, LastControl, false, true, HasJingKe, HasGuiKu);

					return;
				}
				if (HasGuiKu && SDK->UTILITY.GetRandomIntEx(1, 10) <= randomRate && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) != 20 && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) != 21) {
					//ӵ�й����ʱ 70%���������Ծ
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
					while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
						if (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel)) {
							//0.20 ~ 0.54f �ڴ������жϵ��˶���
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	if (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel)) {

		while (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {

			float CurTime = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
			if (CurTime >= 0.20f && CurTime <= 0.54f) {
				bool WhiteMode = !Enemy->BasicPlayerInfo.YinZiPing_F2 && (Enemy->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) || SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastParryTime < 1.f);

				if (WhiteMode) {
					//�׵�ģʽʱ
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
					SDK->KEYBOARD.Set_IsActiveLeft(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
					SDK->KEYBOARD.Set_IsActiveLeft(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.52f);
					LastControl = Control::DirectInput;
					return;
				}
				else {

					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					std::wstring gkAnim;
					bool find = false;
					float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
					while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
						SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, gkAnim);
						if (gkAnim == skCrypt(L"male_dagger_flashjump_attack_soul_01")) {
							find = true;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (find) {
						//��޶���2.81f���2��a
						SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 2.81f);
						if (SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
							J2aJb(LocalPlayer, Enemy, LastControl);
						}
						LastControl = Control::DirectInput;
						return;
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		LastControl = Control::DirectInput;
	}

	if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
		int ChargeType = GetNextChargeType(Enemy, EnemyIsDontAttack, Distance);

		//��ȡ�ϴν�ֱʱ�� �����ֱ���߾Ͷ��� ��ֱ���;�����

		auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
		float Time = SDK->GAMEFUNC.GetGlobalTime();
		if ((Time - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) ||
			(Time - ChangeWeaponTime < 0.2f) ||
			(Time - LocalPlayer->BasicActiontime.LastChongQuanTime < 0.8f)) {
			//printf("�����ܻ��ж�\n");
			//������ ��������֮ǰ �����ܵ���ֱʱ�� < 0.1f.  �մӱ��򵽿�������.
			LastControl = Control::DirectInput;
			ChargeType = 2;
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
					attach_3abcombo(LocalPlayer, Enemy, LastControl, false, false, false, false);
				}
				return;
			}
		}
		if (LastControl == Control::LongFlash)
		{
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.42f)
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
		if (LastControl == Control::FlashStep) {
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
			
				SDK->KEYBOARD.BasicLogic.ShortFlash(TargetIsFlee(LocalPlayer, Enemy) ? 'W' : 'S', false);
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
	BOOL CanToAttack = SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel) && StackValue != 3 && ChargeTime < 0.49f;
	if (IsCharge) {
		bool HasKangLong = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_KangLongYouHui);
		bool HasGuiRen = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_GuiRenAnZha);
		bool KangLongMode = HasKangLong && SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
		int EnemyEndureLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel);
		if (!EnemyIsDontAttack) {

			float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

			float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

			float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

			bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
				SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicPlayerInfo.IsCharm;
			bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 1.0f || 
				Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.GuQingHan_Frost || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied || Enemy->BasicPlayerInfo.IsShaNanFHurt || Enemy->BasicPlayerInfo.IsStunned || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel);

			bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) /*|| SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)*/;

			bool CanCastCharge = Enemy->BasicPlayerInfo.WuTianF1Tick < SDK->GAMEFUNC.GetGlobalTime();

			float LastCrouchTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastCrouchTime;

			float LastBladeParryTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastBladeParryTime;

			HurtCast hurtExecuteMode = HurtCast::Null;

			auto CurrentHurt = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);

			if (CurrentHurt.z > 0.f && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && !Enemy->BasicPlayerInfo.IsBladeParry) {
				//��ǰ��ҽ�ֱ > 0.f ��ʼ����
				hurtExecuteMode = (CurrentHurt.z + ChargeTime > 0.49f) ? HurtCast::Charge : HurtCast::AA;
			}

			bool ShuiPaoMode = false;

			//��������Ŀ��
			bool AirFlag = SDK->ANIMHASH.PLAYER_INAIR(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel);

			//ֻ�п������� ��Ŀ��Ϊ�������ʱ �ſ���ˮ��
			if (HasKangLong && SDK->KEYBOARD.Get_IsActiveLeft()) {
				if (Enemy->BasicPlayerInfo.CuiSanNiang_F || Enemy->BasicPlayerInfo.CuiSanNiang_V_tied) {
					float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
					ShuiPaoMode = ChargeTime + FewTime >= 1.10f;
				}

				if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
					float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
					ShuiPaoMode = ChargeTime + FewTime >= 1.10f;
				}
			}
			
			if (Distance <= (SDK->KEYBOARD.IsPressKey(1) ? LeftChargeRange : RightChargeRange) && CanCastCharge && LastBladeParryTime > 1.2f && IntoCharge) {
				bool CanNext = ((isbigMode || AirFlag) && SDK->KEYBOARD.IsPressKey(1)) || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || isHitMode || (hurtlandFlag && SDK->KEYBOARD.IsPressKey(2)) || hurtExecuteMode == HurtCast::Charge;
				if (ShuiPaoMode ? KangLongMode : CanNext) {
					bool IsLeft = false;
					if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
						SDK->KEYBOARD.Set_IsActiveLeft(false);
						IsLeft = true;
					}
					if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
					}
					//ԥ������ ���� 2.33f ת���� 1.20f
					std::wstring name;
					bool canSL = LocalPlayer->BasicPlayerInfo.CanExecuteFlash;
					bool againexecute = true;
					bool klactive = false;
					float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsLeft ? (KangLongMode ? 0.63f : 0.68f) : (canSL ? (HasGuiRen ? 1.40f : 1.20f) : (HasGuiRen ? 1.84f : 2.33f)));
					while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
						if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
							//�Լ����񵶾�����ѭ��
							againexecute = false;
							break;
						}
						SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, name);
						if (name.find(skCrypt(L"soul_execute_02")) != std::wstring::npos) {
							//�����鵽�����лڵ��ı�
							klactive = true;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (!againexecute) {
						LastControl = Control::DirectInput;
						return;
					}
					if (klactive) {
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 2.20f;
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
							if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
								//�Լ����񵶾�����ѭ��
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						LastControl = Control::LongFlash;
					}
					else {
						if (SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
							if (!HasGuiRen && !IsLeft && !KangLongMode && canSL) {
								if (SDK->KEYBOARD.BasicLogic.ShortFlash('S', false)) {
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.20f);
								}
							}
							attach_3abcombo(LocalPlayer, Enemy, LastControl, IsLeft, isbigMode || SDK->UTILITY.GetRandomIntEx(1, 10) <= randomRate2 || HasGuiKu, HasJingKe, HasGuiKu);
						}
					}
					return;
				}
			}


			if (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.1f && HitMark > SDK->GAMEFUNC.GetGlobalTime() && EnableHitMark && IsCrouchCharge(LocalPlayer) && CanToAttack) {
				//printf("into cab\n");
				//ץ�� ��ʱ�� CAB  ����0.17f֤���з��Ƕ���
				//0.744542 | male_dagger_attack_heavy_02 | enemyhurt:0.000000 | 0.520630 | 0.000000
				//	0.000000 | male_dagger_attack_heavy_02_recover
				//B->0.74f�� �����ٴ�A��
				if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {
					//����һ�ι���ʱ
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
					SDK->KEYBOARD.Set_IsActiveLeft(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					if (attach_cab(LocalPlayer, Enemy, LastControl, true)) {
						HitMark = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
					}
					LastControl = Control::FlashStep;
					return;
				}
			}

			bool ParryMode = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastParryTime <= 0.63f;
			if (CanToAttack && DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime()) {

				if (TryStealAA && StackValue == 1 && !IsCrouchCharge(LocalPlayer)) {

					if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && IsInAttackRange(Distance) && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) == 20) {

						float time = SDK->GAMEFUNC.GetChargeTime(Enemy->ActorModel);

						float RealDelay = SDK->KEYBOARD.IsPressKey(1) ? (SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) ? 0.37f : 0.31f) : (SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) ? (StealAAStyle == 1 ? 0.38f : 0.48f) : 0.33f);

						float StealOffset = 0.51f - RealDelay;

						if (time >= StealOffset && time <= (StealOffset + 0.02f)) { //0.33f 

							if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {

								if (StackValue == 1) {

									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
									LastControl = Control::FlashStep;
									if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) < RealDelay + 0.1f) {
										//����ε�û���ж��� ��ʹ��B+����
										LastControl = Control::LongFlash;
										ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 0.25f;
									}
									return;
								}
							}
							if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {

								if (StackValue == 1) {
									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
									if (StealAAStyle == 1 && SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel)) {
										//����Լ�����A ��ôCһ�º��ٶ���
										SDK->KEYBOARD.SendKeyBoard('C', 0);
										std::this_thread::sleep_for(std::chrono::milliseconds(25));
										SDK->KEYBOARD.SendKeyBoard('C', 2);
										SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', false, 0, 0.f);
										StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
										return;
									}
									LastControl = Control::FlashStep;
									return;

								}
							}

						}
					}
				}

				if (!Enemy->BasicPlayerInfo.YinZiPing_F2 && !Enemy->BasicPlayerInfo.JiYingYing_Ghost) {
					bool sdMode = (SDK->GAMEFUNC.GetGlobalTime() - ChangeWeaponTime < 1.0f) && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);

					bool Execute1 = Enemy->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IS_IDLE(Enemy->ActorModel) ||
						(hurtlandFlag && !SDK->KEYBOARD.IsPressKey(1)) || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) ||
						SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || (hurtExecuteMode == HurtCast::AA && (!hurtlandFlag || SDK->KEYBOARD.IsPressKey(2))) || sdMode || ParryMode || ChainShotTime < 1.0f || LastCrouchTime < 0.1f || SDK->ANIMHASH.IS_WUCHEN_teleported(Enemy->ActorModel);
					bool Execute2 = (FlashExecuteTime <= 0.1f && (!SDK->KEYBOARD.IsPressKey(1) || Enemy->BasicActiontime.LastFlashType != EVADE_TYPE::EVADE_Double) && (LocalPlayer->BasicActiontime.LastFlashTime < Enemy->BasicActiontime.LastFlashTime)) || (SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) && SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel) != 20);

					if (IsInAttackRange(Distance)) {

						if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {


							if (IsCrouchCharge(LocalPlayer)) {
								if (Execute1 || Execute2) {
									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									attach_cab(LocalPlayer, Enemy, LastControl, true);
									return;
								}

							}
							else {
								if (Execute1 || Execute2) {
									if (StackValue == 1) {

										//����һ�ι���ʱ
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
										SDK->KEYBOARD.Set_IsActiveLeft(false);
										//�����ӳ�һ�� �¶���δ��ȫչ��
										std::this_thread::sleep_for(std::chrono::milliseconds(50));
										if (Left_ComboRun(LocalPlayer, Enemy, LastControl, true)) {
											if (EnableHitMark) {
												//printf("����C1\n");
												SDK->KEYBOARD.SendKeyBoard('C', 0);
												SDK->KEYBOARD.Set_IsActiveCrouch(true);
												std::this_thread::sleep_for(std::chrono::milliseconds(10));
												SDK->KEYBOARD.SendKeyBoard('C', 2);
												SDK->KEYBOARD.Set_IsActiveCrouch(false);
												ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
											}
											HitMark = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
										}
										else {
											if (LastControl == Control::UseSkill) {
												CheckCastSkill(LocalPlayer, LastControl);
											}
										}
										return;
									}
									if (StackValue == 2) {
										//����һ�ι���ʱ
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
										SDK->KEYBOARD.Set_IsActiveLeft(false);
										std::this_thread::sleep_for(std::chrono::milliseconds(50));
										SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.57f);
										LastControl = Control::DirectInput;
										//CAB�� ��һ��C ����׷�ԭ������ ����HitMark
										if (EnableHitMark) {
											//printf("����C2\n");
											SDK->KEYBOARD.SendKeyBoard('C', 0);
											SDK->KEYBOARD.Set_IsActiveCrouch(true);
											std::this_thread::sleep_for(std::chrono::milliseconds(10));
											SDK->KEYBOARD.SendKeyBoard('C', 2);
											SDK->KEYBOARD.Set_IsActiveCrouch(false);
											ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
										}
										HitMark = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
										return;
									}
								}
							}
						}
						if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2) && !IsCrouchCharge(LocalPlayer)) {

							if (Execute1 || Execute2) {
								if (StackValue == 1) {
									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									if (Right_ComboRun(LocalPlayer, Enemy, LastControl, true)) {
										if (EnableHitMark) {
											//printf("����C3\n");
											SDK->KEYBOARD.SendKeyBoard('C', 0);
											SDK->KEYBOARD.Set_IsActiveCrouch(true);
											std::this_thread::sleep_for(std::chrono::milliseconds(10));
											SDK->KEYBOARD.SendKeyBoard('C', 2);
											SDK->KEYBOARD.Set_IsActiveCrouch(false);
											ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
										}
										HitMark = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
									}
									else {
										if (LastControl == Control::UseSkill) {
											CheckCastSkill(LocalPlayer, LastControl);
										}
									}
									return;
								}
								if (StackValue == 2) {
									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f);
									//CAB�� ��һ��C ����׷�ԭ������ ����HitMark
									if (EnableHitMark) {
										//printf("����C4\n");
										SDK->KEYBOARD.SendKeyBoard('C', 0);
										SDK->KEYBOARD.Set_IsActiveCrouch(true);
										std::this_thread::sleep_for(std::chrono::milliseconds(10));
										SDK->KEYBOARD.SendKeyBoard('C', 2);
										SDK->KEYBOARD.Set_IsActiveCrouch(false);
										ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
									}
									HitMark = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
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

					if (Distance <= CARange) {

						if (attach_cab(LocalPlayer, Enemy, LastControl)) {
							//CAB�� ��һ��C ����׷�ԭ������ ����HitMark
							if (EnableHitMark) {
								SDK->KEYBOARD.SendKeyBoard('C', 0);
								SDK->KEYBOARD.Set_IsActiveCrouch(true);
								std::this_thread::sleep_for(std::chrono::milliseconds(10));
								SDK->KEYBOARD.SendKeyBoard('C', 2);
								SDK->KEYBOARD.Set_IsActiveCrouch(false);
								ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
							}
							HitMark = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
						}
						return;
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
			//���Ŀ�겻�ڹ�����Χ��ʱ

			bool x1flag = (SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime < 1.f) ? Distance > (SDK->KEYBOARD.IsPressKey(1) ? LeftChargeRange : RightChargeRange) : !IsInAttackRange(Distance);
			if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
				if (CanToAttack && StackValue == 1 && SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) && !IsInAttackRange(Distance, 2.f)) {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					//�����ӳ�һ�� �¶���δ��ȫչ��
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.47f);
					SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, EnemyIsDontAttack, Distance), 'W', false, 0, 0.f);
					StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
					return;
				}
			}
			if ((x1flag && ChargeTime > 0.24f)) {
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
				auto boolean = SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(Enemy, EnemyIsDontAttack, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0, 0.f);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				return;
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				LastControl = Control::DirectInput;
			}
		}
	}
}
void Dagger::RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	//�������������� ����.
	//ƴ���¼�
	if (SDK->CORE.GetLastComboFuncExecuteTime() > SDK->GAMEFUNC.GetGlobalTime()) {
		return;
	}
	if (LocalPlayer->BasicPlayerInfo.IsBounce) {
		//����һ�а���
		SDK->KEYBOARD.StopCombo();
		float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.70f;
		while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
			if (!LocalPlayer->BasicPlayerInfo.IsBounce || SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel)) {
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
		if (Temp_EnemyPos.DistTo(MyPosition) < LeftChargeRange && Temp_Closepos.DistTo(MyPosition) > LeftChargeRange) {
			ClosestMouseTarget = Enemy;
		}
	}
	if (!ClosestMouseTarget) {
		ClosestMouseTarget = Enemy;
	}
	//���������׶�
	bool HasJingKe = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_JingKeXianBi);
	bool HasGuiKu = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_GuiKuShenHao);
	auto NearPosition = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
	bool CanCastCharge = SDK->UTILITY.CanCastChargeByPosRange(NearPosition, SDK->KEYBOARD.IsPressKey(1) ? LeftChargeRange : RightChargeRange);
	bool IsCharge = Get_IsCharge(LocalPlayer);
	if (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel)) {
		//�õ���ǰ����ʱ��
		bool SuccessCC = false;
		while (SDK->ANIMHASH.IS_FLASHSTEP(LocalPlayer->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
			float CurTime = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
			//�ε� �Եз��������м��
			if (SDK->GAMEFUNC.GetGlobalTime() - ClosestMouseTarget->BasicActiontime.LastFlashTime < 0.1f ||
				SDK->GAMEFUNC.GetGlobalTime() - ClosestMouseTarget->BasicActiontime.LastFlashstepTime < 0.1f) {
				//�Է����� ����Ҫ��ץ ֱ�ӽ�������
				ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 1.f;
				LastControl = Control::DirectInput;
				return;
			}
			if (CurTime > 0.36f) {
				ResetCC();
				LastControl = Control::DirectInput;
				return;
			}
			if (CurTime > 0.31f) {

				if (CanCastCharge) {
					if (HasGuiKu) {
						//ӵ�й����ʱ 70%���������Ծ
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
						SDK->KEYBOARD.Set_IsActiveRight(true);
						std::this_thread::sleep_for(std::chrono::milliseconds(25));
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
						float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
						while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
							if (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel)) {
								//0.20 ~ 0.54f �ڴ������жϵ��˶���
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
					}
					//�ܹ����ȹ�
					if (HasJingKe) {
						//�����ͷŹ���~~~ ֱ�ӵ���CALL
						attach_3abcombo(LocalPlayer, ClosestMouseTarget, LastControl, false, true, HasJingKe, HasGuiKu, true);
						LastControl = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::FlashStep : Control::DirectInput;
						return;
					}
				}

			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	if (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel)) {

		while (SDK->ANIMHASH.IS_FLASH_JUMP(LocalPlayer->ActorModel) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {

			float CurTime = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
			if (CurTime >= 0.20f && CurTime <= 0.54f) {
				if (CanCastCharge) {
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
					SDK->KEYBOARD.Set_IsActiveRight(true);
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
					SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
					SDK->KEYBOARD.Set_IsActiveRight(false);
					std::wstring gkAnim;
					bool find = false;
					float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
					while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
						SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, gkAnim);
						if (gkAnim == skCrypt(L"male_dagger_flashjump_attack_soul_01")) {
							find = true;
							break;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					if (find) {
						//��޶���2.81f���2��a
						SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, ClosestMouseTarget, 2.81f);
						LastControl = Control::LongFlash;
						return;
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		LastControl = LocalPlayer->BasicPlayerInfo.CanExecuteFlash ? Control::FlashStep : Control::DirectInput;
		return;
	}
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
	float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
	float Distance = NearPosition.DistTo(MyPosition);
	if (IsCharge) {
		bool HasGuiKu = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_GuiKuShenHao);
		bool HasGuiRen = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_GuiRenAnZha);
		bool soulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_KangLongYouHui) && SDK->KEYBOARD.ActiveCombo(ActiveMode::SoulMode);
		bool KangLongMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Dagger_KangLongYouHui) && SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
		float realRange = SDK->KEYBOARD.IsPressKey(1) ? LeftChargeRange : RightChargeRange;
		if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel) && CanCastCharge) {
			if (Distance <= realRange) {
				if (CheckCanCastCharge(NearPosition, 4.f)) {
					if (!soulMode || KangLongMode) {
						bool IsLeft = false;
						if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
							SDK->KEYBOARD.Set_IsActiveLeft(false);
							IsLeft = true;
						}
						if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
							SDK->KEYBOARD.Set_IsActiveRight(false);
						}
						//ԥ������ ���� 2.33f ת���� 1.20f
						bool canSL = LocalPlayer->BasicPlayerInfo.CanExecuteFlash;
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (IsLeft ? (KangLongMode ? 2.15f : 0.68f) : (canSL ? (HasGuiRen ? 1.40f : 1.20f) : (HasGuiRen ? 1.84f : 2.33f)));
						while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
							if (LocalPlayer->BasicPlayerInfo.IsShocked || LocalPlayer->BasicPlayerInfo.IsBounce) {
								//�Լ����񵶾�����ѭ��
								againexecute = false;
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(1));
						}
						if (!againexecute) {
							LastControl = Control::LongFlash;
							return;
						}
						if (SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && SDK->GAMEFUNC.IsHiting(ClosestMouseTarget->ActorModel) && ((IsLeft && !KangLongMode) || HasGuiRen || (canSL && SDK->KEYBOARD.BasicLogic.ShortFlash('S', false) && SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, ClosestMouseTarget, 0.20f)))) {
							attach_3abcombo(LocalPlayer, ClosestMouseTarget, LastControl, IsLeft, true, HasJingKe, HasGuiKu);
						}
						else {
							LastControl = Control::LongFlash;
						}
						return;
					}
				}
			}
		}
		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && Distance > (SDK->KEYBOARD.IsPressKey(1) ? LeftChargeRange : RightChargeRange) && SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel) > 0.36f) {
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