#include <Windows.h>
#include <thread>
#include "../../LC_SDK/LC_SDK.h"
#include <random>

std::string CFGName = skCrypt("官方太刀配置");
std::string CONFIG = skCrypt("CONFIG");

LC_SDK_STRUCT* SDK = nullptr;

bool TryStealAA = true;
bool EnableHighACCALogic = true;
bool EnableForceFlashWithFlashStep = true;
bool EnableExploit = true;
int   randomRate = 6;
float MaxChargeTime = 1.36f;

bool TryUseSkill = true;

bool DianShaoLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Target);

namespace LC_Katana {
	enum Control {
		LongFlash = 0,
		CCReset = 1,
		DirectInput = 2,
		UseSkill = 3
	};
	//蓄力攻击范围
	const float LeftAARange = 4.5f;
	const float RightAARange = 5.2f;
	const float CARange = 5.3f;
	const float CBRange = 5.0f;
	const float JumpAARange = 5.5f;
	const float ShockAARange = 12.f;
	const float ChargeRange = 5.5f;
	const float SoulRange = 10.f;
	//下一次操作
	int LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;

	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;

	float ForceUseLeft = 0;
	float ForceUseRight = 0;

	float GetMaxHurt(LC_PLAYER_INFO* Player) {
		return SDK->GAMEFUNC.GetActionVaildTime(Player->ActorModel).y;
	}
	int GetNextChargeType(LC_PLAYER_INFO* LocalPlayer,LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack, float Distance) {
		if (ForceUseRight > SDK->GAMEFUNC.GetGlobalTime())
			return 2;
		if (ForceUseLeft > SDK->GAMEFUNC.GetGlobalTime())
			return 1;
		if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel) || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel)) {
			return 2;
		}
		if (SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastChongQuanTime < 1.0f) {
			return 1;
		}
		auto WepId = SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel);
		if ((SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(SDK->GAMEFUNC.GetLocalPlayer()) == 1 || 
			(WepId != 102 && WepId != 106) || 
			SDK->ANIMHASH.IS_IDLE(Enemy->ActorModel) ||
			EnemyIsDontAttack || 
			SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel) ||
			SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastParryTime < 0.6f || 
			!SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel))) && Distance <= LeftAARange) {

			return 1;
		}
		return 2;
	}

	bool IsInAttackRange(float Distance, float ExtraRange = 0.f) {
		return Distance <= ((SDK->KEYBOARD.Get_IsActiveLeft() ? LeftAARange : RightAARange) + ExtraRange);
	}

	int GetRealHealth(LC_PLAYER_INFO* Player) {
		return Player->BasicPlayerInfo.Property.CurHp + Player->BasicPlayerInfo.Property.Curshield;
	}

	bool IsFastStatck(LC_PLAYER_INFO* Player) {
		std::wstring Name;
		if (SDK->GAMEFUNC.GetActionName(Player->ActorModel, Name)) {
			return (Name.find(skCrypt(L"_attack_light")) != std::wstring::npos || Name.find(skCrypt(L"_attack_heavy")) != std::wstring::npos) && (Name.find(skCrypt(L"_03")) != std::wstring::npos || Name.find(skCrypt(L"_05")) != std::wstring::npos || Name.find(skCrypt(L"_09")) != std::wstring::npos) && Name.find(skCrypt(L"_pre")) == std::wstring::npos && Name.find(skCrypt(L"_charge")) == std::wstring::npos;
		}
		return false;
	}

	bool Get_IsCharge(LC_PLAYER_INFO* Player) {
		return SDK->GAMEFUNC.GetPlayerEndureLevel(Player->ActorModel) == 20 && (SDK->GAMEFUNC.PlayerIsCharge(Player->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(Player->ActorModel)) && !IsFastStatck(Player);
	}
	void CheckCastSkill(LC_PLAYER_INFO* Player) {
		auto Data = SDK->GAMEFUNC.GetAppointSkill(Player->ActorModel, 1);
		if (Data.OutputKey == 'F' && TryUseSkill) {
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
			if (IsSuccess) {
				Tick = SDK->GAMEFUNC.GetGlobalTime() + Data.ExecuteAnimTime;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
		LastControl = Control::LongFlash;
	}
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer)
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

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
		return false;
	}
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, int& NextInput) {
		if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
			NextInput = Control::DirectInput;
			return false;
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(true);
		if (SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) && AttackShock(LocalPlayer))
		{
			auto SoulMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_DuoHun);
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, SoulMode ? 0.51f : 0.98f)) {
				SDK->KEYBOARD.StopCombo();
				return false;
			}
			if (!SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 0);
			}
			do {
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, ShockAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, false)) {
					if (SDK->KEYBOARD.IsPressKey('W')) {
						SDK->KEYBOARD.SendKeyBoard('W', 2);
					}
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

			} while (!SoulMode && (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel)) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && !SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) && !LocalPlayer->BasicPlayerInfo.IsBounce);

			if (SDK->KEYBOARD.IsPressKey('W')) {
				SDK->KEYBOARD.SendKeyBoard('W', 2);
			}

			NextInput = Control::LongFlash;
			SDK->KEYBOARD.Set_IsExecuteCombo(false);
			return true;
		}
		NextInput = Control::LongFlash;
		SDK->KEYBOARD.StopCombo();
		return false;
	}
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, int& NextInput) {
		//设置一个50%概率 是直接升龙 还是继续中闪ACCA找机会 前提是对方没有马刀、阔刀
		int WepId = SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel);
		bool againACCA = SDK->UTILITY.GetRandomIntEx(1, 10) <= randomRate;
		bool IsMaxBreak = false;
		if (EnableHighACCALogic && againACCA && WepId != 119 && WepId != 103 && !LocalPlayer->BasicPlayerInfo.HuoNan_V)
		{
			do {
				if (SDK->KEYBOARD.BasicLogic.ExecuteCC()) {
					//目标没有倒地 继续进行连招
					if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (GetMaxHurt(Enemy) >= 0.77f || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						break;
					}
					else {
						if (!LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
							if (!SDK->KEYBOARD.BasicLogic.ExecuteCC()) {
								SDK->KEYBOARD.StopCombo();
								NextInput = Control::DirectInput;
								return false;
							}
						}
						else {
							if (!SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0)) {
								//中闪失败时直接返回 按理说不会失败 多一个保险
								SDK->KEYBOARD.StopCombo();
								NextInput = Control::DirectInput;
								return false;
							}
						}
					}
					//这里再出一次刀 这里很关键 判断是否继续ACCA
					if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (GetMaxHurt(Enemy) >= 0.77f || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						break;
					}
				}
			} while (SDK->KEYBOARD.ActiveCombo(ActiveMode::All));

			//if (SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) && GetMaxHurt(Enemy) < 0.67f) {
			//	//出2A
			//	if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, true)) {
			//		SDK->KEYBOARD.StopCombo();
			//		NextInput = Control::DirectInput;
			//		return false;
			//	}
			//	if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
			//		SDK->KEYBOARD.StopCombo();
			//		NextInput = Control::DirectInput;
			//		return false;
			//	}
			//	//打完连招 返回.
			//	NextInput = Control::DirectInput;
			//	SDK->KEYBOARD.StopCombo();
			//	return true;
			//}
		}
		//这里直接转为升龙 如果ACCA僵直在0.77~1.00时 正好可以接一个
		bool ActiveChainShoot = SDK->KEYBOARD.ActiveCombo(ActiveMode::ChainShootMode) && SDK->GAMEFUNC.GetItemCount(ItemId::ChainShoot) > 0;
		if (!LocalPlayer->BasicPlayerInfo.CanExecuteFlash || ActiveChainShoot) {
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteCB()) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (ActiveChainShoot) {
				SDK->KEYBOARD.SendKeyBoard('Q', 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				SDK->KEYBOARD.SendKeyBoard('Q', 2);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
				SDK->KEYBOARD.Set_IsActiveLeft(true);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				float MaxTick = SDK->GAMEFUNC.GetGlobalTime() + 3.f;
				bool successflag = false;
				while (SDK->GAMEFUNC.GetGlobalTime() < MaxTick) {
					if (Get_IsCharge(LocalPlayer)) {
						DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 0.7f;
						SDK->KEYBOARD.Set_IsExecuteCombo(false);
						return true;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
				SDK->KEYBOARD.Set_IsActiveLeft(false);
				SDK->KEYBOARD.StopCombo();
				return false;
			}
			else {
				//尝试浮空百裂斩
				std::wstring nnn;
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
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
				//判断敌人血量
				if (GetRealHealth(Enemy) <= 420) {
					//低血量使用B+CA斩杀
					if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CARange) || !SDK->KEYBOARD.BasicLogic.ExecuteCA()) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.40f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
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
					SDK->KEYBOARD.Set_IsExecuteCombo(false);
					NextInput = Control::LongFlash;
					return true;
				}
				else {
					//浮空百裂斩 使用
					if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
					//判断下如果1A没打中就直接转2B
					float gTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
					if (gTime > 0.2f) {
						//if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
						//	SDK->KEYBOARD.StopCombo();
						//	NextInput = Control::DirectInput;
						//	return false;
						//}
						//if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
						//	SDK->KEYBOARD.StopCombo();
						//	NextInput = Control::DirectInput;
						//	return false;
						//}
						//没打中原地捏蓝双压
						ForceUseRight = SDK->GAMEFUNC.GetGlobalTime() + 5.f;
						SDK->KEYBOARD.Set_IsExecuteCombo(false);
						NextInput = Control::DirectInput;
						return true;
					}
					else {
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
						if (SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
							std::wstring Name;
							SDK->GAMEFUNC.GetActionName(Enemy->ActorModel, Name);
							//如果目标被打倒地了 补跳A
							if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
								SDK->KEYBOARD.StopCombo();
								NextInput = Control::DirectInput;
								return false;
							}
							float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.6f;
							while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
								if (SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel) || SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel)) {
									std::this_thread::sleep_for(std::chrono::milliseconds(50));
									break;
								}
								if (SDK->GAMEFUNC.IsHiting(LocalPlayer->ActorModel) || LocalPlayer->BasicPlayerInfo.IsBounce) {
									break;
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(1));
							}
							SDK->KEYBOARD.Set_IsExecuteCombo(false);
							NextInput = Control::LongFlash;
							return true;
						}
						else {
							//如果2A就打中了一下. 判断下HitTime
							float oTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
							if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
								if (oTime > 0.1f) {
									//没打中 直接出3B
									if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
										SDK->KEYBOARD.StopCombo();
										NextInput = Control::DirectInput;
										return false;
									}
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.80);
									SDK->KEYBOARD.Set_IsExecuteCombo(false);
									NextInput = Control::LongFlash;
									return true;
								}
								else {
									ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
									SDK->KEYBOARD.Set_IsExecuteCombo(false);
									NextInput = Control::DirectInput;
									return true;
								}
							}
							else {
								std::wstring Name;
								SDK->GAMEFUNC.GetActionName(Enemy->ActorModel, Name);
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
								SDK->KEYBOARD.Set_IsExecuteCombo(false);
								NextInput = Control::LongFlash;
								return true;
							}
						}
					}
				}
			}
		}
		else {
			//滑铲升龙
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteSliderB())
			{
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if ((EnableExploit && LocalPlayer->BasicPlayerInfo.CanExecuteFlash && GetRealHealth(Enemy) <= 750)) {
				//如果能闪 使用拳切技巧 提前0.2s预输入切拳
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.4f)) {
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
				SDK->KEYBOARD.SendKeyBoard(localweapon, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				SDK->KEYBOARD.SendKeyBoard(localweapon, 2);
				SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTDOWN);
				SDK->KEYBOARD.Set_IsActiveLeft(true);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				float MaxChargeTime = SDK->GAMEFUNC.GetGlobalTime() + 0.5f;
				while (MaxChargeTime > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel) || SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel)) {
						//printf("DONT WHITE!\n");
						DontWhiteAttackTime = SDK->GAMEFUNC.GetGlobalTime() + 1.f;
						SDK->KEYBOARD.Set_IsExecuteCombo(false);
						return true;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
			else {
				//按下W键
				//0.60f为最高升龙点
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.7f)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				SDK->KEYBOARD.SendKeyBoard('W', 0);
				bool success_land = false;
				float Tick = SDK->GAMEFUNC.GetGlobalTime() + 2.0f;
				while (Tick > SDK->GAMEFUNC.GetGlobalTime()) {
					if (SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) ||
						SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(LocalPlayer->ActorModel)) {
						success_land = true;
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				if (!success_land || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel) || !SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
					SDK->KEYBOARD.SendKeyBoard('W', 2);
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
				SDK->KEYBOARD.SendKeyBoard('W', 2);
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
				if (SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
					//如果目标被打倒地了 补跳A
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
					SDK->KEYBOARD.Set_IsExecuteCombo(false);
					NextInput = Control::LongFlash;
					return true;
				}
				else {
					//如果2A就打中了一下. 判断下HitTime
					float oTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
					if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
						if (oTime > 0.1f) {
							//没打中 直接出3B
							if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0)) {
								SDK->KEYBOARD.StopCombo();
								NextInput = Control::DirectInput;
								return false;
							}
							SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.80);
							SDK->KEYBOARD.Set_IsExecuteCombo(false);
							NextInput = Control::LongFlash;
							return true;
						}
						else {
							ForceUseLeft = SDK->GAMEFUNC.GetGlobalTime() + 0.2f;
							SDK->KEYBOARD.Set_IsExecuteCombo(false);
							NextInput = Control::DirectInput;
							return true;
						}
					}
					else {
						if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, JumpAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true)) {
							SDK->KEYBOARD.StopCombo();
							NextInput = Control::DirectInput;
							return false;
						}
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
						SDK->KEYBOARD.Set_IsExecuteCombo(false);
						NextInput = Control::LongFlash;
						return true;
					}
				}
			}
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		NextInput = Control::DirectInput;
		return true;
	}
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, int& NextInput, bool IsAAAfter = false) {
		if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
			NextInput = Control::DirectInput;
			return false;
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(true);
		if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, RightAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(2, 0))) {
			//跑B的出招速度有所不同
			bool IsRunningB = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel);
			float RealDelay = IsRunningB ? 0.35f : 0.30f;
			bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
			bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
			if (!Logic1 || !Logic2) {
				//如果这里没打中 判断下对面是闪了还是怎样.防止被闪A抓A闪
				if (!Logic2) {
					if (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f ||
						SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f) {
						//对面闪了 就原地捏蓝防止再被反抓
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
				}
				SDK->KEYBOARD.StopCombo();
				NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 ? Control::UseSkill : Control::LongFlash;
				return false;
			}
			//这里判断敌方僵直情况
			if (GetMaxHurt(Enemy) <= 0.43f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || GetRealHealth(Enemy) <= 240 || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
				//这里直接跳A结束连招
				//没死就出个跳A
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
				NextInput = Control::LongFlash;
				return true;
			}
			//中闪过去抓
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				if (!SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0)) {
					//中闪失败时直接返回 按理说不会失败 多一个保险
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				//没精力用CC;
			}
			//直接左A.如果没打中或者僵直不够则直接返回假.继续写升龙就好了
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			return AllUse_AfterCombo(LocalPlayer, Enemy, NextInput);
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(false);
		SDK->KEYBOARD.StopCombo();
		return false;
	}
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, int& NextInput, bool IsAAAfter = false) {
		if (SDK->KEYBOARD.Get_IsExecuteCombo()) {
			NextInput = Control::DirectInput;
			return false;
		}
		SDK->KEYBOARD.Set_IsExecuteCombo(true);
		if (IsAAAfter || (SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) && SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0))) {
			bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f);
			bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
			if (!Logic1 || !Logic2) {
				if (!Logic2) {
					if (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f ||
						SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f) {
						//对面闪了 就原地捏蓝防止再被反抓
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
				}
				//printf("LOGIN1 ERROR! %d  %d", Logic1, Logic2);
				SDK->KEYBOARD.StopCombo();
				NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 ? Control::UseSkill : Control::LongFlash;
				return false;
			}
			if (LocalPlayer->BasicPlayerInfo.HuoNan_V && LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				DianShaoLogic(LocalPlayer, Enemy);
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::LongFlash;
				return false;
			}
			//这里判断敌方僵直情况
			if (GetMaxHurt(Enemy) < 0.77f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) || GetRealHealth(Enemy) <= 240 || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
				//这里直接跳A结束连招
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
				NextInput = Control::LongFlash;
				return true;
			}
			if (!SDK->KEYBOARD.BasicLogic.ExecuteCC()) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, LeftAARange) || !SDK->KEYBOARD.BasicLogic.ExecuteAA(1, 0)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//连招被躲 或 对方开出技能 此时原地捏蓝返回.
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//这里判断一下血量 是否可以快速斩杀 不无脑升龙
			if (GetRealHealth(Enemy) <= 500) {
				//此时出2A + 跳A斩杀
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
				if (SDK->GAMEFUNC.IsDead(Enemy->ActorModel)) {
					//如果目标死亡了 连招直接返回并进入捏蓝
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return true;
				}
				//没死就出个跳A
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
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {

		auto EnemyPosition = SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel);
		auto MyPosition = SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel);
		float Distance = EnemyPosition.DistTo(MyPosition);

		bool IsCharge = Get_IsCharge(LocalPlayer);

		bool EnemyIsDontAttack = SDK->ANIMHASH.Action_HuoNan_IsF(Enemy->ActorModel) || Enemy->BasicPlayerInfo.GuQingHan_F;

		if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
			int ChargeType = GetNextChargeType(LocalPlayer,Enemy, EnemyIsDontAttack, Distance);
			auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
			if (SDK->GAMEFUNC.GetGlobalTime() - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) {
				//printf("进入受击判断\n");
				//这里是 进入连招之前 自身受到僵直时间 < 0.1f.  刚从被打到可以捏蓝.
				LastControl = Control::DirectInput;
				ChargeType = 1;
			}
			if (LastControl == Control::LongFlash)
			{
				if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.5f)
				{
					if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel)) {

						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(ChargeType, 'W', false, 0)) {
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
		BOOL CanToAttack = SDK->GAMEFUNC.PlayerIsChargeRelay(LocalPlayer->ActorModel) && StackValue != 3 && StackValue != 5 && StackValue != 9 && ChargeTime < 0.49f;
		//printf("开始执行222 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
		if (IsCharge) {
			//printf("IS CHARGE\n");
			int level = SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel);
			bool HasQingGui = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_QingGui);
			bool ShiHunZhanMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_ShiHunZhan) && level == 2;
			int EnemyEndureLevel = SDK->GAMEFUNC.GetPlayerEndureLevel(Enemy->ActorModel);

			if (!EnemyIsDontAttack) {
				bool A3Mode = StackValue == 3 && !SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel);

				float ChainShotTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastChainTime;

				float FlashExecuteTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime;

				float FlashStepTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime;

				bool isbigMode = Enemy->BasicPlayerInfo.Dafo_V || Enemy->BasicPlayerInfo.ShenMiao_V || Enemy->BasicPlayerInfo.YueShan_V || Enemy->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(Enemy->ActorModel) == 6 ||
					SDK->GAMEFUNC.GetHeroBattleStatus(Enemy->ActorModel) == 1 || Enemy->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
				bool isHitMode = Enemy->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || Enemy->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel)) || ChainShotTime < 0.5f || Enemy->BasicPlayerInfo.CuiSanNiang_F || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || Enemy->BasicPlayerInfo.GuQingHan_Frost;

				bool hurtlandFlag = SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel);

				float LastBladeParryTime = SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastBladeParryTime;

				bool CanCastCharge = Enemy->BasicPlayerInfo.WuTianF1Tick < SDK->GAMEFUNC.GetGlobalTime();

				bool ShuiPaoMode = false;

				if (Enemy->BasicPlayerInfo.CuiSanNiang_F) {
					float FewTime = Enemy->BasicActiontime.CuiSanNiangFHurtActionTime - SDK->GAMEFUNC.GetGlobalTime();
					ShuiPaoMode = ChargeTime + FewTime > 1.10f;
				}

				if (Enemy->BasicPlayerInfo.GuQingHan_Frost) {
					float FewTime = Enemy->BasicActiontime.GuQingHanFrostActionTime - SDK->GAMEFUNC.GetGlobalTime();
					ShuiPaoMode = ChargeTime + FewTime > 1.10f;
				}

				//printf("开始执行333 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
				if (Distance <= ChargeRange && LastBladeParryTime > 1.2f && CanCastCharge) {
					bool CanNext = false;
					if (A3Mode && !SDK->KEYBOARD.IsPressKey(1)) {
						CanNext = (hurtlandFlag && SDK->KEYBOARD.IsPressKey(2)) || isHitMode;
					}
					else if(IntoCharge) {
						CanNext = isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || GetMaxHurt(Enemy) >= 0.5f;
					}
					//如果巨大化 / 落地状态 / 抓闪 /必中状态下 进入了蓄力层级就释放. 或者在落地状态且不是左键蓄力而且进入A3状态时释放右键.
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
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (A3Mode ? (HasQingGui ? 1.1f : 0.8f) : (ShiHunZhanMode ? 1.1f : 0.6f));
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
						bool Soul_JingLei = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_JingLeiShiJie);

						if (!A3Mode  && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || (Soul_JingLei && SDK->UTILITY.GetRandomIntEx(1, 10) <= 5))) {
							//释放惊雷十劫 || 必中惊雷
							//DbgPrint("释放惊雷十劫 按住右键");
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
							SDK->KEYBOARD.Set_IsActiveRight(true);
							std::wstring Name;

							bool success = false;

							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
								if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
									if (Name.find(skCrypt(L"katana_attack_heavy_09_pre")) != std::wstring::npos) { //太刀右键_惊雷十劫(太刀蓄力之后的右键追击魂玉)
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
										SDK->KEYBOARD.Set_IsActiveRight(false);
										success = true;
										break;
									}
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(1));
							}
							if (success) {
								float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (Soul_JingLei ? 1.64f : 0.84f);
								while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
									if (LocalPlayer->BasicPlayerInfo.IsShocked) {
										//自己被振刀就跳出循环
										break;
									}
								}
							}
							else {
								if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
									//DbgPrint("没释放出惊雷十劫.松开鼠标右键");
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
								}
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

				if (CanToAttack && DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime()) {

					//printf("NO1 WHITE!\n");

					if (TryStealAA && StackValue == 1) {

						bool CanTwoExecuteFlash = LocalPlayer->BasicPlayerInfo.Property.CurEnergy - (LocalPlayer->BasicPlayerInfo.Property.ConsumEnergy * 2.f) > -200;

						if (CanTwoExecuteFlash && IsInAttackRange(Distance)) {

							float time = SDK->GAMEFUNC.GetChargeTime(Enemy->ActorModel);

							float RealDelay = SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) ? 0.35f : 0.30f;

							float StealOffset = 0.49f - RealDelay;

							if (time >= StealOffset && time <= StealOffset + 0.05f) { //0.33f 

								if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {

									if (StackValue == 1) {

										//处于一段攻击时
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
										SDK->KEYBOARD.Set_IsActiveRight(false);
										std::this_thread::sleep_for(std::chrono::milliseconds(100));
										SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
										LastControl = ((SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f) || (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f)) ? Control::DirectInput : Control::LongFlash;
										return;
									}
								}
							}
						}
					}

					if (!Enemy->BasicPlayerInfo.YinZiPing_F2 && EnemyEndureLevel != 20 && EnemyEndureLevel != 31 && EnemyEndureLevel != 50 &&
						(Enemy->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IS_IDLE(Enemy->ActorModel) || SDK->ANIMHASH.IS_RUNNING(Enemy->ActorModel)  || (!Enemy->BasicPlayerInfo.CuiSanNiang_F && !Enemy->BasicPlayerInfo.GuQingHan_Frost && !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel))) ||
							SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(Enemy->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(Enemy->ActorModel) || ((hurtlandFlag || SDK->ANIMHASH.IS_WALKWALL(Enemy->ActorModel) && EnemyEndureLevel != 20) && !SDK->KEYBOARD.IsPressKey(1)) || (GetMaxHurt(Enemy) >= 0.30f && LocalPlayer->BasicPlayerInfo.HuoNan_V) ||
							ParryMode || ChainShotTime < 0.5f || (FlashExecuteTime <= 0.1f && (!SDK->KEYBOARD.IsPressKey(1) || Enemy->BasicActiontime.LastFlashType != EVADE_TYPE::EVADE_Double) && (LocalPlayer->BasicActiontime.LastFlashTime < Enemy->BasicActiontime.LastFlashTime)))) {

						if (IsInAttackRange(Distance)) {

							if (SDK->KEYBOARD.Get_IsActiveLeft() && SDK->KEYBOARD.IsPressKey(1)) {

								if (StackValue == 1) {

									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									//这里延迟一会 怕动作未完全展开
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
									if (!Left_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer);
									}
									return;
								}
								if (StackValue == 2) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.70f);
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
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
									if (!Right_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer);
									}
									return;
								}
								if (StackValue == 2) {
									//处于一段攻击时
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//这里延迟一会 怕动作未完全展开
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
									SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.60f);
									LastControl = Control::DirectInput;
									return;
								}
							}
						}
						else if (Distance <= JumpAARange) {
							SDK->KEYBOARD.BasicLogic.ExecuteJumpAA(1, true);
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
							LastControl = EnemyPosition.DistTo(MyPosition) <= LeftAARange ? Control::DirectInput : Control::LongFlash;
							return;
						}
					}
				}
				if (!CanToAttack && LocalPlayer->BasicPlayerInfo.CanExecuteFlash && ParryMode) {
					
					
					//超出放蓄时间时. 例如对方在2.0时出振刀 脚本会来不及读取ParryBcakSwingTime. 所以判断超出时间后转中闪抓!

					float TransitionTime = Enemy->BasicHurtInfo.ParryBackSwingTransitionTime - SDK->GAMEFUNC.GetGlobalTime();

					if (MaxChargeTime < ChargeTime + TransitionTime) {

						if (Distance <= LeftAARange + 3.f) {

							if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0)) {
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
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && DontWhiteAttackTime < SDK->GAMEFUNC.GetGlobalTime()) {
				//如果目标不在攻击范围内时
				bool x1flag = !IsInAttackRange(Distance);
				bool x2flag = EnableForceFlashWithFlashStep && SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime <= 0.2f;
				if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
					if (CanToAttack && StackValue == 1 && SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) && !IsInAttackRange(Distance, 2.f)) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
						//这里延迟一会 怕动作未完全展开
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.35f);
						SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(LocalPlayer,Enemy, EnemyIsDontAttack, Distance), 'W', false, 0);
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
						return;
					}
				}
				if ((x1flag && ChargeTime > 0.24f) || x2flag) {
					SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(2, 'W', false, 0);
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
					auto boolean = SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(GetNextChargeType(LocalPlayer,Enemy, EnemyIsDontAttack, Distance), SDK->KEYBOARD.BasicLogic.GetBestDashKey(Enemy, Distance), false, 0);
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
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
		//拼刀事件g
		if (LocalPlayer->BasicPlayerInfo.IsBounce ) {
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
		if (!ClosestMouseTarget) {
			ClosestMouseTarget = Enemy;
		}
		bool IsCharge = Get_IsCharge(LocalPlayer);
		if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
			if (LastControl == Control::LongFlash) {
				auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
				if (SDK->GAMEFUNC.GetGlobalTime() - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) {
					//printf("进入受击判断\n");
					//这里是 进入连招之前 自身受到僵直时间 < 0.1f.  刚从被打到可以捏蓝.
					LastControl = Control::DirectInput;
				}
				if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && SDK->GAMEFUNC.GetGlobalTime() - LocalPlayer->BasicActiontime.LastFlashTime > 0.5f)
				{
					if (!SDK->ANIMHASH.IS_RUNNING(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_DODGE(LocalPlayer->ActorModel) && !SDK->ANIMHASH.IS_Inertia(LocalPlayer->ActorModel)) {
						StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();

						if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0)) {
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
		}
		//获取基础信息
		auto NearPosition = SDK->GAMEFUNC.GetPlayerPosition(ClosestMouseTarget->ActorModel);
		float Distance = NearPosition.DistTo(MyPosition);
		float ChargeTime = SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel);
		bool CanCastCharge = SDK->UTILITY.CanCastChargeByPosRange(NearPosition, ChargeRange);
		if (IsCharge) {
			bool soulMode = SDK->KEYBOARD.ActiveCombo(ActiveMode::SoulMode) && SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_ShiHunZhan);
			bool ShiHunZhanMode = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_ShiHunZhan) && SDK->GAMEFUNC.GetChargeLevel(LocalPlayer->ActorModel) == 2;
			if (SDK->GAMEFUNC.PlayerIsCharge(LocalPlayer->ActorModel) && CanCastCharge) {
				if (Distance <= (soulMode ? SoulRange : ChargeRange)) {
					bool isbigMode = ClosestMouseTarget->BasicPlayerInfo.Dafo_V || ClosestMouseTarget->BasicPlayerInfo.ShenMiao_V || ClosestMouseTarget->BasicPlayerInfo.YueShan_V || ClosestMouseTarget->BasicPlayerInfo.YaoDaoji_V || SDK->GAMEFUNC.GetBeControlledStatus(ClosestMouseTarget->ActorModel) == 6 ||
						SDK->GAMEFUNC.GetHeroBattleStatus(ClosestMouseTarget->ActorModel) == 1 || ClosestMouseTarget->BasicPlayerInfo.NingHongYeTick > SDK->GAMEFUNC.GetGlobalTime();
					bool isHurtMode = SDK->ANIMHASH.IS_JUMP_DOWN_TO_LAND(ClosestMouseTarget->ActorModel) || SDK->ANIMHASH.IS_JUMPAA_RIGHT_RECOVER(ClosestMouseTarget->ActorModel) || ClosestMouseTarget->BasicPlayerInfo.IsBounce || SDK->GAMEFUNC.GetPlayerEndureLevel(ClosestMouseTarget->ActorModel) == 6;
					bool isHitMode = ClosestMouseTarget->BasicHurtInfo.AllowCastChargeTime > SDK->GAMEFUNC.GetGlobalTime() || ClosestMouseTarget->BasicHurtInfo.ParryBcakSwingTime > SDK->GAMEFUNC.GetGlobalTime() ||
						ClosestMouseTarget->BasicHurtInfo.AllowNormalAttackTime > SDK->GAMEFUNC.GetGlobalTime() || SDK->ANIMHASH.IsHurtFlayingToLand(ClosestMouseTarget->ActorModel) ||
						SDK->ANIMHASH.IS_IDLE(ClosestMouseTarget->ActorModel) || SDK->ANIMHASH.IS_RUNNING(ClosestMouseTarget->ActorModel) || SDK->ANIMHASH.IsInjuredLie(ClosestMouseTarget->ActorModel) || !SDK->UTILITY.IsCanDodgeWeapon(SDK->GAMEFUNC.GetWeaponID(ClosestMouseTarget->ActorModel));
					if (soulMode ? ShiHunZhanMode : (isbigMode || isHurtMode || isHitMode)) {

						if (SDK->KEYBOARD.IsPressKey(1) && SDK->KEYBOARD.Get_IsActiveLeft()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
							SDK->KEYBOARD.Set_IsActiveLeft(false);
						}
						if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
							SDK->KEYBOARD.Set_IsActiveRight(false);
						}
						bool againexecute = true;
						float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + (ShiHunZhanMode ? 1.1f : 0.6f);
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
						if (SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_JingLeiShiJie) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
							//释放惊雷十劫
							//DbgPrint("释放惊雷十劫 按住右键");
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
							SDK->KEYBOARD.Set_IsActiveRight(true);
							std::wstring Name;

							bool success = false;

							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
								if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
									if (Name.find(skCrypt(L"male_katana_attack_heavy_09_pre")) != std::wstring::npos) { //太刀右键_惊雷十劫(太刀蓄力之后的右键追击魂玉)
										SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
										SDK->KEYBOARD.Set_IsActiveRight(false);
										success = true;
										break;
									}
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(1));
							}
							if (success) {
								float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 1.44f;
								while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
									if (LocalPlayer->BasicPlayerInfo.IsShocked) {
										//自己被振刀就跳出循环
										break;
									}
								}
							}
							else {
								if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
									//DbgPrint("没释放出惊雷十劫.松开鼠标右键");
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
								}
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
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash && Distance > (SDK->KEYBOARD.ActiveCombo(ActiveMode::SoulMode) && SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_ShiHunZhan) ? SoulRange : ChargeRange) && SDK->GAMEFUNC.GetChargeTime(LocalPlayer->ActorModel) > 0.36f) {
				SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0);
				StartChargeTime = SDK->GAMEFUNC.GetGlobalTime();
				return;
			}
			//重新蓄力阶段
			if (ChargeTime >= MaxChargeTime) {
				if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash)
				{
					SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', false, 0);
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
}

