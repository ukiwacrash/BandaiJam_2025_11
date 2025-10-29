#include "environment.h"

//-----------------------------------------
// メイン関数
//-----------------------------------------

#define ENEMYMAX 5

void Main()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(Palette::Black);

	// --- プレイヤー ---
	Player player;

	// --- コア ---
	Core core(100);

	// --- 敵管理 ---
	EnemyManager enemyManager;

	Stopwatch spawnTimer{ StartImmediately::Yes };
	const double spawnInterval = 0.5;

	// --- 背景 ---
	const Texture background{ U"example/image/GamePlayBack_TOD.png" };

	while (System::Update())
	{
		if (KeySpace.pressed()) return;
		double delta = Scene::DeltaTime();

		// --- 背景 ---
		background.draw(0, 0);

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

			int type = Random(0, 3);
			switch (type)
			{
			case 0: enemyManager.spawnMoney(); break;
			case 1: enemyManager.spawnGreed(); break;
			case 2: enemyManager.spawnSake(); break;
			case 3: enemyManager.spawnCircle(); break;
			}
		}

		// --- 敵更新 ---
		enemyManager.update(delta, core.getPos());
		

		for (auto* enemy : enemyManager.getActiveEnemies())
		{
			if (player.intersects(enemy->getPos(), enemy->getRadius()))
			{
				enemy->damage(100);
			}
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
