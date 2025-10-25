#pragma once

class Player
{
private:
	Vec2 pos{ 380, 280 };            // プレイヤーの位置
	Vec2 baseSize{ 80, 80 };         // 元サイズ
	Vec2 size{ baseSize };           // 現在の描画サイズ
	double speed = 750.0;          // 移動速度（ピクセル/秒）
	double targetScale = 1.0;      // スケーリング目標
	double currentScale = 1.0;     // 現在のスケーリング
	Texture texture{ U"example/image/player.png" }; // ← ここで固定指定
	int32 maxHp = 100;
	int32 currentHp = 100;
	bool alive = true;

	double radius = 40.0;  // 当たり判定用半径
	const double baseRadius = 40.0; // 元の半径

	double healCooldown = 1.0;     // 回復間隔（秒）
	double healTimer = 0.0;        // 前回回復からの経過時間

	double shakeTimer = 0.0;         // 揺れ時間管理
	double shakeStrength = 0.0;      // 揺れの強さ
	Vec2 shakeOffset{ 0,0 };         // 揺れのオフセット（位置に加算）

	Texture textureHeal{ U"example/image/healing2.png" };


public:
	//Player_oya();

	// --- 移動・更新 ---
	void update(double delta)
	{
		healTimer += delta; // 経過時間をカウント

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


		Vec2 move{ 0,0 };
		if (KeyW.pressed()) move.y -= speed;
		if (KeyS.pressed()) move.y += speed;
		if (KeyA.pressed()) move.x -= speed;
		if (KeyD.pressed()) move.x += speed;

		// --- 斜め移動でも速度を同じにする ---
		if (move.lengthSq() > 0)
			pos.moveBy(move.normalized() * speed * delta);

		// --- マウス追従 ---
		else if (MouseL.pressed())
		{
			Vec2 dir = Cursor::PosF() - pos;
			double dist = dir.length();
			if (dist > 4.0)
			{
				pos += dir.normalized() * speed * delta;
			}
		}

		// 画面内制限
		double halfW = size.x / 2.0;
		double halfH = size.y / 2.0;

		pos.x = Clamp(pos.x, halfW, Scene::Width() - halfW);
		pos.y = Clamp(pos.y, halfH, Scene::Height() - halfH);		// スケーリングを滑らかに更新
		currentScale = Math::Lerp(currentScale, targetScale, delta * 8.0);

		// 描画矩形更新
		//Vec2 center = rect.center();
		size = baseSize * currentScale;
		//rect.w = size.x;
		//rect.h = size.y;
		//rect.setCenter(center);
	}

	// --- 描画 ---
	void draw() const
	{
		texture.resized(size).drawAt(pos + shakeOffset);
		Circle(pos, radius).drawFrame(2, Palette::Red);

	}

	// --- HP操作 ---
	void heal_size(int32 amount)
	{
		currentHp = Min(currentHp + amount, maxHp);
		alive = (currentHp > 0);
		targetScale = Min(targetScale + amount / 100.0, 1.0);
		radius = baseRadius * targetScale;

	}

	// --- 一定間隔でのみ回復 ---
	void inAreaHeal_size(int32 amount)
	{
		if (healTimer >= healCooldown) // ← 1秒経ったら回復OK
		{
			heal_size(amount);
			healTimer = 0.0; // タイマーリセット
		}
		double angle = Scene::Time() * 2.0; // 2.0 は回転速度（ラジアン/秒）
		textureHeal.rotated(-angle).drawAt(pos);


	}

	void damage_size(int32 amount)
	{

		currentHp = Max(currentHp - amount, 0);
		alive = (currentHp > 0);
		targetScale = Max(targetScale - amount / 100.0, 0.25);
		radius = baseRadius * targetScale;
		shakeTimer = 1.0;
		shakeStrength = amount * 2;

	}

	// --- 当たり判定 ---
	bool intersects(const Vec2& otherPos, double otherRadius) const
	{
		return (pos - otherPos).length() <= (radius + otherRadius);
	}


	// --- ゲッター ---
	const Vec2& getPos() const { return pos; }
	const Vec2& getSize() const { return size; }
	int32 getHp() const { return currentHp; }
	int32 getMaxHp() const { return maxHp; }
	bool isAlive() const { return alive; }
	//RectF getRect() const { return rect; }
	double getRadius() const { return radius; }


	// --- セッター ---
	void setPos(const Vec2& p) { pos = p; }
	void setSpeed(double spd) { speed = spd; }
};
