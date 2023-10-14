#pragma once

#include "LC_CLASS.h"

#include <string>
#include <vector>

#include "LC_XORSTR.h"

#define LC_Function extern "C" __declspec(dllexport)

#define COL_U32_R_SHIFT 0
#define COL_U32_G_SHIFT 8
#define COL_U32_B_SHIFT 16
#define COL_U32_A_SHIFT 24
#define COL_U32_A_MASK 0xFF000000

#define COL_U32(R, G, B, A) (((unsigned __int32)(A) << COL_U32_A_SHIFT) | ((unsigned __int32)(B) << COL_U32_B_SHIFT) | ((unsigned __int32)(G) << COL_U32_G_SHIFT) | ((unsigned __int32)(R) << COL_U32_R_SHIFT))

#define COL_U32_WHITE COL_U32(255, 255, 255, 255) // ��ɫ = 0xFFFFFFFF
#define COL_U32_BLACK COL_U32(0, 0, 0, 255) // ��ɫ = 0xFF000000
#define COL_U32_BLACK_TRANS COL_U32(0, 0, 0, 0) // ͸�� = 0x00000000
#define COL_U32_HOTPINK COL_U32(255, 122, 172, 255) // �ۺ�ɫ = 0xFFFF7AAC
#define COL_U32_RED COL_U32(255, 0, 0, 255) // ��ɫ = 0xFFFF0000
#define COL_U32_GREEN COL_U32(0, 255, 0, 255) // ��ɫ = 0xFF00FF00
#define COL_U32_BLUE COL_U32(0, 0, 255, 255) // ��ɫ = 0xFF0000FF
#define COL_U32_YELLOW COL_U32(255, 255, 0, 255) // ��ɫ = 0xFFFFFF00
#define COL_U32_PURPLE COL_U32(255, 0, 255, 255) // ��ɫ = 0xFFFF00FF

enum class Color {
	Default,
	Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
};

//�����������ActorModel��ActorKit�Ĺ�ϵ
typedef DWORD64 LC_ACTORMODEL;
typedef DWORD64 LC_ACTORKIT;

//�����ṩ�񵶽ṹ

enum ItemId {
	ChainShoot = 0x00325E9C
};
struct OP_DATA
{
	//�Ƿ�Ϊ����������
	bool IsBlueAttack;
	//����
	float Distance;
	//��ʱ�� ��Ҫת������Ϸʱ��(delay / 1000.f)
	int ParryDelay;
	//�Ƿ���Ҫ���ܴ���
	bool FlashEvade;
	//��ͬ����ĸ���
	bool ControlDistanceMode;
	//������ڶ���ʱ���Զ��
	float ControlDistance;
	//�Ƿ�Ϊ������ʽ
	bool IsDoubleAttack;
	//���������ʽ���ʱ��;
	int  DoubleAttackNextTime;
	//�Ƿ�ɹ���ܶ���
	bool SuccessEvadeFirstAttack;
	//��ҪW��
	bool NeedWFlash;
	//�Ƿ�ǿ���ڴ���ģʽ;
	bool OnlyMemoryMode;
};
struct private_use {
	int shout_count;
	char _pad[60];
};
struct POPData {
	float StartTime = 0;
	LC_ACTORMODEL ActorModel = 0;
	bool IsEvadeSuccess = false;
	float EvadeFirstOrderTime = 0;
	float ExecuteParryDelay = 0;
	bool IsSetWaitFlag = false;
	int Double_Flag = 0;
	OP_DATA DATA;
	private_use pad;
};


