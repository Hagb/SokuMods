//
// Created by Gegel85 on 31/10/2020
//

#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <shlwapi.h>
#include <dinput.h>
#include <SokuLib.hpp>
#include "BattleAnimation.hpp"
#include "Menu.hpp"
#include "Pack.hpp"
#include "Actions.hpp"

#ifndef _DEBUG
#define puts(...)
#define printf(...)
#endif

static bool goToTitle = false;
static int (SokuLib::Battle::* ogBattleOnProcess)();
static int (SokuLib::Battle::* ogBattleOnRender)();
static int (SokuLib::Title::* ogTitleOnProcess)();
static int (SokuLib::MenuResult::* ogResultOnProcess)();
static int (SokuLib::MenuResult::* ogResultOnRender)();
static SokuLib::MenuResult *(SokuLib::MenuResult::* ogResultOnDestruct)(unsigned char);
static bool stopRepeat = false;
void loadSoku2CSV(LPWSTR path)
{
	std::ifstream stream{path};
	std::string line;

	if (stream.fail()) {
		printf("%S: %s\n", path, strerror(errno));
		return;
	}
	while (std::getline(stream, line)) {
		std::stringstream str{line};
		unsigned id;
		std::string idStr;
		std::string codeName;
		std::string shortName;
		std::string fullName;
		std::string skillInputs;

		std::getline(str, idStr, ';');
		std::getline(str, codeName, ';');
		std::getline(str, shortName, ';');
		std::getline(str, fullName, ';');
		std::getline(str, skillInputs, ';');
		if (str.fail()) {
			printf("Skipping line %s: Stream failed\n", line.c_str());
			continue;
		}
		try {
			id = std::stoi(idStr);
		} catch (...) {
			printf("Skipping line %s: Invalid id\n", line.c_str());
			continue;
		}

		auto &infos = characterSkills[static_cast<SokuLib::Character>(id)];

		validCharacters[id] = codeName;
		infos.clear();
		infos.emplace_back();
		for (auto c : skillInputs) {
			if (c == ',')
				infos.emplace_back();
			else
				infos.back() += c;
		}
	}
}

void loadSoku2Config()
{
	puts("Looking for Soku2 config...");

	int argc;
	wchar_t app_path[MAX_PATH];
	wchar_t setting_path[MAX_PATH];
	wchar_t **arg_list = CommandLineToArgvW(GetCommandLineW(), &argc);

	wcsncpy(app_path, arg_list[0], MAX_PATH);
	PathRemoveFileSpecW(app_path);
	if (GetEnvironmentVariableW(L"SWRSTOYS", setting_path, sizeof(setting_path)) <= 0) {
		if (arg_list && argc > 1 && StrStrIW(arg_list[1], L"ini")) {
			wcscpy(setting_path, arg_list[1]);
			LocalFree(arg_list);
		} else {
			wcscpy(setting_path, app_path);
			PathAppendW(setting_path, L"\\SWRSToys.ini");
		}
		if (arg_list) {
			LocalFree(arg_list);
		}
	}
	printf("Config file is %S\n", setting_path);

	wchar_t moduleKeys[1024];
	wchar_t moduleValue[MAX_PATH];
	GetPrivateProfileStringW(L"Module", nullptr, nullptr, moduleKeys, sizeof(moduleKeys), setting_path);
	for (wchar_t *key = moduleKeys; *key; key += wcslen(key) + 1) {
		wchar_t module_path[MAX_PATH];

		GetPrivateProfileStringW(L"Module", key, nullptr, moduleValue, sizeof(moduleValue), setting_path);

		wchar_t *filename = wcsrchr(moduleValue, '/');

		printf("Check %S\n", moduleValue);
		if (!filename)
			filename = app_path;
		else
			filename++;
		for (int i = 0; filename[i]; i++)
			filename[i] = tolower(filename[i]);
		if (wcscmp(filename, L"soku2.dll") != 0)
			continue;

		hasSoku2 = true;
		wcscpy(module_path, app_path);
		PathAppendW(module_path, moduleValue);
		while (auto result = wcschr(module_path, '/'))
			*result = '\\';
		PathRemoveFileSpecW(module_path);
		printf("Found Soku2 module folder at %S\n", module_path);
		PathAppendW(module_path, L"\\config\\info\\characters.csv");
		loadSoku2CSV(module_path);
		return;
	}
}

