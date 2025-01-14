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

#include "DfInitialGuess.h"

#include <fstream>

#include "CnError.h"
#include "DfDmatrix.h"
#include "DfInitialGuessHarris.h"
#include "DfInitialGuessHuckel.h"
#include "TlStringTokenizer.h"
#include "df_population_lapack.h"

DfInitialGuess::DfInitialGuess(TlSerializeData* pPdfParam)
    : DfObject(pPdfParam) {
    this->isNormalizeDensityMatrix_ = true;
    if ((*this->pPdfParam_)["guess/normalize_density_matrix"]
            .getStr()
            .empty() == false) {
        this->isNormalizeDensityMatrix_ =
            (*this->pPdfParam_)["guess/normalize_density_matrix"].getBoolean();
    }
}

DfInitialGuess::~DfInitialGuess() {}

void DfInitialGuess::exec() {
    unsigned int calcState = this->loadCalcState();

    if ((calcState & GUESS_DONE) == 0) {
        switch (this->initialGuessType_) {
            case GUESS_RHO:
            // go below
            case GUESS_FILE_RHO:
                // this->createRho();
                // this->createOccupation();
                this->log_.critical("sorry. guess rho parameter is obsolete.");
                CnErr.abort();
                break;

            case GUESS_DENSITY:
                this->createInitialGuessUsingDensityMatrix();
                break;

            case GUESS_LCAO:
                this->createInitialGuessUsingLCAO();
                break;

            case GUESS_HUCKEL:
                this->createOccupation();
                this->createInitialGuessUsingHuckel();
                break;

            case GUESS_CORE:
                this->createOccupation();
                this->createInitialGuessUsingCore();
                break;

            case GUESS_HARRIS:
                this->createOccupation();
                this->createInitialGuessUsingHarris();
                break;

            default:
                this->log_.warn("unknown initial guess parameter.");
                break;
        }

        calcState |= GUESS_DONE;
        this->saveCalcState(calcState);
    }
}

unsigned int DfInitialGuess::loadCalcState() const {
    unsigned int cs = GUESS_UNPROCESSED;

    const bool isRestart = (*this->pPdfParam_)["restart"].getBoolean();
    if (isRestart) {
        this->logger("restart calculation is enabled.");
        cs = (*this->pPdfParam_)["control"]["guess_state"].getUInt();
    }

    return cs;
}

void DfInitialGuess::saveCalcState(unsigned int cs) {
    (*this->pPdfParam_)["control"]["guess_state"] = cs;

    // save PDF parameter
    const std::string pdfParamPath = (*this->pPdfParam_)["pdf_param_path"].getStr();
    TlMsgPack pdfParam_mpac(*this->pPdfParam_);
    pdfParam_mpac.save(pdfParamPath);
}

void DfInitialGuess::createInitialGuessUsingHuckel() {
    DfInitialGuessHuckel huckel(this->pPdfParam_);
    huckel.createGuess();
}

void DfInitialGuess::createInitialGuessUsingCore() {
    DfInitialGuessHuckel huckel(this->pPdfParam_);
    huckel.createGuess();
}

void DfInitialGuess::createInitialGuessUsingHarris() {
    switch (this->m_nMethodType) {
        case METHOD_RKS: {
            DfInitialGuessHarris harris(this->pPdfParam_);
            harris.main();
        } break;

        case METHOD_UKS:
            CnErr.abort(
                "Sorry. harris method is not supported except RKS. stop.\n");
            break;

        case METHOD_ROKS:
            CnErr.abort(
                "Sorry. harris method is not supported except RKS. stop.\n");
            break;

        default:
            CnErr.abort();
            break;
    }
}

// LCAO ================================================================
void DfInitialGuess::createInitialGuessUsingLCAO() {
    switch (this->m_nMethodType) {
        case METHOD_RKS:
            this->createInitialGuessUsingLCAO(RUN_RKS);
            break;

        case METHOD_UKS:
            this->createInitialGuessUsingLCAO(RUN_UKS_ALPHA);
            this->createInitialGuessUsingLCAO(RUN_UKS_BETA);
            break;

        case METHOD_ROKS:
            this->createInitialGuessUsingLCAO(RUN_ROKS_CLOSED);
            this->createInitialGuessUsingLCAO(RUN_ROKS_OPEN);
            break;

        default:
            abort();
            break;
    }
}

void DfInitialGuess::createInitialGuessUsingLCAO(const RUN_TYPE runType) {
    // read guess lcao
    const TlDenseGeneralMatrix_Lapack LCAO = this->getLCAO_LAPACK(runType);
    this->saveC0(runType, LCAO);

    // read guess occupation
    const TlDenseVector_Lapack aOccupation = this->getOccupation(runType);
    this->saveOccupation(runType, aOccupation);

    {
        TlSerializeData tmpParam = *(this->pPdfParam_);
        tmpParam["orbital-correspondence"] = false;
        tmpParam["orbital-overlap-correspondence-method"] = "simple";
        tmpParam["num_of_iterations"] = 0;

        // 密度行列の作成
        DfDmatrix dfDmatrix(&tmpParam);
        dfDmatrix.run();  // RKS only?
    }
}

