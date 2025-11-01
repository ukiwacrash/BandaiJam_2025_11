#include "environment.h"

//-----------------------------------------
// メイン関数
//-----------------------------------------

#define ENEMYMAX 5

void gameMain()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(Palette::Black);

	// --- プレイヤー ---
	Player player;

	// --- コア ---
	Core core(100);
	Font font(24);

	// --- 敵管理 ---
	EnemyManager enemyManager;

	Stopwatch spawnTimer{ StartImmediately::Yes };
	const double spawnInterval = 0.5;

	auto& sound = SoundManager::Instance();
	sound.playBGM(BGMType::Title, 1.0); // タイトルBGMを1秒フェードで再生

	// --- 背景 ---
	const Texture background{ U"example/image/GamePlayBack_TOD.png" };

	// --- メニュー管理 ---
	bool isMenuOpen = false;

	while (System::Update())
	{
		if (KeySpace.pressed()) return;
		double delta = Scene::DeltaTime();

		// 毎フレーム update を呼ぶのを忘れないこと
		SoundManager::Instance().update(delta);

		// --- 背景 ---
		background.draw(0, 0);

		// --- メニューボタン入力 ---
		if (KeyP.down()) // ESCキーで開閉
		{
			isMenuOpen = !isMenuOpen;
		}

		if (isMenuOpen)
		{
			// 半透明背景（メニュー用）
			Rect{ 0, 0, Scene::Width(), Scene::Height() }.draw(ColorF(0, 0, 0, 0.5));

			// メニューウィンドウ
			Rect menuRect{ 400, 200, 480, 300 };
			menuRect.draw(ColorF(0.1, 0.1, 0.1, 0.9));
			menuRect.drawFrame(2, Palette::White);

			font(U"設定メニュー").drawAt(menuRect.center().x, menuRect.y + 40);

			// --- 音量スライダーをメニュー内に描画 ---
			sound.drawVolumeUI(menuRect.pos + Vec2{ 60, 100 });

			if (SimpleGUI::Button(U"閉じる", menuRect.pos + Vec2{ 180, 230 }, 120))
			{
				isMenuOpen = false;
			}

			continue; // 🎯 メニュー中はゲーム処理を止める
		}

		// --- プレイヤー更新 ---
		player.update(delta);
		if (player.intersects(core.getPos(), core.getHealArea()))
		{
			if (player.getHp() != player.getMaxHp() && core.getHp() > 1)
			{
				core.tryDamage(15);
				player.inAreaHeal_size(10);
			}
		}

		// --- 敵生成 ---
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

		// --- 敵更新 ---
		enemyManager.update(delta, core.getPos());


		for (auto* enemy : enemyManager.getActiveEnemies())
		{
			if (player.intersects(enemy->getPos(), enemy->getRadius()))
			{
				enemy->damage(100);
				sound.playDamage();
			}

			enemy->getCurrentHp();

			font(enemy->getCurrentHp()).drawAt(enemy->getPos() - Vec2(0, enemy->getRadius() + 10), Palette::White);
		}
		// 敵死亡処理はこのタイミングでまとめて
		enemyManager.releaseDeadEnemies();

		// --- プレイヤー処理 ---
		player.heal_size((int32)delta);

		// --- コア処理 ---
		core.update(delta);
		core.autoHeal(delta);
		core.draw();
		Circle(core.getPos(), core.getHealArea()).drawFrame(2, Palette::Green);

		// --- プレイヤー描画 ---
		enemyManager.draw();
		player.draw();
	}
}
