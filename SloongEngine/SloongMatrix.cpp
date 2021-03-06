#include "stdafx.h"
#include "SloongMatrix.h"
#include "SloongVector.h"
#include "SloongMath2.h"
using namespace SoaringLoong::Math::Vector;


int SoaringLoong::Math::Matrix::CMatrix3x2::Mat_Init_3X2(CMatrix3x2* ma, float m00, float m01, float m10, float m11, float m20, float m21)
{
	ma->M[0][0] = m00; ma->M[0][1] = m01;
	ma->M[1][0] = m10; ma->M[1][1] = m11;
	ma->M[2][0] = m20; ma->M[2][1] = m21;

	// return success
	return(1);
}


int SoaringLoong::Math::Matrix::CMatrix3x3::Mat_Mul_3X3(CMatrix3x3* ma, CMatrix3x3* mb, CMatrix3x3* mprod)
{
	// this function multiplies two matrices together and 
	// and stores the result

	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			// compute dot product from row of ma 
			// and column of mb

			float sum = 0; // used to hold result

			for (int index = 0; index < 3; index++)
			{
				// add in next product pair
				sum += (ma->M[row][index] * mb->M[index][col]);
			} // end for index

			// insert resulting row,col element
			mprod->M[row][col] = sum;

		} // end for col

	} // end for row

	return(1);
}

void SoaringLoong::Math::Matrix::CMatrix3x3::Mat_Add_3X3(CMatrix3x3* ma, CMatrix3x3* mb, CMatrix3x3* msum)
{
	// this function adds two 3x3 matrices together and 
	// and stores the result

	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			// insert resulting row,col element
			msum->M[row][col] = ma->M[row][col] + mb->M[row][col];
		} // end for col

	} // end for row

}

void SoaringLoong::Math::Matrix::CMatrix3x3::Mat_Mul_VECTOR3D_3X3(VECTOR3D_PTR va, CMatrix3x3* mb, VECTOR3D_PTR vprod)
{
	// this function multiplies a VECTOR3D against a 
	// 3x3 matrix - ma*mb and stores the result in mprod

	for (int col = 0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row = 0; row < 3; row++)
		{
			// add in next product pair
			sum += (va->M[row] * mb->M[row][col]);
		} // end for index

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col

}

int SoaringLoong::Math::Matrix::CMatrix3x3::Mat_Inverse_3X3(CMatrix3x3* m, CMatrix3x3* mi)
{
	// this function computes the inverse of a 3x3

	// first compute the determinate to see if there is 
	// an inverse
	float det = m->M00*(m->M11*m->M22 - m->M21*m->M12) -
		m->M01*(m->M10*m->M22 - m->M20*m->M12) +
		m->M02*(m->M10*m->M21 - m->M20*m->M11);

	if (fabs(det) < EPSILON_E5)
		return(0);

	// compute inverse to save divides
	float det_inv = 1.0 / det;

	// compute inverse using m-1 = adjoint(m)/det(m)
	mi->M00 = det_inv*(m->M11*m->M22 - m->M21*m->M12);
	mi->M10 = -det_inv*(m->M10*m->M22 - m->M20*m->M12);
	mi->M20 = det_inv*(m->M10*m->M21 - m->M20*m->M11);

	mi->M01 = -det_inv*(m->M01*m->M22 - m->M21*m->M02);
	mi->M11 = det_inv*(m->M00*m->M22 - m->M20*m->M02);
	mi->M21 = -det_inv*(m->M00*m->M21 - m->M20*m->M01);

	mi->M02 = det_inv*(m->M01*m->M12 - m->M11*m->M02);
	mi->M12 = -det_inv*(m->M00*m->M12 - m->M10*m->M02);
	mi->M22 = det_inv*(m->M00*m->M11 - m->M10*m->M01);

	// return success
	return(1);

}

