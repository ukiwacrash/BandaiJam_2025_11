#include <Siv3D.hpp>
#include "environment.h"
#define ENEMYMAX 100

// --- 状態保持 ---
struct GameState
{
	double timer = 0.0;
	int32 score = 0;
	double countdown = 3.0;			// カウントダウン
	double nowTime = 0.0;			// 経過時間
	const double clearTime = 60.0;	// クリアまでの時間
};

// --- フェード効果 ---
struct FadeEffect
{
	double fade = 0.0;
	bool isDead = false;

	void reset() { fade = 0.0; isDead = false; }

	bool update(double hp)
	{
		const double fadeSpeed = 0.8;

		if (hp <= 0) isDead = true;

		if (isDead)
		{
			fade += Scene::DeltaTime() * fadeSpeed;
			fade = Min(fade, 1.0);

			// 上から下へ黒くフェード
			const double fadeHeight = Scene::Height() * fade;
			RectF{ 0.0, 0.0, (double)Scene::Width(), fadeHeight }
			.draw(ColorF{ 0, 0, 0, 1.0 });
		}

		return fade >= 1.0;
	}
};

// --- キー描画関数 ---
void drawKey(const Vec2& pos, const String& label, bool pressed, double keySize)
{
	ColorF col = pressed ? Palette::Yellow : Palette::Gray;
	RectF{ Arg::center(pos), keySize, keySize }.draw(col);
	FontAsset(U"Normal")(label).drawAt(pos, Palette::White);
}

void InitGame(Player& player, Core& core, EnemyManager& enemyManager, FadeEffect& fade, GameState& state)
{
	player.reset();
	core = Core(100);
	enemyManager.resetAll(); // ← ここで全敵リセット
	fade.reset();

	// --- 時間初期化 ---
	state.countdown = 3.0;
	state.nowTime = 0.0;
	state.score = 0;

	// --- 初期敵生成 ---
	int type = Random(0, 5);
	switch (type)
	{
	case 0: enemyManager.spawnMoney(); break;
	case 1: enemyManager.spawnGreed(); break;
	case 2: enemyManager.spawnSake(); break;
	case 3: enemyManager.spawnCircle(); break;
	case 4: enemyManager.spawnSmart(); break;
	case 5: enemyManager.spawnBlink(); break;
	}
}


// --- プレイヤー回復アニメーション用タイマー ---
static double healEffectTimer = 0.0;

// --- プレイヤー・コア更新 ---
void UpdatePlayerAndCore(Player& player, Core& core, double delta, bool& isHealing,SoundManager& sound)
{
	player.update(delta);
	double dynamicHealArea = core.getDynamicHealArea(Scene::DeltaTime());

	// --- 回復判定 ---
	if (player.intersects(core.getPos(), dynamicHealArea))
	{
		if (player.getHp() != player.getMaxHp() && core.getHp() > 1)
		{
			isHealing = true;
			core.tryDamage(8);
			player.inAreaHeal_size(10);
		}
		else
		{
			isHealing = false;
		}
	}
	else
	{
		isHealing = false;
	}

	// --- 完全回復キー ---
	if (KeyE.down() && player.getHp() != player.getMaxHp()&&player.remainingFullHeal>0)
	{
		player.useFullHeal();
		sound.playHeal();
		healEffectTimer = 0.5; // 少し長めに光らせる
	}

	// --- 回復アニメーション描画 ---
	if (healEffectTimer > 0.0)
	{
		healEffectTimer -= delta;
		double alpha = healEffectTimer / 0.5; // フェードアウト
		double size = player.getRadius() * (1.0 + (1.0 - alpha) * 5.0);
		Circle(player.getPos(), size).drawFrame(4, ColorF(0.5, 1.0, 0.5, alpha));
	}

	// --- プレイヤーとコアの衝突処理 ---
	if (player.intersects(core.getPos(), core.getRadius()))
	{
		Vec2 dir = player.getPos() - core.getPos();
		double dist = dir.length();
		if (dist > 0)
		{
			double minDist = player.getRadius() + core.getRadius();
			double overlap = minDist - dist;
			Vec2 pushDir = dir / dist;
			player.setPos(player.getPos() + pushDir * overlap);
		}
	}

	core.update(delta);
	core.autoHeal(delta);
}

