#include "Collision.h"

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, 
								float radiusA,
								const DirectX::XMFLOAT3& positionB,
								float radiusB, 
								DirectX::XMFLOAT3& outPositionB)
{
	//A→Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}

	//AがBを押し出す
	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, 
											float radiusA, 
											float heightA, 
											const DirectX::XMFLOAT3& positionB, 
											float radiusB,
											float heightB, 
											DirectX::XMFLOAT3& outpositionB)
{
	//Aの足元がBの頭より上なら当たってない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}
	//Aの頭がBの足元より下なら当たってない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}

	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrt(vx * vx + vz * vz);

	if (distXZ > range)
	{
		return false;
	}

	//AがBを押し出す
	vx /= distXZ;
	vz /= distXZ;

	outpositionB.x = positionA.x + (vx * range);
	outpositionB.y = positionB.y;
	outpositionB.z = positionA.z + (vz * range);

	return true;
}

//球と円柱の交差判定
bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition,
											float sphereRadius,
											const DirectX::XMFLOAT3& cylinderPosition,
											float cylinderRadius,
											float cylinderHeight,
											DirectX::XMFLOAT3& outCylinderPosition)
{
#if 1
	{
		//球と円柱の中心の距離を計算
		DirectX::XMVECTOR SphereCenter = DirectX::XMLoadFloat3(&spherePosition);
		DirectX::XMVECTOR CylinderCenter = DirectX::XMLoadFloat3(&cylinderPosition);
		float distanceY = fabs(spherePosition.y - cylinderPosition.y);

		//球と円柱の半径の和
		float sumRadius = sphereRadius * cylinderRadius;

		//球と円柱の中心の距離が半径の和より大きい場合は交差していない
		if (distanceY > cylinderHeight / 2 + sphereRadius) return false;

		//XZ平面上での球と円柱の距離を計算
		float distanceXZ = sqrtf((spherePosition.x - cylinderPosition.x) * (spherePosition.x - cylinderPosition.x) +
			(spherePosition.z - cylinderPosition.z) * (spherePosition.z - cylinderPosition.z));

		//XZ平面上での球と円柱の半径の和
		float sumRadiusXZ = sphereRadius + cylinderRadius;

		//XZ平面上での球と円柱の距離が半径の和より大きい場合は交差していない
		if (distanceXZ > sumRadiusXZ) return false;

		//球が円柱の高さ内にあり、かつXZ平面上で交差している場合は交差している
		//この場合球が円柱を押し出す必要がある
		float penetrationDepth = sumRadiusXZ - distanceXZ;
		float penetrationRatio = sumRadiusXZ / distanceXZ;

		//球の中心と円柱の中心のベクトル
		DirectX::XMVECTOR centerVector = DirectX::XMVectorSubtract(SphereCenter, CylinderCenter);
		DirectX::XMVECTOR normalizeCenterVector = DirectX::XMVector3Normalize(centerVector);

		//球が押し出される距離
		DirectX::XMVECTOR pushOut = DirectX::XMVectorScale(normalizeCenterVector, penetrationDepth);
		DirectX::XMVECTOR newCylinderPosition = DirectX::XMVectorAdd(CylinderCenter, pushOut);
		
		//新しい円柱の位置をCyrinderPositionに保存
		DirectX::XMStoreFloat3(&outCylinderPosition, newCylinderPosition);
	}
#endif // 1

	//授業回答
#if 0
	{
		//高さチェック
		if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;
		if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight) return false;

		//XZ平面での範囲チェック
		float vx = cylinderPosition.x - spherePosition.x;
		float vz = cylinderPosition.z - spherePosition.z;
		float range = sphereRadius + cylinderRadius;
		float distXZ = sqrtf(vx * vx + vz * vz);
		if (distXZ > range)return false;

		//球が円柱を押し出す
		vx /= distXZ;
		vz /= distXZ;
		outCylinderPosition.x = spherePosition.x + (vx * range);
		outCylinderPosition.y = spherePosition.y;
		outCylinderPosition.z = spherePosition.z + (vz * range);
	}

#endif	//0
	return true;
}

//レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start, 
	const DirectX::XMFLOAT3& end, 
	const Model* model, 
	HitResult& result)
{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(WorldRayVec);

	//ワールド空間のレイの長さ
	DirectX::XMStoreFloat(&result.distance, worldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		//メッシュノード取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		//レイをワールド空間からローカル空間へ変換	Inverse:逆、反対
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

		//レイの長さ
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		//三角形(面)との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT>indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				//三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				//三角形の三辺ベクトルを算出
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

				//三角形の法線ベクトル(外積)を算出
				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

				//内積の結果がプラスならば裏向き
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);		//d2
				float dot;
				DirectX::XMStoreFloat(&dot, Dot);
				if (dot >= 0.0f)continue;

				//レイと平面の交点を算出
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR D1 = DirectX::XMVector3Dot(SA, N);		//d1
				float d1;
				DirectX::XMStoreFloat(&d1, D1);
				float x = d1 / dot;
				if (x<.0f || x>neart) continue;		//交点までの距離が今までに計算した最近距離より
													//大きいときはスキップ
				//S+V*x
				DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));


				//交点が三角形の内側にあるか判定
				//1つ目
				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0.0f)continue;

				//2つ目
				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0.0f)continue;

				//3つ目
				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0.0f)continue;

				//最近距離を更新
				neart = x;

				//交点と法線を更新
				HitPosition = P;
				HitNormal = N;
				materialIndex = subset.materialIndex;
			}
		}
		if (materialIndex >= 0)
		{
			//ローカル空間からワールド空間へ変換
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition,
																				WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			//ヒット情報保存
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);


				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
}

