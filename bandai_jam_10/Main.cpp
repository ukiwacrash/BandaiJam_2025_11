#include "environment.h"

//-----------------------------------------
// メイン関数
//-----------------------------------------

void Main()
{
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF(0.15, 0.2, 0.3));

	Texture tex(U"example/image/aaa.jpg");
	Texture texSlime(U"example/image/無題.png");
	Texture texBat(U"example/image/無題.png");
	Texture texGhost(U"example/image/無題.png");

	Stopwatch spawnTimer{ StartImmediately::Yes };
	const double spawnInterval = 1.5;

	Player player(tex);
	Array<std::shared_ptr<Enemy>> enemies;
	Font font(24);

	double time = 0.0f;

	while (System::Update())
	{
		const double delta = Scene::DeltaTime();
		time += delta;

		if (KeySpace.down())
			player.takeDamage(5);

		if (KeyH.down())
			player.Heal(5);

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
			e->update(delta, player.Pos);

		player.update(delta);

		for (const auto& e : enemies)
			e->draw();

		player.draw();

		Vec2 size = tex.size();

		font(U"WASDで移動").draw(20, 20, ColorF(0.0));
		font(size).draw(20, 70, ColorF(0.0));
	}
}
