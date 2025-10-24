#pragma once

class Player
{
public:

	Vec2 Pos{ 380, 280 };				// プレイヤーの位置
	Vec2 size{ 80, 80 };				// プレイヤーのサイズ
	double speed = 200.0;				// 移動速度（ピクセル/秒）
	Vec2 baseSize = Vec2{ size };		// 元サイズ
	double targetScale = 1.0;			// 1.0 = 元サイズ
	double currentScale = 1.0;
	Texture texture;					// 画像用
	RectF rect;							// 描画矩形

	Player(const Texture& tex) : texture(tex) { rect = RectF(Pos, size); }

	void takeDamage(double amount)
	{
		amount /= 100;
		targetScale = Max(targetScale - amount, 0.25); // 25% 以上に制限
	}

	void Heal(double amount)
	{
		amount /= 100;
		targetScale = Min(targetScale + amount, 1.0); // 最大1.0
	}

	void update(double delta)
	{
		Vec2 move{ 0,0 };
		if (KeyW.pressed()) move.y -= 1;
		if (KeyS.pressed()) move.y += 1;
		if (KeyA.pressed()) move.x -= 1;
		if (KeyD.pressed()) move.x += 1;

		if (move.lengthSq() > 0)
			Pos.moveBy(move.normalized() * speed * delta);
		else if (MouseL.pressed()) // --- マウス操作（クリック中） ---
		{
			Vec2 dir = Cursor::PosF() - Pos; // プレイヤー→マウスの方向ベクトル
			double dist = dir.length();

			if (dist > 4.0) // 少し離れていれば
			{
				Pos += dir.normalized() * speed * delta;
			}
		}

		Pos.x = Clamp(Pos.x, 0.0, Scene::Width() - size.x);
		Pos.y = Clamp(Pos.y, 0.0, Scene::Height() - size.y);

		// 現在の倍率を滑らかに targetScale に近づける
		currentScale = Math::Lerp(currentScale, targetScale, delta * 8.0);

		Vec2 center = rect.center();
		rect.w = baseSize.x * currentScale;
		rect.h = baseSize.y * currentScale;
		rect.setCenter(center);
	}

	void draw() const
	{
		texture.resized(size).draw(Pos);
	}


};