//�����ǻ�ȡ���һЩѪ�������Լ���������
struct PropertyData
{
	int CurHp;
	int HpMax;
	int Curshield;
	int shieldMax;
	int CurEnergy;
	int EnergyMax;
	int ConsumEnergy;
};
//�����ṩ����ģʽ
enum ActiveMode {
	Normal = 1,
	SoulMode = 2,
	ChainShootMode = 3,
	All = 4
};
//�����ṩ��ļ���CD
struct LocalSkillData {
	int SkillID;
	float CD;
	bool IsBanned;
};
struct AppointSkill {
	float CD;
	char OutputKey;
	float ExecuteAnimTime;
	int EndureLevel;
};
//���ﷵ�ض������
enum EVADE_TYPE {
	EVADE_Normal = 0, //��������
	EVADE_Double = 1, //��������
	EVADE_JiCangHai = 2 //���г�ȭ
};
//�����ṩ����ID
enum SoulItems {
	Katana_ShiHunZhan = 3323101,
	Katana_JingLeiShiJie = 3323141,
	Katana_QingGui = 3323111,
	Katana_DuoHun = 3323131,
	Sword_QiXingDuoQiao = 3323251,
	Sword_FengHuangYu = 3323201,
	Sword_LanYue = 3323211,
	Sword_FuTian = 3323221,
	Sword_DuoHun = 3323231,
	Spear_ShuangHuanSao = 3323441,
	Spear_FengJuanYunCan = 3323411,
	Spear_DaShengYou = 3323401,
	Spear_NuLangKuangTao = 3323451,
	Spear_FengHuoChuanXinJiao = 3323452,
	Blade_LieHuoZhan = 3323301,
	Blade_LieKong = 3323321,
	Blade_DuoHun = 3323331,
	Blade_BaZhu = 3323341,
	Blade_FengLeiBu = 3323361,
	Blade_XunFengZhenLeiDao = 3323351,
	Saber_ChiYanZhan = 3323381,
	Rod_WuQingQiMieZhen = 3323453,
	Rod_TongJin = 3323454,
	Rod_Spear_DuoHun = 3323431,
	Rod_DingHaiZhen = 3323707,
	Rod_DaoHaiGun = 3323706,
	Dagger_GuiKuShenHao = 3323504,
	Dagger_KangLongYouHui = 3323506,
	Dagger_YouMingBu = 3323503,
	Dagger_JingKeXianBi = 3323502,
	Dagger_GuiRenAnZha = 3323501,
	Dagger_DuoHun = 3323505,
	Nunchuck_HuXiaoLongPao = 3323605,
	Nunchuck_YangBianJin = 3323602,
	Nunchuck_DuoHun = 3323606,
	Nunchuck_JiChu = 3323603,
	Nunchuck_FeiTI = 3323601,
	Twinblades_QianKunRiYueZhan = 3323704,
	Twinblades_DuoHun = 3323703,
	Dualhalberd_GouXuanZhan = 3323801,
	Dualhalberd_ZhanLongZaiTian = 3323802,
	Dualhalberd_JiaoLongRuHai = 3323800,
	General_DiTangBajiao = 3320972,
	General_YiWuWanHua = 3325090
};
enum Bone : INT
{
	Head = 68, //�Դ�
	Neck = 65, //����
	Chest = 29, //�ز�
	Hips = 4, //����
	LeftUpperLeg = 15,//����
	RightUpperLeg = 16,//����
	LeftLowerLeg = 11,  //��ϥ
	RightLowerLeg = 21,//��ϥ
	LeftFoot = 7,//�����
	RightFoot = 18, // �ҽ���
	LeftToes = 8,//���
	RightToes = 19,//�ҽ�
	LeftShoulder = 60, //���
	RightShoulder = 70, //�Ҽ�
	LeftUpperArm = 32,  //������
	RightUpperArm = 72, //������
	LeftLowerArm = 37, //������
	RightLowerArm = 77, //������
	LeftHand = 43,  //����
	RightHand = 82, //����
};
enum EvadeSkillType {
	Line = 2,
	Circle = 1
};
struct EvadeInfo {
	//�ͷ���FID
	int CasterFid;
	//�ͷž���
	float Distance;
	//������Чʱ��
	float SkillVaildTime;
	//�Ƿ��ܳɹ�
	bool IsEvadeSuccess;
	//��СYaw
	float MinYaw;
	//��ʽ��ʽ
	EvadeSkillType Type;

