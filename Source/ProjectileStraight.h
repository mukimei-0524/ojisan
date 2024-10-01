#pragma once

#include "Graphics/Graphics.h"
#include "Projectile.h"

//直進弾丸
class ProjectileStraight :public Projectile
{
public:
	//ProjectileStraight();
	ProjectileStraight(ProjectileManager* manager);
	~ProjectileStraight() override;		//継承したらオーバーライド

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//発射
	void Launch(const DirectX::XMFLOAT3& direction,
				const DirectX::XMFLOAT3& position
				);

private:
	Model* model = nullptr;
	float speed = 10.0f;
	float lifeTimer = 3.0f;
};