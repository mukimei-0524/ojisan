#pragma once

#include "Graphics/Shader.h"

//前方宣言
class ProjectileManager;

//弾丸
class Projectile
{
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 0,0,1 };	//方向
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	ProjectileManager* manager = nullptr;

	//行列更新処理
	void UpdateTransform();

public:
	//Projectile(){}
	Projectile(ProjectileManager* manager);
	virtual ~Projectile(){}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//デバックプリミティブ描画
	virtual void DrawDebugPrimitive();

	//位置取得
	const DirectX::XMFLOAT3& GetPosition()const { return position; }

	//方向取得
	const DirectX::XMFLOAT3& GetDirection()const { return direction; }

	//スケール取得
	const DirectX::XMFLOAT3& GetScale()const { return scale; }

	//破棄
	void Destroy();

	//半径取得
	float GetRadius()const { return radius; }

protected:
	float radius = 0.5f;
};

//std::set：重複しない
//std::vector：重複する