	EvadeInfo(int CasterFid, float Distnace, float ValidTime, bool IsEvadeSuccess, float MinYaw, EvadeSkillType Type) {
		CasterFid = CasterFid;
		Distance = Distnace;
		SkillVaildTime = ValidTime;
		IsEvadeSuccess = IsEvadeSuccess;
		MinYaw = MinYaw;
		Type = Type;
	}
};
struct HurtInfo {
	float ActionTick;
	float ActionDuration;
	float LastHurtTime;
};
typedef struct _Basic_Action_time {
	//�ϴ�����ʱ��
	float LastFlashTime = 0;
	//�ϴ���������
	int LastFlashType = EVADE_TYPE::EVADE_Normal;
	//�ϴ���ʱ��
	float LastParryTime = 0;
	//�ϴ�������ʯʱ��
	float LastBladeParryTime = 0;
	//�ϴλ��г�ȭʱ��
	float LastChongQuanTime;
	//�ϴ�����ʱ��
	float LastFlashstepTime = 0;
	//�ϴζ׷�ʱ��
	float LastCrouchTime = 0;
	//�ϴι���ʱ��
	float LastChainTime = 0;
	//����������ʱ��
	float CuiSanNiangFHurtActionTime = 0.f;
	//���庮����ʱ��
	float GuQingHanFrostActionTime = 0.f;
	//�ϴζᵶʱ��
	float LastWuTianGrabWeaponTime = 0.f;
	//�ϴλ�Ծʱ��
	float LastHuWeiJumpAATime = 0.f;

}BasicActiontime;

typedef struct _Basic_Hurt_Info {
	//ץ�񵶺�ҡʱ��
	float ParryBcakSwingTime = 0;
	//�����ͷ�����ʱ��
	float AllowCastChargeTime = 0;
	//���г�ȭ��ֱ
	float HuoNanFireLaterTime = 0;
	//����׵�ʱ��
	float AllowNormalAttackTime = 0;
	//�񵶵�����Ĺ���ʱ��
	float ParryBackSwingTransitionTime = 0;
}BasicHurtInfo;

typedef struct _Basic_Player_Info {
	//�Ƿ��˻�
	bool IsRobot = false;
	//�Ƿ��Ѿ�
	bool isAlly = false;
	//��������
	bool IsHealthing = false;
	//��ҿɷ�����
	bool CanExecuteFlash = false;
	//����F1״̬
	float WuTianF1Tick = 0;
	//����ҹ��Vʱ
	float NingHongYeTick = 0;
	//����Ƽ�������ʱ
	bool YinZiPing_F2 = false;
	//����ŭ�����ʱ
	bool HuoNan_V = false;
	//��Ϊ��Vʱ
	bool HuWei_V = false;
	//������ʱ
	bool Dafo_V = false;
	//�Ƿ���ѣ״̬
	bool IsStunned = false;
	//���vʱ
	bool ShenMiao_V = false;
	//��ɽ��Vʱ
	bool YueShan_V = false;
	//����Ƽ����ģʽ
	bool YinZiPing_DontDie = false;
	//��ұ�������
	bool Shufu_NeedE = false;
	//��ұ����ץ��
	bool DafoHandMode = false;
	//����������ʱ
	bool YaoDaoji_V = false;
	//��������ˮ������ʱ
	bool CuiSanNiang_F = false;
	//��������ˮì��ʱ
	bool CuiSanNiang_V_tied = false;
	//�����庮����ʱ;
	bool GuQingHan_Frost = false;
	//���庮���鶳��ʱ
	bool GuQingHan_F = false;
	//���庮����ʱ
	bool GuQingHan_V = false;
	//���߶���
	bool ImmuneFrostTime = false;
	//���庮F���ǰҡ
	bool GuQingHan_V_FDash_delaying = false;
	//���庮F��̽׶�
	bool GuQingHan_V_F_Dashing = false;
	//���￪��ʱ
	bool WuTian_V = false;
	//�����￪��ʱ
	bool CuiSanNiang_V = false;
	//ƴ����־λ
	bool IsBounce = false;;
	//��ʯ��־λ
	bool IsBladeParry = false;
	//������־λ
	bool IsSlider = false;
	//�Ƿ��񵶳ɹ���
	bool IsShock = false;
	//��ӨӨ�Ƿ񿪴�
	bool JiYingYing_V = false;
	//��ӨӨ���״̬
	bool JiYingYing_Ghost = false;
	//�Ƿ�����
	bool IsShocked = false;
	//�Ƿ��Ի�״̬
	bool IsCharm = false;
	//�Ƿ������Ի�
	bool ImmuneCharmTime = false;
	//ɳ��F����ʱ
	bool IsShaNanFHurt = false;
	//��������
	bool IsParry = false;
	//PropertyData ��Ϣ
	PropertyData Property;
	//��ֱ��Ϣ
	HurtInfo HurtData;
}BasicPlayerInfo;

