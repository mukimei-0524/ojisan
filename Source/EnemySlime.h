#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

//スライム
class EnemySlime :public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader)override;

private:
	Model* model = nullptr;

protected:
	void OnDead() override;
};