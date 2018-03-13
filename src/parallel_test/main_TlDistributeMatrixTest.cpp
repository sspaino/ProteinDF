#include <cstdlib>
#include <iostream>
#include <string>

#include "TlCommunicate.h"
#include "TlDistributeMatrix.h"
#include "TlDistributeVector.h"
#include "TlMatrix.h"
#include "TlTime.h"

void showResultMessage(const std::string& sFunction, bool bIsPassed);
void showResultMessageAll(const std::string& sFunction, bool bIsPassed);

void testConstructer();
void testCopyConstructer();
void testSet();
void testOperatorPlusEqual();
void testOperatorMultiEqual();
void testOperatorMultiEqual2();
void testMulti();
void testMulti2();
void testSave();
void testLoad();
void testTranspose();
void testTranspose2();
void testInverse();
void testGetMaxAbsoluteElement();
// void testGetPartialMatrix();
void testGetPartialMatrix2();

int main(int argc, char* argv[]) {
  // initialize
  TlCommunicate& rComm = TlCommunicate::getInstance(argc, argv);
  TlDistributeMatrix::setSystemBlockSize(10);

  // ===================================================================
  testConstructer();
  testCopyConstructer();
  testSet();
  testOperatorPlusEqual();
  testOperatorMultiEqual();
  testOperatorMultiEqual2();
  testMulti();
  testMulti2();
  testSave();
  testLoad();
  testTranspose();
  testTranspose2();
  testInverse();
  testGetMaxAbsoluteElement();
  // testGetPartialMatrix();
  testGetPartialMatrix2();
  // ===================================================================

  // finalize
  rComm.finalize();
  return EXIT_SUCCESS;
}

void showResultMessage(const std::string& sFunction, bool bIsPassed) {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  rComm.barrier();
  if (rComm.isMaster() == true) {
    std::cout << "TEST: " << sFunction << "() ";
    std::cout << ((bIsPassed == true) ? "." : "F") << std::endl;
  }
  rComm.barrier();
}

void showResultMessageAll(const std::string& sFunction, bool bIsPassed) {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  int nProc = rComm.getNumOfProc();
  int nRank = rComm.getRank();

  rComm.barrier();
  if (rComm.isMaster() == true) {
    std::cout << "TEST: " << sFunction << "() ";
  }
  for (int i = 0; i < nProc; ++i) {
    if (i == nRank) {
      std::cout << ((bIsPassed == true) ? "." : "F");
      std::cout.flush();
    }
    rComm.barrier();
  }
  if (rComm.isMaster() == true) {
    std::cout << std::endl;
  }

  rComm.barrier();
}

void testConstructer() {
  TlDistributeMatrix matrix(100, 80);

  int nRow = matrix.getNumOfRows();
  int nCol = matrix.getNumOfCols();

  bool bIsPassed = ((nRow == 100) && (nCol == 80)) ? true : false;

  showResultMessageAll("testConstructer", bIsPassed);
}

