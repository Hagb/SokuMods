//
// Created by Gegel85 on 04/12/2020
//

#include <algorithm>
#include <SokuLib.hpp>
#include <Windows.h>
#include <Shlwapi.h>
#include "ShiftJISDecoder.hpp"
#include "Network/WebServer.hpp"

static bool enabled;
static unsigned short port;
static WebServer webServer;
static struct CachedMatchData {
	SokuLib::Character left;
	SokuLib::Character right;
	std::vector<unsigned short> leftCards;
	std::vector<unsigned short> rightCards;
	std::vector<unsigned short> leftHand;
	std::vector<unsigned short> rightHand;
	std::vector<unsigned short> leftUsed;
	std::vector<unsigned short> rightUsed;
	std::string leftName;
	std::string rightName;
	unsigned int oldLeftScore;
	unsigned int oldRightScore;
	unsigned int leftScore;
	unsigned int rightScore;
} cache;
struct Title{};
struct BattleCL{};
struct BattleSV{};
struct BattleWatch{};
static bool needReset;
static bool needDeckRefresh;
static int (__thiscall BattleWatch::*s_origCBattleWatch_Render)();
static int (__thiscall BattleCL::*s_origCBattleCL_Render)();
static int (__thiscall BattleSV::*s_origCBattleSV_Render)();
static int (__thiscall Title::*s_origCTitle_Render)();

/*static int (__thiscall C::*array[16])();

#define TEST(i)int __fastcall fun##i(C *This)\
{                                            \
        printf(#i "\n");                  \
	return (This->*array[i])();\
}                                            \

TEST(0)
TEST(1)
TEST(2)
TEST(3)
TEST(4)
TEST(5)
TEST(6)
TEST(7)
TEST(8)
TEST(9)
TEST(10)
TEST(11)
TEST(12)
TEST(13)
TEST(14)
TEST(15)

int (__fastcall *funs[16])(C *This) = {
	fun0,
	fun1,
	fun2,
	fun3,
	fun4,
	fun5,
	fun6,
	fun7,
	fun8,
	fun9,
	fun10,
	fun11,
	fun12,
	fun13,
	fun14,
	fun15,
};*/

Socket::HttpResponse root(const Socket::HttpRequest &requ)
{
	Socket::HttpResponse response;

	response.header["Location"] = "http://" + requ.host + "/static/html/overlay.html";
	response.returnCode = 301;
	return response;
}

std::string serializeUShortArray(const std::vector<unsigned short> &arr, int maxSize)
{
	std::string result;

	result.reserve(maxSize);
	result += "[";
	if (!arr.empty())
		result += std::to_string(arr[0]);
	for (size_t i = 1; i < arr.size(); i++) {
		result += ",";
		result += std::to_string(arr[i]);
	}
	result += "]";
	return result;
}

Socket::HttpResponse state(const Socket::HttpRequest &requ)
{
	Socket::HttpResponse response;
	std::string sanitizedLeftName;
	std::string sanitizedRightName;
	std::string leftDeck;
	std::string rightDeck;

	sanitizedLeftName.reserve(cache.leftName.size() * 3);
	for (char c : convertShiftJisToUTF8(cache.leftName.c_str()))
		switch (c) {
		case '"':
		case '\\':
			sanitizedLeftName.push_back('\\');
		default:
			sanitizedLeftName.push_back(c);
		}

	sanitizedRightName.reserve(cache.rightName.size() * 3);
	for (char c : convertShiftJisToUTF8(cache.rightName.c_str()))
		switch (c) {
		case '"':
		case '\\':
			sanitizedRightName.push_back('\\');
		default:
			sanitizedRightName.push_back(c);
		}

	response.returnCode = 200;
	response.header["content-type"] = "application/json";
	response.body = "{"
		 "\"left\":{"
			"\"character\":" + std::to_string(cache.left) + ","
			"\"score\":" + std::to_string(cache.leftScore) + ","
			R"("name":")" + sanitizedLeftName + "\","
			"\"deck\":" + serializeUShortArray(cache.leftCards, 2 + 20 * 3 + 19) + ","
			"\"used\":" + serializeUShortArray(cache.leftUsed, 2 + 20 * 3 + 19) + ","
			"\"hand\":" + serializeUShortArray(cache.leftHand, 2 + 20 * 3 + 19) +
		"},"
		"\"right\":{"
			"\"character\":" + std::to_string(cache.right) + ","
			"\"score\":" + std::to_string(cache.rightScore) + ","
			R"("name":")" + sanitizedRightName + "\","
			"\"deck\":" + serializeUShortArray(cache.rightCards, 2 + 20 * 3 + 19) + ","
			"\"used\":" + serializeUShortArray(cache.rightUsed, 2 + 20 * 3 + 19) + ","
			"\"hand\":" + serializeUShortArray(cache.rightHand, 2 + 20 * 3 + 19) +
		"}"
	"}";
	return response;
}

