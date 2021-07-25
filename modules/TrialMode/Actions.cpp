//
// Created by PinkySmile on 24/07/2021.
//

#include "Actions.hpp"

std::map<SokuLib::Character, std::vector<std::string>> characterSkills{
	{ SokuLib::CHARACTER_REIMU,     {"236", "214", "421", "623"} },
	{ SokuLib::CHARACTER_MARISA,    {"214", "623", "22",  "236"} },
	{ SokuLib::CHARACTER_SAKUYA,    {"623", "214", "236", "22" } },
	{ SokuLib::CHARACTER_ALICE,     {"236", "623", "214", "22" } },
	{ SokuLib::CHARACTER_PATCHOULI, {"236", "22",  "623", "214", "421"} },
	{ SokuLib::CHARACTER_YOUMU,     {"236", "623", "214", "22" } },
	{ SokuLib::CHARACTER_REMILIA,   {"236", "214", "623", "22" } },
	{ SokuLib::CHARACTER_YUYUKO,    {"214", "236", "421", "623"} },
	{ SokuLib::CHARACTER_YUKARI,    {"236", "623", "214", "421"} },
	{ SokuLib::CHARACTER_SUIKA,     {"236", "623", "214", "22" } },
	{ SokuLib::CHARACTER_REISEN,    {"236", "214", "623", "22" } },
	{ SokuLib::CHARACTER_AYA,       {"236", "214", "22",  "421"} },
	{ SokuLib::CHARACTER_KOMACHI,   {"236", "623", "22",  "214"} },
	{ SokuLib::CHARACTER_IKU,       {"236", "623", "22",  "214"} },
	{ SokuLib::CHARACTER_TENSHI,    {"214", "22",  "236", "623"} },
	{ SokuLib::CHARACTER_SANAE,     {"236", "22",  "623", "214"} },
	{ SokuLib::CHARACTER_CIRNO,     {"236", "214", "22",  "623"} },
	{ SokuLib::CHARACTER_MEILING,   {"214", "623", "22",  "236"} },
	{ SokuLib::CHARACTER_UTSUHO,    {"623", "236", "22",  "214"} },
	{ SokuLib::CHARACTER_SUWAKO,    {"214", "623", "236", "22" } }
};

