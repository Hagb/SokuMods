//
// Created by PinkySmile on 04/12/2020
//

#include <Windows.h>
#include "Exceptions.hpp"
#include "Network/Handlers.hpp"
#include "State.hpp"
#include "Utils/InputBox.hpp"
#include <Shlwapi.h>
#include <SokuLib.hpp>
#include <algorithm>
#include <dinput.h>

static bool gameStarted = false;
static bool sessionStarted = false;
static int (__stdcall *s_origRecvFrom)(SOCKET s, char * buf, int len, int flags, sockaddr * from, int * fromlen);

int __stdcall myRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	auto packet = reinterpret_cast<SokuLib::Packet *>(buf);
	int result = s_origRecvFrom(s, buf, len, flags, from, fromlen);

	if (packet->type == SokuLib::HOST_GAME && packet->game.event.type == SokuLib::GAME_MATCH && packet->game.event.match.host.deckId >= 5 && packet->game.event.match.client().deckId >= 5) {
		_cache.leftScore = packet->game.event.match.host.deckId - 5;
		_cache.rightScore = packet->game.event.match.client().deckId - 5;
		_cache.recvScores = true;
	}
	return result;
}

int __fastcall CTitle_OnProcess(SokuLib::Title *This) {
	// super
	int ret = (This->*s_origCTitle_Process)();

	if (gameStarted)
		broadcastOpcode(GAME_ENDED, "null");
	if (sessionStarted)
		broadcastOpcode(SESSION_ENDED, "null");
	_cache.recvScores = false;
	gameStarted = false;
	sessionStarted = false;
	needReset = true;
	needRefresh = true;
	checkKeyInputs();
	return ret;
}

int __fastcall CBattleWatch_OnProcess(SokuLib::BattleWatch *This) {
	// super
	int ret = (This->*s_origCBattleWatch_Process)();

	if (!gameStarted)
		broadcastOpcode(GAME_STARTED, "null");
	if (!sessionStarted)
		broadcastOpcode(SESSION_STARTED, "null");
	gameStarted = true;
	sessionStarted = true;
	updateCache(true);
	return ret;
}

int __fastcall CBattle_OnProcess(SokuLib::Battle *This) {
	// super
	int ret = (This->*s_origCBattle_Process)();

	if (!gameStarted)
		broadcastOpcode(GAME_STARTED, "null");
	if (!sessionStarted)
		broadcastOpcode(SESSION_STARTED, "null");
	gameStarted = true;
	sessionStarted = true;
	if (SokuLib::mainMode == SokuLib::BATTLE_MODE_VSPLAYER)
		updateCache(false);
	return ret;
}

void loadCommon() {
	if (gameStarted)
		broadcastOpcode(GAME_ENDED, "null");
	if (!sessionStarted)
		broadcastOpcode(SESSION_STARTED, "null");
	gameStarted = false;
	sessionStarted = true;
	needRefresh = true;
	checkKeyInputs();
}

int __fastcall CLoading_OnProcess(SokuLib::Loading *This) {
	// super
	int ret = (This->*s_origCLoading_Process)();

	loadCommon();
	return ret;
}

int __fastcall CLoadingWatch_OnProcess(SokuLib::LoadingWatch *This) {
	// super
	int ret = (This->*s_origCLoadingWatch_Process)();

	loadCommon();
	return ret;
}

int __fastcall CBattleManager_KO(SokuLib::BattleManager *This) {
	// super
	int ret = (This->*s_origCBattleManager_KO)();

	onKO();
	return ret;
}

int __fastcall CBattleManager_Start(SokuLib::BattleManager *This) {
	// super
	int ret = (This->*s_origCBattleManager_Start)();

	onRoundStart();
	return ret;
}

int __fastcall CBattleManager_Render(SokuLib::BattleManager *This) {
	// super
	int ret = (This->*s_origCBattleManager_Render)();

	checkKeyInputs();
	return ret;
}