LC_Function LC_SDK_STRUCT* InitLCSDK(LC_SDK_STRUCT* Ptr) {
	SDK = Ptr;
	return SDK;
}

LC_Function void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {

	/*

		在该函数调用时.说明脚本已经开始执行连招代码
		具体请看代码示例. 连招基础结构可以改变 但最好以示例方式进行编写.
		如果你编写的脚本仅支持若干个武器.请一定要通过switch 或 if 判断武器. 并在DrawingCode中编写代码控制是否激活此脚本的该武器逻辑! 不然可能会发生逻辑冲突!
		例如火男开大 冰女开大 季莹莹开大. 你都需要对此函数进行负责. 假如你编写了太刀的脚本 但是没有火男颠勺逻辑 但是你编写了一个火男单板颠勺. 那么二者大概率无法兼容
		所以要么不写 要么就一定要写好写完. 如果你对一个英雄写单板插件.也仅仅是火男F 或者武田躲刀 宁红夜自动F破魔禁疗之类的. 不要试图干涉其他脚本的逻辑.

	*/

	int WeaponID = SDK->GAMEFUNC.GetWeaponID(LocalPlayer->ActorModel);

	//printf("%d\n", SDK->GAMEFUNC.GetRealWeaponID(LocalPlayer->ActorModel));
	if (WeaponID == 102) {

		/*std::wstring name;
		SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, name);
		auto g = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
		float hhh = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);

		printf("%d %f | %ls | enemyhurt:%f | %f | %d\n", SDK->GAMEFUNC.GetPlayerHeroID(LocalPlayer->ActorModel),g, name.c_str(), SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y, hhh,SDK->GAMEFUNC.GetStateNameHash(LocalPlayer->ActorModel));
		*///if (hhh <= 0.04f) {
		//	
		//}



		//if (SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel) < 0.05f) {
		//	printf("GameTime:%f | %f %f %f | %f HashName:%d\n", SDK->GAMEFUNC.GetGlobalTime(),g.x, g.y, g.z, SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel), SDK->GAMEFUNC.GetStateNameHash(LocalPlayer->ActorModel));
		//}
		LC_Katana::RunableLogic(LocalPlayer, Enemy);
	}

}

