#pragma once
#include <Siv3D.hpp>

class Core
{
private:
	int32 maxHp;
	int32 currentHp;
	Texture texture{ U"example/image/芯_first.png" };
	Vec2 pos{ 640, 360 };
	double radius = 30.0;
	double healTimer = 0.0; // 経過時間を記録


public:

	Core(int32 hp = 100)
		: maxHp(hp), currentHp(hp) {
	}

	void draw() const
	{
		// コアの本体
		Circle{ pos, radius }.draw(Palette::Orange);
		texture.resized(60).drawAt(pos);

		// HPバー
		double hpRate = static_cast<double>(currentHp) / maxHp;
		double width = 100.0 * hpRate;
		double height = 10.0;

		Vec2 bl{ pos.x - 50, pos.y + 60 };
		Vec2 br{ bl.x + width, bl.y };
		Vec2 tr{ br.x + 5, br.y - height };
		Vec2 tl{ bl.x + 5, bl.y - height };
		Quad hpBar(tl, tr, br, bl);
		hpBar.draw(Color(GAME_COLOR_BLUE));
		hpBar.drawFrame(2, Palette::Black);


		Quad { tl, tr, br, bl }.draw(ColorF{ 0.2, 0.8, 0.2 });

	}

	void heal(int32 amount)
	{
		currentHp = Min(currentHp + amount, maxHp);
	}

	void damage(int32 amount)
	{
		currentHp = Max(currentHp - amount, 0);
	}

	bool isAlive() const
	{
		return currentHp > 0;
	}

	void autoHeal(double deltaTime)
	{
		if (currentHp == maxHp)return;
		healTimer += deltaTime;
		if (healTimer >= 1.0)
		{
			heal(5);          // 1回復
			Print << U"回復！ 現在HP: " << currentHp;
			healTimer = 0.0;  // タイマーリセット
		}
	}

	const Vec2& getPos() const { return pos; }
	double getRadius() const { return radius; }
};
