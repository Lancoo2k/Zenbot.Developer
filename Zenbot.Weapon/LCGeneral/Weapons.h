#pragma once

enum class Control {
	LongFlash = 0,
	CCReset = 1,
	DirectInput = 2,
	FlashStep = 3,
	UseSkill = 4
};
enum class HurtCast {
	Null = 0,
	AA = 1,
	Charge = 2
};
class WeaponFunc
{
public:
	virtual void MainExecuteCode() = 0;
	virtual void FreeEvent() = 0;
	virtual void LibraryInit() = 0;
	virtual void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) = 0;
};

extern float ChangeWeaponTime;
extern float DisableWeaponWhiteMode;
extern float AllowResetChargeTime;

class Katana : public WeaponFunc {
	bool TryStealAA;
	bool EnableHighACCALogic = false;
	bool EnableForceFlashWithFlashStep = true;
	bool EnableExploit = true;
	int   randomRate = 3;
	float MaxChargeTime = 1.32f;
	bool TryUseSkill = true;
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
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	float ForceUseLeft = 0;
	float ForceUseRight = 0;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	int GetNextChargeType(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack, float Distance);
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool DirectShengLong);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Dagger : public WeaponFunc {
	bool TryStealAA = true;
	int   StealAAStyle = 1;
	bool EnableHitMark = true;
	float MaxChargeTime = 1.32f;
	int   randomRate = 7;
	int   randomRate2 = 5;
	//蓄力攻击范围
	const float LeftAARange = 3.5f;
	const float RightAARange = 3.8f;
	const float CARange = 5.5f;
	const float CBRange = 4.6f;
	const float JumpAARange = 5.f;
	const float ShockAARange = 12.f;
	const float LeftChargeRange = 8.5f;
	const float RightChargeRange = 4.5f;

	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//抓闪时间
	float HitMark = 0.f;

	float ForceUseLeft = 0;
	float ForceUseRight = 0;

public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack, float Distance);
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	bool IsFastStatck(LC_PLAYER_INFO* Obj);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool IsCrouchCharge(LC_PLAYER_INFO* Player);
	void ResetCC();
	bool J2aJb(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool attach_cab(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAfter = false);
	bool attach_3abcombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsSky, bool guifan, bool jingke, bool guiku, bool dontb = false);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Blade : public WeaponFunc {
	const float LeftAARange = 3.6f;
	const float RightAARange = 5.2f;
	const float CARange = 4.8f;
	const float CBRange = 4.9f;
	const float JumpAARange = 4.4f;
	const float ShockAARange = 12.f;
	const float ChargeRange = 5.2f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	float MaxChargeTime = 1.32f;
	//强制时间
	float ForceUseLeft = 0.f;
	float ForceUseRight = 0.f;

public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool CBAB(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Twinblades : public WeaponFunc {
	bool TryStealAA = true;
	bool EnableHighACCALogic = true;
	bool EnableForceFlashWithFlashStep = true;
	bool UseBSHIFTA = true;
	int   randomRate = 6;
	float MaxChargeTime = 1.32f;
	const float LeftAARange = 4.2f;
	const float RightAARange = 4.5f;
	const float CARange = 5.0f;
	const float CBRange = 7.2f;
	const float JumpAARange = 5.2f;
	const float ShockAARange = 12.f;
	const float ChargeRange = 5.f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//强制右键蓄力
	float ForceUseLeft = 0.f;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool Cab(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput,bool disable = false);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Dualhalberd : public WeaponFunc {
	const float LeftAARange = 4.8f;
	const float RightAARange = 5.2f;
	const float CARange = 5.0f;
	const float CBRange = 7.2f;
	const float JumpAARange = 5.2f;
	const float ShockAARange = 12.f;
	const float ChargeRange = 6.f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//强制右键蓄力
	float ForceUseLeft = 0.f;
	//切刀强制攻击
	float forceAttackTime = 0;
	bool TryStealAA = true;
	bool EnableHighACCALogic = true;
	bool EnableForceFlashWithFlashStep = true;
	bool UseBSHIFTA = true;
	bool EnableExploit = true;
	int   randomRate = 6;
	float MaxChargeTime = 1.32f;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput,bool disable = false);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Nunchucks : public WeaponFunc {
	//CA 0.52f 扬鞭劲0.44f 飞踢0.77f
	const float LeftAARange = 4.2f;
	const float RightAARange = 4.5f;
	const float CARange = 5.3f;
	const float CBRange = 5.0f;
	const float JumpAARange = 5.6f;
	const float ShockAARange = 15.f;
	const float ChargeRange = 5.f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	float ForceUseLeft = 0;
	float ForceUseRight = 0;
	float BreakTime = 0.f;
	bool TryStealAA = true;
	bool EnableForceFlashWithFlashStep = true;
	float MaxChargeTime = 1.32f;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack, float Distance);
	bool IsSoulParry(ULONG64 Obj);
	int GetfewPlayerChargeType(LC_PLAYER_INFO* Enemy, float Distance);
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput,bool attachMode = false);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control&NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Saber : public WeaponFunc {
	const float LeftAARange = 3.6f;
	const float RightAARange = 5.2f;
	const float CARange = 4.8f;
	const float CBRange = 4.9f;
	const float JumpAARange = 4.4f;
	const float ShockAARange = 12.f;
	const float ChargeRange = 5.2f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//强制右键蓄力
	float ForceUseRight = 0.f;
	float MaxChargeTime = 1.32f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, bool EnemyIsDontAttack);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	void ResetCC();
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool CBAB(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control&NextInput);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Rod : public WeaponFunc {
	//蓄力攻击范围
	const float LeftAARange = 4.2f;
	const float RightAARange = 4.7f;
	const float CARange = 5.2f;
	const float CBRange = 5.0f;
	const float JumpAARange = 5.7f;
	const float ShockAARange = 12.f;
	const float	ChargeRange = 5.5f;
	float		LastDragonDash = 0.f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//强制右键蓄力
	float ForceUseRight = 0.f;
	//火男继续脑海
	float HNDontJA = 0.f;
	float MaxChargeTime = 1.25f;
	bool EnableForceFlashWithFlashStep = true;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, float Distance);
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool attachMode = false);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Sword : public WeaponFunc {
	//蓄力攻击范围
	const float LeftAARange = 4.7f;
	const float RightAARange = 5.2;
	const float CARange = 5.5f;
	const float CBRange = 5.3f;
	const float JumpAARange = 5.5f;
	const float ShockAARange = 12.f;
	float		ChargeRange = 10.f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//强制右键蓄力
	float ForceUseRight = 0.f;
	bool TryStealAA = true;
	bool EnableHighBCCBLogic = false;
	float MaxChargeTime = 1.32f;
	bool EnableForceFlashWithFlashStep = true;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	int GetNextChargeType(LC_PLAYER_INFO* Enemy, float Distance);
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlayer);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};
class Spear : public WeaponFunc {
	//蓄力攻击范围
	const float LeftAARange = 4.2f;
	const float RightAARange = 4.5f;
	const float CARange = 5.2f;
	const float CBRange = 5.0f;
	const float JumpAARange = 5.7f;
	const float ShockAARange = 12.f;
	const float	ChargeRange = 5.5f;
	float		LastDragonDash = 0.f;
	//下一次操作
	Control LastControl = Control::LongFlash;
	//当前进入蓄力时间
	float StartChargeTime = 0.f;
	//禁止白刀时间
	float DontWhiteAttackTime = 0.f;
	//强制右键蓄力
	float ForceUseRight = 0.f;
	//下一次闹海 防止冲突
	float NextNaoHai = 0.f;
	//火男继续脑海
	float HNDontJA = 0.f;
	float MaxChargeTime = 1.25f;
	bool EnableForceFlashWithFlashStep = true;
	//关闭2A
	float Disable2A = 0.f;
	//强制S闪
	float ForceUseSLongShift = 0.f;
	//强制W闪绕背式
	float ForceUseWLongShiftKeep = 0.f;
public:
	void MainExecuteCode();
	void FreeEvent();
	void LibraryInit();
	void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
private:
	int  GetNextChargeType(LC_PLAYER_INFO* Enemy, float Distance);
	bool IsInAttackRange(float Distance, float ExtraRange = 0.f);
	bool IsFastStatck(LC_PLAYER_INFO* Player);
	bool IsNaoHai(LC_PLAYER_INFO* Player);
	bool WillEnd(LC_PLAYER_INFO* Player);
	bool Get_IsCharge(LC_PLAYER_INFO* Player);
	bool AttackShock(LC_PLAYER_INFO* LocalPlaye);
	bool ExecuteShock(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool LongWangMode, bool ShockAfter);
	bool AllUse_AfterCombo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool attachMode = false);
	bool Right_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	bool Left_ComboRun(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, Control& NextInput, bool IsAAAfter = false);
	void Combo_Solo(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
	void RunableLogic(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy);
};