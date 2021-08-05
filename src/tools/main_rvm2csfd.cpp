#include <iostream>

#include "TlFile.h"
#include "TlGetopt.h"
#include "TlUtils.h"
#include "tl_dense_general_matrix_arrays_mmap_roworiented.h"
// #include "tl_dense_general_matrix_arrays_roworiented.h"
#include "tl_dense_general_matrix_eigen.h"
#include "tl_dense_general_matrix_mmap.h"
#include "tl_matrix_utils.h"

typedef TlMatrixObject::index_type index_type;

void showHelp(const std::string& progname) {
    std::cout << TlUtils::format("%s [options] INPUT_BASENAME OUTPUT_PATH", progname.c_str()) << std::endl;
    std::cout << " OPTIONS:" << std::endl;
    std::cout << "  -h:      show help" << std::endl;
    std::cout << "  -v:      verbose" << std::endl;
}

bool checkMatrixInfo(const std::string& rvmBasePath, TlMatrixObject::index_type* pNumOfRows,
                     TlMatrixObject::index_type* pNumOfCols, int* pNumOfSubunits, int* pSizeOfChunk, int* pSubunitID,
                     bool* pGuess) {
    // TlMatrixObject::index_type numOfRows = 0;
    // TlMatrixObject::index_type numOfCols = 0;
    // int numOfSubunits = 0;
    // int sizeOfChunk = 0;

    // TlMatrixObject::index_type& sizeOfVector = numOfCols;
    // TlMatrixObject::index_type& numOfVectors = numOfRows;

    int subunitID = 0;

    TlMatrixObject::HeaderInfo headerInfo;
    bool isLoadable = false;

    // guess start unit
    {
        int subunitID = 0;
        const std::string inputPath0 = TlDenseMatrix_arrays_mmap_Object::getFileName(rvmBasePath, subunitID);
        std::cerr << "inputPath0: " << inputPath0 << std::endl;
        isLoadable = TlMatrixUtils::getHeaderInfo(inputPath0, &headerInfo);

        std::cerr << "isLoadable: " << isLoadable << std::endl;
        *pGuess = isLoadable;
    }
    // try direct load
    if (isLoadable == false) {
        std::cerr << "rvmBasePath: " << rvmBasePath << std::endl;
        isLoadable = TlMatrixUtils::getHeaderInfo(rvmBasePath, &headerInfo);
        std::cerr << "isLoadable: " << isLoadable << std::endl;
    }

    if (isLoadable) {
        // numOfRows = numOfVectors;
        // numOfCols = sizeOfVector;

        *pNumOfRows = headerInfo.numOfVectors;
        *pNumOfCols = headerInfo.sizeOfVector;
        *pNumOfSubunits = headerInfo.numOfSubunits;
        *pSizeOfChunk = headerInfo.sizeOfChunk;
        *pSubunitID = headerInfo.subunitId;
    }

    return isLoadable;
}

int main(int argc, char* argv[]) {
    TlGetopt opt(argc, argv, "hv");

    if (opt["h"] == "defined") {
        showHelp(opt[0]);
        return EXIT_SUCCESS;
    }
    const bool verbose = (opt["v"] == "defined");

    if (opt.getCount() <= 2) {
        showHelp(opt[0]);
        return EXIT_FAILURE;
    }
    std::string inputBasePath = opt[1];
    std::string outputPath = opt[2];
    if (verbose) {
        std::cerr << "input base path: " << inputBasePath << std::endl;
        std::cerr << "output: " << outputPath << std::endl;
    }

    // transpose2CSFD(inputBaseName, outputPath, verbose, true);
    TlMatrixObject::index_type numOfRows = 0;
    TlMatrixObject::index_type numOfCols = 0;
    int numOfSubunits = 0;
    int sizeOfChunk = 0;
    int subunitID = 0;
    bool sequentialMode = false;
    const bool canGetMatrixInfo = checkMatrixInfo(inputBasePath, &numOfRows, &numOfCols, &numOfSubunits, &sizeOfChunk,
                                                  &subunitID, &sequentialMode);
    if (canGetMatrixInfo == false) {
        std::cerr << "can not open file: " << inputBasePath << std::endl;

        return EXIT_FAILURE;
    }

    if (verbose) {
        std::cerr << "rows: " << numOfRows << std::endl;
        std::cerr << "cols: " << numOfCols << std::endl;
        std::cerr << "units: " << numOfSubunits << std::endl;
        std::cerr << "chunk: " << sizeOfChunk << std::endl;
    }

    // prepare output file
    if (TlFile::isExistFile(outputPath)) {
        std::cerr << "overwrite output matrix: " << outputPath << std::endl;
        TlDenseGeneralMatrix_mmap outMat(outputPath);
        if ((outMat.getNumOfRows() != numOfRows) || (outMat.getNumOfCols() != numOfCols)) {
            std::cerr << TlUtils::format("matrix size mismatch (%d, %d) != (%d, %d).", numOfRows, numOfCols,
                                         outMat.getNumOfRows(), outMat.getNumOfCols())
                      << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        std::cerr << "creating output matrix: " << outputPath << std::endl;
        TlDenseGeneralMatrix_mmap outMat(outputPath, numOfRows, numOfCols);
        if (verbose) {
            std::cerr << "output matrix has been prepared by mmap." << std::endl;
        }
    }

    //
    TlDenseGeneralMatrix_mmap outMat(outputPath);
    if (sequentialMode) {
        for (int unit = 0; unit < numOfSubunits; ++unit) {
            std::cerr << TlUtils::format(">>>> unit %d/%d", unit, numOfSubunits - 1) << std::endl;
            const std::string inputPath = TlDenseMatrix_arrays_mmap_Object::getFileName(inputBasePath, unit);

            std::cerr << "load matrix: " << inputPath << std::endl;
            TlDenseGeneralMatrix_arrays_mmap_RowOriented inMat(inputPath);
            std::cerr << "convert layout" << std::endl;
            inMat.convertMemoryLayout("", verbose, verbose);
            std::cerr << "set" << std::endl;
            inMat.set2csfd(&outMat, verbose, verbose);
        }
    } else {
        std::cerr << TlUtils::format(">>>> unit %d/%d", subunitID, numOfSubunits - 1) << std::endl;
        TlDenseGeneralMatrix_arrays_mmap_RowOriented inMat(inputBasePath);
        inMat.convertMemoryLayout("", verbose, verbose);
        inMat.set2csfd(&outMat, verbose, verbose);
    }

    if (verbose) {
        std::cerr << "end." << std::endl;
    }
    return EXIT_SUCCESS;
}