const std::map<std::string, SokuLib::Action> actionsFromStr{
	{ "66a",        SokuLib::ACTION_66A },
	{ "1a",         SokuLib::ACTION_2A },
	{ "2a",         SokuLib::ACTION_2A },
	{ "3a",         SokuLib::ACTION_3A },
	{ "4a",         SokuLib::ACTION_4A },
	{ "5a",         SokuLib::ACTION_5A },
	{ "6a",         SokuLib::ACTION_6A },
	{ "5aa",        SokuLib::ACTION_5AA },
	{ "5aaa",       SokuLib::ACTION_5AAA },
	{ "5aaaa",      SokuLib::ACTION_5AAAA },
	{ "5aaaaa",     SokuLib::ACTION_5AAAAA },
	{ "5aaa6a",     SokuLib::ACTION_5AAAAA },
	{ "5aaa3a",     SokuLib::ACTION_5AAA3A },
	{ "j2a",        SokuLib::ACTION_j2A },
	{ "j5a",        SokuLib::ACTION_j5A },
	{ "j5aa",       SokuLib::ACTION_j5AA },
	{ "j6a",        SokuLib::ACTION_j6A },
	{ "j8a",        SokuLib::ACTION_j8A },

	{ "66b",        SokuLib::ACTION_66B },
	{ "2b",         SokuLib::ACTION_2B },
	{ "3b",         SokuLib::ACTION_3B },
	{ "4b",         SokuLib::ACTION_4B },
	{ "5b",         SokuLib::ACTION_5B },
	{ "6b",         SokuLib::ACTION_6B },
	{ "j2b",        SokuLib::ACTION_j2B },
	{ "j4b",        SokuLib::ACTION_j4B },
	{ "j5b",        SokuLib::ACTION_j5B },
	{ "j6b",        SokuLib::ACTION_j6B },

	{ "66c",        SokuLib::ACTION_66C },
	{ "2c",         SokuLib::ACTION_2C },
	{ "5c",         SokuLib::ACTION_5C },
	{ "6c",         SokuLib::ACTION_6C },
	{ "j1c",        SokuLib::ACTION_j1C },
	{ "j2c",        SokuLib::ACTION_j2C },
	{ "j5c",        SokuLib::ACTION_j5C },
	{ "j6c",        SokuLib::ACTION_j6C },

	{ "skill1b",    SokuLib::ACTION_DEFAULT_SKILL1_B },
	{ "skill1c",    SokuLib::ACTION_DEFAULT_SKILL1_C },
	{ "skill1b",    SokuLib::ACTION_ALT1_SKILL1_B },
	{ "skill1c",    SokuLib::ACTION_ALT1_SKILL1_C },
	{ "skill1b",    SokuLib::ACTION_ALT2_SKILL1_B },
	{ "skill1c",    SokuLib::ACTION_ALT2_SKILL1_C },
	{ "jskill1b",   SokuLib::ACTION_DEFAULT_SKILL1_AIR_B },
	{ "jskill1c",   SokuLib::ACTION_DEFAULT_SKILL1_AIR_C },
	{ "jskill1b",   SokuLib::ACTION_ALT1_SKILL1_AIR_B },
	{ "jskill1c",   SokuLib::ACTION_ALT1_SKILL1_AIR_C },
	{ "jskill1b",   SokuLib::ACTION_ALT2_SKILL1_AIR_B },
	{ "jskill1c",   SokuLib::ACTION_ALT2_SKILL1_AIR_C },
	{ "skill2b",    SokuLib::ACTION_DEFAULT_SKILL2_B },
	{ "skill2c",    SokuLib::ACTION_DEFAULT_SKILL2_C },
	{ "skill2b",    SokuLib::ACTION_ALT1_SKILL2_B },
	{ "skill2c",    SokuLib::ACTION_ALT1_SKILL2_C },
	{ "skill2b",    SokuLib::ACTION_ALT2_SKILL2_B },
	{ "skill2c",    SokuLib::ACTION_ALT2_SKILL2_C },
	{ "jskill2b",   SokuLib::ACTION_DEFAULT_SKILL2_AIR_B },
	{ "jskill2c",   SokuLib::ACTION_DEFAULT_SKILL2_AIR_C },
	{ "jskill2b",   SokuLib::ACTION_ALT1_SKILL2_AIR_B },
	{ "jskill2c",   SokuLib::ACTION_ALT1_SKILL2_AIR_C },
	{ "jskill2b",   SokuLib::ACTION_ALT2_SKILL2_AIR_B },
	{ "jskill2c",   SokuLib::ACTION_ALT2_SKILL2_AIR_C },
	{ "skill3b",    SokuLib::ACTION_DEFAULT_SKILL3_B },
	{ "skill3c",    SokuLib::ACTION_DEFAULT_SKILL3_C },
	{ "skill3b",    SokuLib::ACTION_ALT1_SKILL3_B },
	{ "skill3c",    SokuLib::ACTION_ALT1_SKILL3_C },
	{ "skill3b",    SokuLib::ACTION_ALT2_SKILL3_B },
	{ "skill3c",    SokuLib::ACTION_ALT2_SKILL3_C },
	{ "jskill3b",   SokuLib::ACTION_DEFAULT_SKILL3_AIR_B },
	{ "jskill3c",   SokuLib::ACTION_DEFAULT_SKILL3_AIR_C },
	{ "jskill3b",   SokuLib::ACTION_ALT1_SKILL3_AIR_B },
	{ "jskill3c",   SokuLib::ACTION_ALT1_SKILL3_AIR_C },
	{ "jskill3b",   SokuLib::ACTION_ALT2_SKILL3_AIR_B },
	{ "jskill3c",   SokuLib::ACTION_ALT2_SKILL3_AIR_C },
	{ "skill4b",    SokuLib::ACTION_DEFAULT_SKILL4_B },
	{ "skill4c",    SokuLib::ACTION_DEFAULT_SKILL4_C },
	{ "skill4b",    SokuLib::ACTION_ALT1_SKILL4_B },
	{ "skill4c",    SokuLib::ACTION_ALT1_SKILL4_C },
	{ "skill4b",    SokuLib::ACTION_ALT2_SKILL4_B },
	{ "skill4c",    SokuLib::ACTION_ALT2_SKILL4_C },
	{ "jskill4b",   SokuLib::ACTION_DEFAULT_SKILL4_AIR_B },
	{ "jskill4c",   SokuLib::ACTION_DEFAULT_SKILL4_AIR_C },
	{ "jskill4b",   SokuLib::ACTION_ALT1_SKILL4_AIR_B },
	{ "jskill4c",   SokuLib::ACTION_ALT1_SKILL4_AIR_C },
	{ "jskill4b",   SokuLib::ACTION_ALT2_SKILL4_AIR_B },
	{ "jskill4c",   SokuLib::ACTION_ALT2_SKILL4_AIR_C },
	{ "skill5b",    SokuLib::ACTION_DEFAULT_SKILL5_B },
	{ "skill5c",    SokuLib::ACTION_DEFAULT_SKILL5_C },
	{ "skill5b",    SokuLib::ACTION_ALT1_SKILL5_B },
	{ "skill5c",    SokuLib::ACTION_ALT1_SKILL5_C },
	{ "skill5b",    SokuLib::ACTION_ALT2_SKILL5_B },
	{ "skill5c",    SokuLib::ACTION_ALT2_SKILL5_C },
	{ "jskill5b",   SokuLib::ACTION_DEFAULT_SKILL5_AIR_B },
	{ "jskill5c",   SokuLib::ACTION_DEFAULT_SKILL5_AIR_C },
	{ "jskill5b",   SokuLib::ACTION_ALT1_SKILL5_AIR_B },
	{ "jskill5c",   SokuLib::ACTION_ALT1_SKILL5_AIR_C },
	{ "jskill5b",   SokuLib::ACTION_ALT2_SKILL5_AIR_B },
	{ "jskill5c",   SokuLib::ACTION_ALT2_SKILL5_AIR_C },

	{ "SC200",      SokuLib::ACTION_USING_SC_ID_200 },
	{ "SC201",      SokuLib::ACTION_USING_SC_ID_201 },
	{ "SC202",      SokuLib::ACTION_USING_SC_ID_202 },
	{ "SC203",      SokuLib::ACTION_USING_SC_ID_203 },
	{ "SC204",      SokuLib::ACTION_USING_SC_ID_204 },
	{ "SC205",      SokuLib::ACTION_USING_SC_ID_205 },
	{ "SC206",      SokuLib::ACTION_USING_SC_ID_206 },
	{ "SC207",      SokuLib::ACTION_USING_SC_ID_207 },
	{ "SC208",      SokuLib::ACTION_USING_SC_ID_208 },
	{ "SC209",      SokuLib::ACTION_USING_SC_ID_209 },
	{ "SC210",      SokuLib::ACTION_USING_SC_ID_210 },
	{ "SC211",      SokuLib::ACTION_USING_SC_ID_211 },
	{ "SC212",      SokuLib::ACTION_USING_SC_ID_212 },
	{ "SC213",      SokuLib::ACTION_USING_SC_ID_213 },
	{ "SC214",      SokuLib::ACTION_USING_SC_ID_214 },
	{ "SC215",      SokuLib::ACTION_USING_SC_ID_215 },
	{ "SC216",      SokuLib::ACTION_USING_SC_ID_216 },
	{ "SC217",      SokuLib::ACTION_USING_SC_ID_217 },
	{ "SC218",      SokuLib::ACTION_USING_SC_ID_218 },
	{ "SC219",      SokuLib::ACTION_USING_SC_ID_219 },

	{ "hj9",        SokuLib::ACTION_FORWARD_HIGH_JUMP },
	{ "hj8",        SokuLib::ACTION_NEUTRAL_HIGH_JUMP },
	{ "hj7",        SokuLib::ACTION_BACKWARD_HIGH_JUMP },
	{ "be1",        SokuLib::ACTION_BE1 },
	{ "be2",        SokuLib::ACTION_BE2 },
	{ "be4",        SokuLib::ACTION_BE4 },
	{ "be6",        SokuLib::ACTION_BE6 },

	{ "j66",        SokuLib::ACTION_FORWARD_AIRDASH },
	{ "j44",        SokuLib::ACTION_BACKWARD_AIRDASH },
	{ "44",         SokuLib::ACTION_BACKDASH },
	{ "66",         SokuLib::ACTION_FORWARD_DASH },
	{ "d4",         SokuLib::ACTION_BACKDASH },
	{ "d6",         SokuLib::ACTION_FORWARD_DASH },
	{ "j1d",        SokuLib::ACTION_FLY },
	{ "j2d",        SokuLib::ACTION_FLY },
	{ "j3d",        SokuLib::ACTION_FLY },
	{ "j4d",        SokuLib::ACTION_FLY },
	{ "j6d",        SokuLib::ACTION_FLY },
	{ "j7d",        SokuLib::ACTION_FLY },
	{ "j8d",        SokuLib::ACTION_FLY },
	{ "j9d",        SokuLib::ACTION_FLY },

	{ "9",          SokuLib::ACTION_FORWARD_JUMP },
	{ "8",          SokuLib::ACTION_NEUTRAL_JUMP },
	{ "7",          SokuLib::ACTION_BACKWARD_JUMP },

	{ "drug",       SokuLib::ACTION_SPELL_BREAKING_DRUG },
	{ "hackoruken", SokuLib::ACTION_FORWARD_DASH },
};

