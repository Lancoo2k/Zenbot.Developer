#include <Windows.h>
#include <thread>
#include "../../Zenbot_SDK/LC_SDK.h"

extern LC_SDK_STRUCT* SDK;

bool DianShaoLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Target) {

	std::wstring vname;
	bool sucget = SDK->GAMEFUNC.GetActionName(Target->ActorModel, vname);
	if (!SDK->UTILITY.CheckInRange(Target->ActorModel, 4.5f) || !SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
		SDK->KEYBOARD.StopCombo();
		return false;
	}
	bool success = false;
	float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
	while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
		if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Target->ActorModel) < 0.1f) {
			success = true;
			break;
		}
	}
	if (!success) {
		return false;
	}
	std::wstring Name;
	int lastLogic = 1;
	bool successFlag1 = false;
	bool successFlag2 = false;
	bool successFlag3 = false;
	int  DianShaoCount = 0;
	while (true) {
		bool logic1 = SDK->KEYBOARD.ActiveCombo(ActiveMode::All);
		bool logic2 = LocalPlayer->BasicPlayerInfo.HuoNan_V;
		bool logic3 = SDK->GAMEFUNC.IsHiting(Target->ActorModel);

		if (!logic1 || !logic2 || !logic3) {
			//SDK->GAMEFUNC.GetActionName(Target->ActorModel, Name);
			//printf("break: %d %d %d Name->%ls\n", logic1, logic2, logic3, Name.c_str());
			break;
		}

		if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && DianShaoCount < 1 && lastLogic == 1) {
			SDK->KEYBOARD.SendKeyBoard('S', 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 0);
			float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {

				if (SDK->ANIMHASH.IS_SPRINT(LocalPlayer->ActorModel)) {
					//printf("成功跑起来\n");
					SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 2);
					successFlag1 = true;
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			SDK->KEYBOARD.SendKeyBoard('C', 0);
			SDK->KEYBOARD.Set_IsActiveCrouch(true);
			//======================================滑铲升龙
			Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
				if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
					if (Name.find(skCrypt(L"sprint_downhill_in_01")) != std::wstring::npos) {
						if (SDK->KEYBOARD.IsPressKey('S')) {
							SDK->KEYBOARD.SendKeyBoard('S', 2);
						}
						if (SDK->KEYBOARD.Get_IsActiveCrouch()) {
							SDK->KEYBOARD.SendKeyBoard('C', 2);
							SDK->KEYBOARD.Set_IsActiveCrouch(false);
						}
						successFlag2 = true;
						break;
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (!successFlag2) {
				if (SDK->KEYBOARD.IsPressKey('S')) {
					SDK->KEYBOARD.SendKeyBoard('S', 2);
				}
				if (SDK->KEYBOARD.Get_IsActiveCrouch()) {
					SDK->KEYBOARD.SendKeyBoard('C', 2);
					SDK->KEYBOARD.Set_IsActiveCrouch(false);
				}
				return false;
			}
			SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
			SDK->KEYBOARD.Set_IsActiveRight(true);
			Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.25f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
				if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
					if (Name.find(skCrypt(L"slide_attack_heavy_01")) != std::wstring::npos) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
						DianShaoCount++;
						successFlag3 = true;
						break;
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			if (!successFlag3) {
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
				SDK->KEYBOARD.Set_IsActiveRight(false);
			}
			Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.6f;
			while (Tick > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
				if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(LocalPlayer->ActorModel) < 0.1f) {
					break;
				}
			}
		}
		else {
			//printf("站立逻辑\n");
			lastLogic = 2;
			std::wstring Name;
			if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) 
			{
				if (SDK->ANIMHASH.IS_IDLE(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel)) {
					if (SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
						//DbgPrint("重置");
						DianShaoCount = 0;
						float Tick = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
						bool first = false;
						while (Tick > SDK->GAMEFUNC.GetGlobalTime() && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
							if (!first && SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Target->ActorModel) < 0.1f) {
								first = true;
								break;
							}
							if (first && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
								break;
							}
						}
						lastLogic = 1;
					}
				}
			}
		}
	}
	if (SDK->KEYBOARD.IsPressKey(VK_SHIFT)) {
		SDK->KEYBOARD.SendKeyBoard(VK_SHIFT, 2);
	}
	if (SDK->KEYBOARD.IsPressKey('S')) {
		SDK->KEYBOARD.SendKeyBoard('S', 2);
	}
	if (SDK->KEYBOARD.IsPressKey('C')) {
		SDK->KEYBOARD.SendKeyBoard('C', 2);
		SDK->KEYBOARD.Set_IsActiveCrouch(false);
	}
	if (SDK->KEYBOARD.Get_IsActiveRight()) {
		SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
		SDK->KEYBOARD.Set_IsActiveRight(false);
	}
	return true;
}