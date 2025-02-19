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

#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>

#include "DfObject.h"
#include "TlFile.h"
#include "TlGetopt.h"
#include "TlMsgPack.h"
#include "TlOrbitalInfo.h"
#include "TlSerializeData.h"
#include "TlUtils.h"
#include "tl_dense_symmetric_matrix_lapack.h"

#define AU_BOHR 1.889762

struct LCAO {
public:
    int index;
    double value;
};

struct LCAO_cmp {
public:
    bool operator()(const LCAO& a, const LCAO& b) {
        return (std::fabs(a.value) > std::fabs(b.value));
    }
};

enum EXEC_MODE { UNDEFINED,
                 MO_MODE,
                 ATOM_MODE,
                 ATOMGROUP_MODE,
                 SAVE_MODE
};

void showHelp(const std::string& name) {
    std::cout << TlUtils::format("%s <OPTIONS> -M MO_LEVEL NUM_OF_ITEMS", name.c_str()) << std::endl;
    std::cout << " or " << std::endl;
    std::cout << TlUtils::format("%s <OPTIONS> -A ATOM_INDEX", name.c_str()) << std::endl;
    std::cout << " or " << std::endl;
    std::cout << TlUtils::format("%s <OPTIONS> -G ATOMGROUP_FILE", name.c_str()) << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -d DIRECTORY    set directory to read (default: current "
                 "directory)"
              << std::endl;
    std::cout << "  -c path         specify LCAO matrix (default: guessed)" << std::endl;
    std::cout << "  -s path         specify overlap matrix (default: guessed)" << std::endl;
    std::cout << "  -h              show help(this)" << std::endl;
    std::cout << "  -v              enable verbose mode" << std::endl;
    std::cout << "  <MO mode>" << std::endl;
    std::cout << "  -M MO_LEVEL     show AO components for the MO level" << std::endl;
    std::cout << "  -a              output all basis functions" << std::endl;
    std::cout << "  -n NUM          output number of items" << std::endl;
    std::cout << "  <atom mode>" << std::endl;
    std::cout << "  -A ATOM_INDEX   output MO levels in which AOs are major" << std::endl;
    std::cout << "  -t THRESHOLD    threshold of w_atom" << std::endl;
    std::cout << "  <save mode>" << std::endl;
    std::cout << "  -S path         save intermediate files" << std::endl;
    std::cout << "  -g g_PATH       save G-value vector file (default: g-value.vct)" << std::endl;
    std::cout << "  -m ORB_CNT_PATH save orbital contribution matrix file" << std::endl;
    std::cout << "  <atomgroup mode>" << std::endl;
    std::cout << "  -G path         atomgroup path" << std::endl;
    std::cout << "  -o output_PATH  output MOs by groups as msgpack file (default: output.mpac)" << std::endl;
}

/// MO モード
/// topに-1が指定された場合は、全軌道が出力される
int exec_MO_mode(const TlDenseGeneralMatrix_Lapack& CS, const TlDenseGeneralMatrix_Lapack& C,
                 const TlOrbitalInfo& readOrbInfo, int level, int top, bool isVerbose = false) {
    // MO mode -------------------------------------------------------------
    const int numOfAOs = C.getNumOfRows();
    const int numOfMOs = C.getNumOfCols();

    if (isVerbose == true) {
        std::cerr << "MO level: " << level << std::endl;
    }

    if (level >= numOfMOs) {
        std::cerr << TlUtils::format("input level is out of range. input=%d, max=%d", level, C.getNumOfCols())
                  << std::endl;
        return EXIT_FAILURE;
    }

    // MO
    const TlDenseVector_Lapack MO_vec = C.getColVector_tmpl<TlDenseVector_Lapack>(level);
    std::vector<LCAO> AOs(numOfAOs);
    double sum = 0.0;
    for (int i = 0; i < numOfAOs; ++i) {
        const double w = std::fabs(CS.get(level, i) * MO_vec.get(i));
        AOs[i].index = i;
        AOs[i].value = w;
        sum += w;
    }

    // sort
    std::sort(AOs.begin(), AOs.end(), LCAO_cmp());

    // display
    const char shellTypes[] = {'s', 'p', 'd'};
    const char* angularMomentumTypes[] = {"s", "px", "py", "pz", "dxy", "dyz", "dzx", "dxx-yy", "dzz"};

    std::cout << TlUtils::format("MO level: %d", level) << std::endl;

    if (top == -1) {
        top = numOfAOs;
    }

    const double inv_sum = 1.0 / sum;
    double accumulate = 0.0;
    for (int rank = 0; rank < top; ++rank) {
        const int AO = AOs[rank].index;
        const double w = AOs[rank].value;
        const TlPosition pos = readOrbInfo.getPosition(AO);

        const double rate = w * inv_sum * 100.0;
        accumulate += rate;
        std::cout << TlUtils::format(
                         "No.%2d %5d %2s (% 8.3f, % 8.3f, % 8.3f) %c(%6s) coef=% "
                         "8.3f w=% 8.3f(%6.2f%%; %6.2f%%)",
                         rank + 1, readOrbInfo.getAtomIndex(AO), readOrbInfo.getAtomName(AO).c_str(), pos.x() / AU_BOHR,
                         pos.y() / AU_BOHR, pos.z() / AU_BOHR, shellTypes[readOrbInfo.getShellType(AO)],
                         angularMomentumTypes[readOrbInfo.getBasisType(AO)], MO_vec.get(AO), w, rate, accumulate)
                  << std::endl;
    }
    std::cout << TlUtils::format("sum = % 8.3f", sum) << std::endl;

    return EXIT_SUCCESS;
}

int exec_atom_mode(const TlDenseGeneralMatrix_Lapack& CS, const TlDenseGeneralMatrix_Lapack& C,
                   const TlOrbitalInfo& readOrbInfo, int inputAtomIndex, const double threshold,
                   bool isVerbose = false) {
    const int numOfAOs = C.getNumOfRows();
    const int numOfMOs = C.getNumOfCols();

    if (isVerbose) {
        std::cerr << TlUtils::format("threshold: %8.2e", threshold) << std::endl;
    }

    std::vector<int> pickupMO;
    for (int MO = 0; MO < numOfMOs; ++MO) {
        const TlDenseVector_Lapack MO_vec = C.getColVector_tmpl<TlDenseVector_Lapack>(MO);

        if (isVerbose) {
            std::cerr << TlUtils::format("check %5dth MO ...", MO) << std::endl;
        }
        double sum = 0.0;
        double w_atom = 0.0;
        for (int i = 0; i < numOfAOs; ++i) {
            const double w = std::fabs(CS.get(MO, i) * MO_vec.get(i));
            sum += w;

            const int atomIndex = readOrbInfo.getAtomIndex(i);
            if (atomIndex == inputAtomIndex) {
                w_atom += w;
            }
        }

        const double ratio = w_atom / sum;
        if (ratio > threshold) {
            std::cout << TlUtils::format("MO=%5d w_atom=% 8.3f", MO, w_atom) << std::endl;
        }
    }

    return EXIT_SUCCESS;
}

TlDenseGeneralMatrix_Lapack getOrbContributionMatrix(const TlDenseGeneralMatrix_Lapack& CS,
                                                     const TlDenseGeneralMatrix_Lapack& C,
                                                     const std::string& orbContributionMatrixPath = "",
                                                     const std::string& gValueVectorPath = "",
                                                     bool isVerbose = false) {
    const TlMatrixObject::index_type numOfAOs = C.getNumOfRows();
    const TlMatrixObject::index_type numOfMOs = C.getNumOfCols();
    assert(CS.getNumOfRows() == numOfMOs);
    assert(CS.getNumOfCols() == numOfAOs);

    if (isVerbose) {
        std::cerr << "calc orbital contribution matrix..." << std::endl;
    }
    TlDenseGeneralMatrix_Lapack orbContribMat(numOfAOs, numOfMOs);
    for (TlMatrixObject::index_type mo = 0; mo < numOfMOs; ++mo) {
        const TlDenseVector_Lapack MO_vec = C.getColVector_tmpl<TlDenseVector_Lapack>(mo);
        assert(MO_vec.getSize() == numOfAOs);
        for (TlMatrixObject::index_type ao = 0; ao < numOfAOs; ++ao) {
            const double w = CS.get(mo, ao) * MO_vec.get(ao);
            orbContribMat.set(ao, mo, w);
        }
    }

    if (orbContributionMatrixPath.empty() != true) {
        if (isVerbose) {
            std::cerr << "outout orb contribution matrix: " << orbContributionMatrixPath << std::endl;
        }
        orbContribMat.save(orbContributionMatrixPath);
    }

    // calc G-value
    if (gValueVectorPath.empty() != true) {
        TlDenseVector_Lapack G(numOfMOs);
        TlDenseVector_Lapack validation(numOfMOs);
        for (TlMatrixObject::index_type mo = 0; mo < numOfMOs; ++mo) {
            double sum = 0.0;
            double g = 0.0;
            for (TlMatrixObject::index_type ao = 0; ao < numOfAOs; ++ao) {
                const double v = orbContribMat.get(ao, mo);
                const double vv = v * v;
                sum += v;
                g += vv;
            }
            validation.set(mo, sum);
            G.set(mo, g);
        }
        std::cerr << TlUtils::format("save G-values: %s", gValueVectorPath.c_str()) << std::endl;
        G.save(gValueVectorPath);
        // validation.save("validation.vct");
    }

    return orbContribMat;
}

TlDenseGeneralMatrix_Lapack getAtomContributionMatrix(const TlDenseGeneralMatrix_Lapack& CS, const TlDenseGeneralMatrix_Lapack& C,
                                                      const TlOrbitalInfo& orbInfo, const double threshold,
                                                      const std::string& orbContributionMatrixPath = "",
                                                      const std::string& gValueVectorPath = "",
                                                      bool isVerbose = false) {
    const TlMatrixObject::index_type numOfAOs = C.getNumOfRows();
    const TlMatrixObject::index_type numOfMOs = C.getNumOfCols();
    const int numOfAtoms = orbInfo.getNumOfAtoms();

    const TlDenseGeneralMatrix_Lapack orbContribMat =
        getOrbContributionMatrix(CS, C, orbContributionMatrixPath, gValueVectorPath, isVerbose);
    assert(orbContribMat.getNumOfRows() == numOfAOs);
    assert(orbContribMat.getNumOfCols() == numOfMOs);

    std::cerr << "calc atom contribution matrix..." << std::endl;
    TlDenseGeneralMatrix_Lapack atomContribMat(numOfAtoms, numOfMOs);
    for (TlMatrixObject::index_type mo = 0; mo < numOfMOs; ++mo) {
        for (TlMatrixObject::index_type ao = 0; ao < numOfAOs; ++ao) {
            const double w = orbContribMat.get(ao, mo);

            const int atomIndex = orbInfo.getAtomIndex(ao);
            atomContribMat.add(atomIndex, mo, w);
        }
    }

    std::string atomContribMatPath = "atom_contrib.mat";
    std::cerr << "outout atom contribution matrix: " << atomContribMatPath << std::endl;
    atomContribMat.save(atomContribMatPath);

    return atomContribMat;
}

int exec_atomgroup_mode(const TlDenseGeneralMatrix_Lapack& CS, const TlDenseGeneralMatrix_Lapack& C,
                        const TlOrbitalInfo& orbInfo, const double threshold,
                        const std::string& atomGroupPath, const std::string& output,
                        const std::string& orbContributionMatrixPath = "",
                        const std::string& gValueVectorPath = "",
                        bool isVerbose = false) {
    const TlMatrixObject::index_type numOfMOs = C.getNumOfCols();

    TlSerializeData atomGroup;
    {
        TlMsgPack msgPack;
        msgPack.load(atomGroupPath);
        atomGroup = msgPack.getSerializeData();
    }

    if (isVerbose) {
        std::cerr << "reading group info ..." << std::endl;
    }
    const int numOfGroups = atomGroup.getSize();
    std::vector<std::set<int> > groups(numOfGroups);
    for (int groupIndex = 0; groupIndex < numOfGroups; ++groupIndex) {
        const TlSerializeData group = atomGroup.getAt(groupIndex);

        TlSerializeData::ArrayConstIterator itEnd = group.endArray();
        for (TlSerializeData::ArrayConstIterator it = group.beginArray(); it != itEnd; ++it) {
            groups[groupIndex].insert(it->getInt());
        }
        // std::cout << ">>>> group: " << groupIndex << std::endl;
        // std::cout << group.str() << std::endl;
    }

    if (isVerbose) {
        std::cerr << "calc atom contribute matrix..." << std::endl;
    }
    const TlDenseGeneralMatrix_Lapack atomContribMat =
        getAtomContributionMatrix(CS, C, orbInfo, threshold, orbContributionMatrixPath, gValueVectorPath, isVerbose);

    if (isVerbose) {
        std::cerr << "search MOs by each group ..." << std::endl;
        std::cerr << "group threshold = " << threshold << std::endl;
    }
    std::vector<std::set<int> > groupMOs(numOfGroups);
    for (int mo = 0; mo < numOfMOs; ++mo) {
        for (int groupIndex = 0; groupIndex < numOfGroups; ++groupIndex) {
            double contrib_group = 0.0;
            std::set<int>::const_iterator itAtomEnd = groups[groupIndex].end();
            for (std::set<int>::const_iterator itAtom = groups[groupIndex].begin(); itAtom != itAtomEnd; ++itAtom) {
                const int atomIndex = *itAtom;
                const double w = atomContribMat.get(atomIndex, mo);
                contrib_group += w;
            }
            if (contrib_group >= threshold) {
                groupMOs[groupIndex].insert(mo);
            }
        }
    }

    if (isVerbose) {
        std::cerr << "output: " << output << std::endl;
    }
    {
        TlSerializeData groupMOsData;
        groupMOsData.resize(numOfGroups);
        for (int groupIndex = 0; groupIndex < numOfGroups; ++groupIndex) {
            TlSerializeData MOs;
            std::set<int>::const_iterator itEnd = groupMOs[groupIndex].end();
            for (std::set<int>::const_iterator it = groupMOs[groupIndex].begin(); it != itEnd; ++it) {
                MOs.pushBack(*it);
            }
            groupMOsData[groupIndex] = MOs;
        }

        TlMsgPack mpack(groupMOsData);
        mpack.save(output);
    }

    return EXIT_SUCCESS;
}

int exec_save_mode(const TlDenseGeneralMatrix_Lapack& CS, const TlDenseGeneralMatrix_Lapack& C,
                   const std::string& savePath,
                   const std::string& orbContributionMatrixPath = "",
                   const std::string& gValueVectorPath = "",
                   bool isVerbose = false) {
    const TlDenseGeneralMatrix_Lapack CSC =
        getOrbContributionMatrix(CS, C, orbContributionMatrixPath, gValueVectorPath, isVerbose);

    if (isVerbose) {
        std::cerr << TlUtils::format("save matrix: %s", savePath.c_str()) << std::endl;
    }
    CSC.save(savePath);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    TlGetopt opt(argc, argv, "A:G:M:S:ad:c:g:hm:n:o:s:t:v");

    // parameters - common
    if (opt["h"] == "defined") {
        showHelp(opt[0]);
        return EXIT_FAILURE;
    }

    const bool isVerbose = (opt["v"] == "defined");

    std::string readDir = "./";
    if (opt["d"].empty() == false) {
        readDir = opt["d"];
    }

    std::string LCAO_path = "";
    if (opt["c"].empty() == false) {
        LCAO_path = opt["c"];
    }

    std::string Spq_path = "";
    if (opt["s"].empty() == false) {
        Spq_path = opt["s"];
    }

    // parameters - exec mode
    int MO_level = 0;
    int inputAtomIndex = 0;
    std::string atomGroupPath = "";
    std::string savePath = "";

    EXEC_MODE execMode = UNDEFINED;
    if (opt["M"].empty() == false) {
        execMode = MO_MODE;
        MO_level = std::atoi(opt["M"].c_str());
    } else if (opt["A"].empty() == false) {
        execMode = ATOM_MODE;
        inputAtomIndex = std::atoi(opt["A"].c_str());
    } else if (opt["S"].empty() == false) {
        execMode = SAVE_MODE;
        savePath = opt["S"];
    } else if (opt["G"].empty() == false) {
        execMode = ATOMGROUP_MODE;
        atomGroupPath = opt["G"];
    }

    // parameters - MO mode
    int top = 10;
    if (opt["n"].empty() == false) {
        top = std::atoi(opt["n"].c_str());
    }
    if (opt["a"] == "defined") {
        top = -1;
    }

    // parameters - atom mode
    double threshold = 0.33;
    if (opt["t"].empty() == false) {
        threshold = std::atof(opt["t"].c_str());
    }

    // parameters - save mode / atomgroup mode
    std::string orbContributionMatrixPath = "";
    if (opt["m"].empty() == false) {
        orbContributionMatrixPath = opt["m"];
    }
    std::string gValueVectorPath = "g-value.vct";
    if (opt["g"].empty() == false) {
        gValueVectorPath = opt["g"];
    }
    std::string output = "output.mpac";
    if (opt["o"].empty() == false) {
        output = opt["o"];
    }

    // for reading object
    const std::string readParamPath = TlUtils::format("%s/pdfparam.mpac", readDir.c_str());
    TlMsgPack readMsgPack;
    readMsgPack.load(readParamPath);
    TlSerializeData pdfparam = readMsgPack.getSerializeData();
    const TlOrbitalInfo readOrbInfo(pdfparam["coordinates"], pdfparam["basis_set"]);
    const int lastIteration = pdfparam["num_of_iterations"].getInt();
    DfObject dfObj(&pdfparam);

    // calc CS
    TlDenseGeneralMatrix_Lapack C;
    TlDenseGeneralMatrix_Lapack CS;
    {
        // load S
        if (Spq_path.empty()) {
            Spq_path = dfObj.getSpqMatrixPath();
        }
        TlDenseSymmetricMatrix_Lapack S;
        if (isVerbose == true) {
            std::cout << TlUtils::format("read Spq: %s", Spq_path.c_str()) << std::endl;
        }
        S.load(Spq_path);

        // load C
        if (LCAO_path.empty()) {
            LCAO_path = dfObj.getCMatrixPath(DfObject::RUN_RKS, lastIteration);
        }
        if (isVerbose == true) {
            std::cout << TlUtils::format("read LCAO: %s", LCAO_path.c_str()) << std::endl;
        }

        C.load(LCAO_path);
        if (isVerbose == true) {
            std::cout << TlUtils::format("row = %d, col = %d", C.getNumOfRows(), C.getNumOfCols()) << std::endl;
        }
        const TlDenseGeneralMatrix_Lapack tC = C.transpose();

        CS = tC * S;
    }

    // exec
    int answer = 0;
    switch (execMode) {
        case MO_MODE:
            answer = exec_MO_mode(CS, C, readOrbInfo, MO_level, top, isVerbose);
            break;
        case ATOM_MODE:
            answer = exec_atom_mode(CS, C, readOrbInfo, inputAtomIndex, threshold, isVerbose);
            break;
        case SAVE_MODE:
            answer = exec_save_mode(CS, C, savePath, orbContributionMatrixPath, gValueVectorPath, isVerbose);
            break;
        case ATOMGROUP_MODE:
            answer = exec_atomgroup_mode(CS, C, readOrbInfo, threshold, atomGroupPath,
                                         orbContributionMatrixPath, gValueVectorPath, output, isVerbose);
            break;
        default:
            std::cerr << "please specify mode: " << std::endl;
            showHelp(opt[0]);
            break;
    }

    return answer;
}
