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

#ifndef TLVECTORMATRIXOBJECT_H
#define TLVECTORMATRIXOBJECT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "TlMatrixObject.h"

/// 配列の配列として行列を扱うためのコンテナ
class TlVectorMatrixObject : public TlMatrixObject {
public:
    explicit TlVectorMatrixObject(index_type numOfVectors = 1,
                                  index_type sizeOfVector = 1, 
                                  int numOfMembers = 1, int ID = 0,
                                  bool isUsingMemManager = false);
    TlVectorMatrixObject(const TlVectorMatrixObject& rhs);


    virtual ~TlVectorMatrixObject();

    TlVectorMatrixObject& operator=(const TlVectorMatrixObject& rhs);

public:
    virtual index_type getNumOfRows() const =0;
    virtual index_type getNumOfCols() const =0;

    /// インスタンスのメモリサイズを返す
    virtual std::size_t getMemSize() const;

private:
    virtual double get(index_type row, index_type col) const =0;
    virtual void set(index_type row, index_type col, double value) =0;
    virtual void add(index_type row, index_type col, double value) =0;

public:
    virtual bool load(const std::string& path);
    virtual bool save(const std::string& path) const;

    virtual bool load(const std::string& basename, int subunitID);
        
public:
    void resize(index_type newNumOfVectors,
                index_type newVectorSize);

    /// 値を代入する
    void set_to_vm(index_type vectorIndex,
                   index_type index,
                   double value);

    void add_to_vm(index_type vectorIndex,
                   index_type index,
                   double value);

    double get_from_vm(index_type vectorIndex,
                       index_type index) const;

    TlVector getVector(index_type vectorIndex) const;
    void getVector(const index_type vectorIndex, double* pBuf, const index_type length) const;
    void setVector(index_type vectorIndex, const TlVector& v);

public:
    static std::string getFileName(const std::string& basename,
                                   const int subunitID);

    /// 指定したファイルがこのクラスで読み込めるかどうかをチェックする。
    /// 読み込める場合はtrueを返す。
    /// このとき、pNumOfSubunitsが指定されていれば、全サブユニット数が代入される。
    /// また、pSubunitIDが指定されていれば、サブユニットIDが代入される。
    static bool isLoadable(const std::string& filepath,
                           index_type* pNumOfVectors = NULL,
                           index_type* pSizeOfVector = NULL,
                           int* pNumOfSubunits = NULL,
                           int* pSubunitID = NULL);

public:
    index_type getSizeOfVector() const {
        return this->sizeOfVector_;
    };

    index_type getNumOfVectors() const {
        return this->numOfVectors_;
    };


    index_type getNumOfLocalVectors() const {
        return this->numOfLocalVectors_;
    };
    
    int getNumOfSubunits() const {
        return this->numOfSubunits_;
    };

    /// 自分のIDを返す
    int getSubunitID() const {
        return this->subunitID_;
    };

    /// 該当するベクトルを持っているPE番号を返す
    int getSubunitID(const index_type vectorIndex) const;

protected:
    /// 前もってvector sizeを設定する
    void reserveVectorSize(index_type vectorSize);

    /// data_ メンバ変数を破棄する
    void destroy();

protected:
    bool saveByTheOtherType(const std::string& basename) const;

private:
    index_type numOfVectors_;      /// ベクトルの総数(global)
    index_type sizeOfVector_;        /// 1ベクトルの大きさ

    /// 行列全体を構成するオブジェクトの総数
    /// 通常はプロセスの総数
    int numOfSubunits_; 

    /// このオブジェクトのID
    /// 通常はプロセスID
    int subunitID_;

    index_type numOfLocalVectors_; // ベクトルの総数(local)

    index_type reservedVectorSize_; /// あらかじめ保持している1ベクトルの大きさ

    bool isUsingMemManager_;     /// 独自のメモリマネージャを使う(true)
    std::vector<double* > data_; /// データ

};

#endif // TLCOLVECTORMATRIX2_H
