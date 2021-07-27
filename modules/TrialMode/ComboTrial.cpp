//
// Created by PinkySmile on 23/07/2021.
//

#include "ComboTrial.hpp"
#include "Actions.hpp"
#include "Menu.hpp"

#ifndef _DEBUG
#define puts(...)
#define printf(...)
#endif

#define dxLockRect(texture, ...) (*((int (__stdcall**)(void*, int, D3DLOCKED_RECT*, int, int))(*(int*)texture + 0x4c)))(texture, __VA_ARGS__)
#define dxUnlockRect(texture, ...) (*((int (__stdcall**)(void*, int))(*(int*)texture + 0x50)))(texture, __VA_ARGS__)

static SokuLib::KeyInput empty{0, 0, 0, 0, 0, 0, 0, 0};

ComboTrial::ComboTrial(SokuLib::Character player, const nlohmann::json &json)
{
	int text;

	if (!editorMode) {
		if (!json.contains("score") || !json["score"].is_array())
			throw std::invalid_argument("The \"score\" field is not present or invalid.");
		if (json["score"].size() != 4)
			throw std::invalid_argument("The \"score\" field doesn't have exactly 4 elements.");
	}
	if (!json.contains("expected") || !json["expected"].is_string())
		throw std::invalid_argument("The \"expected\" field is not present or invalid.");
	if (json.contains("hint") && !json["hint"].is_array())
		throw std::invalid_argument("The \"hint\" field is not valid.");
	if (!json["player"].contains("pos") || !json["player"]["pos"].is_number())
		throw std::invalid_argument(R"(The field "pos" in the "player" field is not present or invalid.)");
	if (!json["dummy"].contains("pos") || !json["dummy"]["pos"].is_object())
		throw std::invalid_argument(R"(The field "pos" in the "dummy" field is not present or invalid.)");
	if (!json["dummy"]["pos"].contains("x") || !json["dummy"]["pos"]["x"].is_number())
		throw std::invalid_argument(R"(The field "x" of the field "pos" in the "dummy" field is not present or invalid.)");
	if (!json["dummy"]["pos"].contains("y") || !json["dummy"]["pos"]["y"].is_number())
		throw std::invalid_argument(R"(The field "y" of the field "pos" in the "dummy" field is not present or invalid.)");

	memset(&this->_skills, 0xFF, sizeof(this->_skills));
	if (json.contains("skills") && json["skills"].is_array() && json["skills"].size() == characterSkills[player].size()) {
		for (int i = 0; i < json["skills"].size(); i++) {
			auto &arr = json["skills"][i];

			if (!arr.is_array() || arr.size() != 2 || !arr[0].is_number() || arr[0].get<int>() < 0 || arr[0].get<int>() > 2 || !arr[1].is_number()) {
				MessageBox(
					SokuLib::window,
					("Element #" + std::to_string(i) + " in the skill array was not valid and will be discarded").c_str(),
					"Invalid skill array",
					MB_ICONWARNING
				);
				this->_skills[i].notUsed = false;
				this->_skills[i].level = 0;
			} else {
				this->_skills[i + characterSkills[player].size() * arr[0].get<int>()].notUsed = false;
				this->_skills[i + characterSkills[player].size() * arr[0].get<int>()].level = arr[1];
			}
		}
	} else
		for (int i = 0; i < characterSkills[player].size(); i++) {
			this->_skills[i].notUsed = false;
			this->_skills[i].level = 0;
		}

	try {
		this->_hand = json.contains("hand") && json["hand"].is_array() ? json["hand"].get<std::vector<unsigned short>>() : std::vector<unsigned short>{};
	} catch (...) {
		throw std::invalid_argument("Hand contains invalid values");
	}

	for (auto card : this->_hand) {
		if (card <= 20)
			continue;
		for (int i = 0; i < SokuLib::leftPlayerInfo.effectiveDeck.size; i++)
			if (card == SokuLib::leftPlayerInfo.effectiveDeck[i])
				goto ok;
		throw std::invalid_argument("Player deck doesn't have card " + std::to_string(card));
	ok:
		continue;
	}

	this->_weather = json.contains("weather") && json["weather"].is_number() ? static_cast<SokuLib::Weather>(json["weather"].get<int>()) : SokuLib::WEATHER_CLEAR;
	this->_disableLimit = json.contains("disable_limit") && json["disable_limit"].is_boolean() ? json["disable_limit"].get<bool>() : false;
	this->_uniformCardCost = json.contains("uniform_card_cost") && json["uniform_card_cost"].is_number() ? json["uniform_card_cost"].get<int>() : -1;
	this->_playComboAfterIntro = json.contains("play_combo_after_intro") && json["play_combo_after_intro"].is_boolean() ? json["play_combo_after_intro"].get<bool>() : false;
	this->_playerStartPos = json["player"]["pos"];
	this->_dummyStartPos.x = json["dummy"]["pos"]["x"];
	this->_dummyStartPos.y = json["dummy"]["pos"]["y"];
	this->_loadExpected(json["expected"]);

	this->_gear.texture.loadFromResource(myModule, MAKEINTRESOURCE(12));
	this->_gear.setPosition({559, 70});
	this->_gear.setSize({64, 64});
	this->_gear.rect.left = 0;
	this->_gear.rect.top = 0;
	this->_gear.rect.width = this->_gear.texture.getSize().x;
	this->_gear.rect.height = this->_gear.texture.getSize().y;

	this->_gearShadow.texture.loadFromResource(myModule, MAKEINTRESOURCE(16));
	this->_gearShadow.setPosition({561, 72});
	this->_gearShadow.setSize({64, 64});
	this->_gearShadow.rect.left = 0;
	this->_gearShadow.rect.top = 0;
	this->_gearShadow.rect.width = this->_gearShadow.texture.getSize().x;
	this->_gearShadow.rect.height = this->_gearShadow.texture.getSize().y;

	this->_doll.texture.loadFromResource(myModule, MAKEINTRESOURCE(20));
	this->_doll.setPosition({577, 80});
	this->_doll.setSize({33, 46});
	this->_doll.setMirroring(true, false);
	this->_doll.rect.left = 0;
	this->_doll.rect.top = 0;
	this->_doll.rect.width = this->_doll.texture.getSize().x / 4;
	this->_doll.rect.height = this->_doll.texture.getSize().y;

	ScorePrerequisites *old = nullptr;

	if (!json.contains("score") || !json["score"].is_array() || json["score"].size() != 4)
		for (int i = 0; i < 4; i++)
			old = &this->_scores.emplace_back(nlohmann::json{}, old);
	else
		for (auto &j : json["score"])
			try {
				old = &this->_scores.emplace_back(j, old);
			} catch (std::exception &e) {
				throw std::invalid_argument("Score element #" + std::to_string(this->_scores.size()) + " is invalid : " + e.what());
			}
}

