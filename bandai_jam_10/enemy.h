#pragma once
#include"Core.h"
class Enemy
{
protected:
	Vec2 pos{ 0,0 };
	Vec2 size{ 100, 100 };
	int32 maxHp;
	int32 currentHp;
	double speed = 1;
	double radius = 50;
	Texture texture;
	String name;

	bool alive = true;

public:

	//コンストラクタ修正版			//hp=100がダメやった　（後で消して）
	Enemy(const Texture& tex, const String& n, int32 hp, double spd, double rds = 50.0f)
		: texture(tex), name(n), maxHp(hp), currentHp(hp), speed(spd), radius(rds)
	{
		pos = randomStartPosition();
	}

	// --- プールに返すべきか判定 ---
	virtual bool isDead() const
	{
		// HPゼロなら返却
		return (currentHp <= 0);
	}

	// --- 更新 ---
	virtual void update(double delta, const Vec2& CorePos)
	{
		Vec2 dir = (CorePos - pos).normalized();
		pos += dir * (speed*2) * delta;
	}

	// --- 当たり判定 ---
	virtual double getRadius()
	{
		return radius;
	}

	// 当たり判定用
	bool intersects(const Vec2& otherPos, double otherRadius) const
	{
		return (pos - otherPos).length() <= (radius + otherRadius);
	}



	// --- 位置取得 ---
	const Vec2& getPos() const { return pos; }

	// --- 位置取得 ---
	Vec2 randomStartPosition()
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

		return pos;
	}

	// --- 描画処理 ---
	virtual void draw() const
	{
		texture.resized(size).drawAt(pos);
	}

	virtual void damage(int32 amount)
	{
		currentHp = Max(currentHp - amount, 0);
		if (currentHp <= 0) alive = false;
	}

	virtual void reset() {
		currentHp = maxHp;
		pos = randomStartPosition(); // 画面端ランダム生成
	}

	double getRadius() const { return radius; }

	int32 getCurrentHp() const { return currentHp; }
};

//--------------------------------------------
// Money（金）
//--------------------------------------------
class Money : public Enemy
{
public:
	Money(const Texture& tex)
		: Enemy(tex, U"Money", 80, 30.0) {
	}// ✅ 引数をすべて渡す
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
		: Enemy(tex, U"Greed", 100, 120.0) {
	}