LC_Function void LibraryInit() {

	/*

		在该函数调用时.说明脚本已经成功运行 这里是初始化你脚本的操作
		请最好不加任何高延迟代码 例如 Sleep(1000)乃至更久 这样会卡主辅助主进程. 后面的一些脚本加载可能导致缓慢.

	*/
	char szBuf[64];
	sprintf_s(szBuf, "%d", VK_CAPITAL);

	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("TryStealAA"), skCrypt("1"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("EnableHighACCALogic"), skCrypt("1"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("ACCACCARate"), skCrypt("7"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("EnableExploit"), skCrypt("1"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("TryUseSkillAntiAttack"), skCrypt("1"));
	SDK->CORE.PrintMsg("%s", Color::Red, skCrypt("官方最强太刀加载. 版本号v1.0.0.7"));
	SDK->CORE.PrintMsg("%s", Color::Red, skCrypt("TryStealAA偷刀 ACCALOGIC 启用无限ACCA RATE概率 EExp使用211斩杀 TryUseSkillAntiAttack尝试使用F防抓"));
}

LC_Function void FreeEvent() {

	/*

		在该函数调用时.说明脚本执行结束.
		如果你创建了一些线程 或编写了一些其他脱离环境的函数. 你需要在这里释放或回收你的函数.
		比如我使用std::thread 或 CreateThread 创建了一个线程. 那么在这里你需要终止你的线程. 否则会产生一些难以预估的错误.
		推荐使用更安全的方法释放. 比如在线程中编写标志位 while(!flag) 这样flag == true时 线程会自动返回 这样是安全的释放线程

	*/
}

int superstoi(std::string name) {
	if (name.empty()) {
		return 0;
	}
	return std::stoi(name);
}

LC_Function void MainExecuteCode() {
	/*

		在该函数调用时.说明脚本在主线程中循环调用此函数
		请勿使用高Sleep函数.

	*/
	TryStealAA = SDK->CORE.Read_Config(CFGName, CONFIG.c_str(), skCrypt("TryStealAA")) == "1";
	EnableHighACCALogic = SDK->CORE.Read_Config(CFGName, CONFIG.c_str(), skCrypt("EnableHighACCALogic")) == "1";

	randomRate = superstoi(SDK->CORE.Read_Config(CFGName, CONFIG.c_str(), skCrypt("ACCACCARate")));
	EnableExploit = SDK->CORE.Read_Config(CFGName, CONFIG.c_str(), skCrypt("EnableExploit")) == "1";

	TryUseSkill = SDK->CORE.Read_Config(CFGName, CONFIG.c_str(), skCrypt("TryUseSkillAntiAttack")) == "1";
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	/*

	   在该函数调用时.说明脚本比LibraryInit更提前加载
	   但不建议你在此处编写任何代码.此位置并不受核心管控.
	   PROCESS_ATTACH == LibraryInit
	   PROCESS_DETACH == FreeEvent;

   */
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {

	}
	if (ul_reason_for_call == DLL_PROCESS_DETACH) {

	}
	return TRUE;
}
