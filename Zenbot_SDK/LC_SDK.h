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

#define COL_U32_WHITE COL_U32(255, 255, 255, 255) // 白色 = 0xFFFFFFFF
#define COL_U32_BLACK COL_U32(0, 0, 0, 255) // 黑色 = 0xFF000000
#define COL_U32_BLACK_TRANS COL_U32(0, 0, 0, 0) // 透明 = 0x00000000
#define COL_U32_HOTPINK COL_U32(255, 122, 172, 255) // 粉红色 = 0xFFFF7AAC
#define COL_U32_RED COL_U32(255, 0, 0, 255) // 红色 = 0xFFFF0000
#define COL_U32_GREEN COL_U32(0, 255, 0, 255) // 绿色 = 0xFF00FF00
#define COL_U32_BLUE COL_U32(0, 0, 255, 255) // 蓝色 = 0xFF0000FF
#define COL_U32_YELLOW COL_U32(255, 255, 0, 255) // 黄色 = 0xFFFFFF00
#define COL_U32_PURPLE COL_U32(255, 0, 255, 255) // 紫色 = 0xFFFF00FF

enum class Color {
	Default,
	Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
};

//这里便于区分ActorModel与ActorKit的关系
typedef DWORD64 LC_ACTORMODEL;
typedef DWORD64 LC_ACTORKIT;

//这里提供振刀结构

enum ItemId {
	ChainShoot = 0x00325E9C
};
struct OP_DATA
{
	//是否为蓝霸体招数
	bool IsBlueAttack;
	//距离
	float Distance;
	//振刀时间 需要转换到游戏时间(delay / 1000.f)
	int ParryDelay;
	//是否需要闪避处理
	bool FlashEvade;
	//不同距离的更换
	bool ControlDistanceMode;
	//距离大于多少时可以躲避
	float ControlDistance;
	//是否为二段招式
	bool IsDoubleAttack;
	//进入二段招式后的时间;
	int  DoubleAttackNextTime;
	//是否成功躲避二段
	bool SuccessEvadeFirstAttack;
	//需要W闪
	bool NeedWFlash;
	//是否强制内存躲避模式;
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


//这里是获取玩家一些血量护甲以及精力消耗
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
//这里提供连招模式
enum ActiveMode {
	Normal = 1,
	SoulMode = 2,
	ChainShootMode = 3,
	All = 4
};
//这里提供你的技能CD
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
//这里返回躲避类型
enum EVADE_TYPE {
	EVADE_Normal = 0, //正常闪避
	EVADE_Double = 1, //翻滚闪避
	EVADE_JiCangHai = 2 //火男冲拳
};
//这里提供魂玉ID
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
	Head = 68, //脑袋
	Neck = 65, //脖子
	Chest = 29, //胸部
	Hips = 4, //骨盆
	LeftUpperLeg = 15,//左髋
	RightUpperLeg = 16,//右髋
	LeftLowerLeg = 11,  //左膝
	RightLowerLeg = 21,//右膝
	LeftFoot = 7,//左脚踝
	RightFoot = 18, // 右脚踝
	LeftToes = 8,//左脚
	RightToes = 19,//右脚
	LeftShoulder = 60, //左肩
	RightShoulder = 70, //右肩
	LeftUpperArm = 32,  //左手肘
	RightUpperArm = 72, //右手肘
	LeftLowerArm = 37, //左手腕
	RightLowerArm = 77, //右手腕
	LeftHand = 43,  //左手
	RightHand = 82, //右手
};
enum EvadeSkillType {
	Line = 2,
	Circle = 1
};
struct EvadeInfo {
	//释放者FID
	int CasterFid;
	//释放距离
	float Distance;
	//技能有效时间
	float SkillVaildTime;
	//是否躲避成功
	bool IsEvadeSuccess;
	//最小Yaw
	float MinYaw;
	//招式方式
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
	//上次闪避时间
	float LastFlashTime = 0;
	//上次闪避类型
	int LastFlashType = EVADE_TYPE::EVADE_Normal;
	//上次振刀时间
	float LastParryTime = 0;
	//上次阔刀磐石时间
	float LastBladeParryTime = 0;
	//上次火男冲拳时间
	float LastChongQuanTime;
	//上次闪步时间
	float LastFlashstepTime = 0;
	//上次蹲伏时间
	float LastCrouchTime = 0;
	//上次钩锁时间
	float LastChainTime = 0;
	//崔三娘束缚时间
	float CuiSanNiangFHurtActionTime = 0.f;
	//顾清寒冰冻时间
	float GuQingHanFrostActionTime = 0.f;
	//上次夺刀时间
	float LastWuTianGrabWeaponTime = 0.f;
	//上次虎跃时间
	float LastHuWeiJumpAATime = 0.f;

}BasicActiontime;