void SoaringLoong::Math::Matrix::CMatrix3x3::Mat_Init_3X3(CMatrix3x3* ma, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
{
	// this function fills a 3x3 matrix with the sent data in row major form

	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22;

}

void SoaringLoong::Math::Matrix::CMatrix3x3::Print_Mat_3X3(CMatrix3x3* ma, char *name)
{
	// prints out a 3x3 matrix
	Write_Error("\n%s=\n", name);

	for (int r = 0; r < 3; r++, Write_Error("\n"))
	for (int c = 0; c < 3; c++)
		Write_Error("%f ", ma->M[r][c]);

}

float SoaringLoong::Math::Matrix::CMatrix3x3::Mat_Det_3X3(CMatrix3x3* m)
{
	// computes the determinate of a 3x3 matrix using co-factor
	// expansion

	return(m->M00*(m->M11*m->M22 - m->M21*m->M12) -
		m->M01*(m->M10*m->M22 - m->M20*m->M12) +
		m->M02*(m->M10*m->M21 - m->M20*m->M11));

}

int SoaringLoong::Math::Matrix::CMatrix3x3::Solve_3X3_System(CMatrix3x3* A, MATRIX1X3_PTR X, MATRIX1X3_PTR B)
{
	// solves the system AX=B and computes X=A(-1)*B
	// by using cramers rule and determinates

	// step 1: compute determinate of A
	float det_A = Mat_Det_3X3(A);

	// test if det(a) is zero, if so then there is no solution
	if (fabs(det_A) < EPSILON_E5)
		return(0);

	// step 2: create x,y,z numerator matrices by taking A and
	// replacing each column of it with B(transpose) and solve
	MATRIX3X3 work_mat; // working matrix

	// solve for x /////////////////

	// copy A into working matrix
	MAT_COPY_3X3(A, &work_mat);

	// swap out column 0 (x column)
	work_mat.MAT_COLUMN_SWAP_3X3(&work_mat, 0, B);

	// compute determinate of A with B swapped into x column
	float det_ABx = Mat_Det_3X3(&work_mat);

	// now solve for X00
	X->M00 = det_ABx / det_A;

	// solve for y /////////////////

	// copy A into working matrix
	MAT_COPY_3X3(A, &work_mat);

	// swap out column 1 (y column)
	work_mat.MAT_COLUMN_SWAP_3X3(&work_mat, 1, B);

	// compute determinate of A with B swapped into y column
	float det_ABy = Mat_Det_3X3(&work_mat);

	// now solve for X01
	X->M01 = det_ABy / det_A;

	// solve for z /////////////////

	// copy A into working matrix
	MAT_COPY_3X3(A, &work_mat);

	// swap out column 2 (z column)
	work_mat.MAT_COLUMN_SWAP_3X3(&work_mat, 2, B);

	// compute determinate of A with B swapped into z column
	float det_ABz = Mat_Det_3X3(&work_mat);

	// now solve for X02
	X->M02 = det_ABz / det_A;

	// return success
	return(1);
}

void SoaringLoong::Math::Matrix::CMatrix4x4::Initialize(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	// this function fills a 4x4 matrix with the sent data in 
	// row major form
	this->M00 = m00; this->M01 = m01; this->M02 = m02; this->M03 = m03;
	this->M10 = m10; this->M11 = m11; this->M12 = m12; this->M13 = m13;
	this->M20 = m20; this->M21 = m21; this->M22 = m22; this->M23 = m23;
	this->M30 = m30; this->M31 = m31; this->M32 = m32; this->M33 = m33;

}

void SoaringLoong::Math::Matrix::CMatrix4x4::Initialize(const CMatrix4x4& mSrc)
{
	
	this->M00 = mSrc.M00; this->M01 = mSrc.M01; this->M02 = mSrc.M02; this->M03 = mSrc.M03;
	this->M10 = mSrc.M10; this->M11 = mSrc.M11; this->M12 = mSrc.M12; this->M13 = mSrc.M13;
	this->M20 = mSrc.M20; this->M21 = mSrc.M21; this->M22 = mSrc.M22; this->M23 = mSrc.M23;
	this->M30 = mSrc.M30; this->M31 = mSrc.M31; this->M32 = mSrc.M32; this->M33 = mSrc.M33;
}

void SoaringLoong::Math::Matrix::CMatrix4x4::Mat_Add_4X4(CMatrix4x4* ma, CMatrix4x4* mb, CMatrix4x4* msum)
{
	// this function adds two 4x4 matrices together and 
	// and stores the result

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			// insert resulting row,col element
			msum->M[row][col] = ma->M[row][col] + mb->M[row][col];
		} // end for col

	} // end for row

}