// --- 敵処理 ---
void UpdateEnemies(EnemyManager& enemyManager, Player& player, Core& core, double delta,
					SoundManager&sound, GameState& state)
{
	// --- 敵生成 ---
	static Stopwatch spawnTimer{ StartImmediately::Yes };

	const double spawnInterval = 0.15;
	if (spawnTimer.sF() > spawnInterval)
	{
		spawnTimer.restart();
		int type = Random(0, 5);
		switch (type)
		{
		case 0: enemyManager.spawnMoney(); break;
		case 1: enemyManager.spawnGreed(); break;
		case 2: enemyManager.spawnSake(); break;
		case 3: enemyManager.spawnCircle(); break;
		case 4: enemyManager.spawnSmart(); break;
		case 5: enemyManager.spawnBlink(); break;
		}
	}

	// --- 敵の更新 ---
	enemyManager.update(delta, core.getPos());

	// --- 各敵との当たり判定 ---
	for (auto* enemy : enemyManager.getActiveEnemies())
	{
		if (enemy->intersects(core.getPos(), core.getRadius()))
		{
			sound.playDamage();
			core.damage(10);
			enemy->damage(100);
		}

		if (enemy->intersects(player.getPos(), player.getRadius()))
		{
			sound.playDamage();
			player.damage_size(2);
			enemy->damage(100);
			state.score++;
		}

		//if (KeyE.down())
		//{
		//	if (enemy->intersects(player.getPos(), player.getAttackArea()))
		//	{
		//		enemy->damage(100);
		//	}
		//}
	}

	// --- 死んだ敵をプールに返す ---
	enemyManager.releaseDeadEnemies();

	// --- 描画 ---
	enemyManager.draw();
}


// --- UI描画 ---
void DrawUI(Player& player)
{
	Vec2 basePos(110, Scene::Height() - 100);
	double keySize = 60;
	double spacing = 10;

	drawKey(basePos + Vec2(0, -keySize - spacing), U"W", KeyW.pressed(), keySize);
	drawKey(basePos + Vec2(-keySize - spacing, 0), U"A", KeyA.pressed(), keySize);
	drawKey(basePos + Vec2(0, 0), U"S", KeyS.pressed(), keySize);
	drawKey(basePos + Vec2(keySize + spacing, 0), U"D", KeyD.pressed(), keySize);
	drawKey(basePos + Vec2(keySize + spacing, -keySize - spacing), U"E", KeyE.pressed(), keySize);
	drawKey(basePos + Vec2(-keySize - spacing, -keySize - spacing), U"Q", KeyQ.pressed(), keySize);

	Rect{ 1100, 550, 140, 120 }.rounded(15, 30, 45, 60).draw(ColorF{ 0.2 });
	Circle(1160, 540, 60).drawPie(270_deg, 90_deg, MouseL.pressed() ? ColorF{ Palette::Yellow } : ColorF{ 1.0 });
	Circle(1180, 540, 60).drawPie(0_deg, 90_deg, /*MouseR.pressed() ? ColorF{ Palette::Yellow } :*/ ColorF{ 0.2 });

	if (MouseL.pressed()&&player.getHp()>0)
	{
		Line(player.getPos(), Cursor::PosF()).draw(2, Palette::Yellow);
		Circle(Cursor::PosF(), 10).drawFrame(2, Palette::Red);
	}
}