const std::map<std::string, std::vector<SokuLib::KeyInput>> actionStrToInputs{
	{ "66a",        {{1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 1, 0, 0, 0, 0, 0}} },
	{ "1a",         {{-1, 1, 1, 0, 0, 0, 0, 0}} },
	{ "2a",         {{0, 1, 1, 0, 0, 0, 0, 0}} },
	{ "3a",         {{1, 1, 1, 0, 0, 0, 0, 0}} },
	{ "4a",         {{-1, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5a",         {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "6a",         {{1, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5aa",        {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5aaa",       {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5aaaa",      {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5aaaaa",     {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5aaa6a",     {{0, 0, 1, 0, 0, 0, 0, 0}, {1, 0, 1, 0, 0, 0, 0, 0}} },
	{ "5aaa3a",     {{0, 0, 1, 0, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0, 0, 0}} },
	{ "j2a",        {{0, 1, 1, 0, 0, 0, 0, 0}} },
	{ "j5a",        {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "j5aa",       {{0, 0, 1, 0, 0, 0, 0, 0}} },
	{ "j6a",        {{1, 0, 1, 0, 0, 0, 0, 0}} },
	{ "j8a",        {{0, -1, 1, 0, 0, 0, 0, 0}} },

	{ "66b",        {{1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 1, 0, 0, 0, 0}} },
	{ "2b",         {{0, 1, 0, 1, 0, 0, 0, 0}} },
	{ "3b",         {{1, 1, 0, 1, 0, 0, 0, 0}} },
	{ "4b",         {{-1, 0, 0, 1, 0, 0, 0, 0}} },
	{ "5b",         {{0, 0, 0, 1, 0, 0, 0, 0}} },
	{ "6b",         {{1, 0, 0, 1, 0, 0, 0, 0}} },
	{ "j2b",        {{0, 1, 0, 1, 0, 0, 0, 0}} },
	{ "j4b",        {{1, 0, 0, 1, 0, 0, 0, 0}} },
	{ "j5b",        {{0, 0, 0, 1, 0, 0, 0, 0}} },
	{ "j6b",        {{1, 0, 0, 1, 0, 0, 0, 0}} },

	{ "66c",        {{1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 1, 0, 0, 0}} },
	{ "2c",         {{0, 1, 0, 0, 1, 0, 0, 0}} },
	{ "5c",         {{0, 0, 0, 0, 1, 0, 0, 0}} },
	{ "6c",         {{1, 0, 0, 0, 1, 0, 0, 0}} },
	{ "j1c",        {{-1, 1, 0, 0, 1, 0, 0, 0}} },
	{ "j2c",        {{0, 1, 0, 0, 1, 0, 0, 0}} },
	{ "j5c",        {{0, 0, 0, 0, 1, 0, 0, 0}} },
	{ "j6c",        {{1, 0, 0, 0, 1, 0, 0, 0}} },

	{ "SC200",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC201",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC202",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC203",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC204",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC205",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC206",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC207",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC208",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC209",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC210",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC211",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC212",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC213",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC214",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC215",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC216",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC217",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC218",      {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "SC219",      {{0, 0, 0, 0, 0, 0, 0, 1}} },

	{ "hj9",        {{1, -1, 0, 0, 0, 1, 0, 0}} },
	{ "hj8",        {{0, -1, 0, 0, 0, 1, 0, 0}} },
	{ "hj7",        {{-1, -1, 0, 0, 0, 1, 0, 0}} },
	{ "be1",        {{-0, 0, 0, 0, 0, 1, 0, 0}, {-1, 1, 0, 0, 0, 0, 0, 0}, {-1, 1, 0, 0, 0, 1, 0, 0}} },
	{ "be2",        {{-0, 0, 0, 0, 0, 1, 0, 0}, {0, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 1, 0, 0}} },
	{ "be4",        {{-0, 0, 0, 0, 0, 1, 0, 0}, {-1, 0, 0, 0, 0, 0, 0, 0}, {-1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "be6",        {{-0, 0, 0, 0, 0, 1, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 1, 0, 0}} },

	{ "j66",        {{1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}} },
	{ "j44",        {{-1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {-1, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}} },
	{ "44",         {{-1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "66",         {{1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "d4",         {{-1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "d6",         {{1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "j1d",        {{-1, 1, 0, 0, 0, 1, 0, 0}} },
	{ "j2d",        {{0, 1, 0, 0, 0, 1, 0, 0}} },
	{ "j3d",        {{1, 1, 0, 0, 0, 1, 0, 0}} },
	{ "j4d",        {{-1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "j6d",        {{1, 0, 0, 0, 0, 1, 0, 0}} },
	{ "j7d",        {{-1, -1, 0, 0, 0, 1, 0, 0}} },
	{ "j8d",        {{0, -1, 0, 0, 0, 1, 0, 0}} },
	{ "j9d",        {{1, -1, 0, 0, 0, 1, 0, 0}} },

	{ "9",          {{1, -1, 0, 0, 0, 0, 0, 0}} },
	{ "8",          {{0, -1, 0, 0, 0, 0, 0, 0}} },
	{ "7",          {{-1, -1, 0, 0, 0, 0, 0, 0}} },

	{ "drug",       {{0, 0, 0, 0, 0, 0, 0, 1}} },
	{ "hackoruken", {{0, 0, 0, 0, 0, 0, 0, 1}} },
};