void updateCache(int ret)
{
	auto &battleMgr = SokuLib::getBattleMgr();
	auto &netObj = SokuLib::getNetObject();

	if (needReset) {
		cache.leftScore = 0;
		cache.rightScore = 0;
		needReset = false;
	}

	auto oldLeftCards = cache.leftCards;
	auto oldRightCards = cache.rightCards;

	cache.leftCards.clear();
	cache.rightCards.clear();
	cache.leftHand.clear();
	cache.rightHand.clear();

	int CardsLeftOld = battleMgr.leftCharacterManager->mDeckInfo2Obj.UnknownSubStructure.CardsLeft;
	int UCCOld = battleMgr.leftCharacterManager->mDeckInfo2Obj.UnknownSubStructure.UnknownCardCounter;
	if (CardsLeftOld == 20)
		cache.leftUsed.clear();
	for (int i = 0; i < CardsLeftOld; i++)
		cache.leftCards.push_back(SokuLib::getCard(&battleMgr.leftCharacterManager->mDeckInfo2Obj));
	battleMgr.leftCharacterManager->mDeckInfo2Obj.UnknownSubStructure.CardsLeft = CardsLeftOld;
	battleMgr.leftCharacterManager->mDeckInfo2Obj.UnknownSubStructure.UnknownCardCounter = UCCOld;
	std::sort(cache.leftCards.begin(), cache.leftCards.end());

	CardsLeftOld = battleMgr.rightCharacterManager->mDeckInfo2Obj.UnknownSubStructure.CardsLeft;
	UCCOld = battleMgr.rightCharacterManager->mDeckInfo2Obj.UnknownSubStructure.UnknownCardCounter;
	if (CardsLeftOld == 20)
		cache.rightUsed.clear();
	for (int i = 0; i < CardsLeftOld; i++)
		cache.rightCards.push_back(SokuLib::getCard(&battleMgr.rightCharacterManager->mDeckInfo2Obj));
	battleMgr.rightCharacterManager->mDeckInfo2Obj.UnknownSubStructure.CardsLeft = CardsLeftOld;
	battleMgr.rightCharacterManager->mDeckInfo2Obj.UnknownSubStructure.UnknownCardCounter = UCCOld;
	std::sort(cache.rightCards.begin(), cache.rightCards.end());

	for (auto id : cache.leftCards) {
		auto it = std::find(oldLeftCards.begin(), oldLeftCards.end(), id);

		if (it != oldLeftCards.end())
			oldLeftCards.erase(it);
	}
	for (auto id : cache.rightCards) {
		auto it = std::find(oldRightCards.begin(), oldRightCards.end(), id);

		if (it != oldRightCards.end())
			oldRightCards.erase(it);
	}
	for (auto id : oldLeftCards)
		cache.leftUsed.push_back(id);
	for (auto id : oldRightCards)
		cache.rightUsed.push_back(id);

	std::sort(cache.leftUsed.begin(), cache.leftUsed.end());
	std::sort(cache.rightUsed.begin(), cache.rightUsed.end());
	if (
		cache.oldLeftScore != battleMgr.leftCharacterManager->score ||
		cache.oldRightScore != battleMgr.rightCharacterManager->score
	) {
		cache.leftScore += battleMgr.leftCharacterManager->score == 2;
		cache.rightScore += battleMgr.rightCharacterManager->score == 2;
	}
	cache.oldLeftScore = battleMgr.leftCharacterManager->score;
	cache.oldRightScore = battleMgr.rightCharacterManager->score;
	cache.left = SokuLib::leftChar;
	cache.right = SokuLib::rightChar;
	cache.leftName = netObj.profile1name;
	cache.rightName = netObj.profile2name;
}