void testCopyConstructer() {
  TlDistributeMatrix A(100, 80);
  A(10, 10) = 10.0;
  A(17, 4) = 56.0;
  A(21, 18) = -2.5;
  A(0, 10) = 12.0;

  // A.print(std::cout);

  const TlDistributeMatrix B(A);

  bool bIsPassed = true;
  if ((B.getNumOfRows() != 100) || (B.getNumOfCols() != 80)) {
    bIsPassed = false;
  }
  if (std::fabs(B(10, 10) - 10.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(B(17, 4) - 56.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(B(21, 18) - (-2.5)) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(B(0, 10) - 12.0) > 1.0E-16) {
    bIsPassed = false;
  }

  showResultMessageAll("testCopyConstructer", bIsPassed);
}

void testSet() {
  const int nMaxRow = 100;
  const int nMaxCol = 100;
  TlDistributeMatrix A(nMaxRow, nMaxCol);

  {
    TlCommunicate& rComm = TlCommunicate::getInstance();
    rComm.barrier();
    // std::cout << TlTime::getNow() << std::endl;

    for (int r = 0; r < nMaxRow; ++r) {
      for (int c = 0; c < nMaxCol; ++c) {
        // A.set(r, c, rand());
        A(r, c) = rand();
      }
    }

    rComm.barrier();

    for (int r = 0; r < nMaxRow; ++r) {
      for (int c = 0; c < nMaxCol; ++c) {
        A.set(r, c, rand());
        // A(r, c) = rand();
      }
    }

    rComm.barrier();
  }

  A(10, 10) = 10.0;
  A(17, 4) = 56.0;
  A(21, 18) = -2.5;
  A(0, 10) = 12.0;

  bool bIsPassed = true;
  if ((A.getNumOfRows() != nMaxRow) || (A.getNumOfCols() != nMaxCol)) {
    bIsPassed = false;
  }
  if (std::fabs(A(10, 10) - 10.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(17, 4) - 56.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(21, 18) - (-2.5)) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(0, 10) - 12.0) > 1.0E-16) {
    bIsPassed = false;
  }

  showResultMessageAll("testSet", bIsPassed);
}

void testOperatorPlusEqual() {
  TlDistributeMatrix A(25, 25);
  A(10, 10) = 10.0;
  A(17, 4) = 56.0;
  A(21, 18) = -2.5;
  A(0, 10) = 12.0;

  TlDistributeMatrix B(25, 25);
  B(10, 10) = 5.0;
  B(0, 10) = -12.0;
  B(3, 17) = 51.0;

  A += B;

  bool bIsPassed = true;
  if ((A.getNumOfRows() != 25) || (A.getNumOfCols() != 25)) {
    bIsPassed = false;
  }
  if (std::fabs(A(10, 10) - 15.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(17, 4) - 56.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(21, 18) - (-2.5)) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(0, 10) - 0.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(3, 17) - 51.0) > 1.0E-16) {
    bIsPassed = false;
  }

  showResultMessageAll("testOperatorPlusEqual", bIsPassed);
}

void testOperatorMultiEqual() {
  TlDistributeMatrix A(25, 25);
  A(10, 10) = 10.0;
  A(17, 4) = 56.0;
  A(21, 18) = -2.5;
  A(0, 10) = 12.0;

  A *= 3.0;

  bool bIsPassed = true;
  if ((A.getNumOfRows() != 25) || (A.getNumOfCols() != 25)) {
    bIsPassed = false;
  }
  if (std::fabs(A(10, 10) - 10.0 * 3.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(17, 4) - 56.0 * 3.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(21, 18) - (-2.5) * 3.0) > 1.0E-16) {
    bIsPassed = false;
  }
  if (std::fabs(A(0, 10) - 12.0 * 3.0) > 1.0E-16) {
    bIsPassed = false;
  }

  showResultMessageAll("testOperatorMultiEqual", bIsPassed);
}

