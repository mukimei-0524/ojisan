#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include <thread>

//ローディングシーン
class SceneLoading :public Scene
{
public:
	SceneLoading() {};
	~SceneLoading() override {};

	//初期化
	void Initialize() override;

	//終了化
	void Finalize() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render() override;

	SceneLoading(Scene* nextScene) :nextScene(nextScene) {};

private:
	Sprite* sprite = nullptr;
	float angle = 0.0f;

	//ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;
};