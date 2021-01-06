#pragma once
#include "IComponent.h"

namespace Framework
{
	struct IKChain
	{
		IKChain() : index(0), enableAxisLimit(false), limitMax(0),
			limitMin(0), prevAngle(0), rot(0,0,0,1) {}
		uint index;   // transform or node index
		bool enableAxisLimit;
		Vector3	limitMax;
		Vector3	limitMin;
		Vector3	prevAngle;
		Quaternion rot;
		int dof = 1;
	};

	struct IKProblem
	{
		IKChain& AddIKChain() { return ikChains.emplace_back(IKChain()); }
		uint  ik_index;
		uint  edge_index;
		uint  iterateCount;
		float limitAngle;
		bool  enable;
		bool  baseAnimEnable;
		std::vector<IKChain> ikChains;
	};

	class IKSolver : public IComponent
	{
	private:
		enum class SolveAxis : uint {X = 0, Y = 1, Z = 2};
	public:
		IKSolver(class Context* context);
		virtual ~IKSolver();

		void Init();
		virtual bool Update() override;
		virtual void Clear() override;

	protected:
		IKProblem& AddIKPloblem() { return _ikPloblems.emplace_back(IKProblem()); }
		Vector3 GetRotateAxis(const SolveAxis& axis);
		Vector3 GetRotatePlan(const SolveAxis& axis);

		void Solve_Jacobian(IKProblem& ikProblem);
		void Solve_Jacobian_Plane(IKChain& chain, class Matrix_V& J, const Vector3& E, const Vector3& A, const SolveAxis& axis_type);
		void Solve_Jacobian_Sphere(IKChain& chain, class Matrix_V& J, const Vector3& E, const Vector3& A);

		void Solve(IKProblem& ikProblem);
		void SolvePlane(IKChain& chain, const float theta, const Vector3& ikVec, const Vector3& targetVec, const SolveAxis& axis);
		void SolveSphere(IKChain& chain, const float theta, const Vector3& ikVec, const Vector3& targetVec);

	protected:
		std::vector<IKProblem> _ikPloblems;
		std::shared_ptr<class Skeleton> _skeleton;
		class Transform* _transform;

		std::vector<class Transform*> _transform_array;
		bool _use_ik = true;


	protected:  // constant
		float _delta = 0.01f;
		class Matrix_V _identity;
	};
}