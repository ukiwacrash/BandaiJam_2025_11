#pragma once
//--------------------------------------------
// EnemyManager
//--------------------------------------------
class EnemyManager
{
private:
	ObjectPool<Money> moneyPool;
	ObjectPool<Greed> greedPool;
	ObjectPool<Sake> sakePool;
	ObjectPool<CircleEnemy> circlePool;

	// --- 敵テクスチャ（共通読み込み） ---
	Texture texMoney{ U"example/image/無題.png" };
	Texture texGreed{ U"example/image/無題.png" };
	Texture texSake{ U"example/image/無題.png" };
	Texture texCircle{ U"example/image/aaa.jpg" };

public:
	EnemyManager()
	{
		// 各種プールを事前生成
		moneyPool.reserve(10, [this]() { return std::make_unique<Money>(texMoney); });
		greedPool.reserve(10, [this]() { return std::make_unique<Greed>(texGreed); });
		sakePool.reserve(10, [this]() { return std::make_unique<Sake>(texSake); });
		circlePool.reserve(10, [this]() { return std::make_unique<CircleEnemy>(texCircle); });
	}

	void spawnMoney() { moneyPool.acquire(); }
	void spawnGreed() { greedPool.acquire(); }
	void spawnSake() { sakePool.acquire(); }
	void spawnCircle() { circlePool.acquire(); }

	void update(double delta, const Vec2& playerPos)
	{
		moneyPool.updateAll(delta, playerPos);
		moneyPool.releaseIfDead();

		greedPool.updateAll(delta, playerPos);
		greedPool.releaseIfDead();

		sakePool.updateAll(delta, playerPos);
		sakePool.releaseIfDead();

		circlePool.updateAll(delta, playerPos);
		circlePool.releaseIfDead();
	}

	void draw()
	{
		moneyPool.drawAll();
		greedPool.drawAll();
		sakePool.drawAll();
		circlePool.drawAll();
	}
};