void SoaringLoong::Math::Matrix::CMatrix4x4::Multiply(CMatrix4x4* ma, CMatrix4x4* mb)
{
	// this function multiplies two 4x4 matrices together and 
	// and stores the result in mprod
	// note later we will take advantage of the fact that we know
	// that w=1 always, and that the last column of a 4x4 is
	// always 0

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			// compute dot product from row of ma 
			// and column of mb

			float sum = 0; // used to hold result

			for (int index = 0; index < 4; index++)
			{
				// add in next product pair
				sum += (ma->M[row][index] * mb->M[index][col]);
			} // end for index

			// insert resulting row,col element
			this->M[row][col] = sum;

		} // end for col

	} // end for row

}

void SoaringLoong::Math::Matrix::CMatrix4x4::Mat_Mul_1X4_4X4(MATRIX1X4_PTR ma, CMatrix4x4* mb, MATRIX1X4_PTR mprod)
{
	// this function multiplies a 1x4 matrix against a 
	// 4x4 matrix - ma*mb and stores the result
	// no tricks or assumptions here, just a straight multiply

	for (int col = 0; col < 4; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row = 0; row < 4; row++)
		{
			// add in next product pair
			sum += (ma->M[row] * mb->M[row][col]);
		} // end for index

		// insert resulting col element
		mprod->M[col] = sum;

	} // end for col
}

CVector3D SoaringLoong::Math::Matrix::CMatrix4x4::Multiply( const CVector3D& va )
{
	// this function multiplies a VECTOR3D against a 
	// 4x4 matrix - ma*mb and stores the result in mprod
	// the function assumes that the vector refers to a 
	// 4D homogenous vector, thus the function assumes that
	// w=1 to carry out the multiply, also the function
	// does not carry out the last column multiply since
	// we are assuming w=1, there is no point
	CVector3D vTemp;
	for (int col = 0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		int row;
		for (row = 0; row < 3; row++)
		{
			// add in next product pair
			sum += (va.M[row] * this->M[row][col]);
		} // end for index

		// add in last element in column or w*mb[3][col]
		sum += this->M[row][col];

		// insert resulting col element
		vTemp.M[col] = sum;

	} // end for col
	return vTemp;
}

CVector4D SoaringLoong::Math::Matrix::CMatrix4x4::Multiply( const CVector4D& va )
{
	return CVector4D::Multiply(va,*this);
}

void SoaringLoong::Math::Matrix::CMatrix4x3::Mat_Mul_VECTOR4D_4X3(VECTOR4D_PTR va, CMatrix4x3* mb, VECTOR4D_PTR vprod)
{
	// this function multiplies a VECTOR4D against a 
	// 4x3 matrix - ma*mb and stores the result in mprod
	// the function assumes that the last column of
	// mb is [0 0 0 1]t , thus w just gets replicated
	// from the vector [x y z w]

	for (int col = 0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row = 0; row < 4; row++)
		{
			// add in next product pair
			sum += (va->M[row] * mb->M[row][col]);
		} // end for index

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col

	// copy back w element
	vprod->M[3] = va->M[3];

}

int SoaringLoong::Math::Matrix::CMatrix4x4::Mat_Inverse_4X4(CMatrix4x4* m, CMatrix4x4* mi)
{
	// computes the inverse of a 4x4, assumes that the last
	// column is [0 0 0 1]t

	float det = (m->M00 * (m->M11 * m->M22 - m->M12 * m->M21) -
		m->M01 * (m->M10 * m->M22 - m->M12 * m->M20) +
		m->M02 * (m->M10 * m->M21 - m->M11 * m->M20));

	// test determinate to see if it's 0
	if (fabs(det) < EPSILON_E5)
		return(0);

	float det_inv = 1.0f / det;

	mi->M00 = det_inv * (m->M11 * m->M22 - m->M12 * m->M21);
	mi->M01 = -det_inv * (m->M01 * m->M22 - m->M02 * m->M21);
	mi->M02 = det_inv * (m->M01 * m->M12 - m->M02 * m->M11);
	mi->M03 = 0.0f; // always 0

	mi->M10 = -det_inv * (m->M10 * m->M22 - m->M12 * m->M20);
	mi->M11 = det_inv * (m->M00 * m->M22 - m->M02 * m->M20);
	mi->M12 = -det_inv * (m->M00 * m->M12 - m->M02 * m->M10);
	mi->M13 = 0.0f; // always 0

	mi->M20 = det_inv * (m->M10 * m->M21 - m->M11 * m->M20);
	mi->M21 = -det_inv * (m->M00 * m->M21 - m->M01 * m->M20);
	mi->M22 = det_inv * (m->M00 * m->M11 - m->M01 * m->M10);
	mi->M23 = 0.0f; // always 0

	mi->M30 = -(m->M30 * mi->M00 + m->M31 * mi->M10 + m->M32 * mi->M20);
	mi->M31 = -(m->M30 * mi->M01 + m->M31 * mi->M11 + m->M32 * mi->M21);
	mi->M32 = -(m->M30 * mi->M02 + m->M31 * mi->M12 + m->M32 * mi->M22);
	mi->M33 = 1.0f; // always 0

	// return success 
	return(1);

}