bool ComboTrial::update(bool &canHaveNextFrame)
{
	auto &battleMgr = SokuLib::getBattleMgr();

	this->_rotation += 0.025;
	this->_dollAnim++;
	this->_dollAnim &= 0b11111;
	if (this->_freezeCounter) {
		canHaveNextFrame = this->_freezeCounter % 2;
		this->_freezeCounter--;
		return false;
	}

	if (this->_finished && !this->_playingIntro)
		return true;

	if (this->_isStart) {
		this->_initGameStart();
		return false;
	}
	if (this->_waitCounter) {
		this->_waitCounter--;
	} else if (this->_playingIntro)
		this->_playIntro();
	else if (this->_actionCounter != this->_exceptedActions.size() && battleMgr.leftCharacterManager.objectBase.action == this->_exceptedActions[this->_actionCounter]->action)
		this->_actionCounter++;

	if (!this->_finished && this->_actionCounter == this->_exceptedActions.size() && this->_scores.front().met(this->_attempts)) {
		SokuLib::playSEWaveBuffer(44);
		if (!this->_playingIntro)
			this->_freezeCounter = 60;
		this->_finished = true;
		return false;
	}

	if (this->_disableLimit) {
		battleMgr.leftCharacterManager.combo.limit = 0;
		battleMgr.rightCharacterManager.combo.limit = 0;
		battleMgr.leftCharacterManager.realLimit = 0;
		battleMgr.rightCharacterManager.realLimit = 0;
	}

	auto hit = battleMgr.rightCharacterManager.objectBase.action >= SokuLib::ACTION_STAND_GROUND_HIT_SMALL_HITSTUN &&
	           battleMgr.rightCharacterManager.objectBase.action <= SokuLib::ACTION_FORWARD_DASH;

	if (this->_actionCounter && !this->_dummyHit) {
		this->_timer++;
	} else
		this->_timer = 0;
	this->_isStart = this->_timer >= 60;
	battleMgr.leftCharacterManager.score = 1;
	SokuLib::weatherCounter = this->_weather == SokuLib::WEATHER_CLEAR ? 0 : 999;
	if (this->_dummyHit && !hit)
		this->_isStart = true;
	this->_dummyHit |= hit;
	if (!this->_dummyHit) {
		battleMgr.rightCharacterManager.objectBase.speed.y = 0;
		battleMgr.rightCharacterManager.objectBase.position.x = this->_dummyStartPos.x;
		battleMgr.rightCharacterManager.objectBase.position.y = this->_dummyStartPos.y;
		if (battleMgr.rightCharacterManager.objectBase.action != SokuLib::ACTION_IDLE && battleMgr.rightCharacterManager.objectBase.action != SokuLib::ACTION_LANDING)
			battleMgr.rightCharacterManager.objectBase.action = SokuLib::ACTION_FALLING;
	}
	return false;
}

