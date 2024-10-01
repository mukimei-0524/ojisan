#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	stage = new Stage();

	//�v���C���[������
	player = new Player();

	//�Q�[�W�X�v���C�g
	gauge = new Sprite();

	//�J���������ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	//�J�����R���g���[���[�̏�����
	cameraController = new CameraController();

	//�G�l�~�[������
	//enemySlime = new EnemySlime();
	EnemyManager& enemyManager = EnemyManager::Instance();

	for (int i = 0; i < 2; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}
}

// �I����
void SceneGame::Finalize()
{
	//�X�e�[�W�I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	//�v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	//�G�l�~�[�I����
	EnemyManager::Instance(). Clear();

	//�Q�[�W�X�v���C�g�I����
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�X�e�[�W�X�V����
	stage->Update(elapsedTime);

	//�v���C���[�X�V����
	player->Update(elapsedTime);

	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	//�G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	//�J�����p�����[�^�[�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//// �r���[�s��
	//{
	//	DirectX::XMFLOAT3 eye = { 0, 10, -10 };	// �J�����̎��_�i�ʒu�j
	//	DirectX::XMFLOAT3 focus = { 0, 0, 0 };	// �J�����̒����_�i�^�[�Q�b�g�j
	//	DirectX::XMFLOAT3 up = { 0, 1, 0 };		// �J�����̏����

	//	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	//	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	//	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	//	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	//	DirectX::XMStoreFloat4x4(&rc.view, View);
	//}
	//// �v���W�F�N�V�����s��
	//{
	//	float fovY = DirectX::XMConvertToRadians(45);	// ����p
	//	float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();	// ��ʏc���䗦
	//	float nearZ = 0.1f;	// �J�������f���o���̍ŋߋ���
	//	float farZ = 1000.0f;	// �J�������f���o���̍ŉ�����
	//	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	//	DirectX::XMStoreFloat4x4(&rc.projection, Projection);
	//}

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		//�X�e�[�W�`��
		stage->Render(dc, shader);
		//�v���C���[�`��
		player->Render(dc, shader);
		//�G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);

		shader->End(dc);
	}

	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		player->DrawDebugPrimitive();

		//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance().DrawDebugPrimitive();
		
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);

	}

	// 2D�X�v���C�g�`��
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		//player�f�o�b�N�`��
		player->DrawDebugGUI();
	}
}

void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	//�r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//�ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//�S�Ă̓G�̓����HP�Q�[�W��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		//�G�l�~�[�̓���̃��[���h���W(�G�̈ʒu)
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();

		//y�̈ʒu�̍���������
		worldPosition.y += enemy->GetHeight();

		//�G�̈ʒu��XMVECTOR�ɕϊ�
		DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

		//���[���h���W(3D)����X�N���[�����W(2D)�֕ϊ�
		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project
		(WorldPosition,
			0.0f,
			0.0f,
			viewport.Width,
			viewport.Height,
			0.0f,
			1.0f,
			DirectX::XMLoadFloat4x4(&projection),
			DirectX::XMLoadFloat4x4(&view),
			DirectX::XMMatrixIdentity()
		);

		//�X�N���[�����W����i3D����2D�ɂ���XMVECTOR��XMFLOAT2�ɕϊ��j
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		//�Q�[�W�̒���
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		//�Q�[�W�̒������v�Z�i���N��Ԕ䗦�j�i���N���/�ő匒�N���)
		float healthRate = enemy->GetHealth() / (float)enemy->GetMaxHealth();

		//�Q�[�W�`��
		gauge->Render(dc,
			screenPosition.x - gaugeWidth / 2,					//�`��̍����W�ʒu�i�G�̂��ʒu - �Q�[�W���̔����j
			screenPosition.y - gaugeHeight,						//�`��̏���W�ʒu�i�G�̂��ʒu- �Q�[�W�����j
			gaugeWidth * healthRate,							//Sprite�̕`�敝�i�Q�[�W�̕� * ���N��Ԕ䗦�j
			gaugeHeight,										//Sprite�̕`�捂��(�Q�[�W�̍����j
			0, 0,												//Texture�̐؂���ʒu�w�C�x
			static_cast<float>(gauge->GetTextureWidth()),		// Texture�̐؂��蕝
			static_cast<float>(gauge->GetTextureHeight()),		// Texture�̐؂��荂��
			0.0f,												// Sprite�̊p�x
			0.0f, 1.0f, 0.5f, 1.0f								// Sprite�̐F(r,g,b,a)
		);

		//gauge->Render(dc,
		//	640, 360,
		//	200, 50,
		//	0, 0,												//Texture�̐؂���ʒu�w�C�x
		//	static_cast<float>(gauge->GetTextureWidth()),		// Texture�̐؂��蕝
		//	static_cast<float>(gauge->GetTextureHeight()),		// Texture�̐؂��荂��
		//	0.0f,												// Sprite�̊p�x
		//	0.0f, 1.0f, 0.5f, 1.0f								// Sprite�̐F(r,g,b,a)
		//);
	}
}
