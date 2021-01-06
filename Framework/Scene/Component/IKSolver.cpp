#include "Framework.h"
#include "IKSolver.h"

#include "Scene/Component/Transform.h"
#include "Scene/Component/Animator.h"
#include "Resource/Animation.h"
#include "Scene/Actor.h"

#include "Resource/Skeletion.h"

using namespace Framework;

IKSolver::IKSolver(class Context* context)
	: IComponent(context)
{
	typecode = TypeCode::IKSolver;
	_identity.Resize(3, 3);
	_identity.SetIdentity();
}

IKSolver::~IKSolver()
{
}

void IKSolver::Init()
{
	Clear();

	auto animator = _actor->GetComponent<Animator>();
	if(animator)
		_use_ik = animator->GetAnimation()->UseIK();

	_transform = _actor->GetComponent<Transform>();
	_transform_array = _transform->Get_Transform_Array();
	_skeleton = _actor->GetComponent<Transform>()->Get_Skeleton();
	if (!_skeleton)
	{
		LOG_WARNING("No available skeleton.")
		return;
	}
	
	for (auto iter = _skeleton->GetBoneMap().begin(); iter != _skeleton->GetBoneMap().end(); iter++)
	{
		const auto& bone = iter->second;
		if (bone.ikTargetBone_index != -1)
		{
			auto& problem = AddIKPloblem();
			problem.edge_index = bone.ikTargetBone_index;
			problem.ik_index = bone.index;
			problem.iterateCount = bone.ikIterationCount;
			problem.limitAngle = bone.ikItertationAngleLimit;
			for (const auto& ikLink : bone.ikLinks)
			{
				auto& ikChain = problem.AddIKChain();
				ikChain.enableAxisLimit = ikLink.enableAxisLimit;
				ikChain.index = ikLink.ikBoneIndex;
				ikChain.limitMax = ikLink.limitMax;
				ikChain.limitMin = ikLink.limitMin;	
			}
		}		
	}
}

bool IKSolver::Update()
{
	for (auto& ikProblem : _ikPloblems)
	{
		Solve_Jacobian(ikProblem);
		//Solve(ikProblem);	
	}
	return true;
}

void IKSolver::Clear()
{
	_ikPloblems.clear(); _ikPloblems.shrink_to_fit();
	_skeleton = nullptr;
	_transform_array.clear(); _transform_array.shrink_to_fit();

}

Vector3 IKSolver::GetRotateAxis(const SolveAxis& axis)
{
	switch (axis)
	{
	case SolveAxis::X: return Vector3(1, 0, 0);
	case SolveAxis::Y: return Vector3(0, 1, 0);
	case SolveAxis::Z: return Vector3(0, 0, 1);
	default:
		return Vector3(1, 0, 0);
	}
}

Vector3 IKSolver::GetRotatePlan(const SolveAxis& axis)
{
	switch (axis)
	{
	case SolveAxis::X: return Vector3(0, 1, 1);
	case SolveAxis::Y: return Vector3(1, 0, 1);
	case SolveAxis::Z: return Vector3(1, 1, 0);
	default:
		return Vector3(1, 1, 1);
	}
}

void IKSolver::Solve_Jacobian(IKProblem& ikProblem)
{
	const auto& ikNode = _transform_array[ikProblem.ik_index];
	const auto& edgeNode = _transform_array[ikProblem.edge_index];

	for (auto& chain : ikProblem.ikChains)
	{
		chain.prevAngle = 0;
		chain.rot = { 0,0,0,1 };
		auto ik_node = _transform->Get_Transform_Array()[chain.index];
	
		ik_node->SetIKRotation(chain.rot);
		ik_node->Update();
	}
	
	float min_length = std::numeric_limits<int>::max();

	for (int i = 0; i < ikProblem.iterateCount * 3 ; i++)
	{
		auto T = ikNode->GetPosition();
		auto E = edgeNode->GetPosition();
		auto V = T - E;
		auto vlength = V.Length();
		if (min_length > vlength)
			min_length = vlength;
		else
			break;
		if (V.Length() < 1.0e-4f)
			break;

		if (i < 15)
			_delta = 0.01f;
		else
			_delta = 0.1f;

		Matrix_V J;

		for (auto& chain : ikProblem.ikChains)
		{
			if (chain.enableAxisLimit)
			{
				// X,Y,Z 한축만 사용하는 경우 따로 뺌
				if ((chain.limitMin.x != 0 || chain.limitMax.x != 0) &&
					(chain.limitMin.y == 0 || chain.limitMax.y == 0) &&
					(chain.limitMin.z == 0 || chain.limitMax.z == 0)
					)
				{
					Solve_Jacobian_Plane(chain, J, T, E, SolveAxis::X);
					continue;
				}
				else if ((chain.limitMin.y != 0 || chain.limitMax.y != 0) &&
					(chain.limitMin.x == 0 || chain.limitMax.x == 0) &&
					(chain.limitMin.z == 0 || chain.limitMax.z == 0)
					)
				{
					Solve_Jacobian_Plane(chain, J, T, E, SolveAxis::Y);
					continue;
				}
				else if ((chain.limitMin.z != 0 || chain.limitMax.z != 0) &&
					(chain.limitMin.x == 0 || chain.limitMax.x == 0) &&
					(chain.limitMin.y == 0 || chain.limitMax.y == 0)
					)
				{
					Solve_Jacobian_Plane(chain, J, T, E, SolveAxis::Z);
					continue;
				}
			}
			Solve_Jacobian_Sphere(chain, J, T, E);
		}
	}
}