typedef struct _Basic_Hurt_Info {
	//抓振刀后摇时间
	float ParryBcakSwingTime = 0;
	//允许释放蓄力时间
	float AllowCastChargeTime = 0;
	//火男冲拳后僵直
	float HuoNanFireLaterTime = 0;
	//允许白刀时间
	float AllowNormalAttackTime = 0;
	//振刀到放蓄的过渡时间
	float ParryBackSwingTransitionTime = 0;
}BasicHurtInfo;

typedef struct _Basic_Player_Info {
	//是否人机
	bool IsRobot = false;
	//是否友军
	bool isAlly = false;
	//正在治疗
	bool IsHealthing = false;
	//玩家可否闪避
	bool CanExecuteFlash = false;
	//武田F1状态
	float WuTianF1Tick = 0;
	//宁红夜开V时
	float NingHongYeTick = 0;
	//殷紫萍灵机护体时
	bool YinZiPing_F2 = false;
	//火男怒火焚身时
	bool HuoNan_V = false;
	//胡为开V时
	bool HuWei_V = false;
	//大佛变身时
	bool Dafo_V = false;
	//是否晕眩状态
	bool IsStunned = false;
	//沈妙开v时
	bool ShenMiao_V = false;
	//岳山开V时
	bool YueShan_V = false;
	//殷紫萍不死模式
	bool YinZiPing_DontDie = false;
	//玩家被束缚了
	bool Shufu_NeedE = false;
	//玩家被大佛抓了
	bool DafoHandMode = false;
	//妖刀姬开大时
	bool YaoDaoji_V = false;
	//被崔三娘水泡命中时
	bool CuiSanNiang_F = false;
	//被崔三娘水矛扎时
	bool CuiSanNiang_V_tied = false;
	//被顾清寒冰冻时;
	bool GuQingHan_Frost = false;
	//顾清寒冰块冻结时
	bool GuQingHan_F = false;
	//顾清寒开大时
	bool GuQingHan_V = false;
	//免疫冻结
	bool ImmuneFrostTime = false;
	//顾清寒F冲刺前摇
	bool GuQingHan_V_FDash_delaying = false;
	//顾清寒F冲刺阶段
	bool GuQingHan_V_F_Dashing = false;
	//武田开大时
	bool WuTian_V = false;
	//崔三娘开大时
	bool CuiSanNiang_V = false;
	//拼刀标志位
	bool IsBounce = false;;
	//磐石标志位
	bool IsBladeParry = false;
	//滑铲标志位
	bool IsSlider = false;
	//是否振刀成功中
	bool IsShock = false;
	//季莹莹是否开大
	bool JiYingYing_V = false;
	//季莹莹鬼魂状态
	bool JiYingYing_Ghost = false;
	//是否被振刀中
	bool IsShocked = false;
	//是否迷魂状态
	bool IsCharm = false;
	//是否免疫迷魂
	bool ImmuneCharmTime = false;
	//沙男F吹起时
	bool IsShaNanFHurt = false;
	//正在振刀中
	bool IsParry = false;
	//PropertyData 信息
	PropertyData Property;
	//僵直信息
	HurtInfo HurtData;
}BasicPlayerInfo;

struct LC_PLAYER_INFO {
	int   PlayerFID = 0;
	bool  IsValid = false;
	LC_ACTORMODEL ActorModel = 0;

	//请注意.玩家信息与全部脚本共享数据 包括主核心. 请不要对其进行任何错误更改. 

	BasicActiontime BasicActiontime;
	BasicHurtInfo BasicHurtInfo;
	BasicPlayerInfo BasicPlayerInfo;

