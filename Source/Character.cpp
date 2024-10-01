#include "Character.h"
#include "Stage.h"

//行列更新処理
void Character::UpdateTransform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

//ダメージを与える
bool Character::ApplyDamage(int damage, float invincibleTime)
{
	//ダメージが0の場合は健康状態を変更する必要がない
	if (damage == 0)return false;

	//死亡している場合は健康状態を変更しない
	if (health <= 0)return false;

	//無敵時間中はダメージを与えない
	if (invincibleTimer > 0.0f) return false;

	//無敵時間設定
	invincibleTimer = invincibleTime;

	//ダメージ処理
	health-=damage;

	//死亡通知
	if (health <= 0)
	{
		OnDead();
	}
	//ダメージ通知
	else
	{
		OnDamaged();
	}
	//健康状態が変更した場合はtrueを返す
	return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//速力に力を加える
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

//無敵時間更新
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//移動処理
//void Character::Move(float elapsedTime, float vx, float vz, float speed)
void Character::Move(float vx, float vz,float speed)
{
	//speed *= elapsedTime;
	//position.x += vx * speed;
	//position.z += vz * speed;

	//移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	//最大速度設定
	maxMoveSpeed = speed;
}

//旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//進行ベクトルがゼロベクトルの場合は処理する必要なし
	float progLength = sqrtf(vx * vx + vz * vz);
	if (progLength == 0.0f)
	{
		return;
	}

	//進行ベクトルを単位ベクトル化
	vx /= progLength;
	vz /= progLength;

	//自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (vx * vz) + (vx * vz);

	//内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	//小さいほど1.0に近づくという性質を利用して回転速度を調節する
	float rot = 1.0 - dot;
	if (rot > speed)
	{
		rot = speed;
	}

	//左右判定を行うために2つのベクトルの外積を計算する
	float cross = frontX * vz - frontZ * vx;

	//2Dの外積値が正の場合か負の場合によって左右判定が行える
	//左右判定を行うことで左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}

//ジャンプ処理
void Character::Jump(float speed)
{
	//上方向の力を設定
	velocity.y = speed;
}

//速力処理更新
void Character::UpdateVelocity(float elapsedTime)
{
	//経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	//垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);

	//垂直移動更新処理
	UpdateVerticalMove(elapsedTime);

	//水平速力更新処理
	UpdateHorizontalVelocity(elapsedFrame);

	//水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
}

//垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	//重力処理
	velocity.y += gravity * elapsedFrame;
}

//垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
	////移動処理
	//position.y += velocity.y * elapsedTime;
	//
	////地面判定
	//if (position.y < 0.0f)
	//{
	//	position.y = 0.0f;
	//	velocity.y = 0.0f;
	//
	//	//着地した
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

	//垂直方向の移動量
	float my = velocity.y * elapsedTime;

	slopeRate = 0.0f;

	//落下中
	if (my < 0.0f)
	{
		//レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x,position.y + my,position.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			//地面に接している
			position.y = hit.position.y;

			//傾斜率の計算
			



			//着地した
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			//空中に浮いてる
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

//水平速力更新処理
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	//XZ平面の速力を減速する
	float length = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (length > 0.0f)
	{
		//摩擦力
		float friction = this->friction * elapsedFrame;

		//空中にいるときは摩擦力を減らす
		if (!isGround)friction *= airControl;

		//摩擦による横方向の減速処理
		if (length > friction)
		{
			//単位ベクトル化
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;

			//float ratio = 1.0f - friction / length;
			//velocity.x *= ratio;
			//velocity.z *= ratio;
		}
		//横方向の速力が摩擦力以下になったので速力を無効化
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ平面の速力を加速する
	if (length <= maxMoveSpeed)
	{
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength >  0.0f)
		{
			//図にしてみる
			//加速力
			float accelration = this->acceleration * elapsedFrame;
			
			//空中にいるときは摩擦力を減らす
			if (!isGround)accelration *= airControl;

			//移動ベクトルによる加速処理
			velocity.x += moveVecX * accelration;
			velocity.z += moveVecZ * accelration;

			//最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;

			}

			//下り坂でガタガタしないようにする



		}
	}
	//移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;

}

//水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
	//移動処理
	position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;

#if 0
	//壁ずり移動
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		//水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { position.x + stepOffset,position.y,position.z + stepOffset };
		DirectX::XMFLOAT3 end = { position.x + mx,position.y,position.z + mz };

		//レイキャストによる壁判定
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			//壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			//壁の法線
			DirectX::XMVECTOR Normal = DirectX::XMVector3Normalize(Vec);

			//入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot();

			//補正位置の計算

		}
		else
		{
			//移動
			position.x += mx;
			position.z += mz;
		}
	}
#endif // 0
}