void IKSolver::Solve_Jacobian_Plane(IKChain& chain, class Matrix_V& J, const Vector3& T, const Vector3& E, const SolveAxis& axis_type)
{
	chain.dof = 1;
	auto chainNode = _transform_array[chain.index];
	Vector3 chain_pos = chainNode->GetPosition();
	Vector3 local_axis = GetRotateAxis(axis_type);
	Vector3 axis = chainNode->GetRotation() * local_axis;

	auto cross = Vector3::Cross(axis, (E - chain_pos));
	float theta = Vector3::Dot(cross, (T - E));

	uint axis_index = static_cast<uint>(axis_type);
	float limitMin = (&chain.limitMin.x)[axis_index];
	float limitMax = (&chain.limitMax.x)[axis_index];
	theta = Math::Clamp(theta, limitMin, limitMax);

	chain.rot = chain.rot * Quaternion::QuaternionFromAngleAxis(theta * _delta, local_axis);
	chainNode->SetIKRotation(chain.rot);
	chainNode->Update();
}

void IKSolver::Solve_Jacobian_Sphere(IKChain& chain, class Matrix_V& J, const Vector3& T, const Vector3& E)
{
	auto chainNode = _transform_array[chain.index];
	Vector3 chain_pos = chainNode->GetPosition();
	chain.dof = 3;
	for (int i = 0; i < 3; i++)
	{
		Vector3 local_axis = GetRotateAxis(static_cast<SolveAxis>(i));
		Vector3 axis = chainNode->GetRotation() * local_axis;

		auto cross = Vector3::Cross(axis, (E - chain_pos));
		float theta = Vector3::Dot(cross, (T - E));
		chain.rot = chain.rot * Quaternion::QuaternionFromAngleAxis(theta * _delta, local_axis);
	}
	chainNode->SetIKRotation(chain.rot);
	chainNode->Update();
}

void IKSolver::Solve(IKProblem& ikProblem)
{
	const auto& ikNode = _transform_array[ikProblem.ik_index];
	const auto& edgeNode = _transform_array[ikProblem.edge_index];

	// 얘가 점점 작아질텐데, 그게 끝나면 iterate 종료
	float dist_smallest = std::numeric_limits<int>::max();
	float dist_tmp = 0;

	// T 자세랑 가장 가까운 자세로 하기 위해서 필요함 
	for (auto& chain : ikProblem.ikChains)
	{
		chain.prevAngle = 0;
		chain.rot = { 0,0,0,1 };
		auto ik_node = _transform->Get_Transform_Array()[chain.index];
	
		ik_node->SetIKRotation(chain.rot);
		ik_node->Update();
	}
	
	for (int i = 0; i < ikProblem.iterateCount; i++)
	{
		for (auto& chain : ikProblem.ikChains)
		{
			if (_transform_array[chain.index]->GetName() == edgeNode->GetName())
				break;

			Matrix invChain = _transform_array[chain.index]->GetWorldMatrix().Inverse_SRT();
			auto ikpos = ikNode->GetPosition();
			auto edgePos = edgeNode->GetPosition();

			// chain node 가 원점인 공간으로 이동 후 원점과 ikNode, 원점과 TargetNode 의 벡터를 구함 
			Vector3 ikVec = (invChain * ikpos).Normalize();
			Vector3 edgeVec = (invChain * edgePos).Normalize();

			// 위에서 구한 두 벡터같의 각도를 구함
			float theta = std::acos(Math::Clamp(Vector3::Dot(ikVec, edgeVec), -1.f, 1.f));
			theta = Math::Clamp(theta, -ikProblem.limitAngle, ikProblem.limitAngle);			
			// theta 가 작으면 할일이 없음
			if (theta < 1.0e-5f)
				continue;

			float newAngle = 0.0f;
			if (chain.enableAxisLimit)
			{
				// X,Y,Z 한축만 사용하는 경우 따로 뺌
				if ((chain.limitMin.x != 0 || chain.limitMax.x != 0) &&
					(chain.limitMin.y == 0 || chain.limitMax.y == 0) &&
					(chain.limitMin.z == 0 || chain.limitMax.z == 0)
					)
				{
					SolvePlane(chain, theta, ikVec, edgeVec, SolveAxis::X);
					continue;
				}
				else if ((chain.limitMin.y != 0 || chain.limitMax.y != 0) &&
					(chain.limitMin.x == 0 || chain.limitMax.x == 0) &&
					(chain.limitMin.z == 0 || chain.limitMax.z == 0)
					)
				{
					SolvePlane(chain, theta, ikVec, edgeVec, SolveAxis::Y);
					continue;
				}
				else if ((chain.limitMin.z != 0 || chain.limitMax.z != 0) &&
					(chain.limitMin.x == 0 || chain.limitMax.x == 0) &&
					(chain.limitMin.y == 0 || chain.limitMax.y == 0)
					)
				{
					SolvePlane(chain, theta, ikVec, edgeVec, SolveAxis::Z);
					continue;
				}
			}
			SolveSphere(chain, theta, ikVec, edgeVec);
		}
		dist_tmp = (ikNode->GetPosition() - edgeNode->GetPosition()).Length();
		
		if (dist_tmp < dist_smallest)
			dist_smallest = dist_tmp;
		else {
			break;
		}
	}
}