TlDenseGeneralMatrix_Lapack DfInitialGuess::getLCAO_LAPACK(
    const RUN_TYPE runType) {
    TlDenseGeneralMatrix_Lapack lcaoMatrix;
    const std::string binFile = DfInitialGuess::getLcaoPath_bin(runType);
    const std::string txtFile = DfInitialGuess::getLcaoPath_txt(runType);

    this->log_.info("get LCAO");
    if (TlFile::isExistFile(binFile)) {
        // LCAO file is prepared by binary file.
        this->log_.info(TlUtils::format("LCAO: loading: %s", binFile.c_str()));
        lcaoMatrix.load(binFile);
    } else if (TlFile::isExistFile(txtFile)) {
        this->log_.info(TlUtils::format("LCAO: loading: %s", txtFile.c_str()));
        // LCAO file is prepared by text file.
        std::ifstream fi;
        fi.open(txtFile.c_str(), std::ios::in);
        if ((fi.rdstate() & std::ifstream::failbit) != 0) {
            CnErr.abort(
                TlUtils::format("cannot open file %s.\n", txtFile.c_str()));
        }

        std::string dummy_line;
        fi >> dummy_line;

        int row_dimension, col_dimension;
        fi >> row_dimension >> col_dimension;
        if (row_dimension != this->m_nNumOfAOs) {
            CnErr.abort("DfInitialGuess", "", "prepare_occupation_and_or_mo",
                        "inputted guess lcao has illegal dimension");
        }
        lcaoMatrix.resize(row_dimension, col_dimension);

        const int maxRows = row_dimension;
        const int maxCols = col_dimension;
        for (int i = 0; i < maxRows; ++i) {
            for (int j = 0; j < maxCols; ++j) {
                double v;
                fi >> v;
                lcaoMatrix.set(i, j, v);
            }
        }
    } else {
        this->log_.warn(
            TlUtils::format("file not found.: %s", binFile.c_str()));
    }

    return lcaoMatrix;
}

std::string DfInitialGuess::getLcaoPath_txt(const RUN_TYPE runType) {
    return TlUtils::format("./guess.lcao.%s.txt",
                           DfObject::m_sRunTypeSuffix[runType].c_str());
}

std::string DfInitialGuess::getLcaoPath_bin(const RUN_TYPE runType) {
    return TlUtils::format("./guess.lcao.%s.mat",
                           DfObject::m_sRunTypeSuffix[runType].c_str());
}

// density matrix ======================================================
void DfInitialGuess::createInitialGuessUsingDensityMatrix() {
    switch (this->m_nMethodType) {
        case METHOD_RKS:
            this->createInitialGuessUsingDensityMatrix(RUN_RKS);
            break;

        case METHOD_UKS:
            this->createInitialGuessUsingDensityMatrix(RUN_UKS_ALPHA);
            this->createInitialGuessUsingDensityMatrix(RUN_UKS_BETA);
            break;

        case METHOD_ROKS:
            this->createInitialGuessUsingDensityMatrix(RUN_ROKS_CLOSED);
            this->createInitialGuessUsingDensityMatrix(RUN_ROKS_OPEN);
            break;

        default:
            abort();
            break;
    }

    this->createOccupation();
}

void DfInitialGuess::createInitialGuessUsingDensityMatrix(
    const RUN_TYPE runType) {
    // read guess lcao
    TlDenseSymmetricMatrix_Lapack P =
        this->getInitialDensityMatrix<TlDenseSymmetricMatrix_Lapack>(runType);
    if (this->isNormalizeDensityMatrix_) {
        P = this->normalizeDensityMatrix<TlDenseSymmetricMatrix_Lapack,
                                         DfPopulation_Lapack>(runType, P);
    }
    this->savePpqMatrix(runType, 0, P);

    // make occupation data
    this->createOccupation();
}

TlDenseVector_Lapack DfInitialGuess::getOccupation(const RUN_TYPE runType) {
    TlDenseVector_Lapack occupation;
    const std::string binFile = TlUtils::format(
        "./guess.occ.%s.vtr", this->m_sRunTypeSuffix[runType].c_str());
    const std::string txtFile = TlUtils::format(
        "./guess.occ.%s.txt", this->m_sRunTypeSuffix[runType].c_str());

    if (TlFile::isExistFile(binFile)) {
        occupation.load(binFile);
    } else if (TlFile::isExistFile(txtFile)) {
        occupation.loadText(txtFile);
    } else {
        this->log_.warn(
            TlUtils::format("file not found.: %s", binFile.c_str()));
    }

    return occupation;
}

void DfInitialGuess::createOccupation() {
    switch (this->m_nMethodType) {
        case METHOD_RKS:
            this->createOccupation(RUN_RKS);
            break;

        case METHOD_UKS:
            this->createOccupation(RUN_UKS_ALPHA);
            this->createOccupation(RUN_UKS_BETA);
            break;

        case METHOD_ROKS:
            this->createOccupation(RUN_ROKS_CLOSED);
            this->createOccupation(RUN_ROKS_OPEN);
            break;

        default:
            CnErr.abort();
            break;
    }
}

