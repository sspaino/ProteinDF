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

#ifndef TLCOLVECTORMATRIX_H
#define TLCOLVECTORMATRIX_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "TlMatrixObject.h"
#include "TlVectorMatrixObject.h"
#include "TlMatrix.h"

/// 行ベクトルで表される行列
 /// 行個数の、列個サイズのベクトルが集合したコンテナクラス
class TlColVectorMatrix : public TlVectorMatrixObject {
public:
    explicit TlColVectorMatrix(index_type row =1, index_type col =1,
                               int numOfSubunits =1,
                               int subunitID =0,
                               bool isUsingMemManager =false);
    explicit TlColVectorMatrix(const TlMatrix& rhs,
                               int numOfSubunits =1,
                               int subunitID =0,
                               bool isUsingMemManager =false);
    TlColVectorMatrix(const TlColVectorMatrix& rhs);

    virtual ~TlColVectorMatrix();

public:
    virtual void resize(index_type row, index_type col);
    void reserveRowSize(const index_type reserveRowSize);

    index_type getNumOfRows() const {
        return this->getSizeOfVector();
    };

    index_type getNumOfCols() const {
        return this->getNumOfVectors();
    };

    virtual void set(index_type row, index_type col, double value);
    virtual void add(index_type row, index_type col, double value);
    virtual double get(index_type row, index_type col) const;

    virtual TlVector getColVector(const index_type col) const;
    virtual void getColVector(const index_type col, double* pBuf, const index_type length) const;

public:
    /// TlMatrixオブジェクトを返す(for debug)
    TlMatrix getTlMatrixObject() const;

    /// TlRowVectorMatrix形式で保存
    void saveByTlRowVectorMatrix(const std::string& basename) const;
};

#endif // TLCOLVECTORMATRIX_H