void testOperatorMultiEqual2() {
  TlCommunicate& rComm = TlCommunicate::getInstance();

  int row = 100;
  int col = 100;
  TlDistributeMatrix A(row, col);
  TlMatrix eA(row, col);
  {
    double count = 1.0;
    for (int i = 0; i < row; ++i) {
      for (int j = 0; j < col; ++j) {
        A(i, j) = count;
        eA(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  TlDistributeMatrix B(col, row);
  TlMatrix eB(col, row);
  {
    double count = -5.0;
    for (int i = 0; i < col; ++i) {
      for (int j = 0; j < row; ++j) {
        B(i, j) = count;
        eB(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  A *= B;
  eA *= eB;

  bool bIsPassed = true;
  if ((A.getNumOfRows() != row) || (A.getNumOfCols() != row)) {
    bIsPassed = false;
  }

  for (int i = 0; i < row; ++i) {
    for (int j = 0; j < row; ++j) {
      double a = A(i, j);
      double ea = eA(i, j);

      if (fabs(a - ea) > 1.0E-5) {
        bIsPassed = false;
        if (rComm.isMaster() == true) {
          std::cout << TlUtils::format(
                           "(%2d, %2d): actual= %f, expect= %f, err= %e", i, j,
                           a, ea, fabs(a - ea))
                    << std::endl;
        }
      }
    }
  }

  showResultMessageAll("testOperatorMultiEqual2", bIsPassed);
}

void testMulti() {
  bool bIsPassed = true;

  TlDistributeMatrix A(9, 9);
  TlMatrix exactA(9, 9);
  {
    double count = 1.0;
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        A(i, j) = count;
        exactA(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  TlDistributeMatrix B(9, 9);
  TlMatrix exactB(9, 9);
  {
    double count = 1.82;
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        B(i, j) = count;
        exactB(i, j) = count;
        count -= 1.0 / 100;
      }
    }
  }

  TlDistributeMatrix C = A * B;
  TlMatrix exactC = exactA * exactB;

  // judge
  for (int r = 0; r < 9; ++r) {
    for (int s = 0; s < 9; ++s) {
      double c = C(r, s);
      double exact_c = exactC(r, s);

      if (fabs(c - exact_c) > 1.0E-5) {
        bIsPassed = false;
        std::cout << TlUtils::format("(%2d, %2d): actual = %f, expect = %f", r,
                                     s, c, exact_c)
                  << std::endl;
      }
    }
  }

  showResultMessageAll("testMulti", bIsPassed);
}

void testMulti2() {
  bool bIsPassed = true;

  int M = 100;
  int N = 30;

  TlDistributeMatrix A(M, N);
  TlMatrix exactA(M, N);
  {
    double count = 1.0;
    for (int i = 0; i < M; ++i) {
      for (int j = 0; j < N; ++j) {
        A(i, j) = count;
        exactA(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  TlDistributeVector B(N);
  TlVector exactB(N);
  {
    double count = 1.82;
    for (int i = 0; i < N; ++i) {
      B[i] = count;
      exactB[i] = count;
      count -= 1.0 / 100;
    }
  }

  TlDistributeVector C = A * B;
  TlVector exactC = exactA * exactB;

  // judge
  for (int r = 0; r < M; ++r) {
    double c = C[r];
    double exact_c = exactC[r];

    if (fabs(c - exact_c) > 1.0E-5) {
      bIsPassed = false;
      std::cout << TlUtils::format("[%2d]: actual = %f, expect = %f", r, c,
                                   exact_c)
                << std::endl;
    }
  }

  showResultMessageAll("testMulti2", bIsPassed);
}

void testSave() {
  bool bIsPassed = true;

  const int nRow = 100;
  const int nCol = 80;
  TlDistributeMatrix A(nRow, nCol);
  TlMatrix B(nRow, nCol);

  int count = 0;
  for (int r = 0; r < nRow; ++r) {
    for (int c = 0; c < nCol; ++c) {
      double v = double(count);
      A(r, c) = v;
      B(r, c) = v;

      ++count;
    }
  }

  A.save("test.matrix");
  B.save("test.TlMatrix.matrix");

  showResultMessageAll("tesSave", bIsPassed);
}

void testLoad() {
  bool bIsPassed = true;

  TlDistributeMatrix A;
  A.load("C.mtx");
  TlMatrix B;
  B.load("C.mtx");

  if ((A.getNumOfRows() != B.getNumOfRows()) ||
      (A.getNumOfCols() != B.getNumOfCols())) {
    std::cout << "row = " << A.getNumOfRows() << std::endl;
    std::cout << "col = " << A.getNumOfRows() << std::endl;
    bIsPassed = false;
  }

  const int nRow = A.getNumOfRows();
  const int nCol = A.getNumOfCols();
  for (int r = 0; r < nRow; ++r) {
    for (int c = 0; c < nCol; ++c) {
      if (fabs(A(r, c) - B(r, c)) > 1.0E-5) {
        bIsPassed = false;
        break;
      }
    }
  }

  showResultMessageAll("testLoad", bIsPassed);
}

void testTranspose() {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  bool bIsPassed = true;

  int row = 40;
  int col = 40;
  TlDistributeMatrix A(row, col);
  TlMatrix exactA(row, col);
  {
    double count = 1.0;
    for (int i = 0; i < row; ++i) {
      for (int j = 0; j < col; ++j) {
        A.set(i, j, count);
        exactA(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  A.transpose();
  exactA.transpose();

  // A.save("transA");
  // exactA.save("transAexact");

  // judge
  for (int r = 0; r < row; ++r) {
    for (int s = 0; s < col; ++s) {
      double a = A(r, s);
      double ea = exactA(r, s);
      if (fabs(a - ea) > 1.0E-12) {
        bIsPassed = false;
        if (rComm.isMaster() == true) {
          std::cout << TlUtils::format(
                           "(%2d, %2d): actual= %f, expect= %f, err= %e", r, s,
                           a, ea, fabs(a - ea))
                    << std::endl;
        }
      }
    }
  }

  showResultMessageAll("testTranspose", bIsPassed);
}

void testTranspose2() {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  bool bIsPassed = true;

  int row = 40;
  int col = 30;
  TlDistributeMatrix A(row, col);
  TlMatrix exactA(row, col);
  {
    double count = 1.0;
    for (int i = 0; i < row; ++i) {
      for (int j = 0; j < col; ++j) {
        A.set(i, j, count);
        exactA(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  A.transpose();
  exactA.transpose();

  // judge
  for (int r = 0; r < col; ++r) {
    for (int s = 0; s < row; ++s) {
      double a = A(r, s);
      double ea = exactA(r, s);
      if (fabs(a - ea) > 1.0E-12) {
        bIsPassed = false;
        if (rComm.isMaster() == true) {
          std::cout << TlUtils::format(
                           "(%2d, %2d): actual= %f, expect= %f, err= %e", r, s,
                           a, ea, fabs(a - ea))
                    << std::endl;
        }
      }
    }
  }

  showResultMessageAll("testTranspose2", bIsPassed);
}

void testInverse() {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  bool bIsPassed = true;

  TlDistributeMatrix A(9, 9);
  {
    double count = 1.0;
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j <= i; ++j) {
        A(i, j) = count;
        count += 1.0 / 100;
      }
    }
  }

  TlDistributeMatrix B = A;

  bool check = B.inverse();
  if (check == false) {
    std::cout << "inverse function returns false!" << std::endl;
  }

  TlDistributeMatrix C = A * B;

  // judge
  for (int r = 0; r < 9; ++r) {
    for (int s = 0; s < 9; ++s) {
      double c = C(r, s);
      double exact_c = (r == s) ? 1.0 : 0.0;

      if (fabs(c - exact_c) > 1.0E-12) {
        bIsPassed = false;
        if (rComm.isMaster() == true) {
          std::cout << TlUtils::format(
                           "(%2d, %2d): actual= %f, expect= %f, err= %e", r, s,
                           c, exact_c, fabs(c - exact_c))
                    << std::endl;
        }
        rComm.barrier();
      }
    }
  }

  showResultMessageAll("testInverse", bIsPassed);
}

void testGetMaxAbsoluteElement() {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  bool bIsPassed = true;

  TlDistributeMatrix A(100, 100);
  A(3, 17) = -51.0;
  A(3, 9) = 33.0;

  int row, col;
  double v = A.getMaxAbsoluteElement(&row, &col);

  if (fabs(v - 51.0) > 1.0E-10) {
    bIsPassed = false;
  }
  if ((row != 3) || (col != 17)) {
    if (rComm.isMaster() == true) {
      std::cout << TlUtils::format(
                       "(%2d, %2d): actual= %f, expect= %f, err= %e", row, col,
                       v, 51.0, fabs(v - 51.0))
                << std::endl;
    }
    bIsPassed = false;
  }

  showResultMessageAll("testGetMaxAbsoluteValue", bIsPassed);
}

// void testGetPartialMatrix()
// {
//     bool bIsPassed = true;

//     TlDistributeMatrix A(100, 100);
//     A(3, 17) = -51.0;
//     A(3,  9) = 33.0;

//     TlSparseMatrix B(100, 100);
//     B(1,  0) = 1.0;
//     B(3, 17) = 2.0;
//     B(3,  9) = 3.0;
//     B(51, 17) = 4.0;

//     A.getPartialMatrix(B);

//     if (fabs(B(1,  0) - 0.0) > 1.0E-10) {
//         bIsPassed = false;
//     }
//     if (fabs(B(3, 17) - (-51.0)) > 1.0E-10) {
//         bIsPassed = false;
//     }
//     if (fabs(B(3,  9) - 33.0) > 1.0E-10) {
//         bIsPassed = false;
//     }
//     if (fabs(B(51, 17) - 0.0) > 1.0E-10) {
//         bIsPassed = false;
//     }

//     showResultMessageAll("testPartialMatrix", bIsPassed);
// }

void testGetPartialMatrix2() {
  TlCommunicate& rComm = TlCommunicate::getInstance();
  bool bIsPassed = true;

  TlDistributeMatrix A(1000, 1000);
  A(3, 17) = -51.0;
  A(3, 9) = 33.0;

  TlSparseMatrix B(1000, 1000);
  B(1, 0) = 1.0;
  B(3, 17) = 2.0;
  B(3, 9) = 3.0;
  B(51, 17) = 4.0;

  int fin = 0;
  const int target = 0;
  const int END_LOOP = 99;
  if (rComm.getRank() != target) {
    rComm.iReceiveData(fin, target, END_LOOP);
  }
  // std::cerr << "start loop." << std::endl;
  while (true) {
    if (rComm.getRank() == target) {
      bool isComplete = A.getSparseMatrixX(&B);
      //             std::cerr << "is complete? "<< isComplete << std::endl;
      if (isComplete == true) {
        for (int i = 0; i < rComm.getNumOfProc(); ++i) {
          if (i != target) {
            rComm.sendData(fin, i, END_LOOP);
          }
        }
        break;
      }
    } else {
      A.getPartialMatrixX(NULL);
      if (rComm.test(fin) == true) {
        rComm.wait(fin);
        break;
      }
    }
  }
  // std::cerr << "end loop." << std::endl;
  A.getPartialMatrixX(NULL, true);

  if (rComm.getRank() == target) {
    if (fabs(B(1, 0) - 0.0) > 1.0E-10) {
      std::cerr << TlUtils::format("B(1, 0)=%f(actual), %f(expect)", B(1, 0),
                                   0.0)
                << std::endl;
      bIsPassed = false;
    }
    if (fabs(B(3, 17) - (-51.0)) > 1.0E-10) {
      std::cerr << TlUtils::format("B(3, 17)=%f(actual), %f(expect)", B(3, 17),
                                   -51.0)
                << std::endl;
      bIsPassed = false;
    }
    if (fabs(B(3, 9) - 33.0) > 1.0E-10) {
      std::cerr << TlUtils::format("B(3, 9)=%f(actual), %f(expect)", B(3, 9),
                                   33.0)
                << std::endl;
      bIsPassed = false;
    }
    if (fabs(B(51, 17) - 0.0) > 1.0E-10) {
      std::cerr << TlUtils::format("B(51, 17)=%f(actual), %f(expect)",
                                   B(51, 17), 0.0)
                << std::endl;
      bIsPassed = false;
    }
    if (target != 0) {
      rComm.sendData(bIsPassed, 0);
    }
  } else if (rComm.isMaster() == true) {
    if (target != 0) {
      rComm.receiveData(bIsPassed, target);
    }
  }

  showResultMessageAll("testPartialMatrix2", bIsPassed);
}
