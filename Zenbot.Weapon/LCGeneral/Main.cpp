#include <Windows.h>
#include "Tools.h"

LC_SDK_STRUCT* SDK = nullptr;

WeaponFunc* baseArray[10]{ new Katana(),new Dagger(),new Blade(),new Twinblades(),new Dualhalberd(),new Nunchucks(),new Saber(),new Rod(),new Sword(),new Spear() };

//ȫ�� - �е�ʱ��
float ChangeWeaponTime = 0.f;
//ȫ�� - �׵���ֹ
float DisableWeaponWhiteMode = 0.f;
//ȫ�� - �ӳ�����ʱ��
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
	//float yaw = fabsf(SDK->MATH.AngleDifference(SDK->MATH.get_enityangle(Enemy->ActorModel), SDK->MATH.VectorToRotationYaw(SDK->MATH.FindLookAtVector(SDK->GAMEFUNC.GetPlayerPosition(Enemy->ActorModel), SDK->GAMEFUNC.GetPlayerPosition(LocalPlayer->ActorModel)))));//�������� �Լ� �� �Ƕ�

	////printf("%d %d %ls\n", SDK->GAMEFUNC.GetWeaponID(LocalPlayer->ActorModel), SDK->ANIMHASH.GET_STACK_VALUE_BY_OBJ(LocalPlayer->ActorModel), name.c_str());
	//printf("%f | %ls | enemyhurt:%f | %f %f %d yaw:%f\n", g, name.c_str(), SDK->GAMEFUNC.GetActionVaildTime(Enemy->ActorModel).y, hhh, validTime.z,SDK->GAMEFUNC.GetStateNameHash(LocalPlayer->ActorModel),yaw);


	///printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	//printf("����ʱ��x1:%d\n", GetTickCount64() - time1);
	
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

		�ڸú�������ʱ.˵���ű��Ѿ��ɹ����� �����ǳ�ʼ����ű��Ĳ���
		����ò����κθ��ӳٴ��� ���� Sleep(1000)�������� �����Ῠ������������. �����һЩ�ű����ؿ��ܵ��»���.

	*/
	//printf("zrrr\n");
	for (const auto array : baseArray) {
		array->LibraryInit();
	}

}
LC_Function void FreeEvent() {

	/*

		�ڸú�������ʱ.˵���ű�ִ�н���.
		����㴴����һЩ�߳� ���д��һЩ�������뻷���ĺ���. ����Ҫ�������ͷŻ������ĺ���.
		������ʹ��std::thread �� CreateThread ������һ���߳�. ��ô����������Ҫ��ֹ����߳�. ��������һЩ����Ԥ���Ĵ���.
		�Ƽ�ʹ�ø���ȫ�ķ����ͷ�. �������߳��б�д��־λ while(!flag) ����flag == trueʱ �̻߳��Զ����� �����ǰ�ȫ���ͷ��߳�

	*/

	for (const auto array : baseArray) {
		array->FreeEvent();
	}
}

LC_Function void MainExecuteCode() {
	/*

		�ڸú�������ʱ.˵���ű������߳���ѭ�����ô˺���
		����ʹ�ø�Sleep����.

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