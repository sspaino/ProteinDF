#include "tl_dense_symmetric_matrix_impl_viennacl.h"
#include "tl_dense_general_matrix_impl_viennacl.h"
#include "tl_dense_symmetric_matrix_impl_eigen.h"
#include "tl_dense_vector_impl_viennacl.h"
#include "viennacl/linalg/power_iter.hpp"
#include "viennacl/linalg/qr-method.hpp"
#include "viennacl/matrix.hpp"
#include "viennacl/vector.hpp"

TlDenseSymmetricMatrix_ImplViennaCL::TlDenseSymmetricMatrix_ImplViennaCL(
    const TlMatrixObject::index_type dim)
    : TlDenseGeneralMatrix_ImplViennaCL(dim, dim) {}

TlDenseSymmetricMatrix_ImplViennaCL::TlDenseSymmetricMatrix_ImplViennaCL(
    const TlDenseSymmetricMatrix_ImplViennaCL& rhs)
    : TlDenseGeneralMatrix_ImplViennaCL(rhs) {}

TlDenseSymmetricMatrix_ImplViennaCL::TlDenseSymmetricMatrix_ImplViennaCL(
    const TlDenseGeneralMatrix_ImplViennaCL& rhs)
    : TlDenseGeneralMatrix_ImplViennaCL() {
  const TlMatrixObject::index_type dim = rhs.getNumOfRows();
  this->matrix_.resize(dim, dim);

  const TlMatrixObject::index_type maxDim = std::max(dim, rhs.getNumOfCols());
  for (TlMatrixObject::index_type c = 0; c < maxDim; ++c) {
    for (TlMatrixObject::index_type r = 0; r <= c; ++r) {
      this->set(r, c, rhs.get(r, c));
    }
  }
}

TlDenseSymmetricMatrix_ImplViennaCL::TlDenseSymmetricMatrix_ImplViennaCL(
    const TlDenseSymmetricMatrix_ImplEigen& rhs) {
  viennacl::copy(rhs.matrix_, this->matrix_);
}

TlDenseSymmetricMatrix_ImplViennaCL::~TlDenseSymmetricMatrix_ImplViennaCL() {}

void TlDenseSymmetricMatrix_ImplViennaCL::set(
    const TlMatrixObject::index_type row, const TlMatrixObject::index_type col,
    const double value) {
  this->matrix_(row, col) = value;
  if (row != col) {
    this->matrix_(col, row) = value;
  }
}

void TlDenseSymmetricMatrix_ImplViennaCL::add(
    const TlMatrixObject::index_type row, const TlMatrixObject::index_type col,
    const double value) {
  this->matrix_(row, col) += value;
  if (row != col) {
    this->matrix_(col, row) += value;
  }
}

// ---------------------------------------------------------------------------
// operators
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// operations
// ---------------------------------------------------------------------------
TlDenseSymmetricMatrix_ImplViennaCL
TlDenseSymmetricMatrix_ImplViennaCL::transpose() const {
  // do nothing
  return *this;
}

TlDenseSymmetricMatrix_ImplViennaCL
TlDenseSymmetricMatrix_ImplViennaCL::inverse() const {
  const TlDenseSymmetricMatrix_ImplViennaCL answer =
      TlDenseGeneralMatrix_ImplViennaCL::inverse();

  return answer;
}

bool TlDenseSymmetricMatrix_ImplViennaCL::eig(
    TlDenseVector_ImplViennaCL* pEigval,
    TlDenseGeneralMatrix_ImplViennaCL* pEigvec) const {
  const TlMatrixObject::index_type dim = this->getNumOfRows();
  pEigvec->resize(dim, dim);
  pEigval->resize(dim);

  // power iteration
  viennacl::linalg::power_iter_tag ptag(1e-8);
  pEigvec->matrix_ =
      viennacl::linalg::eig(this->matrix_, ptag, pEigval->vector_);

  // QR method
  // viennacl::linalg::qr_method_sym(this->matrix_, pEigvec->matrix_,
  //                                 pEigval->vector_);

  return true;
}

bool TlDenseSymmetricMatrix_ImplViennaCL::eig_QR(
    TlDenseVector_ImplViennaCL* pEigval,
    TlDenseGeneralMatrix_ImplViennaCL* pEigvec) const {
  const TlMatrixObject::index_type dim = this->getNumOfRows();
  pEigvec->resize(dim, dim);
  pEigval->resize(dim);

  // QR method
  MatrixDataType A = this->matrix_;
  viennacl::linalg::qr_method_sym(A, pEigvec->matrix_, pEigval->vector_);

  return true;
}

// ---------------------------------------------------------------------------
// private
// ---------------------------------------------------------------------------
