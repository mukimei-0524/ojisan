#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

//�v���C���[
class Player :public Character
{
public:
	Player();
	~Player() override;

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//�W�����v���͏���
	void InputJump();

	//�e�ۂƓG�̏Փˏ���
	void CollistionProjectilesVsEnemies();

protected:
	//���n�����Ƃ��ɌĂ΂��
	void OnLanding() override;

private:
	Model* model = nullptr;
	Effect* hitEffect = nullptr;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float moveSpeed = 5.0f;

	float jumpSpeed = 20.0f;
	
	int jumpCount = 0;
	int jumpLimit = 2;

	ProjectileManager projectileManager;

	//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;


	//���͏���
	void InputMove(float elapsedTime);


	//�v���C���[�ƃG�l�~�[�̏Փˏ���
	void CollisionPlayerVsEnemies();

	//�e�ۓ��͏���
	void InputProjectile();
};

//elapsedTime:�o�ߎ���