void ComboTrial::render()
{
	if (this->_finished && !this->_playingIntro)
		return;

	SokuLib::Vector2i pos = {120, 60};

	if (this->_playingIntro) {
		this->_gearShadow.setRotation(this->_rotation);
		this->_gearShadow.draw();

		this->_gear.setRotation(this->_rotation);
		this->_gear.draw();

		this->_doll.rect.left = (this->_dollAnim >> 3 & 0b11) * this->_doll.texture.getSize().x / 4;
		this->_doll.draw();
	}
	for (int i = 0; i < this->_exceptedActions.size(); i++) {
		auto &elem = this->_exceptedActions[i];

		if (this->_actionCounter == i)
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0x60, 0xFF, 0x60};
		else if (this->_actionCounter > i)
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor{0x60, 0x60, 0x60};
		else
			elem->sprite.tint = SokuLib::DrawUtils::DxSokuColor::White;
		elem->sprite.setPosition(pos);
		elem->sprite.draw();
		pos.y += elem->sprite.getSize().y;
	}
}

int ComboTrial::getScore()
{
	return -1;
}

void ComboTrial::_initGameStart()
{
	auto &battleMgr = SokuLib::getBattleMgr();

	if (this->_first) {
		SokuLib::displayedWeather = this->_weather;
		SokuLib::activeWeather = SokuLib::WEATHER_CLEAR;
		SokuLib::weatherCounter = this->_weather == SokuLib::WEATHER_CLEAR ? 0 : 999;
		this->_waitCounter = 180;
	} else if (!this->_playingIntro)
		this->_attempts++;

	this->_isStart = false;
	this->_dummyHit = false;
	this->_finished = false;
	this->_playingIntro = this->_playComboAfterIntro;
	this->_playComboAfterIntro = false;
	this->_actionCounter = 0;
	this->_first = false;
	if (this->_playingIntro)
		this->_waitCounter += 30;

	battleMgr.leftCharacterManager.cardGauge = 0;
	battleMgr.leftCharacterManager.hand.size = 0;
	for (auto card : this->_hand) {
		auto obj = battleMgr.leftCharacterManager.addCard(card);

		if (this->_uniformCardCost)
			obj->cost = this->_uniformCardCost;
	}

	battleMgr.leftCharacterManager.objectBase.hp = 10000;
	battleMgr.leftCharacterManager.objectBase.action = SokuLib::ACTION_FALLING;
	battleMgr.leftCharacterManager.objectBase.position.x = this->_playerStartPos;
	battleMgr.leftCharacterManager.objectBase.position.y = 0;
	memcpy(&battleMgr.leftCharacterManager.skillMap, &this->_skills, sizeof(this->_skills));

	battleMgr.rightCharacterManager.objectBase.hp = 10000;
	battleMgr.rightCharacterManager.objectBase.action = SokuLib::ACTION_USING_SC_ID_200;
	battleMgr.rightCharacterManager.objectBase.actionBlockId = 0;
	battleMgr.rightCharacterManager.objectBase.frameCount = 0;
	battleMgr.rightCharacterManager.objectBase.animationSubFrame = 0;
	battleMgr.rightCharacterManager.objectBase.position.x = this->_dummyStartPos.x;
	battleMgr.rightCharacterManager.objectBase.position.y = this->_dummyStartPos.y;
	battleMgr.rightCharacterManager.objectBase.direction =
		battleMgr.rightCharacterManager.objectBase.position.x > battleMgr.leftCharacterManager.objectBase.position.x ?
		SokuLib::LEFT : SokuLib::RIGHT;
}

