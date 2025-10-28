#pragma once
//--------------------------------------------
// ObjectPool（汎用プールクラス）
//--------------------------------------------
template <class T>
class ObjectPool
{
private:
	Array<std::unique_ptr<T>> pool;  // 未使用オブジェクト
	Array<std::unique_ptr<T>> active; // 使用中オブジェクト

public:
	ObjectPool() = default;

	// --- 事前生成 ---
	void reserve(size_t count, const std::function<std::unique_ptr<T>()>& generator)
	{
		for (size_t i = 0; i < count; i++)
		{
			pool << generator();
		}
	}

	// --- 使用要求（enqueue） ---
	T* acquire()
	{
		if (pool.isEmpty())
			return nullptr;

		std::unique_ptr<T> obj = std::move(pool.back());
		pool.pop_back();

		T* rawPtr = obj.get();
		active << std::move(obj);

		return rawPtr;
	}

	// --- 返却（dequeue） ---
	void release(T* object)
	{
		// active → pool に戻す
		auto it = std::find_if(active.begin(), active.end(),
			[object](const std::unique_ptr<T>& ptr) { return ptr.get() == object; });

		if (it != active.end())
		{
			pool << std::move(*it);
			active.remove(it);
		}
	}

	// --- 死亡オブジェクトの自動返却 ---
	void releaseIfDead()
	{
		std::vector<T*> toRelease;
		for (auto* obj : activeObjects)
		{
			if (obj->isDead()) toRelease.push_back(obj);
		}
		for (auto* obj : toRelease)
		{
			release(obj);
		}
	}

	// --- 更新・描画ヘルパー ---
	void updateAll(double delta, const Vec2& playerPos)
	{
		for (auto& e : active)
		{
			e->update(delta, playerPos);
		}
	}

	void drawAll() const
	{
		for (auto& e : active)
		{
			e->draw();
		}
	}

	// --- アクティブ数 ---
	size_t activeCount() const { return active.size(); }
};