// �ݒ胍�[�h
void LoadSettings()
{
	char buffer[MAX_PATH];

	puts("Loading settings...");
	*packsLocation = 0;
	if (GetPrivateProfileInt("TrialMode", "IsPathAbsolute", 1, profilePath)) {
		strcpy(packsLocation, profileFolderPath);
		strcat(packsLocation, "\\");
	}
	GetPrivateProfileString("TrialMode", "PackLocation", "packs", buffer, sizeof(buffer), profilePath);
	strcat(packsLocation, buffer);
	strcat(packsLocation, "\\*");

	loadSoku2Config();
}

extern "C" __declspec(dllexport) bool CheckVersion(const BYTE hash[16]) {
	return memcmp(hash, SokuLib::targetHash, 16) == 0;
}

static bool canHaveNextFrame = true;

// ToDo Launch Text function
int __fastcall myBattleOnProcess(SokuLib::Battle *This)
{
	if (!loadedTrial)
		return (This->*ogBattleOnProcess)();

	int buffer = !canHaveNextFrame ? SokuLib::SCENE_BATTLE : (This->*ogBattleOnProcess)();

	canHaveNextFrame = true;
	if (loadedTrial->update(canHaveNextFrame))
		buffer = SokuLib::SCENE_SELECT;
	goToTitle = buffer == SokuLib::SCENE_TITLE;
	if (buffer != SokuLib::SCENE_BATTLE)
		loadedTrial.reset();
	if (buffer == SokuLib::SCENE_SELECT)
		return SokuLib::SCENE_TITLE;
	return buffer;
}

int __fastcall myBattleOnRender(SokuLib::Battle *This)
{
	int buffer = (This->*ogBattleOnRender)();

	if (!loadedTrial)
		return buffer;
	loadedTrial->render();
	return buffer;
}

int __fastcall myTitleOnProcess(SokuLib::Title *This)
{
	int buffer = (This->*ogTitleOnProcess)();

	if (loadRequest) {
		loadRequest = false;
		puts("Moving to LOADING !");
		return SokuLib::SCENE_LOADING;
	}
	return buffer;
}

int __fastcall myResultOnProcess(SokuLib::MenuResult *This)
{
	if (SokuLib::checkKeyOneshot(DIK_ESCAPE, 0, 0, 0)) {
		SokuLib::playSEWaveBuffer(0x29);
		return 0;
	}
	return menuOnProcess(This) && !goToTitle;
}

int __fastcall myResultOnRender(SokuLib::MenuResult *This)
{
	menuOnRender(This);
	return 0;
}

SokuLib::MenuResult *__fastcall myResultOnDestruct(SokuLib::MenuResult *This, int _, unsigned char param)
{
	menuUnloadAssets();
	wasPressed = true;
	goToTitle = false;
	return (This->*ogResultOnDestruct)(param);
}



extern "C" __declspec(dllexport) bool Initialize(HMODULE hMyModule, HMODULE hParentModule)
{
	DWORD old;

#ifdef _DEBUG
	FILE *_;

	AllocConsole();
	freopen_s(&_, "CONOUT$", "w", stdout);
	freopen_s(&_, "CONOUT$", "w", stderr);
#endif

	puts("Initializing...");

	GetModuleFileName(hMyModule, profilePath, 1024);
	PathRemoveFileSpec(profilePath);
	strcpy(profileFolderPath, profilePath);
	PathAppend(profilePath, "TrialMode.ini");
	LoadSettings();

	VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, PAGE_EXECUTE_WRITECOPY, &old);
	ogBattleOnRender  = SokuLib::TamperDword(&SokuLib::VTable_Battle.onRender,  myBattleOnRender);
	ogBattleOnProcess = SokuLib::TamperDword(&SokuLib::VTable_Battle.onProcess, myBattleOnProcess);
	ogResultOnRender  = SokuLib::TamperDword(&SokuLib::VTable_Result.onRender,  myResultOnRender);
	ogResultOnProcess = SokuLib::TamperDword(&SokuLib::VTable_Result.onProcess, myResultOnProcess);
	ogResultOnDestruct= SokuLib::TamperDword(&SokuLib::VTable_Result.onDestruct,myResultOnDestruct);
	ogTitleOnProcess  = SokuLib::TamperDword(&SokuLib::VTable_Title.onProcess,  myTitleOnProcess);
	VirtualProtect((PVOID)RDATA_SECTION_OFFSET, RDATA_SECTION_SIZE, old, &old);

	FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
	puts("Done...");
	srand(time(nullptr));
	return true;
}

extern "C" int APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	myModule = hModule;
	return TRUE;
}