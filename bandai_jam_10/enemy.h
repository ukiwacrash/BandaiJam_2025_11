//#pragma once
//
//class Enemy
//{
//public:
//	Vec2 pos;
//	Vec2 size{ 40, 40 };
//	int32 maxHp;
//	int32 currentHp;
//	double speed = 100.0;
//	double radius = 50;
//	Texture texture;
//	String name;
//
//	Enemy(const Texture& tex, const String& n, int32 hp = 100, double spd, double rds)
//		: texture(tex), name(n), maxHp(hp), currentHp(hp), speed(spd), radius(rds)
//	{
//		// --- 画面端ランダム生成 ---
//		int side = Random(0, 3); // 0:上 1:下 2:左 3:右
//
//		auto Rand = [](double min, double max) {
//			return static_cast<double>(Random(static_cast<int32>(min), static_cast<int32>(max)));
//			};
//
//		switch (side)
//		{
//		case 0: pos = { Rand(0, Scene::Width()), -size.y }; break;
//		case 1: pos = { Rand(0, Scene::Width()), Scene::Height() + size.y }; break;
//		case 2: pos = { -size.x, Rand(0, Scene::Height()) }; break;
//		case 3: pos = { Scene::Width() + size.x, Rand(0, Scene::Height()) }; break;
//		}
//	}
//
//	virtual void update(double delta, const Vec2& playerPos)
//	{
//		Vec2 dir = (playerPos - pos).normalized();
//		pos += dir * speed * delta;
//	}
//
//	virtual void draw() const
//	{
//		texture.resized(size).drawAt(pos);
//	}
//};
//
////--------------------------------------------
//// スライム（大きくて遅い）
////--------------------------------------------
//class Slime : public Enemy
//{
//public:
//	Slime(const Texture& tex)
//		: Enemy(tex, U"Slime", 80.0, 30.0)
//	{
//		size = Vec2(80, 80);
//	}
//};
//
////--------------------------------------------
//// バット（中くらい、速い、ジグザグ）
////--------------------------------------------
//class Bat : public Enemy
//{
//	double t = 0.0;
//
//public:
//	Bat(const Texture& tex)
//		: Enemy(tex, U"Bat", 160.0, 20)
//	{
//		size = Vec2(50, 50);
//	}
//
//	void update(double delta, const Vec2& playerPos) override
//	{
//		t += delta * 5.0;
//		Vec2 dir = (playerPos - pos).normalized();
//		Vec2 zigzag = Vec2(Math::Sin(t) * 60, 0);
//		pos += (dir * speed * delta) + zigzag * delta;
//	}
//};
//
////--------------------------------------------
//// ゴースト（小さくて遅い、ふわふわ）
////--------------------------------------------
//class Ghost : public Enemy
//{
//	double t = 0.0;
//
//public:
//	Ghost(const Texture& tex)
//		: Enemy(tex, U"Ghost", 60.0, 10)
//	{
//		size = Vec2(35, 35);
//	}
//
//	void update(double delta, const Vec2& playerPos) override
//	{
//		t += delta * 2.0;
//		Vec2 dir = (playerPos - pos).normalized();
//		pos += dir * speed * delta;
//		pos.y += Math::Sin(t * 2.0) * 1.5;
//	}
//};

#pragma once
#include <Siv3D.hpp> // これを忘れると Vec2, Texture などが使えない！

class Enemy
{
public:
	Vec2 pos;
	Vec2 size{ 40, 40 };
	int32 maxHp;
	int32 currentHp;
	double speed = 100.0;
	double radius = 50;
	Texture texture;
	String name;
	//コンストラクタ修正版			//hp=100がダメやった　（後で消して）
	Enemy(const Texture& tex, const String& n, int32 hp, double spd, double rds)
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

	virtual void update(double delta, const Vec2& playerPos)
	{
		Vec2 dir = (playerPos - pos).normalized();
		pos += dir * speed * delta;
	}

	virtual void draw() const
	{
		texture.resized(size).drawAt(pos);
	}
};

//--------------------------------------------
// スライム（大きくて遅い）
//--------------------------------------------
class Slime : public Enemy
{
public:
	Slime(const Texture& tex)
		: Enemy(tex, U"Slime", 80, 30.0, 60.0) // ✅ 引数をすべて渡す
	{
		size = Vec2(80, 80);
	}
};

//--------------------------------------------
// バット（中くらい、速い、ジグザグ）
//--------------------------------------------
class Bat : public Enemy
{
	double t = 0.0;

public:
	Bat(const Texture& tex)
		: Enemy(tex, U"Bat", 100, 120.0, 40.0)
	{
		size = Vec2(50, 50);
	}

	void update(double delta, const Vec2& playerPos) override
	{
		t += delta * 5.0;
		Vec2 dir = (playerPos - pos).normalized();
		Vec2 zigzag = Vec2(Math::Sin(t) * 60, 0);
		pos += (dir * speed * delta) + zigzag * delta;
	}
};

//--------------------------------------------
// ゴースト（小さくて遅い、ふわふわ）
//--------------------------------------------
class Ghost : public Enemy
{
	double t = 0.0;

public:
	Ghost(const Texture& tex)
		: Enemy(tex, U"Ghost", 60, 60.0, 30.0)
	{
		size = Vec2(35, 35);
	}

	void update(double delta, const Vec2& playerPos) override
	{
		t += delta * 2.0;
		Vec2 dir = (playerPos - pos).normalized();
		pos += dir * speed * delta;
		pos.y += Math::Sin(t * 2.0) * 1.5;
	}
};
