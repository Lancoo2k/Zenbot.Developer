#include <Windows.h>
#include "Tools.h"

LC_SDK_STRUCT* SDK = nullptr;

WeaponFunc* baseArray[10]{ new Katana(),new Dagger(),new Blade(),new Twinblades(),new Dualhalberd(),new Nunchucks(),new Saber(),new Rod(),new Sword(),new Spear() };

//全局 - 切刀时间
float ChangeWeaponTime = 0.f;
//全局 - 白刀禁止
float DisableWeaponWhiteMode = 0.f;
//全局 - 延长蓄力时间
float AllowResetChargeTime = 0.f;
bool activeSKILL = true;

LC_Function LC_SDK_STRUCT* InitLCSDK(LC_SDK_STRUCT* Ptr) {
	SDK = Ptr;
	return SDK;
}

LC_Function void ExecuteCode(LC_PLAYER_INFO* LocalPlayer, LC_PLAYER_INFO* Enemy) {
	//auto time1 = GetTickCount64();
	//std::wstring name;
	//auto g = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
	//float hhh = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
	//auto validTime = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel); 
	//SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, name);
	//float yaw = fabsf(SDK->MATH.AngleDifference(SDK->MATH.get_enityangle(Enemy->ActorModel), SDK->MATH.VectorToRotationYaw(SDK->MATH.FindLookAtVector(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel), SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel)))));//敌人面向 自己 的 角度

	////printf("%d %d %ls\n", SDK->GAMEFUNC.GetWeaponID(LocalPlayer->ActorModel), SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(LocalPlayer->ActorModel), name.c_str());
	//printf("%f | %ls | enemyhurt:%f | %f %f %d yaw:%f\n", g, name.c_str(), SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y, hhh, validTime.z,SDK->GAMEFUNC.GetStateNameHash(LocalPlayer->ActorModel),yaw);


	///printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	//printf("调用时间x1:%d\n", GetTickCount64() - time1);
	
	/*auto g = SDK->GAMEFUNC.GetCurTimeForLogicState(LocalPlayer->ActorModel);
	float hhh = SDK->GAMEFUNC.GetGlobalTime() - SDK->GAMEFUNC.GetPlayerLastHitTime(Enemy->ActorModel);
	auto validTime = SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel);*/

	//printf("%f | %ls | enemyhurt:%f | %f %f %d\n", g, name.c_str(), SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y, hhh, validTime.z,SDK->GAMEFUNC.GetStateNameHash(LocalPlayer->ActorModel));
	//printf("%d\n", GetTickCount());
	//printf("%f %f %f| %f %f %f\n", LocalPlayer->BasicPlayerInfo.Velocity.x, LocalPlayer->BasicPlayerInfo.Velocity.y, LocalPlayer->BasicPlayerInfo.Velocity.z,
	//	Enemy->BasicPlayerInfo.Velocity.x, Enemy->BasicPlayerInfo.Velocity.y, Enemy->BasicPlayerInfo.Velocity.z);
	//std::wstring name;
	//bool sucget = SDK->GAMEFUNC.GetActionName(LocalPlayer->ActorModel, name);
	//printf("sucget:%d name:%ls\n", sucget,name.c_str());

	//printf("fwwwwwwww   %d %d %d %d\n", SDK->GAMEFUNC.GetPlayerMovementIndex(LocalPlayer->ActorModel) , SDK->GAMEFUNC.IsParryGroupMode(LocalPlayer->ActorModel) , SDK->GAMEFUNC.GetBeControlledStatus(LocalPlayer->ActorModel),SDK->GAMEFUNC.GetPlayerEndureLevel(LocalPlayer->ActorModel));

	for (const auto array : baseArray) {
		array->ExecuteCode(LocalPlayer,Enemy);
	}
}
LC_Function void LibraryInit() {

	/*

		在该函数调用时.说明脚本已经成功运行 这里是初始化你脚本的操作
		请最好不加任何高延迟代码 例如 Sleep(1000)乃至更久 这样会卡主辅助主进程. 后面的一些脚本加载可能导致缓慢.

	*/
	//printf("zrrr\n");
	for (const auto array : baseArray) {
		array->LibraryInit();
	}

}
LC_Function void FreeEvent() {

	/*

		在该函数调用时.说明脚本执行结束.
		如果你创建了一些线程 或编写了一些其他脱离环境的函数. 你需要在这里释放或回收你的函数.
		比如我使用std::thread 或 CreateThread 创建了一个线程. 那么在这里你需要终止你的线程. 否则会产生一些难以预估的错误.
		推荐使用更安全的方法释放. 比如在线程中编写标志位 while(!flag) 这样flag == true时 线程会自动返回 这样是安全的释放线程

	*/

	for (const auto array : baseArray) {
		array->FreeEvent();
	}
}

LC_Function void MainExecuteCode() {
	/*

		在该函数调用时.说明脚本在主线程中循环调用此函数
		请勿使用高Sleep函数.

	*/
	
	for (const auto array : baseArray) {
		array->MainExecuteCode();
	}
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