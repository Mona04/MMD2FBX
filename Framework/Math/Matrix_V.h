#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include <vector>

// column ¿ì¼± matrix
class Matrix_V final
{
private:
	static float null_data;
	static std::vector<float> null_vector;

public:
	Matrix_V();
	Matrix_V(int n_row, int n_col);
	~Matrix_V() = default;

public:
	Matrix ToMatrix() const;
	void SetIdentity();

	Matrix_V Transpose() const;
	Matrix_V Inverse33() const;
	float Det() const;
	
	void Resize(int n_row, int n_col);
	void AddColumn(const std::vector<float>& column);
	void AddRow(const std::vector<float>& row);

public:
	std::vector<float>& operator[](int index);
	Matrix_V operator-(const Matrix_V& rhs) const;
	Matrix_V operator*(float rhs) const;
	Matrix_V& operator*=(float rhs);
	const Matrix_V operator*(const Matrix_V& rhs) const;
	const std::vector<float> operator*(const std::vector<float>& rhs) const;

private:
	std::vector<std::vector<float>> data; // row -> col
	int n_row;
	int n_col;
};