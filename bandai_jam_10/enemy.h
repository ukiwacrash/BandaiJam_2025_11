#pragma once

class Enemy
{
protected:
	Vec2 pos{ 0,0 };
	Vec2 size{ 100, 100 };
	int32 maxHp;
	int32 currentHp;
	double speed = 100.0;
	double radius = 50;
	Texture texture;
	String name;
public:
	
	//コンストラクタ修正版			//hp=100がダメやった　（後で消して）
	Enemy(const Texture& tex, const String& n, int32 hp, double spd, double rds = 50.0f)
		: texture(tex), name(n), maxHp(hp), currentHp(hp), speed(spd), radius(rds)
	{
		// --- 画面端ランダム生成 ---
		int side = Random(0, 3); // 0:上 1:下 2:左 3:右

		auto Rand = [](double min, double max) {
			return static_cast<double>(Random(static_cast<int32>(min), static_cast<int32>(max)));
			};

		switch (side)
		{
		case 0: pos = { Rand(0, Scene::Width()), -size.y }; break;
		case 1: pos = { Rand(0, Scene::Width()), Scene::Height() + size.y }; break;
		case 2: pos = { -size.x, Rand(0, Scene::Height()) }; break;
		case 3: pos = { Scene::Width() + size.x, Rand(0, Scene::Height()) }; break;
		}
	}

	// --- プールに返すべきか判定 ---
	virtual bool isDead() const
	{
		// HPゼロなら返却
		return (currentHp <= 0);
	}

	// --- 更新 ---
	virtual void update(double delta, const Vec2& playerPos)
	{
		Vec2 dir = (playerPos - pos).normalized();
		pos += dir * speed * delta;
	}

	// --- 当たり判定 ---
	virtual double getRadius()
	{
		return radius;
	}

	// --- 位置取得 ---
	const Vec2& getPos() const { return pos; }

	// --- 描画処理 ---
	virtual void draw() const
	{
		texture.resized(size).drawAt(pos);
	}

	virtual void damage(int32 damage)
	{
		currentHp -= damage;
		if (currentHp < 0) currentHp = 0;
	}

	double getRadius() const { return radius; }
};

//--------------------------------------------
// Money（金）
//--------------------------------------------
class Money : public Enemy
{
public:
	Money(const Texture& tex)
		: Enemy(tex, U"Money", 80, 30.0) {}// ✅ 引数をすべて渡す
};

//--------------------------------------------
// Greed（欲）
//--------------------------------------------
class Greed : public Enemy
{
private:
	double t = 0.0;

public:
	Greed(const Texture& tex)
		: Enemy(tex, U"Greed", 100, 120.0){ }

	void update(double delta, const Vec2& playerPos) override
	{
		t += delta * 5.0;
		Vec2 dir = (playerPos - pos).normalized();
		Vec2 zigzag = Vec2(Math::Sin(t) * 60, 0);
		pos += (dir * speed * delta) + zigzag * delta;
	}
};

//--------------------------------------------
// Sake（酒）
//--------------------------------------------
class Sake : public Enemy
{
private:
	double t = 0.0;

public:
	Sake(const Texture& tex)
		: Enemy(tex, U"Sake", 60, 60.0){ }

	void update(double delta, const Vec2& playerPos) override
	{
		t += delta * 2.0;
		Vec2 dir = (playerPos - pos).normalized();
		pos += dir * speed * delta;
		pos.y += Math::Sin(t * 2.0) * 1.5;
	}
};

//--------------------------------------------
// CircleEnemy（円を描いて動く敵）
//--------------------------------------------
class CircleEnemy : public Enemy
{
private:
	double t = 0.0;

public:
	CircleEnemy(const Texture& tex)
		: Enemy(tex, U"Circle", 100, 100) {}// speedは0、移動は独自制御

	void update(double delta, const Vec2& playerPos) override
	{
		t += delta * 2.0;
		Vec2 dir = (playerPos - pos).normalized();
		pos += dir * speed * delta;
		pos.y += Math::Sin(t * 2.0) * 1.5;
		pos.x += Math::Cos(t * 2.0) * 1.5;
	}
};
