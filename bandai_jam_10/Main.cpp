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

	// --- 敵複数 ---
	Texture texSlime(U"example/image/無題.png");
	Texture texBat(U"example/image/無題.png");
	Texture texGhost(U"example/image/無題.png");

	Stopwatch spawnTimer{ StartImmediately::Yes };
	const double spawnInterval = 1.5;
	Array<std::shared_ptr<Enemy>> enemies;

	//Array<Chara> enemies;
	//for (int i = 0; i < ENEMYMAX; ++i)
	//{
	//	Vec2 pos;
	//	int side = Random(0, 3); // 0:左,1:上,2:右,3:下
	//	switch (side)
	//	{
	//	case 0: pos = Vec2{ -50, Random(0, 720) }; break;
	//	case 1: pos = Vec2{ Random(0, 1280), -50 }; break;
	//	case 2: pos = Vec2{ 1280 + 50, Random(0, 720) }; break;
	//	case 3: pos = Vec2{ Random(0, 1280), 720 + 50 }; break;
	//	}

	//	enemies.emplace_back(Chara::Enemy, pos, 100, 100.0); // Enemyタイプ
	//}


	const Texture background{ U"example/image/GamePlayBack_TOD.png" };


	while (System::Update())
	{
		if (KeySpace.pressed())return;
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


		// --- 敵処理 ---
		{
			//for (auto& enemy : enemies)
//{
//	if (!enemy.isAlive()) continue;

//	// コアに向かって移動
//	Vec2 dir = core.getPos() - enemy.getPos();
//	double len = dir.length();
//	if (len > 0)
//	{
//		enemy.move(dir / len, delta * 0.5);
//	}

//	// --- プレイヤーとの衝突 ---
//	if (enemy.intersects(player.getPos(), player.getRadius())) // 半径はサイズ/2で代用
//	{
//		player.damage_size(10);
//		enemy.damage(100); // 敵を消す
//	}

//	// --- コアとの衝突 ---
//	if (enemy.intersects(core.getPos(), core.getRadius()))
//	{
//		core.damage(20);
//		enemy.damage(100);
//	}

//	// --- 描画 ---
//	enemy.draw();
//}

		}

		// --- 敵を一定間隔で生成 ---
		if (spawnTimer.sF() > spawnInterval)
		{
			spawnTimer.restart();

			int type = Random(0, 2); // 0=Slime,1=Bat,2=Ghost
			switch (type)
			{
			case 0: enemies << std::make_shared<Slime>(texSlime); break;
			case 1: enemies << std::make_shared<Bat>(texBat); break;
			case 2: enemies << std::make_shared<Ghost>(texGhost); break;
			}
		}

		// --- 敵更新 ---
		for (auto& e : enemies)
			e->update(delta, core.getPos());


		for (const auto& e : enemies)
			e->draw();


		player.heal_size((int32)delta);

		// --- コア処理 ---
		core.update(delta);
		core.autoHeal(delta);
		core.draw();
		Circle(core.getPos(), core.getHealArea()).drawFrame(2, Palette::Green);

		// --- プレイヤー描画 ---
		player.draw();


		// --- その他 ---

	}
}