TlDenseVector_Lapack DfInitialGuess::createOccupation(const RUN_TYPE runType) {
    const TlSerializeData& pdfParam = *(this->pPdfParam_);

    // construct guess occupations
    const index_type numOfMOs = this->m_nNumOfMOs;
    TlDenseVector_Lapack guess_occ(numOfMOs);
    switch (runType) {
        case RUN_RKS: {
            const std::vector<int> docLevel =
                this->getLevel(pdfParam["method/rks/occlevel"].getStr());
            for (std::vector<int>::const_iterator p = docLevel.begin();
                 p != docLevel.end(); ++p) {
                const int level = *p - 1;
                if ((0 <= level) && (level < numOfMOs)) {
                    guess_occ.set(*p - 1, 2.0);
                }
            }
        } break;

        case RUN_UKS_ALPHA: {
            const std::vector<int> occLevel =
                this->getLevel(pdfParam["method/uks/alpha_occlevel"].getStr());
            for (std::vector<int>::const_iterator p = occLevel.begin();
                 p != occLevel.end(); ++p) {
                const int level = *p - 1;
                if ((0 <= level) && (level < numOfMOs)) {
                    guess_occ.set(level, 1.0);
                }
            }
        } break;

        case RUN_UKS_BETA: {
            const std::vector<int> occLevel =
                this->getLevel(pdfParam["method/uks/beta_occlevel"].getStr());
            for (std::vector<int>::const_iterator p = occLevel.begin();
                 p != occLevel.end(); ++p) {
                const int level = *p - 1;
                if ((0 <= level) && (level < numOfMOs)) {
                    guess_occ.set(*p - 1, 1.0);
                }
            }
        } break;

        case RUN_ROKS_CLOSED: {
            const std::vector<int> occLevel_c = this->getLevel(
                pdfParam["method/roks/closed_occlevel"].getStr());
            for (std::vector<int>::const_iterator p = occLevel_c.begin();
                 p != occLevel_c.end(); ++p) {
                const int level = *p - 1;
                if ((0 <= level) && (level < numOfMOs)) {
                    guess_occ.set(*p - 1, 2.0);
                }
            }
        } break;

        case RUN_ROKS_OPEN: {
            const std::vector<int> occLevel_o =
                this->getLevel(pdfParam["method/roks/open_occlevel"].getStr());
            for (std::vector<int>::const_iterator p = occLevel_o.begin();
                 p != occLevel_o.end(); ++p) {
                const int level = *p - 1;
                if ((0 <= level) && (level < numOfMOs)) {
                    guess_occ.set(*p - 1, 1.0);
                }
            }
        } break;

        default:
            CnErr.abort();
            break;
    }

    // output occupation number to a files in fl_Work directory
    const std::string sOccFileName = this->getOccupationPath(runType);
    guess_occ.save(sOccFileName);

    return guess_occ;
}

std::vector<int> DfInitialGuess::getLevel(const std::string& inputStr) {
    std::vector<int> answer;

    // 構文解釈
    std::string numStr = "";
    std::vector<int> stack;
    const int len = inputStr.size();
    for (int i = 0; i < len; ++i) {
        const char c = inputStr[i];
        if (std::isdigit(c) != 0) {
            numStr.append(1, c);
        } else {
            if (numStr.size() > 0) {
                const int num = std::atoi(numStr.c_str());
                stack.push_back(num);
                numStr = "";
            }

            if (c == '-') {
                stack.push_back(-1);
            }
        }
    }
    if (numStr.empty() == false) {
        stack.push_back(std::atoi(numStr.c_str()));
    }

    // 翻訳
    const int stackSize = stack.size();
    for (int i = 0; i < stackSize; ++i) {
        const int v = stack[i];
        if (v > 0) {
            answer.push_back(v);
        } else if (v == -1) {
            const int i1 = i + 1;
            if ((i1 < stackSize) && (answer.size() > 0)) {
                const int end = stack[i1];
                const int start = answer.at(answer.size() - 1);
                if (start > 0) {
                    for (int v = start + 1; v <= end; ++v) {
                        answer.push_back(v);
                    }
                }
                ++i;
            }
        }
    }

    return answer;
}

void DfInitialGuess::saveOccupation(const RUN_TYPE runType,
                                    const TlDenseVector_Lapack& rOccupation) {
    const std::string sOccFileName = this->getOccupationPath(runType);
    rOccupation.save(sOccFileName);
}

void DfInitialGuess::makeDensityMatrix() {
    TlSerializeData tmpParam = *(this->pPdfParam_);
    tmpParam["orbital-correspondence"] = false;
    tmpParam["orbital-overlap-correspondence-method"] = "simple";
    tmpParam["control-iteration"] = 0;

    DfDmatrix* pDfDmat = getDfDmatrixObject(&tmpParam);
    pDfDmat->run();
    delete pDfDmat;
    pDfDmat = NULL;
}

DfDmatrix* DfInitialGuess::getDfDmatrixObject(TlSerializeData* param) {
    DfDmatrix* obj = new DfDmatrix(param);
    return obj;
}
