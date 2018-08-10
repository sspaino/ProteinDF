// Copyright (C) 2002-2014 The ProteinDF project
// see also AUTHORS and README.
//
// This file is part of ProteinDF.
//
// ProteinDF is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ProteinDF is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ProteinDF.  If not, see <http://www.gnu.org/licenses/>.

#include "DfTransFmatrix.h"
#include "CnError.h"
#include "TlUtils.h"
#include "common_matrix.h"

DfTransFmatrix::DfTransFmatrix(TlSerializeData* pPdfParam, bool bExecDiis)
    : DfObject(pPdfParam), m_bExecDiis(bExecDiis) {
  {
    const std::string linearAlgebraPackage = TlUtils::toUpper(
        (*(this->pPdfParam_))["linear_algebra_package/trans_F"].getStr());
#ifdef HAVE_EINGEN3
    if (linearAlgebraPackage == "EIGEN") {
      this->linearAlgebraPackage_ = DfObject::LAP_EIGEN;
    }
#endif  // HAVE_EIGEN3
#ifdef HAVE_LAPACK
    if (linearAlgebraPackage == "LAPACK") {
      this->linearAlgebraPackage_ = DfObject::LAP_LAPACK;
    }
#endif  // HAVE_LAPACK
#ifdef HAVE_VIENNACL
    if (linearAlgebraPackage == "VIENNACL") {
      this->linearAlgebraPackage_ = DfObject::LAP_VIENNACL;
    }
#endif  // HAVE_VIENNACL
  }
}

DfTransFmatrix::~DfTransFmatrix() {}

void DfTransFmatrix::DfTrsFmatMain() {
  switch (this->m_nMethodType) {
    case METHOD_RKS: {
      switch (this->linearAlgebraPackage_) {
#ifdef HAVE_LAPACK
        case LAP_LAPACK:
          this->main<TlDenseGeneralMatrix_Lapack,
                     TlDenseSymmetricMatrix_Lapack>(RUN_RKS);
          break;
#endif // HAVE_LAPACK

#ifdef HAVE_EIGEN3
        case LAP_EIGEN:
          this->main<TlDenseGeneralMatrix_Eigen, TlDenseSymmetricMatrix_Eigen>(
              RUN_RKS);
          break;
#endif // HAVE_EIGEN3

#ifdef HAVE_VIENNACL
        case LAP_VIENNACL:
          this->main<TlDenseGeneralMatrix_ViennaCL,
                     TlDenseSymmetricMatrix_ViennaCL>(RUN_RKS);
          break;
#endif // HAVE_VIENNACL

        default:
          this->main<TlDenseGeneralMatrix_Lapack,
                     TlDenseSymmetricMatrix_Lapack>(RUN_RKS);
          break;
      }
    } break;

    case METHOD_UKS:
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_UKS_ALPHA);
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_UKS_BETA);
      break;

    case METHOD_ROKS:
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_ROKS);
      break;

    default:
      CnErr.abort();
      break;
  }
}

void DfTransFmatrix::DfTrsFmatQclo(const std::string& fragname, int norbcut) {
  this->m_nNumOfMOs = norbcut;

  switch (this->m_nMethodType) {
    case METHOD_RKS:
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_RKS, fragname, true);
      break;

    case METHOD_UKS:
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_UKS_ALPHA, fragname, true);
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_UKS_BETA, fragname, true);
      break;

    case METHOD_ROKS:
      this->main<TlDenseGeneralMatrix_Lapack, TlDenseSymmetricMatrix_Lapack>(
          RUN_ROKS, fragname, true);
      break;

    default:
      CnErr.abort();
      break;
  }
}