void IKSolver::SolvePlane(IKChain& chain, const float theta, const Vector3& ikVec, const Vector3& edgeVec, const SolveAxis& axis)
{
	auto rotateAxis = GetRotateAxis(axis);
	auto rotatePlane = GetRotatePlan(axis);
	uint axis_index = static_cast<uint>(axis);

	auto rot1 = Quaternion::QuaternionFromAngleAxis(theta, rotateAxis);
	auto targetVec1 = (Matrix::QuaternionToMatrix(rot1) * edgeVec).Normalize();
	auto dot1 = Vector3::Dot(targetVec1, ikVec);

	auto rot2 = Quaternion::QuaternionFromAngleAxis(-theta, rotateAxis);
	auto targetVec2 = (Matrix::QuaternionToMatrix(rot2) * edgeVec).Normalize();
	auto dot2 = Vector3::Dot(targetVec2, ikVec);

	// axis 는 하나만 1 이니까 걍 각도 뽑아내는거
	auto newAngle = Vector3::Dot(chain.prevAngle, rotateAxis);
	// dot 은 교환법칙이 성립하므로 방향을 모르니까 양쪽 검사하는 것
	newAngle += dot1 > dot2 ? theta : -theta;

	float limitMin = (&chain.limitMin.x)[axis_index];
	float limitMax = (&chain.limitMax.x)[axis_index];

	// limit 을 넘을 경우 반대 방향으로 같은 양만큼 돌려줌. 왠진 모르겠는데 이게 제일 부드럽게됨 ㅁㅁ
	if (newAngle < limitMin)
		newAngle = limitMin +theta;
	else if (newAngle > limitMax)
		newAngle = limitMax -theta;
	
	// Angle 저장, 
	(&chain.prevAngle.x)[axis_index] = newAngle;
	chain.rot = Quaternion::QuaternionFromAngleAxis(newAngle, rotateAxis);

	auto ik_node = _transform->Get_Transform_Array()[chain.index];
	ik_node->SetIKRotation(chain.rot);
	ik_node->Update();
}

void IKSolver::SolveSphere(IKChain& chain, const float theta, const Vector3& ikVec, const Vector3& edgeVec)
{
	auto rotateAxis = Vector3::Cross(edgeVec, ikVec).Normalize();
	// cross 는 순서가 있어서 얘는 dot 처럼 두방향 할필요가 없음
	auto rot = chain.rot * Quaternion::QuaternionFromAngleAxis(theta, rotateAxis);
	
	if (chain.enableAxisLimit) // 거의 없긴 한데, 일단 야매로 해놨으니까 에러시 참고
	{
		auto newAngle = (rot).ToEulerAngle(); //* _transform_array[chain.index]->GetAnimRotation()
		newAngle.x = Math::Clamp(newAngle.x, chain.limitMin.x, chain.limitMax.x);
		newAngle.y = Math::Clamp(newAngle.y, chain.limitMin.y, chain.limitMax.y);
		newAngle.z = Math::Clamp(newAngle.z, chain.limitMin.z, chain.limitMax.z);
		rot = newAngle.ToQuaternion();
		chain.prevAngle = newAngle;
	}

	chain.rot = rot;
	auto ik_node = _transform->Get_Transform_Array()[chain.index];

	ik_node->SetIKRotation(chain.rot);
	ik_node->Update();
}
