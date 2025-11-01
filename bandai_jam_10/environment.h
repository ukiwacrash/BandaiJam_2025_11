#pragma once

// ゲーム進行モード
enum class GameMode {
	TITLE,
	GAMEPLAY,
	RULE,
	PAUSE,
	GAMECLEAR,
	GAMEOVER,
};

#include <Siv3D.hpp>
#include"sound.h"
#include"objectPool.h"
#include "player.h"
#include "enemy.h"
#include "enemyManager.h"
#include"Core.h"