void SoaringLoong::Math::Matrix::CMatrix4x4::Print_Mat_4X4(CMatrix4x4* ma, char *name)
{
	// prints out a 4x4 matrix
	Write_Error("\n%s=\n", name);
	for (int r = 0; r < 4; r++, Write_Error("\n"))
	for (int c = 0; c < 4; c++)
		Write_Error("%f ", ma->M[r][c]);

}

void SoaringLoong::Math::Matrix::CMatrix4x4::BuildMoveMatrix(const CVector4D& vMove)
{
	Initialize(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		vMove.x, vMove.y, vMove.z, 1);
}

void SoaringLoong::Math::Matrix::CMatrix4x4::BuildRotateMartix(double angleX, double angleY, double angleZ)
{
	CMatrix4x4 mx, my, mz;
	double sinTemp, cosTemp;

	if (angleX > EPSILON_E5)
	{
		sinTemp = CMathBase::Fast_Sin(angleX);
		cosTemp = CMathBase::Fast_Cos(angleX);

		mx.Initialize(1, 0, 0, 0,
			0, cosTemp, sinTemp, 0,
			0, -sinTemp, cosTemp, 0,
			0, 0, 0, 1);
	}
	if (angleY > EPSILON_E5)
	{
		sinTemp = CMathBase::Fast_Sin(angleY);
		cosTemp = CMathBase::Fast_Cos(angleY);

		my.Initialize(cosTemp, 0, -sinTemp, 0,
			0, 1, 0, 0,
			sinTemp, 0, cosTemp, 0,
			0, 0, 0, 1);
	}
	if (angleZ > EPSILON_E5)
	{
		sinTemp = CMathBase::Fast_Sin(angleZ);
		cosTemp = CMathBase::Fast_Cos(angleZ);

		mz.Initialize(cosTemp, sinTemp, 0, 0,
			-sinTemp, cosTemp, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	// �任������ˣ�������ձ任����
	CMatrix4x4 mTemp;
	mTemp.Multiply(&mx, &my);
	this->Multiply(&mTemp, &mz);
}

SoaringLoong::Math::Matrix::CMatrix4x4::CMatrix4x4()
{
	Zero();
}

SoaringLoong::Math::Matrix::CMatrix4x4::~CMatrix4x4()
{

}

void SoaringLoong::Math::Matrix::CMatrix4x4::Zero()
{
	Initialize(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

void SoaringLoong::Math::Matrix::CMatrix4x4::Copy(const CMatrix4x4& mSrc)
{
	Initialize(mSrc);
}

void SoaringLoong::Math::Matrix::CMatrix4x3::Mat_Mul_VECTOR3D_4X3(VECTOR3D_PTR va, CMatrix4x3* mb, VECTOR3D_PTR vprod)
{
	// this function multiplies a VECTOR3D against a 
	// 4x3 matrix - ma*mb and stores the result in mprod
	// the function assumes that the vector refers to a 
	// 4D homogenous vector, thus the function assumes that
	// w=1 to carry out the multiply, also the function
	// does not carry out the last column multiply since
	// we are assuming w=1, there is no point

	for (int col = 0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		int row;
		for (row = 0; row < 3; row++)
		{
			// add in next product pair
			sum += (va->M[row] * mb->M[row][col]);
		} // end for index

		// add in last element in column or w*mb[3][col]
		sum += mb->M[row][col];

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col
}

void SoaringLoong::Math::Matrix::CMatrix2x2::Mat_Init_2X2(CMatrix2x2* ma, float m00, float m01, float m10, float m11)
{
	// this function fills a 2x2 matrix with the sent data in 
	// row major form
	ma->M00 = m00; ma->M01 = m01;
	ma->M10 = m10; ma->M11 = m11;

}

void SoaringLoong::Math::Matrix::CMatrix2x2::Print_Mat_2X2(CMatrix2x2* ma, char *name)
{
	// prints out a 3x3 matrix
	Write_Error("\n%s=\n", name);

	for (int r = 0; r < 2; r++, Write_Error("\n"))
	for (int c = 0; c < 2; c++)
		Write_Error("%f ", ma->M[r][c]);

}

float SoaringLoong::Math::Matrix::CMatrix2x2::Mat_Det_2X2(CMatrix2x2* m)
{
	return(m->M00*m->M11 - m->M01*m->M10);
}

void SoaringLoong::Math::Matrix::CMatrix2x2::Mat_Add_2X2(CMatrix2x2* ma, CMatrix2x2* mb, CMatrix2x2* msum)
{
	// this function adds two 2x2 matrices together and stores
	// the result in msum
	msum->M00 = ma->M00 + mb->M00;
	msum->M01 = ma->M01 + mb->M01;
	msum->M10 = ma->M10 + mb->M10;
	msum->M11 = ma->M11 + mb->M11;

}

void SoaringLoong::Math::Matrix::CMatrix2x2::Mat_Mul_2X2(CMatrix2x2* ma, CMatrix2x2* mb, CMatrix2x2* mprod)
{
	// this function multiplies two 2x2 matrices together and 
	// and stores the result in mprod
	mprod->M00 = ma->M00*mb->M00 + ma->M01*mb->M10;
	mprod->M01 = ma->M00*mb->M01 + ma->M01*mb->M11;

	mprod->M10 = ma->M10*mb->M00 + ma->M11*mb->M10;
	mprod->M11 = ma->M10*mb->M01 + ma->M11*mb->M11;
}

int SoaringLoong::Math::Matrix::CMatrix2x2::Mat_Inverse_2X2(CMatrix2x2* m, CMatrix2x2* mi)
{
	// this function computes the inverse of a 2x2 matrix
	// and stores the result in mi

	// compute determinate
	float det = (m->M00*m->M11 - m->M01*m->M10);

	// if determinate is 0 then inverse doesn't exist
	if (fabs(det) < EPSILON_E5)
		return(0);

	float det_inv = 1.0 / det;

	// fill in inverse by formula
	mi->M00 = m->M11*det_inv;
	mi->M01 = -m->M01*det_inv;
	mi->M10 = -m->M10*det_inv;
	mi->M11 = m->M00*det_inv;

	// return sucess
	return(1);
}

int SoaringLoong::Math::Matrix::CMatrix2x2::Solve_2X2_System(CMatrix2x2* A, MATRIX1X2_PTR X, MATRIX1X2_PTR B)
{
	// solves the system AX=B and computes X=A(-1)*B
	// by using cramers rule and determinates

	// step 1: compute determinate of A
	float det_A = Mat_Det_2X2(A);

	// test if det(a) is zero, if so then there is no solution
	if (fabs(det_A) < EPSILON_E5)
		return(0);

	// step 2: create x,y numerator matrices by taking A and
	// replacing each column of it with B(transpose) and solve
	MATRIX2X2 work_mat; // working matrix

	// solve for x /////////////////

	// copy A into working matrix
	MAT_COPY_2X2(A, &work_mat);

	// swap out column 0 (x column)
	work_mat.MAT_COLUMN_SWAP_2X2(&work_mat, 0, B);

	// compute determinate of A with B swapped into x column
	float det_ABx = Mat_Det_2X2(&work_mat);

	// now solve for X00
	X->M00 = det_ABx / det_A;

	// solve for y /////////////////

	// copy A into working matrix
	MAT_COPY_2X2(A, &work_mat);

	// swap out column 1 (y column)
	work_mat.MAT_COLUMN_SWAP_2X2(&work_mat, 1, B);

	// compute determinate of A with B swapped into y column
	float det_ABy = Mat_Det_2X2(&work_mat);

	// now solve for X01
	X->M01 = det_ABy / det_A;

	// return success
	return(1);

}
