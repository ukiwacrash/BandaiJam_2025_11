#pragma once
#include <Siv3D.hpp>

class Core
{
private:
	int32 maxHp;
	int32 currentHp;
	Texture texture{ U"example/image/Core.png" };

	Vec2 pos{ 640, 360 };
	double radius = 30.0;

	double healTimer = 0.0;          // 自動回復用タイマー
	double damageCooldown = 1.0;     // ダメージ間隔（秒）
	double damageTimer = 0.0;        // ダメージ用タイマー

	// --- HPバー揺れ＆赤残り演出 ---
	double prevHpRate = 1.0;         // 前回のHP割合（赤ゲージ用）
	double shakeTimer = 0.0;         // 揺れ時間管理
	double shakeStrength = 0.0;      // 揺れの強さ
	Vec2 shakeOffset{ 0,0 };         // 揺れのオフセット（位置に加算）

public:
	Core(int32 hp = 100)
		: maxHp(hp), currentHp(hp)
	{
	}

	void update(double delta)
	{
		damageTimer += delta;

		// --- 揺れ制御 ---
		if (shakeTimer > 0.0)
		{
			shakeTimer -= delta;
			shakeStrength = Math::Lerp(shakeStrength, 0.0, delta * 5.0);

			// ランダムな微振動
			shakeOffset = Vec2(Random(-shakeStrength, shakeStrength),
							   Random(-shakeStrength, shakeStrength));
		}
		else
		{
			shakeStrength = 0.0;
			shakeOffset = Vec2(0, 0);
		}
	}

	void draw() const
	{
		// --- HP割合 ---
		double hpRate = static_cast<double>(currentHp) / maxHp;

		// --- HPバー基準位置（揺れも連動） ---
		Vec2 bl{ pos.x - 50 + shakeOffset.x, pos.y + 60 + shakeOffset.y };
		double fullWidth = 100.0;
		double height = 10.0;

		// --- 赤ゲージ（前回のHPを遅れて追う） ---
		static double redRate = hpRate;
		if (redRate > hpRate)
		{
			redRate = Math::Lerp(redRate, hpRate, 0.05);
		}
		else
		{
			redRate = hpRate;
		}

		// --- 赤ゲージ描画 ---
		Vec2 brRed{ bl.x + fullWidth * redRate, bl.y };
		Vec2 trRed{ brRed.x + 5, brRed.y - height };
		Vec2 tlRed{ bl.x + 5, bl.y - height };
		Quad redBar(tlRed, trRed, brRed, bl);
		redBar.draw(ColorF{ Palette::Red });
		redBar.drawFrame(2, Palette::Black); // ← 枠線追加

		// --- 青ゲージ（現在のHP） ---
		Vec2 brBlue{ bl.x + fullWidth * hpRate, bl.y };
		Vec2 trBlue{ brBlue.x + 5, brBlue.y - height };
		Vec2 tlBlue{ bl.x + 5, bl.y - height };
		Quad blueBar(tlBlue, trBlue, brBlue, bl);
		blueBar.draw(Color(0, 180, 255));
		blueBar.drawFrame(2, Palette::Black); // ← 既存の枠線

		// --- コア本体（揺れを反映） ---
		Vec2 drawPos = pos + shakeOffset;
		Circle{ drawPos, radius }.draw(Palette::Orange);
		texture.resized(60).drawAt(drawPos);
	}

	// --- 回復 ---
	void heal(int32 amount)
	{
		currentHp = Min(currentHp + amount, maxHp);
	}

	// --- ダメージ ---
	void damage(int32 amount)
	{
		currentHp = Max(currentHp - amount, 0);

		// 揺れ演出を開始
		shakeTimer = 0.3;     // 揺れる時間（秒）
		shakeStrength = 4.0;  // 揺れの強さ
	}

	// --- 一定間隔でのみダメージを受ける ---
	void tryDamage(int32 amount)
	{
		if (damageTimer >= damageCooldown)
		{
			damage(amount);
			damageTimer = 0.0;
		}
	}

	// --- 自動回復 ---
	void autoHeal(double deltaTime)
	{
		if (currentHp == maxHp) return;

		healTimer += deltaTime;
		if (healTimer >= 1.0)
		{
			heal(5);
			healTimer = 0.0;
		}
	}

	// --- 判定系 ---
	bool isAlive() const { return currentHp > 0; }
	const Vec2& getPos() const { return pos; }
	double getRadius() const { return radius; }
	double getHealArea() const { return radius * 5; }
	double getHp() const { return currentHp; }
	double getMaxHp() const { return maxHp; }
};