void ComboTrial::_loadExpected(const std::string &expected)
{
	bool par = false;
	char last = ' ';

	this->_exceptedActions.clear();
	this->_exceptedActions.emplace_back(new SpecialAction());
	for (auto c : expected) {
		par |= c == '(';
		par &= c != ')';
		if (!par && c == ' ') {
			if (last != ' ')
				this->_exceptedActions.emplace_back(new SpecialAction());
		} else
			this->_exceptedActions.back()->name += c;
		last = c;
	}
	for (auto &action : this->_exceptedActions)
		action->parse();
}

void ComboTrial::_playIntro()
{
	if (this->_actionCounter == this->_exceptedActions.size())
		return;

	auto &battleMgr = SokuLib::getBattleMgr();
	auto &arr = this->_exceptedActions[this->_actionCounter];

	if (this->_actionWaitCounter < arr->delay) {
		this->_actionWaitCounter++;
		return;
	}
	arr->counter = (arr->counter + 1) % arr->inputs.size();
	if (battleMgr.leftCharacterManager.objectBase.action == arr->action) {
		arr->counter = 0;
		this->_actionWaitCounter = 0;
		this->_actionCounter++;
	}
}

void ComboTrial::editPlayerInputs(SokuLib::KeyInput &originalInputs)
{
	if (this->_playingIntro) {
		if (this->_actionCounter == this->_exceptedActions.size())
			return static_cast<void>(memset(&originalInputs, 0, sizeof(originalInputs)));
		if (this->_waitCounter)
			return static_cast<void>(memset(&originalInputs, 0, sizeof(originalInputs)));
		if (this->_exceptedActions[this->_actionCounter]->delay > this->_actionWaitCounter)
			return static_cast<void>(memset(&originalInputs, 0, sizeof(originalInputs)));
		originalInputs = this->_exceptedActions[this->_actionCounter]->inputs[this->_exceptedActions[this->_actionCounter]->counter];
		originalInputs.horizontalAxis *= SokuLib::getBattleMgr().leftCharacterManager.objectBase.direction;
		return;
	}
}

SokuLib::KeyInput ComboTrial::getDummyInputs()
{
	return {0, 0, 0, 0, 0, 0, 0, 0};
}

SokuLib::Action ComboTrial::getMoveAction(SokuLib::Character chr, std::string &name)
{
	try {
		auto act = actionsFromStr.at(name);

		if (act >= SokuLib::ACTION_DEFAULT_SKILL1_B && act <= SokuLib::ACTION_ALT2_SKILL5_AIR_C) {
			auto input = characterSkills[chr].at(name[name.size() - 3]);

			name = name.substr(0, name.size() - strlen("skillXb")) +
			       input +
			       name[name.size() - 1];
		}
		return act;
	} catch (...) {}

	int start = name[0] == 'j';
	int realStart = (name[start] == 'a' ? 2 : 1) + start;
	auto move = name.substr(realStart, name.size() - realStart - 1);
	auto &inputs = characterSkills[chr];
	auto it = std::find(inputs.begin(), inputs.end(), move);

	if (it == inputs.end())
		throw std::exception();
	try {
		return actionsFromStr.at(name.substr(0, realStart) + "skill" + std::to_string(it - inputs.begin() + 1) + name[name.size() - 1]);
	} catch (std::exception &e) {
		printf("%s\n", (name.substr(0, realStart) + "skill" + std::to_string(it - inputs.begin() + 1) + name[name.size() - 1]).c_str());
		assert(false);
		throw;
	}
}

