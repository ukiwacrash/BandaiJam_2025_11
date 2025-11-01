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
	ObjectPool<SmartEnemy> smartPool;
	ObjectPool<BlinkEnemy> blinkPool;

	// 共通テクスチャ
	Texture texMoney{ U"example/image/欲.png" };
	Texture texGreed{ U"example/image/悪.png" };
	Texture texSake{ U"example/image/煙.png" };
	Texture texCircle{ U"example/image/金.png" };
	Texture texSmart{ U"example/image/酒.png" };
	Texture texBlink{ U"example/image/病.png" };

public:
	EnemyManager()
	{
		moneyPool.reserve(10, [this]() { return std::make_unique<Money>(texMoney); });
		greedPool.reserve(10, [this]() { return std::make_unique<Greed>(texGreed); });
		sakePool.reserve(10, [this]() { return std::make_unique<Sake>(texSake); });
		circlePool.reserve(10, [this]() { return std::make_unique<CircleEnemy>(texCircle); });
		smartPool.reserve(10, [this]() { return std::make_unique<SmartEnemy>(texSmart); });
		blinkPool.reserve(10, [this]() { return std::make_unique<BlinkEnemy>(texBlink); });
	}

	void spawnMoney() { moneyPool.acquire(); }
	void spawnGreed() { greedPool.acquire(); }
	void spawnSake() { sakePool.acquire(); }
	void spawnCircle() { circlePool.acquire(); }
	void spawnSmart() { smartPool.acquire(); }
	void spawnBlink() { blinkPool.acquire(); }

	void update(double delta, const Vec2& playerPos)
	{
		moneyPool.updateAll(delta, playerPos);  
		greedPool.updateAll(delta, playerPos);  
		sakePool.updateAll(delta, playerPos);   
		circlePool.updateAll(delta, playerPos);
		smartPool.updateAll(delta, playerPos);
		blinkPool.updateAll(delta, playerPos);
	}

	void draw()
	{
		moneyPool.drawAll();
		greedPool.drawAll();
		sakePool.drawAll();
		circlePool.drawAll();
		smartPool.drawAll();
		blinkPool.drawAll();
	}

	// --- すべてのアクティブ敵を取得 ---
	s3d::Array<Enemy*> getActiveEnemies() const
	{
		s3d::Array<Enemy*> result;

		for (auto* e : moneyPool.getActiveListArray())   result << static_cast<Enemy*>(e);
		for (auto* e : greedPool.getActiveListArray())   result << static_cast<Enemy*>(e);
		for (auto* e : sakePool.getActiveListArray())    result << static_cast<Enemy*>(e);
		for (auto* e : circlePool.getActiveListArray())  result << static_cast<Enemy*>(e);
		for (auto* e : smartPool.getActiveListArray())   result << static_cast<Enemy*>(e);
		for (auto* e : blinkPool.getActiveListArray())   result << static_cast<Enemy*>(e);

		return result;
	}

	void releaseDeadEnemies()
	{
		moneyPool.releaseIfDead();
		greedPool.releaseIfDead();
		sakePool.releaseIfDead();
		circlePool.releaseIfDead();
		smartPool.releaseIfDead();
		blinkPool.releaseIfDead();
	}

	void resetAll()
	{
		// 各プールの全アクティブ敵をリセット
		for (auto* e : moneyPool.getActiveListArray())   e->reset();
		for (auto* e : greedPool.getActiveListArray())   e->reset();
		for (auto* e : sakePool.getActiveListArray())    e->reset();
		for (auto* e : circlePool.getActiveListArray())  e->reset();
		for (auto* e : smartPool.getActiveListArray())   e->reset();
		for (auto* e : blinkPool.getActiveListArray())   e->reset();

		// 死んでいる敵をプールに戻す
		releaseDeadEnemies();
	}
};

