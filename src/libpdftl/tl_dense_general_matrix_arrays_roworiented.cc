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

#include "tl_dense_general_matrix_arrays_roworiented.h"
#include <iostream>
#include "TlFile.h"
#include "TlUtils.h"
#include "tl_dense_general_matrix_mmap.h"

TlDenseGeneralMatrix_arrays_RowOriented::
    TlDenseGeneralMatrix_arrays_RowOriented(const index_type row,
                                            const index_type col,
                                            const int numOfSubunits,
                                            const int subunitID,
                                            bool isUsingMemManager)
    : TlDenseMatrix_arrays_Object(row, col, numOfSubunits, subunitID,
                                  isUsingMemManager) {}

TlDenseGeneralMatrix_arrays_RowOriented::
    TlDenseGeneralMatrix_arrays_RowOriented(
        const TlDenseGeneralMatrix_BLAS_old& rhs, const int numOfSubunits,
        const int subunitID, bool isUsingMemManager)
    : TlDenseMatrix_arrays_Object(rhs.getNumOfRows(), rhs.getNumOfCols(),
                                  numOfSubunits, subunitID, isUsingMemManager) {
  const index_type numOfRows = rhs.getNumOfRows();
  const index_type numOfCols = rhs.getNumOfCols();
  for (index_type r = 0; r < numOfRows; ++r) {
    for (index_type c = 0; c < numOfCols; ++c) {
      this->set(r, c, rhs.get(r, c));
    }
  }
}

TlDenseGeneralMatrix_arrays_RowOriented::
    TlDenseGeneralMatrix_arrays_RowOriented(
        const TlDenseGeneralMatrix_arrays_RowOriented& rhs)
    : TlDenseMatrix_arrays_Object(rhs) {}

TlDenseGeneralMatrix_arrays_RowOriented::
    ~TlDenseGeneralMatrix_arrays_RowOriented() {}

void TlDenseGeneralMatrix_arrays_RowOriented::resize(const index_type row,
                                                     const index_type col) {
  TlDenseMatrix_arrays_Object::resize(row, col);
}

void TlDenseGeneralMatrix_arrays_RowOriented::reserveColSize(
    const index_type reserveColSize) {
  TlDenseMatrix_arrays_Object::reserveVectorSize(reserveColSize);
}

void TlDenseGeneralMatrix_arrays_RowOriented::set(const index_type row,
                                                  const index_type col,
                                                  const double value) {
  TlDenseMatrix_arrays_Object::set_to_vm(row, col, value);
}

void TlDenseGeneralMatrix_arrays_RowOriented::add(const index_type row,
                                                  const index_type col,
                                                  const double value) {
  TlDenseMatrix_arrays_Object::add_to_vm(row, col, value);
}

double TlDenseGeneralMatrix_arrays_RowOriented::get(
    const index_type row, const index_type col) const {
  return TlDenseMatrix_arrays_Object::get_from_vm(row, col);
}

TlVector_BLAS TlDenseGeneralMatrix_arrays_RowOriented::getRowVector(
    const index_type row) const {
  return TlDenseMatrix_arrays_Object::getVector(row);
}

void TlDenseGeneralMatrix_arrays_RowOriented::getRowVector(
    const index_type row, double* pBuf, const index_type length) const {
  TlDenseMatrix_arrays_Object::getVector(row, pBuf, length);
}

TlDenseGeneralMatrix_BLAS_old
TlDenseGeneralMatrix_arrays_RowOriented::getTlMatrixObject() const {
  const index_type numOfRows = this->getNumOfRows();
  const index_type numOfCols = this->getNumOfCols();
  TlDenseGeneralMatrix_BLAS_old answer(numOfRows, numOfCols);

  for (index_type r = 0; r < numOfRows; ++r) {
    TlVector_BLAS v = TlDenseMatrix_arrays_Object::getVector(r);
    assert(v.getSize() == numOfCols);
    for (index_type c = 0; c < numOfCols; ++c) {
      answer.set(r, c, v[c]);
    }
  }

  return answer;
}

void TlDenseGeneralMatrix_arrays_RowOriented::
    saveByTlDenseGeneralMatrix_arrays_ColOriented(
        const std::string& basename) const {
  TlDenseMatrix_arrays_Object::saveByTheOtherType(basename);
}

bool RowVectorMatrix2CSFD(const std::string& rvmBasePath,
                          const std::string& csfdPath, bool verbose,
                          bool showProgress) {
  // check
  TlMatrixObject::index_type numOfRows = 0;
  TlMatrixObject::index_type numOfCols = 0;
  int numOfSubunits = 0;
  {
    int subunitID = 0;
    const std::string inputPath0 =
        TlDenseMatrix_arrays_Object::getFileName(rvmBasePath, subunitID);
    TlMatrixObject::index_type sizeOfVector, numOfVectors;
    const bool isLoadable = TlDenseMatrix_arrays_Object::isLoadable(
        inputPath0, &numOfVectors, &sizeOfVector, &numOfSubunits, &subunitID);
    if (isLoadable != true) {
      std::cerr << "can not open file: " << inputPath0 << std::endl;
      return false;
    }

    // row-vector matrix
    numOfRows = numOfVectors;
    numOfCols = sizeOfVector;

    if (verbose) {
      std::cerr << "rows: " << numOfRows << std::endl;
      std::cerr << "cols: " << numOfCols << std::endl;
      std::cerr << "units: " << numOfSubunits << std::endl;
    }
  }

  // prepare output
  if (TlFile::isExistFile(csfdPath)) {
    if (verbose) {
      std::cerr << "file overwrite: " << csfdPath << std::endl;
    }
    TlFile::remove(csfdPath);
  }
  TlDenseGeneralMatrix_mmap fileMat(csfdPath, numOfRows, numOfCols);

  // load & set
  for (int i = 0; i < numOfSubunits; ++i) {
    if (verbose) {
      std::cerr << TlUtils::format("%d / %d", i + 1, numOfSubunits)
                << std::endl;
    }

    TlDenseGeneralMatrix_arrays_RowOriented m;
    m.load(rvmBasePath, i);

    std::vector<double> vtr(numOfCols);
    for (TlMatrixObject::index_type r = i; r < numOfRows; r += numOfSubunits) {
      m.getVector(r, &(vtr[0]), numOfCols);
      fileMat.setRowVector(r, vtr);

      if (showProgress) {
        TlUtils::progressbar(float(r) / numOfRows);
      }
    }
    if (showProgress) {
      TlUtils::progressbar(1.0);
      std::cout << std::endl;
    }
  }

  if (verbose) {
    std::cerr << "end." << std::endl;
  }

  return true;
}
