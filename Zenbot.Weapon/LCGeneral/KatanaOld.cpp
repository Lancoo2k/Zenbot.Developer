#include <Windows.h>
#include <thread>
#include "../../LC_SDK/LC_SDK.h"
#include <random>

std::string CFGName = skCrypt("�ٷ�̫������");
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
	//����������Χ
	const float LeftAARange = 4.5f;
	const float RightAARange = 5.2f;
	const float CARange = 5.3f;
	const float CBRange = 5.0f;
	const float JumpAARange = 5.5f;
	const float ShockAARange = 12.f;
	const float ChargeRange = 5.5f;
	const float SoulRange = 10.f;
	//��һ�β���
	int LastControl = Control::LongFlash;
	//��ǰ��������ʱ��
	float StartChargeTime = 0.f;

	//��ֹ�׵�ʱ��
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
		//����һ��50%���� ��ֱ������ ���Ǽ�������ACCA�һ��� ǰ���ǶԷ�û����������
		int WepId = SDK->GAMEFUNC.GetWeaponID(Enemy->ActorModel);
		bool againACCA = SDK->UTILITY.GetRandomIntEx(1, 10) <= randomRate;
		bool IsMaxBreak = false;
		if (EnableHighACCALogic && againACCA && WepId != 119 && WepId != 103 && !LocalPlayer->BasicPlayerInfo.HuoNan_V)
		{
			do {
				if (SDK->KEYBOARD.BasicLogic.ExecuteCC()) {
					//Ŀ��û�е��� ������������
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
								//����ʧ��ʱֱ�ӷ��� ����˵����ʧ�� ��һ������
								SDK->KEYBOARD.StopCombo();
								NextInput = Control::DirectInput;
								return false;
							}
						}
					}
					//�����ٳ�һ�ε� ����ܹؼ� �ж��Ƿ����ACCA
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
			//	//��2A
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
			//	//�������� ����.
			//	NextInput = Control::DirectInput;
			//	SDK->KEYBOARD.StopCombo();
			//	return true;
			//}
		}
		//����ֱ��תΪ���� ���ACCA��ֱ��0.77~1.00ʱ ���ÿ��Խ�һ��
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
				//���Ը��հ���ն
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
				//�жϵ���Ѫ��
				if (GetRealHealth(Enemy) <= 420) {
					//��Ѫ��ʹ��B+CAնɱ
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
					//���հ���ն ʹ��
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
					//�ж������1Aû���о�ֱ��ת2B
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
						//û����ԭ������˫ѹ
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
							//���Ŀ�걻�򵹵��� ����A
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
							//���2A�ʹ�����һ��. �ж���HitTime
							float oTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
							if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
								if (oTime > 0.1f) {
									//û���� ֱ�ӳ�3B
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
			//��������
			if (!SDK->UTILITY.CheckInRange(Enemy->ActorModel, CBRange) || !SDK->KEYBOARD.BasicLogic.ExecuteSliderB())
			{
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			if ((EnableExploit && LocalPlayer->BasicPlayerInfo.CanExecuteFlash && GetRealHealth(Enemy) <= 750)) {
				//������� ʹ��ȭ�м��� ��ǰ0.2sԤ������ȭ
				if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.4f)) {
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
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
				//����W��
				//0.60fΪ���������
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
					//���Ŀ�걻�򵹵��� ����A
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
					//���2A�ʹ�����һ��. �ж���HitTime
					float oTime = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
					if (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel)) {
						if (oTime > 0.1f) {
							//û���� ֱ�ӳ�3B
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
			//��B�ĳ����ٶ�������ͬ
			bool IsRunningB = SDK->ANIMHASH.IS_RUNNING_ATTACK(LocalPlayer->ActorModel);
			float RealDelay = IsRunningB ? 0.35f : 0.30f;
			bool Logic1 = SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, RealDelay);
			bool Logic2 = SDK->GAMEFUNC.IsHiting(Enemy->ActorModel);
			if (!Logic1 || !Logic2) {
				//�������û���� �ж��¶��������˻�������.��ֹ����AץA��
				if (!Logic2) {
					if (SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashTime < 0.4f ||
						SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime < 0.4f) {
						//�������� ��ԭ��������ֹ�ٱ���ץ
						SDK->KEYBOARD.StopCombo();
						NextInput = Control::DirectInput;
						return false;
					}
				}
				SDK->KEYBOARD.StopCombo();
				NextInput = Enemy->BasicPlayerInfo.Property.CurHp > 0 ? Control::UseSkill : Control::LongFlash;
				return false;
			}
			//�����жϵз���ֱ���
			if (GetMaxHurt(Enemy) <= 0.43f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || GetRealHealth(Enemy) <= 240 || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
				//����ֱ����A��������
				//û���ͳ�����A
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
			//������ȥץ
			if (LocalPlayer->BasicPlayerInfo.CanExecuteFlash) {
				if (!SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0)) {
					//����ʧ��ʱֱ�ӷ��� ����˵����ʧ�� ��һ������
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return false;
				}
			}
			else {
				SDK->KEYBOARD.BasicLogic.ExecuteCC();
				//û������CC;
			}
			//ֱ����A.���û���л��߽�ֱ������ֱ�ӷ��ؼ�.����д�����ͺ���
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
						//�������� ��ԭ��������ֹ�ٱ���ץ
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
			//�����жϵз���ֱ���
			if (GetMaxHurt(Enemy) < 0.77f || SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || SDK->ANIMHASH.IsHurtFlayingToLand(Enemy->ActorModel) || SDK->ANIMHASH.IsAllGetUp(Enemy->ActorModel) || GetRealHealth(Enemy) <= 240 || SDK->ANIMHASH.IsInjuredLie(Enemy->ActorModel)) {
				//����ֱ����A��������
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
			//���б��� �� �Է��������� ��ʱԭ����������.
			if (!SDK->KEYBOARD.BasicLogic.StartWaitComboDelay(LocalPlayer, Enemy, 0.30f) || !SDK->GAMEFUNC.IsHiting(Enemy->ActorModel)) {
				SDK->KEYBOARD.StopCombo();
				NextInput = Control::DirectInput;
				return false;
			}
			//�����ж�һ��Ѫ�� �Ƿ���Կ���նɱ ����������
			if (GetRealHealth(Enemy) <= 500) {
				//��ʱ��2A + ��Aնɱ
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
					//���Ŀ�������� ����ֱ�ӷ��ز���������
					SDK->KEYBOARD.StopCombo();
					NextInput = Control::DirectInput;
					return true;
				}
				//û���ͳ�����A
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
				//printf("�����ܻ��ж�\n");
				//������ ��������֮ǰ �����ܵ���ֱʱ�� < 0.1f.  �մӱ��򵽿�������.
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
		//printf("��ʼִ��222 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
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

				//printf("��ʼִ��333 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
				if (Distance <= ChargeRange && LastBladeParryTime > 1.2f && CanCastCharge) {
					bool CanNext = false;
					if (A3Mode && !SDK->KEYBOARD.IsPressKey(1)) {
						CanNext = (hurtlandFlag && SDK->KEYBOARD.IsPressKey(2)) || isHitMode;
					}
					else if(IntoCharge) {
						CanNext = isbigMode || isHitMode || hurtlandFlag || FlashExecuteTime < 0.1f || FlashStepTime < 0.1f || GetMaxHurt(Enemy) >= 0.5f;
					}
					//����޴� / ���״̬ / ץ�� /����״̬�� �����������㼶���ͷ�. ���������״̬�Ҳ�������������ҽ���A3״̬ʱ�ͷ��Ҽ�.
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
						bool Soul_JingLei = SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_JingLeiShiJie);

						if (!A3Mode  && SDK->KEYBOARD.ActiveCombo(ActiveMode::All) && (SDK->ANIMHASH.IsHurtFlaying(Enemy->ActorModel) || (Soul_JingLei && SDK->UTILITY.GetRandomIntEx(1, 10) <= 5))) {
							//�ͷž���ʮ�� || ���о���
							//DbgPrint("�ͷž���ʮ�� ��ס�Ҽ�");
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
							SDK->KEYBOARD.Set_IsActiveRight(true);
							std::wstring Name;

							bool success = false;

							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
								if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
									if (Name.find(skCrypt(L"katana_attack_heavy_09_pre")) != std::wstring::npos) { //̫���Ҽ�_����ʮ��(̫������֮����Ҽ�׷������)
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
										//�Լ����񵶾�����ѭ��
										break;
									}
								}
							}
							else {
								if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
									//DbgPrint("û�ͷų�����ʮ��.�ɿ�����Ҽ�");
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
				//printf("��ʼִ��444 | %f\n", SDK->GAMEFUNC.GetGlobalTime());
				//�����0.7f ��ת�� 0.63f 
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

										//����һ�ι���ʱ
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

									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_LEFTUP);
									SDK->KEYBOARD.Set_IsActiveLeft(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
									if (!Left_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer);
									}
									return;
								}
								if (StackValue == 2) {
									//����һ�ι���ʱ
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

									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
									std::this_thread::sleep_for(std::chrono::milliseconds(100));
									if (!Right_ComboRun(LocalPlayer, Enemy, LastControl, true) && LastControl == Control::UseSkill) {
										CheckCastSkill(LocalPlayer);
									}
									return;
								}
								if (StackValue == 2) {
									//����һ�ι���ʱ
									SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
									SDK->KEYBOARD.Set_IsActiveRight(false);
									//�����ӳ�һ�� �¶���δ��ȫչ��
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
					
					
					//��������ʱ��ʱ. ����Է���2.0ʱ���� �ű�����������ȡParryBcakSwingTime. �����жϳ���ʱ���ת����ץ!

					float TransitionTime = Enemy->BasicHurtInfo.ParryBackSwingTransitionTime - SDK->GAMEFUNC.GetGlobalTime();

					if (MaxChargeTime < ChargeTime + TransitionTime) {

						if (Distance <= LeftAARange + 3.f) {

							if (SDK->KEYBOARD.BasicLogic.KeyShift_Long_flash(1, 'W', true, 0)) {
								//����
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
				//���Ŀ�겻�ڹ�����Χ��ʱ
				bool x1flag = !IsInAttackRange(Distance);
				bool x2flag = EnableForceFlashWithFlashStep && SDK->GAMEFUNC.GetGlobalTime() - Enemy->BasicActiontime.LastFlashstepTime <= 0.2f;
				if (SDK->KEYBOARD.Get_IsActiveRight() && SDK->KEYBOARD.IsPressKey(2)) {
					if (CanToAttack && StackValue == 1 && SDK->ANIMHASH.IsRunningCharge(LocalPlayer->ActorModel) && !IsInAttackRange(Distance, 2.f)) {
						SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTUP);
						SDK->KEYBOARD.Set_IsActiveRight(false);
						//�����ӳ�һ�� �¶���δ��ȫչ��
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
		//ƴ���¼�g
		if (LocalPlayer->BasicPlayerInfo.IsBounce ) {
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
		if (!ClosestMouseTarget) {
			ClosestMouseTarget = Enemy;
		}
		bool IsCharge = Get_IsCharge(LocalPlayer);
		if (!SDK->KEYBOARD.Get_IsActiveLeft() && !SDK->KEYBOARD.Get_IsActiveRight() && !IsCharge) {
			if (LastControl == Control::LongFlash) {
				auto MyHurtData = LocalPlayer->BasicPlayerInfo.HurtData;
				if (SDK->GAMEFUNC.GetGlobalTime() - (MyHurtData.LastHurtTime + MyHurtData.ActionDuration) < 0.1f) {
					//printf("�����ܻ��ж�\n");
					//������ ��������֮ǰ �����ܵ���ֱʱ�� < 0.1f.  �մӱ��򵽿�������.
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
		//��ȡ������Ϣ
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
						if (SDK->GAMEFUNC.HasSoulItem(LocalPlayer->ActorModel, SoulItems::Katana_JingLeiShiJie) && SDK->KEYBOARD.ActiveCombo(ActiveMode::All)) {
							//�ͷž���ʮ��
							//DbgPrint("�ͷž���ʮ�� ��ס�Ҽ�");
							SDK->KEYBOARD.SendMouseEventBoard(MOUSEEVENTF_RIGHTDOWN);
							SDK->KEYBOARD.Set_IsActiveRight(true);
							std::wstring Name;

							bool success = false;

							float AllowTime = SDK->GAMEFUNC.GetGlobalTime() + 0.4f;
							while (AllowTime > SDK->GAMEFUNC.GetGlobalTime()) {
								if (SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, Name)) {
									if (Name.find(skCrypt(L"male_katana_attack_heavy_09_pre")) != std::wstring::npos) { //̫���Ҽ�_����ʮ��(̫������֮����Ҽ�׷������)
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
										//�Լ����񵶾�����ѭ��
										break;
									}
								}
							}
							else {
								if (SDK->KEYBOARD.IsPressKey(2) && SDK->KEYBOARD.Get_IsActiveRight()) {
									//DbgPrint("û�ͷų�����ʮ��.�ɿ�����Ҽ�");
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
			//���������׶�
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

		�ڸú�������ʱ.˵���ű��Ѿ���ʼִ�����д���
		�����뿴����ʾ��. ���л����ṹ���Ըı� �������ʾ����ʽ���б�д.
		������д�Ľű���֧�����ɸ�����.��һ��Ҫͨ��switch �� if �ж�����. ����DrawingCode�б�д��������Ƿ񼤻�˽ű��ĸ������߼�! ��Ȼ���ܻᷢ���߼���ͻ!
		������п��� ��Ů���� ��ӨӨ����. �㶼��Ҫ�Դ˺������и���. �������д��̫���Ľű� ����û�л��е����߼� �������д��һ�����е������. ��ô���ߴ�����޷�����
		����Ҫô��д Ҫô��һ��Ҫд��д��. ������һ��Ӣ��д������.Ҳ�����ǻ���F ��������㵶 ����ҹ�Զ�F��ħ����֮���. ��Ҫ��ͼ���������ű����߼�.

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

		�ڸú�������ʱ.˵���ű��Ѿ��ɹ����� �����ǳ�ʼ����ű��Ĳ���
		����ò����κθ��ӳٴ��� ���� Sleep(1000)�������� �����Ῠ������������. �����һЩ�ű����ؿ��ܵ��»���.

	*/
	char szBuf[64];
	sprintf_s(szBuf, "%d", VK_CAPITAL);

	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("TryStealAA"), skCrypt("1"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("EnableHighACCALogic"), skCrypt("1"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("ACCACCARate"), skCrypt("7"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("EnableExploit"), skCrypt("1"));
	SDK->CORE.Init_Config(CFGName, CONFIG.c_str(), skCrypt("TryUseSkillAntiAttack"), skCrypt("1"));
	SDK->CORE.PrintMsg("%s", Color::Red, skCrypt("�ٷ���ǿ̫������. �汾��v1.0.0.7"));
	SDK->CORE.PrintMsg("%s", Color::Red, skCrypt("TryStealAA͵�� ACCALOGIC ��������ACCA RATE���� EExpʹ��211նɱ TryUseSkillAntiAttack����ʹ��F��ץ"));
}

LC_Function void FreeEvent() {

	/*

		�ڸú�������ʱ.˵���ű�ִ�н���.
		����㴴����һЩ�߳� ���д��һЩ�������뻷���ĺ���. ����Ҫ�������ͷŻ������ĺ���.
		������ʹ��std::thread �� CreateThread ������һ���߳�. ��ô����������Ҫ��ֹ����߳�. ��������һЩ����Ԥ���Ĵ���.
		�Ƽ�ʹ�ø���ȫ�ķ����ͷ�. �������߳��б�д��־λ while(!flag) ����flag == trueʱ �̻߳��Զ����� �����ǰ�ȫ���ͷ��߳�

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

		�ڸú�������ʱ.˵���ű������߳���ѭ�����ô˺���
		����ʹ�ø�Sleep����.

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

	   �ڸú�������ʱ.˵���ű���LibraryInit����ǰ����
	   �����������ڴ˴���д�κδ���.��λ�ò����ܺ��Ĺܿ�.
	   PROCESS_ATTACH == LibraryInit
	   PROCESS_DETACH == FreeEvent;

   */
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {

	}
	if (ul_reason_for_call == DLL_PROCESS_DETACH) {

	}
	return TRUE;
}
