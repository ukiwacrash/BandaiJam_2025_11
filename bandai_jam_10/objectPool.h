#pragma once
//--------------------------------------------
// ObjectPool（汎用プールクラス）
//--------------------------------------------
template<class T>
class ObjectPool
{
private:
	std::vector<std::unique_ptr<T>> objects;
	std::vector<T*> activeObjects;
	std::function<std::unique_ptr<T>()> factory;

public:
	void reserve(size_t count, std::function<std::unique_ptr<T>()> createFunc)
	{
		factory = createFunc;
		for (size_t i = 0; i < count; ++i)
			objects.push_back(factory());
	}

	T* acquire()
	{
		for (auto& obj : objects)
		{
			if (std::find(activeObjects.begin(), activeObjects.end(), obj.get()) == activeObjects.end())
			{
				activeObjects.push_back(obj.get());
				return obj.get();
			}
		}
		objects.push_back(factory());
		activeObjects.push_back(objects.back().get());
		return activeObjects.back();
	}

	void release(T* obj)
	{
		activeObjects.erase(std::remove(activeObjects.begin(), activeObjects.end(), obj), activeObjects.end());
	}

	void updateAll(double delta, const Vec2& playerPos)
	{
		for (auto* obj : activeObjects)
			obj->update(delta, playerPos);
	}

	void drawAll() const
	{
		for (auto* obj : activeObjects)
			obj->draw();
	}

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

	const std::vector<T*>& getActiveList() const { return activeObjects; }
};
