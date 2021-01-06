#include "Framework.h"
#include "Matrix.h"
#include "Math/Math.h"
#include "Matrix_V.h"

// DirectX 에서 넣으면 transpose 되어서 들어간다. 나는 DirectX 내에선 column 우선, Hlsl 은 row 우선으로 할 생각이다.

float Matrix_V::null_data;
std::vector<float> Matrix_V::null_vector;

Matrix_V::Matrix_V() : n_row(0), n_col(0)
{
}

Matrix_V::Matrix_V(int n_row, int n_col) : n_row(n_row), n_col(n_col)
{
	Resize(n_row, n_col);
}

Matrix Matrix_V::ToMatrix() const
{
	if (n_row == 3 && n_col == 3)
	{
		return Matrix(
			data[0][0], data[0][1], data[0][2], 0,
			data[1][0], data[1][1], data[1][2], 0,
			data[2][0], data[2][1], data[2][2], 0,
			         0,          0,          0, 1
		);
	}
	if (n_row == 4 || n_col == 4)	
		return Matrix(
			data[0][0], data[0][1], data[0][2], data[0][3],
			data[1][0], data[1][1], data[1][2], data[1][3],
			data[2][0], data[2][1], data[2][2], data[2][3],
			data[3][0], data[3][1], data[3][2], data[3][3]
		);

	return Matrix::identity;
}

void Matrix_V::SetIdentity()
{
	for (int i = 0; i < n_row; i++)
	{
		if (i >= n_col)
			break;
		data[i][i] = 1;
	}
}

Matrix_V Matrix_V::Transpose() const
{
	Matrix_V result(n_col, n_row);
	for (int row = 0; row < n_row; row++)
		for (int col = 0; col < n_col; col++)		
			result.data[col][row] = data[row][col];	

	return result;
}

Matrix_V Matrix_V::Inverse33() const
{
	if (n_row != 3 || n_col != 3)
		return Matrix_V();

	auto& r = data;

	float det = 1 / (
		      r[0][0] * (r[1][1] * r[2][2] - r[1][2] * r[2][1])
			+ r[0][1] * (r[1][2] * r[2][0] - r[1][0] * r[2][2])
			+ r[0][2] * (r[1][0] * r[2][1] - r[1][1] * r[2][0])
			);

	Matrix_V result(3, 3);

	result[0][0] = (r[1][1] * r[2][2] - r[2][1] * r[1][2]) * det;
	result[1][0] = (r[2][0] * r[1][2] - r[1][0] * r[2][2]) * det;
	result[2][0] = (r[1][0] * r[2][1] - r[2][0] * r[1][1]) * det;
	result[0][1] = (r[2][1] * r[0][2] - r[0][1] * r[2][2]) * det;
	result[1][1] = (r[0][0] * r[2][2] - r[2][0] * r[0][2]) * det;
	result[2][1] = (r[2][0] * r[0][1] - r[0][0] * r[2][1]) * det;
	result[0][2] = (r[0][1] * r[1][2] - r[1][1] * r[0][2]) * det;
	result[1][2] = (r[1][0] * r[0][2] - r[0][0] * r[1][2]) * det;
	result[2][2] = (r[0][0] * r[1][1] - r[1][0] * r[0][1]) * det;
	
	return result;
}

float Matrix_V::Det() const
{
	auto& r = data;

	return  1 / (
		r[0][0] * (r[1][1] * r[2][2] - r[1][2] * r[2][1])
		+ r[0][1] * (r[1][2] * r[2][0] - r[1][0] * r[2][2])
		+ r[0][2] * (r[1][0] * r[2][1] - r[1][1] * r[2][0])
	);
}

void Matrix_V::Resize(int row, int col)
{
	n_row = row;
	n_col = col;

	data.resize(n_row);
	for (auto& row : data)
		row.resize(n_col, 0);
}

void Matrix_V::AddColumn(const std::vector<float>& column)
{
	if (n_row != column.size())
		Resize(column.size(), 0);

	n_col += 1;
	for (int row = 0; row < n_row; row++)
		data[row].push_back(column[row]);
}

void Matrix_V::AddRow(const std::vector<float>& row)
{
	if (n_col != row.size())
		Resize(0, row.size());

	n_col += 1;
	for (int col = 0; col < n_col; col++)
		data.push_back(row);
}

std::vector<float>& Matrix_V::operator[](int index)
{
	if (data.size() <= index)
		return null_vector;
	return data[index];
}

Matrix_V Matrix_V::operator-(const Matrix_V& rhs) const
{
	if (n_row != rhs.n_row || n_col != rhs.n_col)
		return Matrix_V();

	Matrix_V result(n_row, n_col);
	for (int row = 0; row < n_row; row++)
		for (int col = 0; col < n_col; col++)
			result.data[row][col] = data[row][col] - rhs.data[row][col];

	return result;

}

Matrix_V Matrix_V::operator*(float rhs) const
{
	Matrix_V result(n_row, n_col);
	for (int row = 0; row < n_row; row++)
		for (int col = 0; col < n_col; col++)
			result.data[row][col] = data[row][col] * rhs;

	return result;
}

Matrix_V& Matrix_V::operator*=(float rhs)
{
	for (int row = 0; row < n_row; row++)
		for (int col = 0; col < n_col; col++)
			data[row][col] = data[row][col] * rhs;

	return *this;
}

const Matrix_V Matrix_V::operator*(const Matrix_V& rhs) const
{
	if (n_col != rhs.n_row)
		return Matrix_V();

	Matrix_V result(n_row, rhs.n_col);
	for (int row = 0; row < n_row; row++)
	{
		for (int col = 0; col < rhs.n_col; col++)
		{
			for (int k = 0; k < n_col; k++)
			{
				result.data[row][col] += data[row][k] * rhs.data[k][col];
			}
		}
	}
	return result;
}

const std::vector<float> Matrix_V::operator*(const std::vector<float>& rhs) const
{
	if (n_col != rhs.size())
		return {};

	std::vector<float> result(n_row, 0);
	for (int row = 0; row < n_row; row++)
	{
		for (int k = 0; k < n_col; k++)
		{
			result[row] += data[row][k] * rhs[k];
		}		
	}
	return result;
}
