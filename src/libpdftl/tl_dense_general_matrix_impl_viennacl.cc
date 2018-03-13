#include "tl_dense_general_matrix_impl_viennacl.h"
#include "tl_dense_general_matrix_impl_eigen.h"
#include "tl_dense_symmetric_matrix_impl_viennacl.h"
#include "tl_dense_vector_impl_viennacl.h"
#include "viennacl/linalg/cg.hpp"
#include "viennacl/linalg/direct_solve.hpp"
#include "viennacl/linalg/lu.hpp"
#include "viennacl/linalg/sum.hpp"
#include "viennacl/matrix.hpp"

#ifdef HAVE_EIGEN3
#include <Eigen/Core>
#include <Eigen/LU>
#endif  // HAVE_EIGEN3

// ---------------------------------------------------------------------------
// constructor & destructor
// ---------------------------------------------------------------------------
TlDenseGeneralMatrix_ImplViennaCL::TlDenseGeneralMatrix_ImplViennaCL(
    const TlMatrixObject::index_type row, const TlMatrixObject::index_type col)
    : matrix_(row, col) {}

TlDenseGeneralMatrix_ImplViennaCL::TlDenseGeneralMatrix_ImplViennaCL(
    const TlDenseGeneralMatrix_ImplViennaCL& rhs) {
  this->matrix_ = rhs.matrix_;
}

TlDenseGeneralMatrix_ImplViennaCL::TlDenseGeneralMatrix_ImplViennaCL(
    const TlDenseSymmetricMatrix_ImplViennaCL& rhs) {
  this->matrix_ = rhs.matrix_;
}

TlDenseGeneralMatrix_ImplViennaCL::TlDenseGeneralMatrix_ImplViennaCL(
    const TlDenseGeneralMatrix_ImplEigen& rhs) {
  viennacl::copy(rhs.matrix_, this->matrix_);
}

TlDenseGeneralMatrix_ImplViennaCL::~TlDenseGeneralMatrix_ImplViennaCL() {}

// ---------------------------------------------------------------------------
// properties
// ---------------------------------------------------------------------------
TlMatrixObject::index_type TlDenseGeneralMatrix_ImplViennaCL::getNumOfRows()
    const {
  return this->matrix_.size1();
}

TlMatrixObject::index_type TlDenseGeneralMatrix_ImplViennaCL::getNumOfCols()
    const {
  return this->matrix_.size2();
}

void TlDenseGeneralMatrix_ImplViennaCL::resize(
    const TlMatrixObject::index_type newRow,
    const TlMatrixObject::index_type newCol) {
  this->matrix_.resize(newRow, newCol, true);
}

double TlDenseGeneralMatrix_ImplViennaCL::get(
    const TlMatrixObject::index_type row,
    const TlMatrixObject::index_type col) const {
  return this->matrix_(row, col);
}

void TlDenseGeneralMatrix_ImplViennaCL::set(
    const TlMatrixObject::index_type row, const TlMatrixObject::index_type col,
    const double value) {
  this->matrix_(row, col) = value;
}

void TlDenseGeneralMatrix_ImplViennaCL::add(
    const TlMatrixObject::index_type row, const TlMatrixObject::index_type col,
    const double value) {
  this->matrix_(row, col) += value;
}

// ---------------------------------------------------------------------------
// operators
// ---------------------------------------------------------------------------
TlDenseGeneralMatrix_ImplViennaCL& TlDenseGeneralMatrix_ImplViennaCL::operator=(
    const TlDenseGeneralMatrix_ImplViennaCL& rhs) {
  if (this != &rhs) {
    this->matrix_ = rhs.matrix_;
  }

  return *this;
}

TlDenseGeneralMatrix_ImplViennaCL& TlDenseGeneralMatrix_ImplViennaCL::
operator+=(const TlDenseGeneralMatrix_ImplViennaCL& rhs) {
  const TlMatrixObject::index_type row1 = this->getNumOfRows();
  const TlMatrixObject::index_type col1 = this->getNumOfCols();
  const TlMatrixObject::index_type row2 = rhs.getNumOfRows();
  const TlMatrixObject::index_type col2 = rhs.getNumOfCols();
  assert(row1 == row2);
  assert(col1 == col2);

  this->matrix_ += rhs.matrix_;

  return *this;
}

TlDenseGeneralMatrix_ImplViennaCL& TlDenseGeneralMatrix_ImplViennaCL::
operator-=(const TlDenseGeneralMatrix_ImplViennaCL& rhs) {
  const TlMatrixObject::index_type row1 = this->getNumOfRows();
  const TlMatrixObject::index_type col1 = this->getNumOfCols();
  const TlMatrixObject::index_type row2 = rhs.getNumOfRows();
  const TlMatrixObject::index_type col2 = rhs.getNumOfCols();
  assert(row1 == row2);
  assert(col1 == col2);

  this->matrix_ -= rhs.matrix_;

  return *this;
}

TlDenseGeneralMatrix_ImplViennaCL& TlDenseGeneralMatrix_ImplViennaCL::
operator*=(const double coef) {
  this->matrix_ *= coef;

  return *this;
}

TlDenseGeneralMatrix_ImplViennaCL& TlDenseGeneralMatrix_ImplViennaCL::
operator/=(const double coef) {
  this->matrix_ *= (1.0 / coef);

  return *this;
}

