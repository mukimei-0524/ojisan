#include <imgui.h>
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "Player.h"
#include "camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

//�R���X�g���N�^
Player::Player()
{
	model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	hitEffect = new Effect("Data/Effect/Hit.efk");

	//���f�����傫���׃X�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;
}

//�f�X�g���N�^
Player::~Player()
{
	delete model;
	delete hitEffect;
}

//�X�V����
void Player::Update(float elapsedTime)
{
	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���s��X�V
	model->UpdateTransform(transform);

	////�i�s�x�N�g���擾
	//DirectX::XMFLOAT3 moveVec = GetMoveVec();

	////�ړ�����
	//float moveSpeed = this->moveSpeed * elapsedTime;
	//position.x += moveVec.x * moveSpeed;
	//position.z += moveVec.z * moveSpeed;

	InputMove(elapsedTime);

	////���͏����擾
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//float ax = gamePad.GetAxisLX();
	//float ay = gamePad.GetAxisLY();

	////�ړ�����
	//float moveSpeed = 5.0f * elapsedTime;
	//{
	//	//���X�e�B�b�N�̓��͏������Ƃ�XZ���ʂւ̈ړ�����
	//	position.x += ax * moveSpeed;
	//	position.z += ay * moveSpeed;
	//}

	////��]����
	//float rotateSpeed = DirectX::XMConvertToRadians(360) * elapsedTime;
	//if (gamePad.GetButton() & GamePad::BTN_A)
	//{
	//	//X����]����
	//	DirectX::CXMMATRIX rotationX = DirectX::XMMatrixRotationX(360);
	//}
	//if (gamePad.GetButton() & GamePad::BTN_B)
	//{
	//	//Y����]����
	//	DirectX::CXMMATRIX rotationY = DirectX::XMMatrixRotationY(360);
	//}
	//if (gamePad.GetButton() & GamePad::BTN_X)
	//{
	//	//Z����]����
	//	DirectX::CXMMATRIX rotationZ = DirectX::XMMatrixRotationZ(360);
	//}

	//�W�����v���͏���
	InputJump();

	//���͏����X�V
	UpdateVelocity(elapsedTime);

	//�e�ۓ��͏���
	InputProjectile();
	
	//�e�ۍX�V����
	projectileManager.Update(elapsedTime);

	//�v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̏Փˏ���
	CollistionProjectilesVsEnemies();

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���s��X�V
	model->UpdateTransform(transform);
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
	//using namespace DirectX;
	//auto vecRight = DirectX::XMLoadFloat3(&cameraRight);
	//auto vecForward = DirectX::XMLoadFloat3(&cameraFront);
	//vecRight = DirectX::XMVector3Normalize(vecRight);

	DirectX::XMFLOAT3 vec;
	//DirectX::XMStoreFloat3(&vec, vecRight * ax + vecForward * ay);
	//return vec;

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	//�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	//�i�s�x�N�g�����v�Z����
	vec.x = cameraRightX * ax + cameraFrontX * ay;
	vec.z = cameraRightZ * ax + cameraFrontZ * ay;
	//Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

//�v���C���[�ƃG�l�~�[�̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSphere(
			position,
			radius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			outPosition
		))
		{
			//�G�̐^��t�߂ɓ���������
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 nomal;
			DirectX::XMStoreFloat3(&nomal, N);

			//�ォ�番�}����ꍇ�͏��W�����v
			if (nomal.y > 0.8f)
			{
				//���W�����v����
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				//�����o����̈ʒu�ݒ�
				enemy->SetPosition(outPosition);
			}
		}
	}
}

//�e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0;
		dir.z = cosf(angle.y);
		
		//���ˈʒu(�v���C���[�̍�������)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//����
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}
	// �ǔ��e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0;
		dir.z = cosf(angle.y);

		//���ˈʒu(�v���C���[�̍�������)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//�^�[�Q�b�g(�f�t�H���g�ł̓v���C���[�̑O��)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//��ԋ߂��̓G���^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			//�G�Ƃ̋�������
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y = enemy->GetHeight() * 0.5f;
			}
		}

		ProjectileHoming* projectileH = new ProjectileHoming(&projectileManager);
		projectileH->Launch(dir, pos, target);
	}
}

//�W�����v���͏���
void Player::InputJump()
{
	//�{�^�����͂ŃW�����v(�W�����v�񐔐����t��)
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		if (jumpCount < jumpLimit)
		{
			//�W�����v
			jumpCount++;
			Jump(jumpSpeed);
		}
	}
}

//�e�ۂƓG�̏Փˏ���
void Player::CollistionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//�Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//�_���[�W��^����
				//enemy->ApplyDamage(1, 0.5f);
				if (enemy->ApplyDamage(1, 0.5f))
				{
					//������΂�
					{
						DirectX::XMFLOAT3 impulse;	//impulse�F�Ռ�

						// �ǂꂮ�炢�̗͂Ő�����Ԃ�
						float power = 10.0f;

						// �G�̈ʒu - �e�̈ʒu�A���ꂼ��� xz ����
						float impX = enemy->GetPosition().x - projectile->GetPosition().x;
						float impZ = enemy->GetPosition().z - projectile->GetPosition().z;


						// ���K������������΂������x�N�g��
						float impLength = sqrtf(impX * impX + impZ * impZ);
						float vx = impX / impLength;
						float vz = impZ / impLength;

						// ������΂������x�N�g���ɗ͂�������i�X�P�[�����O)
						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

#if 0
						//���Ɖ�
						{
							const DirectX::XMFLOAT3& e = enemy->GetPosition();
							const DirectX::XMFLOAT3& p = projectile->GetPosition();
							float vx = e.x - p.x;
							float vz = e.z - p.z;
							float lengthXZ = sqrtf(vx * vx + vz * vz);
							vx /= lengthXZ;
							vz /= lengthXZ;
						}
#endif
						enemy->AddImpulse(impulse);
					}

					//�q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}
					//�e�۔j��
					projectile->Destroy();
				}
			}
		}

	}
}


//�ړ����͏���
void Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//�ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

//���n�����Ƃ��ɌĂ΂��
void Player::OnLanding()
{
	jumpCount = 0;
}

//�`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	//�e�ە`�揈��
	projectileManager.Render(dc, shader);
}

//�f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//�Փ˔���p�̃f�o�b�O����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

	//�Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	//�e�ۃf�o�b�O�v���~�e�B�u�`��
	debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//�ʒu
			ImGui::InputFloat3("Position", &position.x);
			ImGui::InputFloat3("Velocity", &velocity.x);
			//��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//�X�P�[��
			ImGui::InputFloat3("Scale", &scale.x);
		}
		
	}
	ImGui::End();
}


