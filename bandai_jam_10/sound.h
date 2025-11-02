#pragma once
#include <Siv3D.hpp>

enum class BGMType
{
	None,
	Title,
	Battle,
	GameOver,
	GameClear,
};

struct BGMSlot
{
	Audio audio;
	double volume = 0.0;      // 現在の音量 (0.0-1.0)
	double target = 0.0;      // 目標音量
	double fadeTime = 0.0;    // フェードにかける合計時間
	double elapsed = 0.0;     // フェード経過時間
	bool playing = false;

	void updateFade(double dt)
	{
		if (fadeTime <= 0.0)
		{
			volume = target;
		}
		else
		{
			elapsed = Clamp(elapsed + dt, 0.0, fadeTime);
			volume = Math::Lerp(volume, target, (fadeTime > 0.0) ? (dt / fadeTime) : 1.0);
		}

		audio.setVolume(volume);

		// --- フェードアウト完了で停止 ---
		if (playing && volume <= 0.0001 && target <= 0.0)
		{
			audio.stop();
			playing = false;
		}
	}

	void playIfNeeded()
	{
		if (!audio.isPlaying())
			audio.play();
		playing = true;
	}
};

//--------------------------------------------
// 🎵 SoundManager（シングルトン音声管理）
//--------------------------------------------
class SoundManager
{
private:
	// --- BGM ---
	HashTable<BGMType, BGMSlot> bgmMap;
	BGMType currentBGM = BGMType::None;

	// --- 効果音 ---
	Audio seDamage;
	Audio seHeal;
	Audio seMouse;

	// --- ボリューム設定 ---
	double bgmVolume = 0.2;
	double seVolume = 1.0;

	// --- コンストラクタ（private）---
	SoundManager()
	{
		// --- 音声ファイル ---
		seDamage = Audio{ U"example/sound/SE_Damage.mp3" };
		seHeal = Audio{ U"example/sound/SE_Heal.mp3" };
		seMouse = Audio{ U"example/sound/decision7.wav" };

		for (auto& [k, slot] : bgmMap)
		{
			slot.volume = 0.0;
			slot.target = 0.0;
			slot.audio.setVolume(0.0);
		}
	}

public:
	//--------------------------------------------
	// ✅ シングルトンアクセス
	//--------------------------------------------
	static SoundManager& Instance()
	{
		static SoundManager instance;
		return instance;
	}

	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	//--------------------------------------------
	// 🎧 毎フレーム更新（フェード処理など）
	//--------------------------------------------
	void update(double delta)
	{
		for (auto& [type, slot] : bgmMap)
		{
			slot.updateFade(delta);
		}
	}

	//--------------------------------------------
	// 🎵 BGM制御
	//--------------------------------------------
	String BGMPath(BGMType type)
	{
		switch (type)
		{
		case BGMType::Title: return U"example/sound/BGM_Title_01.ogg";
		case BGMType::Battle: return U"example/sound/BGM_Play_01.ogg";
		case BGMType::GameOver: return U"example/sound/BGM_GameOver_01.ogg";
		case BGMType::GameClear: return U"example/sound/Pokemon.mp3";
		default: return U"";
		}
	}

	void playBGM(BGMType type, double fadeSec = 1.0)
	{
		if (!bgmMap.contains(type))
		{
			bgmMap[type] = BGMSlot{ Audio{ BGMPath(type), Loop::Yes } };
		}

		if (type == currentBGM) return;

		// --- 現在のBGMをフェードアウト ---
		if (currentBGM != BGMType::None && bgmMap.contains(currentBGM))
		{
			auto& cur = bgmMap[currentBGM];
			cur.target = 0.0;
			cur.fadeTime = fadeSec;
			cur.elapsed = 0.0;
		}

		// --- 新しいBGMをフェードイン ---
		if (bgmMap.contains(type))
		{
			auto& next = bgmMap[type];
			next.target = bgmVolume;
			next.fadeTime = fadeSec;
			next.elapsed = 0.0;
			next.playIfNeeded();
		}

		currentBGM = type;
	}

	void stopBGMImmediate()
	{
		for (auto& [k, slot] : bgmMap)
		{
			slot.audio.stop();
			slot.playing = false;
			slot.volume = 0.0;
			slot.target = 0.0;
			slot.fadeTime = 0.0;
			slot.elapsed = 0.0;
		}
		currentBGM = BGMType::None;
	}

	void stopBGM(double fadeSec)
	{
		if (currentBGM != BGMType::None)
		{
			auto& cur = bgmMap[currentBGM];
			cur.target = 0.0;
			cur.fadeTime = fadeSec;
			cur.elapsed = 0.0;
			currentBGM = BGMType::None;
		}
	}

	//--------------------------------------------
	// 🔊 効果音
	//--------------------------------------------
	void playDamage() { seDamage.playOneShot(seVolume); }
	void playHeal() { seHeal.playOneShot(seVolume); }
	void MouseSe() { seMouse.playOneShot(seVolume); }

	//--------------------------------------------
	// 🎚 音量設定
	//--------------------------------------------
	void setBGMVolume(double vol)
	{
		bgmVolume = Clamp(vol, 0.0, 1.0);
		if (currentBGM != BGMType::None && bgmMap.contains(currentBGM))
		{
			bgmMap[currentBGM].target = bgmVolume;
		}
	}

	double getBGMVolume() const { return bgmVolume; }

	void setSEVolume(double vol)
	{
		seVolume = Clamp(vol, 0.0, 1.0);
	}

	double getSEVolume() const { return seVolume; }

	//--------------------------------------------
	// 🧩 音量UI（SimpleGUI）
	//--------------------------------------------
	void drawVolumeUI(const Vec2& pos)
	{
		SimpleGUI::Headline(U"音量設定", pos);

		SimpleGUI::Slider(U"BGM", bgmVolume, Vec2{ pos.x, pos.y + 40 }, 120);
		SimpleGUI::Slider(U"SE", seVolume, Vec2{ pos.x, pos.y + 70 }, 120);

		setBGMVolume(bgmVolume);
		setSEVolume(seVolume);
	}
};