	//===================下面不需要管了 由Zenbot核心获取杂项 请勿篡改
	char _pad[232];
};
struct LC_SDK_BasicLogic {
	//安全平滑移动
	void (*SmoothMoveTo)(ULONG dx, ULONG dy, float inc);
	//获取目标是否可继续攻击
	bool (*CanAgainAttack)(LC_PLAYER_INFO* PlayerPtr);
	//获取最佳长闪位置
	char (*GetBestDashKey)(LC_PLAYER_INFO* TargetData, float Distance);
	//通用的攻击
	bool (*ExecuteAA)(int Type, int wait);
	//通用 执行CC操作
	bool (*ExecuteCC)();
	//通用 执行CA操作
	bool (*ExecuteCA)();
	//通用 执行CB操作
	bool (*ExecuteCB)();
	//通用 执行跳A操作
	bool (*ExecuteJumpAA)(int Type, bool PressW);
	//通用 执行长闪
	bool (*KeyShift_Long_flash)(int TYPE, char MoveKey, bool ZhongShan, int delay, float KeepWTime);
	//通用 执行短闪
	bool (*ShortFlash)(char MoveKey, bool EvadeMode);
	//通用 执行闪切
	bool (*DodgeChangeWeapon)(int ID);
	//通用 执行C切
	bool (*CrouchChangeWeapon)(int ID);
	//通用 执行滑铲升龙
	bool (*ExecuteSliderB)();
	//通用动画计时器&&可下一次攻击检查
	bool (*StartWaitComboDelay)(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy, float Delay);
};
struct LC_SDK_KEYBOARD {
	//设置连招开始 防止与其他连招函数冲突 不过均由自己定义 也可以不用此函数但要确保代码无误
	void (*Set_IsExecuteCombo)(bool enable);
	//获取连招是否开始
	bool (*Get_IsExecuteCombo)();

	//设置左键是否按下 | 这里一定要组合 防止一些卡键或按键冲突情况!
	void (*Set_IsActiveLeft)(bool enable);
	//获取左键是否按下
	bool (*Get_IsActiveLeft)();

	//设置右键是否按下 | 这里一定要组合 防止一些卡键或按键冲突情况!
	void (*Set_IsActiveRight)(bool enable);
	//获取右键是否按下
	bool (*Get_IsActiveRight)();

	//设置C键是否按下 | 这里一定要组合 防止一些卡键或按键冲突情况!
	void (*Set_IsActiveCrouch)(bool enable);
	//获取C键是否按下
	bool (*Get_IsActiveCrouch)();

	//设置空格键是否按下 | 这里一定要组合 防止一些卡键或按键冲突情况!
	void (*Set_IsActiveSpace)(bool enable);
	//获取空格键是否按下
	bool (*Get_IsActiveSpace)();

	//获取一个按键是否按下 传递键码 例如 'A' 或者VK_XXX
	bool (*IsPressKey)(int hKey);

	//获取是否进行连招 ActiceMode == 1为常规 == 2为魂玉连招 All为全部接收
	bool (*ActiveCombo)(ActiveMode Mode);

	//发生按键信息 Key为按键 0按下 2抬起
	void (*SendKeyBoard)(WORD Key, DWORD flags);

	//发生移动消息
	void (*SendMouseMove)(LONG dx, LONG dy);

	//发生鼠标信息 MOUSEEVENTF_XXXXX
	void (*SendMouseEventBoard)(UINT Key);

	//抬起全部按键
	void (*StopCombo)();

	//发送一次短闪事件. 请通过 CanExecuteFlash == true时再执行!
	bool (*SendDodgeEvent)(char Key);