// �ݒ胍�[�h
void LoadSettings(LPCSTR profilePath, LPCSTR parentPath) {
#ifdef _DEBUG
	FILE *_;

	AllocConsole();
	freopen_s(&_, "CONOUT$", "w", stdout);
#endif
	port = GetPrivateProfileInt("Server", "Port", 80, profilePath);
	keys[KEY_DECREASE_L_SCORE] = GetPrivateProfileInt("Keys", "DecreaseLeftScore", '1', profilePath);
	keys[KEY_INCREASE_L_SCORE] = GetPrivateProfileInt("Keys", "IncreaseLeftScore", '2', profilePath);
	keys[KEY_CHANGE_L_NAME] = GetPrivateProfileInt("Keys", "ChangeLeftName", '3', profilePath);
	keys[KEY_RESET_SCORES] = GetPrivateProfileInt("Keys", "ResetScores", '5', profilePath);
	keys[KEY_RESET_STATE] = GetPrivateProfileInt("Keys", "ResetState", '6', profilePath);
	keys[KEY_CHANGE_ROUND] = GetPrivateProfileInt("Keys", "ChangeRound", '7', profilePath);
	keys[KEY_DECREASE_R_SCORE] = GetPrivateProfileInt("Keys", "DecreaseRightScore", '8', profilePath);
	keys[KEY_INCREASE_R_SCORE] = GetPrivateProfileInt("Keys", "IncreaseRightScore", '9', profilePath);
	keys[KEY_CHANGE_R_NAME] = GetPrivateProfileInt("Keys", "ChangeRightName", '0', profilePath);

	puts("Create webserver");
	webServer = std::make_unique<WebServer>();
	webServer->addRoute("/", root);
	webServer->addRoute("/state", state);
	Socket::HttpResponse connect(const Socket::HttpRequest &requ);
	webServer->addRoute("/connect", connect);
	webServer->addStaticFolder("/static", std::string(parentPath) + "/static");
	webServer->start(port);
	webServer->onWebSocketConnect(onNewWebSocket);
}

void hookFunctions() {
	DWORD old;

	::VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, PAGE_EXECUTE_WRITECOPY, &old);
	s_origCTitle_Process        = SokuLib::TamperDword(&SokuLib::VTable_Title.onProcess,          CTitle_OnProcess);
	s_origCBattleWatch_Process  = SokuLib::TamperDword(&SokuLib::VTable_BattleWatch.onProcess,    CBattleWatch_OnProcess);
	s_origCLoadingWatch_Process = SokuLib::TamperDword(&SokuLib::VTable_LoadingWatch.onProcess,   CLoadingWatch_OnProcess);
	s_origCBattle_Process       = SokuLib::TamperDword(&SokuLib::VTable_Battle.onProcess,         CBattle_OnProcess);
	s_origCLoading_Process      = SokuLib::TamperDword(&SokuLib::VTable_Loading.onProcess,        CLoading_OnProcess);
	s_origCBattleManager_Start  = SokuLib::TamperDword(&SokuLib::VTable_BattleManager.onSayStart, CBattleManager_Start);
	s_origCBattleManager_KO     = SokuLib::TamperDword(&SokuLib::VTable_BattleManager.onKO,       CBattleManager_KO);
	s_origRecvFrom              = SokuLib::TamperDword(&SokuLib::DLL::ws2_32.recvfrom,            myRecvFrom);
	::VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, old, &old);
	::FlushInstructionCache(GetCurrentProcess(), NULL, 0);
}

extern "C" __declspec(dllexport) bool CheckVersion(const BYTE hash[16]) {
	return true;
}

extern "C" __declspec(dllexport) bool Initialize(HMODULE hMyModule, HMODULE hParentModule) {
	try {
		char profilePath[1024 + MAX_PATH];
		char profileParent[1024 + MAX_PATH];

		GetModuleFileName(hMyModule, profilePath, 1024);
		PathRemoveFileSpec(profilePath);
		strcpy(profileParent, profilePath);
		PathAppend(profilePath, "SokuStreaming.ini");
		LoadSettings(profilePath, profileParent);

		puts("Hook functions");
		hookFunctions();
	} catch (std::exception &e) {
		MessageBoxA(nullptr, e.what(), "Cannot init SokuStreaming", MB_OK | MB_ICONERROR);
		abort();
	} catch (...) {
		MessageBoxA(nullptr, "Wtf ?", "Huh... ok", MB_OK | MB_ICONERROR);
		abort();
	}
	puts("Initialize ended");
	return true;
}

extern "C" int APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	if(fdwReason == DLL_PROCESS_DETACH)
		webServer.reset();
	return TRUE;
}