int __fastcall CTitle_OnRender(Title *This) {
	// super
	int ret = (This->*s_origCTitle_Render)();

	needReset = true;
	needDeckRefresh = true;
	return ret;
}

int __fastcall CBattleWatch_OnRender(BattleWatch *This) {
	// super
	int ret = (This->*s_origCBattleWatch_Render)();

	updateCache(ret);
	return ret;
}

int __fastcall CBattleSV_OnRender(BattleSV *This) {
	// super
	int ret = (This->*s_origCBattleSV_Render)();

	updateCache(ret);
	return ret;
}

int __fastcall CBattleCL_OnRender(BattleCL *This) {
	// super
	int ret = (This->*s_origCBattleCL_Render)();

	updateCache(ret);
	return ret;
}

// �ݒ胍�[�h
void LoadSettings(LPCSTR profilePath, LPCSTR parentPath)
{
	// �����V���b�g�_�E��
	enabled = GetPrivateProfileInt("SokuStreaming", "Enabled", 1, profilePath) != 0;
	port = GetPrivateProfileInt("Server", "Enabled", 80, profilePath);
	webServer.addRoute("/", root);
	webServer.addRoute("/state", state);
	webServer.addStaticFolder("/static", std::string(parentPath) + "/static");
	webServer.start(port);
}

extern "C"
__declspec(dllexport) bool CheckVersion(const BYTE hash[16])
{
	return true;
}

extern "C"
__declspec(dllexport) bool Initialize(HMODULE hMyModule, HMODULE hParentModule)
{
	char profilePath[1024 + MAX_PATH];
	char profileParent[1024 + MAX_PATH];

	GetModuleFileName(hMyModule, profilePath, 1024);
	PathRemoveFileSpec(profilePath);
	strcpy(profileParent, profilePath);
	PathAppend(profilePath, "SokuStreaming.ini");
	LoadSettings(profilePath, profileParent);
	DWORD old;
	::VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, PAGE_EXECUTE_WRITECOPY, &old);
	s_origCTitle_Render = SokuLib::union_cast<int (Title::*)()>(SokuLib::TamperDword(SokuLib::vtbl_CTitle + 8, reinterpret_cast<DWORD>(CTitle_OnRender)));
	s_origCBattleCL_Render = SokuLib::union_cast<int (BattleCL::*)()>(SokuLib::TamperDword(SokuLib::vtbl_CBattleCL + 0x08, reinterpret_cast<DWORD>(CBattleCL_OnRender)));
	s_origCBattleSV_Render = SokuLib::union_cast<int (BattleSV::*)()>(SokuLib::TamperDword(SokuLib::vtbl_CBattleSV + 0x08, reinterpret_cast<DWORD>(CBattleSV_OnRender)));
	s_origCBattleWatch_Render = SokuLib::union_cast<int (BattleWatch::*)()>(SokuLib::TamperDword(SokuLib::vtbl_CBattleWatch + 0x08, reinterpret_cast<DWORD>(CBattleWatch_OnRender)));
	::VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, old, &old);

	/*for (int i = 0; i < 16; i++) {
		DWORD addr = SokuLib::ADDR_VTBL_BATTLE_CL + 0x08 + 60 + 4 * i + 0x40 * 3;

		printf("%i: 0x%X\n", i, addr);
		array[i] = SokuLib::union_cast<int (C::*)()>(SokuLib::TamperDword(addr, reinterpret_cast<DWORD>(funs[i])));
	}*/

	::FlushInstructionCache(GetCurrentProcess(), NULL, 0);

	return true;
}

extern "C"
int APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	return TRUE;
}