	//进阶 -> 基础连招函数
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
	//获取本地玩家实例
	LC_ACTORMODEL(*GetLocalPlayer)();
	//获取玩家阵营
	int (*GetTeam)(LC_ACTORMODEL Player);
	//获取玩家唯一标识符
	int (*GetPlayerfid)(LC_ACTORMODEL Player);
	//获取玩家当前坐标
	Vector3(*GetPlayerPosition)(LC_ACTORMODEL Player);
	//获取玩家英雄ID
	int (*GetPlayerHeroID)(LC_ACTORMODEL Ptr);
	//获取玩家ACTORKIT
	LC_ACTORKIT(*GetPlayerActorKit)(LC_ACTORMODEL ActorModel);
	//从ActorKit中返回到ActorModel
	LC_ACTORMODEL(*GetActorModelByKit)(LC_ACTORKIT ActorKit);
	//获取PropertyData信息
	PropertyData(*GetPropertyData)(LC_ACTORMODEL ActorModel);
	//获取EndureLevel
	int (*GetPlayerEndureLevel)(LC_ACTORMODEL ActorModel);
	//获取RangeReactionType
	bool (*GetRangeReactionType)(LC_ACTORMODEL Ptr);
	//获取当前手持武器ID
	int (*GetWeaponID)(LC_ACTORMODEL adr);
	//获取当前手持具体ID
	int (*GetRealWeaponID)(LC_ACTORMODEL adr);
	//获取当前玩家蓄力时间
	float (*GetChargeTime)(LC_ACTORMODEL ActorModel);
	//获取目标是否死亡
	BOOL(*IsDead)(LC_ACTORMODEL adr);
	//获取目标是否为人机
	bool (*IsRobot)(LC_ACTORMODEL adr);
	//获取目标BeControlledStatus
	int (*GetBeControlledStatus)(LC_ACTORMODEL adr);
	//获取目标HeroBattleStatus
	int(*GetHeroBattleStatus)(LC_ACTORMODEL adr);
	//获取目标PlayerMovementIndex
	int (*GetPlayerMovementIndex)(LC_ACTORMODEL adr);
	//获取目标当前蓄力层级
	int (*GetChargeLevel)(LC_ACTORMODEL ActorModel);
	//玩家是否进入蓄力状态
	bool (*PlayerIsCharge)(LC_ACTORMODEL ActorModel);
	//判断目标是否可以蓄力转白刀
	bool (*PlayerIsChargeRelay)(LC_ACTORMODEL ActorModel);
	//获取目标当前动作文本
	bool (*GetActionName)(LC_ACTORMODEL Obj, std::wstring& Output);
	//获取受击动画时间 X:ActionTicks, Y:ActionDuration, Z:VaildTime
	Vector3(*GetActionVaildTime)(LC_ACTORMODEL Obj);
	//获取当前动画时间 X:Timer Y:StartTime Y:ValidTime
	Vector3(*GetActionSnapVaildTime)(LC_ACTORMODEL Obj);
	//获取游戏窗口宽高
	Vector2(*GetWindowSize)();
	//判断目标是否受击状态
	bool (*IsHiting)(LC_ACTORMODEL Obj);
	//获取目标上一次受击时间
	float (*GetPlayerLastHitTime)(LC_ACTORMODEL Player);
	//获取目标龙虎功层级
	int (*GetDragonBlockStack)(LC_ACTORMODEL Obj);
	//获取当前武器在栏位位置
	int (*GetWeaponIndex)();
	//获取二号位武器ID.返回按键位置
	int (*GetDoubleWeaponID)(char* InputKey);
	//获取本人技能信息  TYPE == 1普通  ==2 终极
	LocalSkillData(*GetSkillID)(LC_ACTORMODEL ActorModel, DWORD TYPE);
	//获取指定类型技能 SkillType == 1 高霸体(蓝金以上)技能 SkillType == 2受击可用
	AppointSkill(*GetAppointSkill)(LC_ACTORMODEL ActorModel, DWORD SkillType);
	//获取目标是否处于格挡状态 常见为火男F等等
	bool (*IsParryGroupMode)(LC_ACTORMODEL Adr);
	//判断是否可以格挡反击 常见为火男F等等
	bool (*CanParryGroupAttack)(LC_ACTORMODEL Obj);
	//获取玩家骨骼位置
	void (*GetBonePosition)(LC_ACTORMODEL Entity, Vector3* resultPos, int index);
	//获取CurTimeForLogicState
	float (*GetCurTimeForLogicState)(LC_ACTORMODEL LocalPlayer);
	//获取当前锁定目标
	LC_ACTORMODEL(*GetLockTargetBase)();
	//获取游戏延迟
	float (*GetGamePing)();
	//将坐标转换为屏幕
	BOOL(*WorldToScreen)(Vector3 WorldLocation, Vector3& retPos);
	//获取玩家BuffId
	BOOL(*HasBuff)(LC_ACTORMODEL Player, int BuffId);
	//获取游戏全局时间
	float (*GetGlobalTime)();
	//获取玩家当前哈希
	int (*GetStateNameHash)(LC_ACTORMODEL Player);
	//获取魂玉物品
	bool (*HasSoulItem)(LC_ACTORMODEL Player, int SoulId);
	//获取物品计次
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
	//核心
	LC_CORE CORE;
	//键鼠函数
	LC_SDK_KEYBOARD KEYBOARD;
	//数学函数
	LC_SDK_MATH MATH;
	//世界实体
	LC_WORLD_ENTITY WORLD;
	//游戏函数
	LC_SDK_PLAYER_BASEINFO GAMEFUNC;
	//哈希名匹配
	LC_SDK_ANIMHASH ANIMHASH;
	//工具类
	LC_SDK_UTILITY UTILITY;
};