TlDenseGeneralMatrix_ImplViennaCL& TlDenseGeneralMatrix_ImplViennaCL::
operator*=(const TlDenseGeneralMatrix_ImplViennaCL& rhs) {
  const TlMatrixObject::index_type row1 = this->getNumOfRows();
  const TlMatrixObject::index_type col1 = this->getNumOfCols();
  const TlMatrixObject::index_type row2 = rhs.getNumOfRows();
  const TlMatrixObject::index_type col2 = rhs.getNumOfCols();
  assert(col1 == row2);
  const MatrixDataType tmp = this->matrix_;

  this->resize(row1, col2);
  this->matrix_ = viennacl::linalg::prod(tmp, rhs.matrix_);

  return *this;
}

// -----------------------------------------------------------------------------
// operations
// -----------------------------------------------------------------------------
double TlDenseGeneralMatrix_ImplViennaCL::sum() const {
  const VectorDataType v = viennacl::linalg::row_sum(this->matrix_);
  const double sum = viennacl::linalg::sum(v);

  return sum;
}

double TlDenseGeneralMatrix_ImplViennaCL::getRMS() const {
  const double elements = this->getNumOfRows() * this->getNumOfCols();

  const MatrixDataType mat2 =
      viennacl::linalg::element_prod(this->matrix_, this->matrix_);
  const VectorDataType rows = viennacl::linalg::row_sum(mat2);
  const double sum2 = viennacl::linalg::sum(rows);
  const double rms = std::sqrt(sum2 / elements);

  return rms;
}

double TlDenseGeneralMatrix_ImplViennaCL::getMaxAbsoluteElement(
    TlMatrixObject::index_type* outRow,
    TlMatrixObject::index_type* outCol) const {
  TlMatrixObject::index_type max_row, max_col;
  double answer = 0.0;
  const unsigned int numOfRows = this->getNumOfRows();
  for (unsigned int r = 0; r < numOfRows; ++r) {
    VectorDataType vec_col;
    viennacl::linalg::matrix_row(this->matrix_, r, vec_col);
    const unsigned int col = viennacl::linalg::index_norm_inf(vec_col);
    double value = vec_col[col];
    if (std::fabs(answer) < std::fabs(value)) {
      max_row = r;
      max_col = col;
      answer = value;
    }
  }

  if (outRow != NULL) {
    *outRow = max_row;
  }
  if (outCol != NULL) {
    *outCol = max_col;
  }

  return 0.0;
}

TlDenseGeneralMatrix_ImplViennaCL&
TlDenseGeneralMatrix_ImplViennaCL::dotInPlace(
    const TlDenseGeneralMatrix_ImplViennaCL& rhs) {
  const MatrixDataType tmp =
      viennacl::linalg::element_prod(this->matrix_, rhs.matrix_);
  this->matrix_ = tmp;
  return *this;
}

TlDenseGeneralMatrix_ImplViennaCL TlDenseGeneralMatrix_ImplViennaCL::transpose()
    const {
  TlDenseGeneralMatrix_ImplViennaCL answer;
  answer.matrix_ = viennacl::trans(this->matrix_);

  return answer;
}

TlDenseGeneralMatrix_ImplViennaCL TlDenseGeneralMatrix_ImplViennaCL::inverse()
    const {
  // const TlMatrixObject::index_type dim = this->getNumOfRows();
  // const VectorDataType v = viennacl::scalar_vector<double>(dim, 1.0);
  // MatrixDataType E = viennacl::diag(v);

  TlDenseGeneralMatrix_ImplViennaCL answer(this->getNumOfCols(),
                                           this->getNumOfRows());
  // answer.matrix_ = viennacl::linalg::solve(this->matrix_, E,
  // viennacl::linalg::cg_tag());

  // LU factorization
  // MatrixDataType tmp = this->matrix_;
  // viennacl::linalg::lu_factorize(tmp);
  // viennacl::linalg::lu_substitute(tmp, E);

#ifdef HAVE_EIGEN3
  {
    EigenMatrixDataType eigenMat(this->getNumOfRows(), this->getNumOfCols());
    copy(this->matrix_, eigenMat);
    const EigenMatrixDataType eigenInvMat = eigenMat.inverse();
    answer.resize(eigenInvMat.rows(), eigenInvMat.cols());
    copy(eigenInvMat, answer.matrix_);
  }
#endif  // HAVE_EIGEN3

  return answer;
}

// ---------------------------------------------------------------------------
// protected
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// others
// ---------------------------------------------------------------------------
TlDenseVector_ImplViennaCL operator*(
    const TlDenseGeneralMatrix_ImplViennaCL& mat,
    const TlDenseVector_ImplViennaCL& vec) {
  assert(mat.getNumOfCols() == vec.getSize());
  TlDenseVector_ImplViennaCL answer(mat.getNumOfRows());
  answer.vector_ = viennacl::linalg::prod(mat.matrix_, vec.vector_);

  return answer;
}

TlDenseVector_ImplViennaCL operator*(
    const TlDenseVector_ImplViennaCL& vec,
    const TlDenseGeneralMatrix_ImplViennaCL& mat) {
  assert(mat.getNumOfRow() == vec.getSize());
  TlDenseVector_ImplViennaCL answer(mat.getNumOfCols());
  answer.vector_ =
      viennacl::linalg::prod(viennacl::trans(mat.matrix_), vec.vector_);

  return answer;
}