// --- ゲームプレイ更新 ---
void ProtectCoreUpdate(Player& player, Core& core, EnemyManager& enemyManager,
					   const Texture& background, GameMode& mode, FadeEffect& fade,
					   bool& isHealing, GameState& state,SoundManager& sound)
{
	double delta = Scene::DeltaTime();

	// --- カウントダウン中 ---
	if (state.countdown > -1.0)
	{
		state.countdown -= delta * 0.8;

		background.draw(0, 0);

		String text = (state.countdown > 0.0) ? Format((int)Ceil(state.countdown)) : U"START!";
		FontAsset(U"Countdown")(text).drawAt(640, 360, Palette::Black);
		return;
	}

	// --- 通常ゲーム処理 ---
	const Transformer2D t{ !isHealing ? Mat3x2::Translate(core.getShakeOffset()) : Mat3x2::Identity() };

	// 一時停止
	if (KeyQ.down())
	{
		mode = GameMode::PAUSE;
		return;
	}

	background.draw(0, 0);

	// --- プレイヤー・コア更新 ---
	UpdatePlayerAndCore(player, core, delta, isHealing,sound);

	// --- 敵更新・描画 ---
	UpdateEnemies(enemyManager, player, core, delta,sound,state);

	// --- タイマー表示 ---
	int remaining = Max(0, (int)(state.clearTime - state.nowTime));

	// 通常の残り時間表示（右上など）
	FontAsset(U"nowTime")(U"残り: {} 秒"_fmt(remaining)).draw(800, 20, Palette::Black);


	//ボス不要になった/
	{
		//{
		//	// --- ボス関連 ---
		//	static Boss boss; // 1体固定
		//	static int count = 0;

		//	// 出現条件（残り20秒）
		//	if (!boss.alive && remaining <= 20 && count == 0)
		//	{
		//		boss.alive = true;
		//		count = 1;
		//	}

		//	if (boss.isAlive())
		//	{
		//		boss.updateEffect(delta); // 出現演出タイマー更新
		//		boss.draw();

		//		// プレイヤーとの当たり判定
		//		bool hitPlayer = (boss.pos - player.getPos()).length() < (boss.radius + player.getRadius() * 1.5);

		//		if (!hitPlayer)
		//		{
		//			// 当たっていない場合は移動＆コアへのダメージ
		//			boss.update(player.getPos(), core.getPos(), core);
		//		}
		//		else
		//		{
		//			// --- プレイヤーにダメージ ---
		//			player.damage_size(1);

		//			// --- プレイヤーを押し返す ---
		//			Vec2 dir = player.getPos() - boss.pos; // プレイヤー → ボス
		//			double dist = dir.length();
		//			if (dist > 0)
		//			{
		//				double minDist = boss.radius + player.getRadius();
		//				double overlap = minDist - dist;
		//				if (overlap > 0)
		//				{
		//					Vec2 pushDir = dir / dist;
		//					player.setPos(player.getPos() + pushDir * (overlap + 1.0));
		//				}
		//			}

		//			// --- ボスにダメージ ---
		//			boss.takeDamage(1);
		//		}
		//	}

		//}

	}
	
	core.draw();
	Circle(core.getPos(), core.getHealArea()).drawFrame(2, Palette::Green);
	player.draw();
	DrawUI(player);

	// --- カウントダウン演出（残り3秒以下のとき） ---
	if (remaining <= 3 && remaining > 0)
	{
		// 大きめのフォントで中央に描画
		FontAsset(U"CountdownEnd")(remaining).drawAt(Scene::Center(), Palette::Red);
	}

	state.nowTime += delta;

	// --- ゲームクリア条件 ---
	if (state.nowTime >= state.clearTime)
	{
		mode = GameMode::GAMECLEAR;
	}

	// --- ゲームオーバー判定 ---
	if (fade.update(core.getHp()))
	{
		mode = GameMode::GAMEOVER;
	}
}

bool DrawButton(const Texture& tex, const RectF& rect, const Vec2& pos,SoundManager& sound)
{
	if (rect.mouseOver())
	{
		tex.draw(pos, ColorF{ 1.2, 1.2, 1.2, 1.0 }); // 明るく
		if (MouseL.down())
		{
			sound.MouseSe();
			return true; // クリックされた
		}
	}
	else
	{
		tex.draw(pos, ColorF{ 1.0, 1.0, 1.0, 1.0 }); // 通常色
	}
	return false;
}