struct LC_PLAYER_INFO {
	int   PlayerFID = 0;
	bool  IsValid = false;
	LC_ACTORMODEL ActorModel = 0;

	//��ע��.�����Ϣ��ȫ���ű��������� ����������. �벻Ҫ��������κδ������. 

	BasicActiontime BasicActiontime;
	BasicHurtInfo BasicHurtInfo;
	BasicPlayerInfo BasicPlayerInfo;

	//===================���治��Ҫ���� ��Zenbot���Ļ�ȡ���� ����۸�
	char _pad[232];
};
struct LC_SDK_BasicLogic {
	//��ȫƽ���ƶ�
	void (*SmoothMoveTo)(ULONG dx, ULONG dy, float inc);
	//��ȡĿ���Ƿ�ɼ�������
	bool (*CanAgainAttack)(LC_PLAYER_INFO* PlayerPtr);
	//��ȡ��ѳ���λ��
	char (*GetBestDashKey)(LC_PLAYER_INFO* TargetData, float Distance);
	//ͨ�õĹ���
	bool (*ExecuteAA)(int Type, int wait);
	//ͨ�� ִ��CC����
	bool (*ExecuteCC)();
	//ͨ�� ִ��CA����
	bool (*ExecuteCA)();
	//ͨ�� ִ��CB����
	bool (*ExecuteCB)();
	//ͨ�� ִ����A����
	bool (*ExecuteJumpAA)(int Type, bool PressW);
	//ͨ�� ִ�г���
	bool (*KeyShift_Long_flash)(int TYPE, char MoveKey, bool ZhongShan, int delay, float KeepWTime);
	//ͨ�� ִ�ж���
	bool (*ShortFlash)(char MoveKey, bool EvadeMode);
	//ͨ�� ִ������
	bool (*DodgeChangeWeapon)(int ID);
	//ͨ�� ִ��C��
	bool (*CrouchChangeWeapon)(int ID);
	//ͨ�� ִ�л�������
	bool (*ExecuteSliderB)();
	//ͨ�ö�����ʱ��&&����һ�ι������
	bool (*StartWaitComboDelay)(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, float Delay);
};
struct LC_SDK_KEYBOARD {
	//�������п�ʼ ��ֹ���������к�����ͻ ���������Լ����� Ҳ���Բ��ô˺�����Ҫȷ����������
	void (*Set_IsExecuteCombo)(bool enable);
	//��ȡ�����Ƿ�ʼ
	bool (*Get_IsExecuteCombo)();

	//��������Ƿ��� | ����һ��Ҫ��� ��ֹһЩ�����򰴼���ͻ���!
	void (*Set_IsActiveLeft)(bool enable);
	//��ȡ����Ƿ���
	bool (*Get_IsActiveLeft)();

	//�����Ҽ��Ƿ��� | ����һ��Ҫ��� ��ֹһЩ�����򰴼���ͻ���!
	void (*Set_IsActiveRight)(bool enable);
	//��ȡ�Ҽ��Ƿ���
	bool (*Get_IsActiveRight)();

	//����C���Ƿ��� | ����һ��Ҫ��� ��ֹһЩ�����򰴼���ͻ���!
	void (*Set_IsActiveCrouch)(bool enable);
	//��ȡC���Ƿ���
	bool (*Get_IsActiveCrouch)();

	//���ÿո���Ƿ��� | ����һ��Ҫ��� ��ֹһЩ�����򰴼���ͻ���!
	void (*Set_IsActiveSpace)(bool enable);
	//��ȡ�ո���Ƿ���
	bool (*Get_IsActiveSpace)();

	//��ȡһ�������Ƿ��� ���ݼ��� ���� 'A' ����VK_XXX
	bool (*IsPressKey)(int hKey);

	//��ȡ�Ƿ�������� ActiceMode == 1Ϊ���� == 2Ϊ�������� AllΪȫ������
	bool (*ActiveCombo)(ActiveMode Mode);

	//����������Ϣ KeyΪ���� 0���� 2̧��
	void (*SendKeyBoard)(WORD Key, DWORD flags);

	//�����ƶ���Ϣ
	void (*SendMouseMove)(LONG dx, LONG dy);

	//���������Ϣ MOUSEEVENTF_XXXXX
	void (*SendMouseEventBoard)(UINT Key);

