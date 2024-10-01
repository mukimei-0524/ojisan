#include "Character.h"
#include "Stage.h"

//�s��X�V����
void Character::UpdateTransform()
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

//�_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime)
{
	//�_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage == 0)return false;

	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0)return false;

	//���G���Ԓ��̓_���[�W��^���Ȃ�
	if (invincibleTimer > 0.0f) return false;

	//���G���Ԑݒ�
	invincibleTimer = invincibleTime;

	//�_���[�W����
	health-=damage;

	//���S�ʒm
	if (health <= 0)
	{
		OnDead();
	}
	//�_���[�W�ʒm
	else
	{
		OnDamaged();
	}
	//���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//���͂ɗ͂�������
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

//���G���ԍX�V
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//�ړ�����
//void Character::Move(float elapsedTime, float vx, float vz, float speed)
void Character::Move(float vx, float vz,float speed)
{
	//speed *= elapsedTime;
	//position.x += vx * speed;
	//position.z += vz * speed;

	//�ړ������x�N�g����ݒ�
	moveVecX = vx;
	moveVecZ = vz;

	//�ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

//���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float progLength = sqrtf(vx * vx + vz * vz);
	if (progLength == 0.0f)
	{
		return;
	}

	//�i�s�x�N�g����P�ʃx�N�g����
	vx /= progLength;
	vz /= progLength;

	//���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (vx * vz) + (vx * vz);

	//���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	//�������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐߂���
	float rot = 1.0 - dot;
	if (rot > speed)
	{
		rot = speed;
	}

	//���E������s�����߂�2�̃x�N�g���̊O�ς��v�Z����
	float cross = frontX * vz - frontZ * vx;

	//2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	//���E������s�����Ƃō��E��]��I������
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}

//�W�����v����
void Character::Jump(float speed)
{
	//������̗͂�ݒ�
	velocity.y = speed;
}

//���͏����X�V
void Character::UpdateVelocity(float elapsedTime)
{
	//�o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	//�������͍X�V����
	UpdateVerticalVelocity(elapsedFrame);

	//�����ړ��X�V����
	UpdateVerticalMove(elapsedTime);

	//�������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	//�����ړ��X�V����
	UpdateHorizontalMove(elapsedTime);
}

//�������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	//�d�͏���
	velocity.y += gravity * elapsedFrame;
}

//�����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
	////�ړ�����
	//position.y += velocity.y * elapsedTime;
	//
	////�n�ʔ���
	//if (position.y < 0.0f)
	//{
	//	position.y = 0.0f;
	//	velocity.y = 0.0f;
	//
	//	//���n����
	//	if (!isGround)
	//	{
	//		OnLanding();
	//	}
	//	isGround = true;
	//}
	//else
	//{
	//	isGround = false;
	//}

	//���������̈ړ���
	float my = velocity.y * elapsedTime;

	slopeRate = 0.0f;

	//������
	if (my < 0.0f)
	{
		//���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
		//���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x,position.y + my,position.z };

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			//�n�ʂɐڂ��Ă���
			position.y = hit.position.y;

			//�X�Η��̌v�Z
			



			//���n����
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			//�󒆂ɕ����Ă�
			position.y += my;
			isGround = false;
		}
	}
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

//�������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	//XZ���ʂ̑��͂���������
	float length = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (length > 0.0f)
	{
		//���C��
		float friction = this->friction * elapsedFrame;

		//�󒆂ɂ���Ƃ��͖��C�͂����炷
		if (!isGround)friction *= airControl;

		//���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			//�P�ʃx�N�g����
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;

			//float ratio = 1.0f - friction / length;
			//velocity.x *= ratio;
			//velocity.z *= ratio;
		}
		//�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ���ʂ̑��͂���������
	if (length <= maxMoveSpeed)
	{
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength >  0.0f)
		{
			//�}�ɂ��Ă݂�
			//������
			float accelration = this->acceleration * elapsedFrame;
			
			//�󒆂ɂ���Ƃ��͖��C�͂����炷
			if (!isGround)accelration *= airControl;

			//�ړ��x�N�g���ɂ���������
			velocity.x += moveVecX * accelration;
			velocity.z += moveVecZ * accelration;

			//�ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;

			}

			//�����ŃK�^�K�^���Ȃ��悤�ɂ���



		}
	}
	//�ړ��x�N�g�������Z�b�g
	moveVecX = 0.0f;
	moveVecZ = 0.0f;

}

//�����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
	//�ړ�����
	position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;

#if 0
	//�ǂ���ړ�
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		//�����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { position.x + stepOffset,position.y,position.z + stepOffset };
		DirectX::XMFLOAT3 end = { position.x + mx,position.y,position.z + mz };

		//���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			//�ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			//�ǂ̖@��
			DirectX::XMVECTOR Normal = DirectX::XMVector3Normalize(Vec);

			//���˃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot();

			//�␳�ʒu�̌v�Z

		}
		else
		{
			//�ړ�
			position.x += mx;
			position.z += mz;
		}
	}
#endif // 0
}