	void update(double delta, const Vec2& CorePos) override
	{
		t += delta * 5.0;
		Vec2 dir = (CorePos - pos).normalized();
		Vec2 zigzag = Vec2(Math::Sin(t) * 60, 0);
		pos += (dir * (speed * 2) * delta) + zigzag * delta;
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
		: Enemy(tex, U"Sake", 60, 60.0) {
	}

	void update(double delta, const Vec2& CorePos) override
	{
		t += delta * 2.0;
		Vec2 dir = (CorePos - pos).normalized();
		pos += dir * (speed * 2) * delta;
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
		: Enemy(tex, U"Circle", 100, 100) {
	}// speedは0、移動は独自制御

	void update(double delta, const Vec2& CorePos) override
	{
		t += delta * 2.0;
		Vec2 dir = (CorePos - pos).normalized();
		pos += dir * (speed*2) * delta;
		pos.y += Math::Sin(t * 2.0) * 1.5;
		pos.x += Math::Cos(t * 2.0) * 1.5;
	}
};

//--------------------------------------------
// SmartEnemy（複合行動型）
//--------------------------------------------
class SmartEnemy : public Enemy
{
private:
	double t = 0.0;           // 内部タイマー
	double dashCooldown = 1.0; // 突進間隔
	double dashTimer = 0.0;
	double speedNormal = 80.0;
	double speedDash = 200.0;
	bool dashing = false;

public:
	SmartEnemy(const Texture& tex)
		: Enemy(tex, U"Smart", 150, 0.0)
	{
		speed = speedNormal;
	}

	void update(double delta, const Vec2& CorePos) override
	{
		t += delta;
		dashTimer += delta;

		Vec2 toPlayer = (CorePos - pos);
		double distance = toPlayer.length();

		// --- 突進判定 ---
		if (dashTimer >= dashCooldown)
		{
			dashing = true;
			dashTimer = 0.0;
		}

		if (dashing)
		{
			Vec2 dir = toPlayer.normalized();
			pos += dir * speedDash * delta;

			// 突進終了判定
			if (distance < 100.0) dashing = false;
		}
		else
		{
			// --- 円運動＋追尾 ---
			Vec2 dir = toPlayer.normalized();
			Vec2 perp = Vec2(-dir.y, dir.x); // 垂直方向
			pos += dir * speedNormal * delta + perp * Math::Sin(t * 3.0) * 30.0 * delta;
		}

		// 画面端制限
		pos.x = Clamp(pos.x, size.x / 2.0, Scene::Width() - size.x / 2.0);
		pos.y = Clamp(pos.y, size.y / 2.0, Scene::Height() - size.y / 2.0);
	}

	void draw() const override
	{
		if (dashing)
		{
			// 突進時は赤く光らせる
			texture.resized(size).drawAt(pos).drawFrame(4, Palette::Red);
		}
		else
		{
			texture.resized(size).drawAt(pos);
		}
	}
};

//--------------------------------------------
// BlinkEnemy（点滅しながら動く敵）
//--------------------------------------------
class BlinkEnemy : public Enemy
{
private:
	double t = 0.0;         // 時間経過
	double blinkSpeed = 5.0; // 点滅の速さ
	double alpha = 1.0;     // 現在の透明度

public:
	BlinkEnemy(const Texture& tex)
		: Enemy(tex, U"Blink", 80, 80.0)
	{
	}

	void update(double delta, const Vec2& CorePos) override
	{
		t += delta;

		// --- 点滅処理 ---
		alpha = (Math::Sin(t * blinkSpeed) * 0.5 + 0.5); // 0〜1の範囲で周期的に変化

		// --- プレイヤーへ追尾 ---
		Vec2 dir = (CorePos - pos).normalized();
		pos += dir * (speed * 2) * delta;
	}

	void draw() const override
	{
		ColorF color(1.0, 1.0, 1.0, alpha); // alpha値を透過に使う
		texture.resized(size).drawAt(pos, color);
	}
};

// --- ボス構造体 ---
class Boss
{
public:
	Vec2 pos;    // 画面外（上）に最初は待機
	Vec2 size{ 160, 160 };
	double speed = 100.0;
	bool alive = false;      // 出現フラグ
	double radius = 100;

	int32 maxHp;
	int32 currentHp;

	Texture texBoss;

	// --- 出現演出用 ---
	bool justSpawned = false;   // 出現した瞬間だけtrue
	double spawnEffectTimer = 0.5; // エフェクトの持続時間（秒）


	Boss(Vec2 startPos = Vec2{ 1280, -100 }, int32 hp = 100, double spd = 50.0)
		: pos(startPos), maxHp(hp), currentHp(hp), speed(spd)
	{
		texBoss = Texture{ U"example/image/Boss.png" }; // ボス用画像
	}

	void update(const Vec2& playerPos, const Vec2& corePos, Core& core)
	{
		if (!alive) return;

		Vec2 dir = (core.getPos() - pos).normalized();
		pos += dir * speed * Scene::DeltaTime();

		double distToCore = pos.distanceFrom(core.getPos());
		if (distToCore < 100.0)
		{
			// core のHPを直接減らす
			double newHP = core.getHp() - 10 * Scene::DeltaTime();
			core.setHP(newHP);
		}


	}
	void updateEffect(double delta)
	{
		if (justSpawned)
		{
			spawnEffectTimer -= delta;
			if (spawnEffectTimer <= 0)
				justSpawned = false; // エフェクト終了
		}
	}


	void takeDamage(int amount)
	{
		currentHp = Max(currentHp - amount, 0);
		if (currentHp <= 0) alive = false;

	}
	// 当たり判定用
	bool intersects(const Vec2& otherPos, double otherRadius) const
	{
		return (pos - otherPos).length() <= (radius + otherRadius);
	}


	void draw() const
	{
		if (!alive) return;

		// --- 出現エフェクト ---
		if (justSpawned)
		{
			double scale = 1.0 + 0.5 * Sin((0.5 - spawnEffectTimer) * Math::Pi * 2); // 少し大きくなる
			texBoss.scaled(scale).drawAt(pos);
			// 輝きエフェクト
			Circle(pos, radius * scale).drawFrame(4, Palette::Yellow);
		}
		else
		{
			texBoss.drawAt(pos);
		}

		Circle(pos, radius).drawFrame(2, Palette::Red); // 当たり判定枠
	}
	bool isAlive() const { return alive; }

};