	//̧��ȫ������
	void (*StopCombo)();

	//����һ�ζ����¼�. ��ͨ�� CanExecuteFlash == trueʱ��ִ��!
	bool (*SendDodgeEvent)(char Key);

	//���� -> �������к���
	LC_SDK_BasicLogic BasicLogic;
};
struct LC_SDK_MATH {
	float (*ConverseAngle)(float x);
	float (*VectorToRotationYaw)(Vector3 pos);
	float (*CalcYaw)(Vector3 Loc, Vector3 Ent);
	float (*RotateYaw)(Vector3 Direction);
	float (*CheckForword)(LC_ACTORMODEL Ptr, Vector3 PtrPos, Vector3 MyPos);
	int   (*AngleDifference)(float a, float b);
	int   (*AngleDifference360)(float a, float b);
	float (*get_enityangle)(LC_ACTORMODEL obj);
	Vector3(*QuatToRot)(Vector4 Quaternion);
	Vector3(*FindLookAtVector)(Vector3 pos1, Vector3 pos2);
};
struct LC_SDK_PLAYER_BASEINFO {
	//��ȡ�������ʵ��
	LC_ACTORMODEL(*GetLocalPlayer)();
	//��ȡ�����Ӫ
	int (*GetTeam)(LC_ACTORMODEL Player);
	//��ȡ���Ψһ��ʶ��
	int (*GetPlayerfid)(LC_ACTORMODEL Player);
	//��ȡ��ҵ�ǰ����
	Vector3(*GetPlayerPosition)(LC_ACTORMODEL Player);
	//��ȡ���Ӣ��ID
	int (*GetPlayerHeroID)(LC_ACTORMODEL Ptr);
	//��ȡ���ACTORKIT
	LC_ACTORKIT(*GetPlayerActorKit)(LC_ACTORMODEL ActorModel);
	//��ActorKit�з��ص�ActorModel
	LC_ACTORMODEL(*GetActorModelByKit)(LC_ACTORKIT ActorKit);
	//��ȡPropertyData��Ϣ
	PropertyData(*GetPropertyData)(LC_ACTORMODEL ActorModel);
	//��ȡEndureLevel
	int (*GetPlayerEndureLevel)(LC_ACTORMODEL ActorModel);
	//��ȡRangeReactionType
	bool (*GetRangeReactionType)(LC_ACTORMODEL Ptr);
	//��ȡ��ǰ�ֳ�����ID
	int (*GetWeaponID)(LC_ACTORMODEL adr);
	//��ȡ��ǰ�ֳ־���ID
	int (*GetRealWeaponID)(LC_ACTORMODEL adr);
	//��ȡ��ǰ�������ʱ��
	float (*GetChargeTime)(LC_ACTORMODEL ActorModel);
	//��ȡĿ���Ƿ�����
	BOOL(*IsDead)(LC_ACTORMODEL adr);
	//��ȡĿ���Ƿ�Ϊ�˻�
	bool (*IsRobot)(LC_ACTORMODEL adr);
	//��ȡĿ��BeControlledStatus
	int (*GetBeControlledStatus)(LC_ACTORMODEL adr);
	//��ȡĿ��HeroBattleStatus
	int(*GetHeroBattleStatus)(LC_ACTORMODEL adr);
	//��ȡĿ��PlayerMovementIndex
	int (*GetPlayerMovementIndex)(LC_ACTORMODEL adr);
	//��ȡĿ�굱ǰ�����㼶
	int (*GetChargeLevel)(LC_ACTORMODEL ActorModel);
	//����Ƿ��������״̬
	bool (*PlayerIsCharge)(LC_ACTORMODEL ActorModel);
	//�ж�Ŀ���Ƿ��������ת�׵�
	bool (*PlayerIsChargeRelay)(LC_ACTORMODEL ActorModel);
	//��ȡĿ�굱ǰ�����ı�
	bool (*GetActionName)(LC_ACTORMODEL Obj, std::wstring& Output);
	//��ȡ�ܻ�����ʱ�� X:ActionTicks, Y:ActionDuration, Z:VaildTime
	Vector3(*GetActionVaildTime)(LC_ACTORMODEL Obj);
	//��ȡ��ǰ����ʱ�� X:Timer Y:StartTime Y:ValidTime
	Vector3(*GetActionSnapVaildTime)(LC_ACTORMODEL Obj);
	//��ȡ��Ϸ���ڿ��
	Vector2(*GetWindowSize)();
	//�ж�Ŀ���Ƿ��ܻ�״̬
	bool (*IsHiting)(LC_ACTORMODEL Obj);
	//��ȡĿ����һ���ܻ�ʱ��
	float (*GetPlayerLastHitTime)(LC_ACTORMODEL Player);
	//��ȡĿ���������㼶
	int (*GetDragonBlockStack)(LC_ACTORMODEL Obj);
	//��ȡ��ǰ��������λλ��
	int (*GetWeaponIndex)();
	//��ȡ����λ����ID.���ذ���λ��
	int (*GetDoubleWeaponID)(char* InputKey);
	//��ȡ���˼�����Ϣ  TYPE == 1��ͨ  ==2 �ռ�
	LocalSkillData(*GetSkillID)(LC_ACTORMODEL ActorModel, DWORD TYPE);
	//��ȡָ�����ͼ��� SkillType == 1 �߰���(��������)���� SkillType == 2�ܻ�����
	AppointSkill(*GetAppointSkill)(LC_ACTORMODEL ActorModel, DWORD SkillType);
	//��ȡĿ���Ƿ��ڸ�״̬ ����Ϊ����F�ȵ�
	bool (*IsParryGroupMode)(LC_ACTORMODEL Adr);
	//�ж��Ƿ���Ը񵲷��� ����Ϊ����F�ȵ�
	bool (*CanParryGroupAttack)(LC_ACTORMODEL Obj);
	//��ȡ��ҹ���λ��
	void (*GetBonePosition)(LC_ACTORMODEL Entity, Vector3* resultPos, int index);
	//��ȡCurTimeForLogicState
	float (*GetCurTimeForLogicState)(LC_ACTORMODEL LocalPlayer);
	//��ȡ��ǰ����Ŀ��
	LC_ACTORMODEL(*GetLockTargetBase)();
	//��ȡ��Ϸ�ӳ�
	float (*GetGamePing)();
	//������ת��Ϊ��Ļ
	BOOL(*WorldToScreen)(Vector3 WorldLocation, Vector3& retPos);
	//��ȡ���BuffId
	BOOL(*HasBuff)(LC_ACTORMODEL Player, int BuffId);
	//��ȡ��Ϸȫ��ʱ��
	float (*GetGlobalTime)();
	//��ȡ��ҵ�ǰ��ϣ
	int (*GetStateNameHash)(LC_ACTORMODEL Player);
	//��ȡ������Ʒ
	bool (*HasSoulItem)(LC_ACTORMODEL Player, int SoulId);
	//��ȡ��Ʒ�ƴ�
	int (*GetItemCount)(int ItemTid);
};
struct LC_WORLD_ENTITY {
	LC_PLAYER_INFO* (*GetPlayerDataByFID)(DWORD FID);
	void (*GetPlayerArrays)(std::vector<LC_PLAYER_INFO>& Output);
	std::vector<POPData>* (*GetBlueAttackList)();
	std::vector<EvadeInfo>* (*GetEvadeSkillList)();
};
struct LC_SDK_UTILITY {
	LC_ACTORMODEL(*GetComboTarget)(float Distance);
	LC_ACTORMODEL(*GetNearMouseTarget)(float fov);
	int (*RealWeaponToWeaponID)(int Weapon);
	void (*GetAllyHeroesList)(std::vector<LC_PLAYER_INFO>& arg);
	void(*GetAllysHeroesListInRange)(std::vector<LC_PLAYER_INFO>& arg, Vector3 Pos, float Range);
	void (*GetEnemysHeroesListInRange)(std::vector<LC_PLAYER_INFO>& arg, Vector3 Pos, float Range);
	int (*GetCountAllyHeroesInRange)(Vector3 Pos, float Range);
	int (*GetCountEnemysHeroesInRangeByList)(std::vector<LC_PLAYER_INFO> list, Vector3 Pos, float Range);
	int (*GetCountEnemysHeroesInRange)(Vector3 Pos, float Range);
	bool (*CanCastChargeByPosRange)(Vector3 pos, float Range);
	bool (*CanCastChargeByList)(std::vector<LC_PLAYER_INFO> list);
	bool (*CanCastChargeByListPosRange)(std::vector<LC_PLAYER_INFO> list, Vector3 pos, float Range);
	bool (*IsCanDodgeWeapon)(int weaponType);
	bool (*CheckInRange)(LC_ACTORMODEL Obj, float Range);
	int  (*GetRandomIntEx)(int Min, int Max);
};
struct LC_SDK_ANIMHASH {
	bool (*IS_RUNNING_ATTACK)(LC_ACTORMODEL Obj);
	bool (*IS_JUMP_AA_LEFT)(LC_ACTORMODEL Obj);
	bool (*IS_JUMP_AA_RIGHT)(LC_ACTORMODEL Obj);
	bool (*Is_HuWeiVAttack)(LC_ACTORMODEL Obj);
	bool (*Is_CuiSanNiangV2)(LC_ACTORMODEL Obj);
	bool (*IS_FLASH_JUMP)(LC_ACTORMODEL Obj);
	bool (*IS_CROUCH_AA)(LC_ACTORMODEL Obj);
	bool (*IS_ATTACK)(LC_ACTORMODEL Obj);
	bool (*IS_PRE_ATTACK)(LC_ACTORMODEL Obj);
	bool (*IS_DODGE)(LC_ACTORMODEL Obj);
	bool (*IS_SLIDE_RECOVER)(LC_ACTORMODEL Obj);
	bool (*IS_CROUCH_ATTACK_RECOVER)(LC_ACTORMODEL Obj);
	bool (*IS_FLASHSTEP)(LC_ACTORMODEL Obj);
	bool (*IS_WALKWALL)(LC_ACTORMODEL Obj);
	bool (*IS_WUCHEN_teleported)(LC_ACTORMODEL Obj);
	bool (*IS_JUMP_DOWN_TO_LAND)(LC_ACTORMODEL Obj);
	bool (*IS_RUNNING)(LC_ACTORMODEL Obj);
	bool (*IS_Inertia)(LC_ACTORMODEL Obj);
	bool (*IS_SPRINT)(LC_ACTORMODEL Obj);
	bool (*IS_PUTDOWN)(LC_ACTORMODEL Obj);
	int  (*GET_STACK_VALUE_BY_OBJ)(LC_ACTORMODEL Obj);
	bool (*IsRunningCharge)(LC_ACTORMODEL Obj);
	bool (*IsHurt)(LC_ACTORMODEL Obj);
	bool (*IsHurtFlaying)(LC_ACTORMODEL Obj);
	bool (*IsInjuredLie)(LC_ACTORMODEL Obj);
	bool (*IsHurtFlayingToLand)(LC_ACTORMODEL Obj);
	bool (*IsAllGetUp)(LC_ACTORMODEL Obj);
	int  (*GetFlashEmptyType)(LC_ACTORMODEL Obj);
	bool (*Action_HuoNanF_CanAttack)(LC_ACTORMODEL Obj);
	bool (*Action_HuoNan_IsF)(DWORD64 Obj);
	bool (*IS_ATTACK_SHOCK)(LC_ACTORMODEL Obj);
	bool (*IS_IDLE)(LC_ACTORMODEL Obj);
	bool (*IS_SUCCESS_SHOCK)(LC_ACTORMODEL Obj);
	bool (*IS_JUMPAA_RIGHT_RECOVER)(LC_ACTORMODEL Obj);
	bool (*PLAYER_INAIR)(LC_ACTORMODEL Obj);
};
struct LC_CORE {
	void (*SetLastComboFuncExecuteTime)(float Time);
	float (*GetLastComboFuncExecuteTime)();

	void (*LockHuWeiJumpTime)(float Time);
	float (*GetLockHuWeiJumpTime)();
};
struct LC_SDK_STRUCT {
	//����
	LC_CORE CORE;
	//������
	LC_SDK_KEYBOARD KEYBOARD;
	//��ѧ����
	LC_SDK_MATH MATH;
	//����ʵ��
	LC_WORLD_ENTITY WORLD;
	//��Ϸ����
	LC_SDK_PLAYER_BASEINFO GAMEFUNC;
	//��ϣ��ƥ��
	LC_SDK_ANIMHASH ANIMHASH;
	//������
	LC_SDK_UTILITY UTILITY;
};
