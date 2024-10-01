#include "Stage.h"

static Stage* instance = nullptr;

//インスタンス取得
Stage& Stage::Instance()
{
	return *instance;
}

//コンストラクタ
Stage::Stage()
{
	//ステージモデルを読み込み
	model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");

	instance = this;
}

Stage::~Stage()
{
	//ステージモデルを破棄
	delete model;
}

//更新処理
void Stage::Update(float elapsedTime)
{
	//今は特にやることはない
}

//描画処理
void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	//シェーダーにモデルを描画してもらう
	shader->Draw(dc, model);
}

bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}