void DrawRuleScene()
{
	static const Font font{ 30 };

	font(U"【ルール説明】").drawAt(Scene::Center().x, 100, Palette::Yellow);
	font(U"・心を守りながら欲望を倒そう！").drawAt(Scene::Center().x, 200);
	font(U"・プレイヤー（理）は WASD で移動/").drawAt(Scene::Center().x, 250);
	font(U"マウス右クリックでカーソルへ移動").drawAt(Scene::Center().x + 75, 300);
	font(U"・60秒間心を保てたらミッション達成！").drawAt(Scene::Center().x, 350);

	font(U"・プレイヤーが敵に当たると小さくなり一定量を超えると消滅！").drawAt(Scene::Center().x, 400);
	font(U"・プレイヤーが心の周りに入ると回復！(心がダメージを受ける)").drawAt(Scene::Center().x, 450);

	//font(U"クリックでタイトルに戻る").drawAt(Scene::Center().x, 500, Palette::Gray);

}
// --- メイン ---
void Main()
{
	// --- フォント登録 ---
	FontAsset::Register(U"Countdown", 120, Typeface::Bold);
	FontAsset::Register(U"CountdownEnd", 200, Typeface::Bold);
	FontAsset::Register(U"nowTime", 80, Typeface::Bold);
	FontAsset::Register(U"Normal", 30, Typeface::Bold);
	Font font(24);

	Window::Resize(1280, 720);
	Scene::SetBackground(Palette::Black);

	GameMode mode = GameMode::TITLE;
	GameState state;

	Player player;
	Core core(100);
	FadeEffect fade;
	bool isHealing = false;

	// --- 敵管理 ---
	EnemyManager enemyManager;

	// --- 画像UI ---
	const Texture background{ U"example/image/GamePlayBack_TOD.png" };
	const Texture background_gameend{ U"example/image/GameEnd.png" };
	const Texture background_texTitle{ U"example/image/new Title.png" };
	const Texture background_gameClear{ U"example/image/GameClear.png" };
	const Texture background_pose{ U"example/image/ポーズ.png" };

	Texture texTitleB{ U"example/image/TitleButton.png" };
	Texture texStartB{ U"example/image/startButton.png" };
	Texture texRuleB{ U"example/image/ruleButton.png" };
	Texture textitle_komono{ U"example/image/title_komono.png" };

	// --- ボタンの配置 ---
	Vec2 buttonPos_title{ 530, 500 };
	RectF buttonRect_title{ buttonPos_title, texTitleB.size() };
	Vec2 buttonPos_start{ 900, 400 };
	RectF buttonRect_start{ buttonPos_start, texStartB.size() };
	Vec2 buttonPos_rule{ 900, 550 };
	RectF buttonRect_rule{ buttonPos_rule, texRuleB.size() };

	InitGame(player, core, enemyManager, fade, state);


	// --- 音 ---
	auto& sound = SoundManager::Instance();


	while (System::Update())
	{

		double delta = Scene::DeltaTime();

		// --- 毎フレームサウンド更新 ---
		sound.update(delta);

		switch (mode)
		{
		case GameMode::TITLE:
		{
			sound.playBGM(BGMType::Title, 1.0);

			Scene::SetBackground(Palette::Black);
			SimpleGUI::Headline(U"Protect Core", Vec2{ 500, 300 });
			background_texTitle.draw();

			Vec2 basePos{ 200, 400 };
			double t = Scene::Time();
			double offsetY = Sin(t * 2.0) * 100.0;
			textitle_komono.drawAt(basePos + Vec2{ 0, offsetY });

			if (DrawButton(texStartB, buttonRect_start, buttonPos_start,sound))
			{
				InitGame(player, core, enemyManager, fade, state);
				mode = GameMode::GAMEPLAY;
			}

			if (DrawButton(texRuleB, buttonRect_rule, buttonPos_rule,sound))
			{
				mode = GameMode::RULE;
			}
#if _DEBUG
			if (KeyEnter.down())
			{
				InitGame(player, core, enemyManager, fade, state);
				mode = GameMode::GAMECLEAR;
			}
#endif
		}
		break;

		case GameMode::RULE:
			Scene::SetBackground(ColorF(0, 0, 0, 0.5));
			DrawRuleScene();
			if (DrawButton(texTitleB, buttonRect_title.movedBy(0, 80), buttonPos_title + Vec2{ 0, 80 }, sound))
			{
				mode = GameMode::TITLE;
			}
			break;

		case GameMode::GAMEPLAY:
			sound.playBGM(BGMType::Battle, 1.0);

			ProtectCoreUpdate(player, core, enemyManager, background, mode, fade, isHealing, state,sound);
			break;

		case GameMode::PAUSE:
		{
			background_pose.draw();
			sound.playBGM(BGMType::Battle, 1.0);

			// --- 音量調整 ---
			// 半透明背景（メニュー用）
			Rect{ 0, 0, Scene::Width(), Scene::Height() }.draw(ColorF(0, 0, 0, 0.5));

			Rect menuRect{ 20, Scene::Center().y + 50, 480, 300 };
			menuRect.draw(ColorF(0.1, 0.1, 0.1, 0.9));
			menuRect.drawFrame(2, Palette::White);

			// タイトル文字
			font(U"設定メニュー").drawAt(menuRect.x + menuRect.w / 2, menuRect.y + 40);

			// 音量スライダー
			sound.drawVolumeUI(menuRect.pos + Vec2{ 100, 100 });

			//if (SimpleGUI::Button(U"閉じる", menuRect.pos + Vec2{ 180, 230 }, 120));


			static const Font fontPose(80);
			const Vec2 centerPos{ Scene::Center().x, 200.0 };

			double t = Scene::Time();
			double offsetY = Sin(t * 3.0) * 20.0;
			Vec2 textPos = centerPos + Vec2{ 0, offsetY };
			fontPose(U"PAUSE").drawAt(textPos, Palette::Red);

			if (DrawButton(texStartB, buttonRect_start, buttonPos_start, sound)) mode = GameMode::GAMEPLAY;
			if (KeyEnter.down()) mode = GameMode::GAMEPLAY;
			if (KeyBackspace.down() || KeyT.down()) mode = GameMode::TITLE;
		}
		break;

		case GameMode::GAMEOVER:
		{
			sound.playBGM(BGMType::GameOver, 1.0);

			Scene::SetBackground(ColorF(0.2, 0.0, 0.0));
			background_gameend.draw();

			static const Font fontGO(80);
			const Vec2 centerPos{ Scene::Center().x, 100.0 };
			double t = Scene::Time();
			double offsetY = Sin(t * 3.0) * 20.0;
			Vec2 textPos = centerPos + Vec2{ 0, offsetY };

			// --- 表示 ---
			fontGO(U"GAME OVER").drawAt(textPos, Palette::Red);
			FontAsset(U"Normal")(U"撃破した煩悩数: {}"_fmt(state.score)).draw(20, 20, Palette::Black);

			if (DrawButton(texTitleB, buttonRect_title.movedBy(400, 80), buttonPos_title + Vec2{ 400, 80 }, sound))
				mode = GameMode::TITLE;

			if (KeyEnter.down())
			{
				InitGame(player, core, enemyManager, fade, state);
				mode = GameMode::TITLE;
			}
		}
		break;

		case GameMode::GAMECLEAR:
		{
			sound.playBGM(BGMType::GameClear, 1.0);

			static double clearTimer = 0.0;
			clearTimer += Scene::DeltaTime();
			background_gameClear.draw();

			static Array<Vec2> sparkles;
			static bool initialized = false;
			if (!initialized)
			{
				sparkles.clear();
				for (int i = 0; i < 100; i++)
				{
					sparkles << Vec2{ Random(0.0, 1280.0), Random(-720.0, 0.0) };
				}
				initialized = true;
			}

			Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, 0.05 });

			for (auto& s : sparkles)
			{
				s.y += 100 * Scene::DeltaTime();
				if (s.y > 720)
				{
					s.y = Random(-100.0, 0.0);
					s.x = Random(0.0, 1280.0);
				}
				double alpha = 0.5 + 0.5 * Sin(Scene::Time() * 5 + s.x);
				Circle(s, 2.5).draw(ColorF{ 1.0, 1.0, Random(0.8, 1.0), alpha });
			}

			double alpha = Min(clearTimer / 2.0, 1.0);

			// --- 表示 ---
			FontAsset(U"Normal")(U"MISSION CLEAR!").drawAt(640, 360, ColorF{ 1.0, 1.0, 0.0, alpha });
			FontAsset(U"nowTime")(U"撃破した煩悩数: {}"_fmt(state.score)).draw(20, 20, Palette::Black);


			if (KeyEnter.down() ||
				DrawButton(texTitleB, buttonRect_title.movedBy(400, 20), buttonPos_title + Vec2{ 400, 20 }, sound))
			{
				InitGame(player, core, enemyManager, fade, state);
				mode = GameMode::TITLE;
				clearTimer = 0.0;
				initialized = false;
			}
		}
		break;
		}
	}
}