void ComboTrial::SpecialAction::parse()
{
	std::string hitsStr;
	std::string delayStr;
	bool d = false;
	bool p = false;

	this->moveName.clear();
	for (auto c : this->name) {
		if (c == ':' && !p) {
			d = !d;
			if (d && !delayStr.empty())
				throw std::invalid_argument("Multiple delays found for move " + this->name);
		}
		if (c == '(' && !p && !d) {
			p = true;
			if (!hitsStr.empty())
				throw std::invalid_argument("Multiple hit counts found for move " + this->name);
		}
		p &= c != ')' || !d;
		if (c == '(' || c == ')' || c == ':')
			continue;
		if (d)
			delayStr += c;
		else if (p)
			hitsStr += c;
		else
			this->moveName += std::tolower(c);
	}
	printf("Move %s -> %s (%s) :%s: -> ", this->name.c_str(), this->moveName.c_str(), hitsStr.c_str(), delayStr.c_str());
	try {
		this->action = getMoveAction(SokuLib::leftChar, this->moveName);
		printf("%i ", this->action);
	} catch (std::exception &) {
		printf("INVALID\n");
		throw std::invalid_argument(this->moveName + " is not a recognized move name");
	}

	try {
		if (!hitsStr.empty())
			this->nbHits = std::stoul(hitsStr);
		else
			this->nbHits = 0;
		printf("%i ", this->nbHits);
	} catch (std::exception &) {
		printf("INVALID\n");
		throw std::invalid_argument(hitsStr + " is not a valid hit count");
	}

	try {
		if (!delayStr.empty())
			this->delay = std::stoul(delayStr);
		else
			this->delay = 0;
		printf("%i\n", this->delay);
	} catch (std::exception &) {
		printf("INVALID\n");
		throw std::invalid_argument(delayStr + " is not a valid delay");
	}

	try {
		this->inputs = actionStrToInputs.at(this->moveName);
	} catch (...) {
		throw std::invalid_argument(this->moveName + " is not yet implemented");
	}

	SokuLib::Vector2i real;

	this->sprite.texture.createFromText(this->moveName.c_str(), defaultFont16, {100, 20}, &real);
	this->sprite.setSize(real.to<unsigned>());
	this->sprite.rect.width = real.x;
	this->sprite.rect.height = real.y;
}

ComboTrial::ScorePrerequisites::ScorePrerequisites(const nlohmann::json &json, const ComboTrial::ScorePrerequisites *other)
{
	if (!other) {
		if (!editorMode) {
			if (json.contains("max_attempts"))
				throw std::invalid_argument("First score element shouldn't have the field \"max_attempts\"");
			if (!json.contains("min_hits"))
				throw std::invalid_argument("First score element is missing the field \"min_hits\"");
			if (!json.contains("min_damage"))
				throw std::invalid_argument("First score element is missing the field \"min_damage\"");
			if (!json.contains("min_limit"))
				throw std::invalid_argument("First score element is missing the field \"min_limit\"");
			if (!json.contains("max_limit"))
				throw std::invalid_argument("First score element is missing the field \"max_limit\"");
		}
	} else
		*this = *other;

	if (json.contains("max_attempts")) {
		if (!json["max_attempts"].is_number())
			throw std::invalid_argument("Field \"max_attempts\" is specified but not a number");
		this->attempts = json["max_attempts"];
	}
	if (json.contains("min_hits")) {
		if (!json["min_hits"].is_number())
			throw std::invalid_argument("Field \"min_hits\" is specified but not a number");
		this->hits = json["min_hits"];
	}
	if (json.contains("min_damage")) {
		if (!json["min_damage"].is_number())
			throw std::invalid_argument("Field \"min_damage\" is specified but not a number");
		this->damage = json["min_damage"];
	}
	if (json.contains("min_limit")) {
		if (!json["min_limit"].is_number())
			throw std::invalid_argument("Field \"min_limit\" is specified but not a number");
		this->minLimit = json["min_limit"];
	}
	if (json.contains("max_limit")) {
		if (!json["max_limit"].is_number())
			throw std::invalid_argument("Field \"max_limit\" is specified but not a number");
		this->maxLimit = json["max_limit"];
	}
}

bool ComboTrial::ScorePrerequisites::met(unsigned currentAttempts) const
{
	auto &battle = SokuLib::getBattleMgr();

	if (this->attempts < currentAttempts)
		return false;
	if (this->hits > battle.leftCharacterManager.combo.nbHits)
		return false;
	if (this->damage > battle.leftCharacterManager.combo.damages)
		return false;
	if (this->minLimit > battle.leftCharacterManager.combo.limit)
		return false;
	if (this->maxLimit < battle.leftCharacterManager.combo.limit)
		return false;
	return true;
}
