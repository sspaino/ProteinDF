#include <vector>
#include <string>
#include "gtest/gtest.h"
#include "DfFunctional_B3LYP.h"

static const double EPS = 1.0E-10;

// =====================================================================
// data used from: http://www.cse.scitech.ac.uk/ccg/dft/data_pt_x_lda.html
//

// test1
//  rhoa= 0.17E+01 rhob= 0.17E+01 sigmaaa= 0.81E-11 sigmaab= 0.81E-11 sigmabb= 0.81E-11
//  zk            = -0.323236848314E+01
//  vrhoa         = -0.125186680903E+01
//  vrhob         = -0.125186680903E+01
//  vsigmaaa      = -0.144159896521E-02
//  vsigmaab      =  0.455761547380E-04
//  vsigmabb      = -0.144159896521E-02

//  v2rhoa2       = -0.218336878948E+00
//  v2rhoab       = -0.165375839434E-01
//  v2rhob2       = -0.218336878948E+00

//  v2rhoasigmaaa =  0.105592839796E-02
//  v2rhoasigmaab = -0.236203506407E-04
//  v2rhoasigmabb =  0.657036117021E-04
//  v2rhobsigmaaa =  0.657036117021E-04
//  v2rhobsigmaab = -0.236203506407E-04
//  v2rhobsigmabb =  0.105592839796E-02

//  v2sigmaaa2    =  0.182480573750E-04
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.182480573750E-04
TEST(DfFunctional_B3LYP, pointwise1)
{
    // input
    const double dRhoA = 0.17E+01;
    const double dRhoB = 0.17E+01;
    const double dGammaAA = 0.81E-11;
    const double dGammaAB = 0.81E-11;
    const double dGammaBB = 0.81E-11;
    
    // expected value
    const double zk = -0.323236848314E+01;
    const double vRhoA = -0.125186680903E+01;
    const double vRhoB = -0.125186680903E+01;
    const double vGammaAA = -0.144159896521E-02;
    const double vGammaAB =  0.455761547380E-04;
    const double vGammaBB = -0.144159896521E-02;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA, dRoundF_roundRhoB;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB, dRoundF_roundGammaBB;
    f.getDerivativeFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB,
                              &dRoundF_roundRhoA, &dRoundF_roundRhoB,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB, &dRoundF_roundGammaBB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


TEST(DfFunctional_B3LYP, pointwise1_RKS)
{
    // input
    const double dRhoA = 0.17E+01;
    //const double dRhoB = 0.17E+01;
    const double dGammaAA = 0.81E-11;
    //const double dGammaAB = 0.81E-11;
    //const double dGammaBB = 0.81E-11;
    
    // expected value
    const double zk = -0.323236848314E+01;
    const double vRhoA = -0.125186680903E+01;
    //const double vRhoB = -0.125186680903E+01;
    const double vGammaAA = -0.144159896521E-02;
    const double vGammaAB =  0.455761547380E-04;
    //const double vGammaBB = -0.144159896521E-02;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dGammaAA);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB;
    f.getDerivativeFunctional(dRhoA, dGammaAA,
                              &dRoundF_roundRhoA,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    //EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    //EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


// test2
//  rhoa= 0.17E+01 rhob= 0.17E+01 sigmaaa= 0.17E+01 sigmaab= 0.17E+01 sigmabb= 0.17E+01
//  zk            = -0.323714327026E+01
//  vrhoa         = -0.125005562910E+01
//  vrhob         = -0.125005562910E+01
//  vsigmaaa      = -0.141357814083E-02
//  vsigmaab      =  0.455761547380E-04
//  vsigmabb      = -0.141357814083E-02

//  v2rhoa2       = -0.220535209854E+00
//  v2rhoab       = -0.167224803305E-01
//  v2rhob2       = -0.220535209854E+00

//  v2rhoasigmaaa =  0.993868359460E-03
//  v2rhoasigmaab = -0.236203506407E-04
//  v2rhoasigmabb =  0.657036117021E-04
//  v2rhobsigmaaa =  0.657036117021E-04
//  v2rhobsigmaab = -0.236203506407E-04
//  v2rhobsigmabb =  0.993868359460E-03

//  v2sigmaaa2    =  0.150310955024E-04
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.150310955024E-04
TEST(DfFunctional_B3LYP, pointwise2)
{
    // input
    const double dRhoA = 0.17E+01;
    const double dRhoB = 0.17E+01;
    const double dGammaAA = 0.17E+01;
    const double dGammaAB = 0.17E+01;
    const double dGammaBB = 0.17E+01;
    
    // expected value
    const double zk = -0.323714327026E+01;
    const double vRhoA = -0.125005562910E+01;
    const double vRhoB = -0.125005562910E+01;
    const double vGammaAA = -0.141357814083E-02;
    const double vGammaAB =  0.455761547380E-04;
    const double vGammaBB = -0.141357814083E-02;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA, dRoundF_roundRhoB;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB, dRoundF_roundGammaBB;
    f.getDerivativeFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB,
                              &dRoundF_roundRhoA, &dRoundF_roundRhoB,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB, &dRoundF_roundGammaBB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


TEST(DfFunctional_B3LYP, pointwise2_RKS)
{
    // input
    const double dRhoA = 0.17E+01;
    //const double dRhoB = 0.17E+01;
    const double dGammaAA = 0.17E+01;
    //const double dGammaAB = 0.17E+01;
    //const double dGammaBB = 0.17E+01;
    
    // expected value
    const double zk = -0.323714327026E+01;
    const double vRhoA = -0.125005562910E+01;
    //const double vRhoB = -0.125005562910E+01;
    const double vGammaAA = -0.141357814083E-02;
    const double vGammaAB =  0.455761547380E-04;
    //const double vGammaBB = -0.141357814083E-02;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dGammaAA);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB;
    f.getDerivativeFunctional(dRhoA, dGammaAA,
                              &dRoundF_roundRhoA,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    //EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    //EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


// test3
//  rhoa= 0.15E+01 rhob= 0.15E+01 sigmaaa= 0.36E+02 sigmaab= 0.36E+02 sigmabb= 0.36E+02
//  zk            = -0.284301452788E+01
//  vrhoa         = -0.116897854637E+01
//  vrhob         = -0.116897854637E+01
//  vsigmaaa      = -0.127654157835E-02
//  vsigmaab      =  0.568120340362E-04
//  vsigmabb      = -0.127654157835E-02

//  v2rhoa2       = -0.264800968146E+00
//  v2rhoab       = -0.246962380202E-01
//  v2rhob2       = -0.264800968146E+00

//  v2rhoasigmaaa =  0.605149740433E-03
//  v2rhoasigmaab = -0.333125831681E-04
//  v2rhoasigmabb =  0.899525589269E-04
//  v2rhobsigmaaa =  0.899525589269E-04
//  v2rhobsigmaab = -0.333125831681E-04
//  v2rhobsigmabb =  0.605149740433E-03

//  v2sigmaaa2    =  0.667366853939E-05
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.667366853939E-05
TEST(DfFunctional_B3LYP, pointwise3)
{
    // input
    const double dRhoA = 0.15E+01;
    const double dRhoB = 0.15E+01;
    const double dGammaAA = 0.36E+02;
    const double dGammaAB = 0.36E+02;
    const double dGammaBB = 0.36E+02;
    
    // expected value
    const double zk = -0.284301452788E+01;
    const double vRhoA = -0.116897854637E+01;
    const double vRhoB = -0.116897854637E+01;
    const double vGammaAA = -0.127654157835E-02;
    const double vGammaAB =  0.568120340362E-04;
    const double vGammaBB = -0.127654157835E-02;
    
    // execute test
    DfFunctional_B3LYP f;
  
    double dFunctionalValue = f.getFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA, dRoundF_roundRhoB;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB, dRoundF_roundGammaBB;
    f.getDerivativeFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB,
                              &dRoundF_roundRhoA, &dRoundF_roundRhoB,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB, &dRoundF_roundGammaBB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


TEST(DfFunctional_B3LYP, pointwise3_RKS)
{
    // input
    const double dRhoA = 0.15E+01;
    //const double dRhoB = 0.15E+01;
    const double dGammaAA = 0.36E+02;
    //const double dGammaAB = 0.36E+02;
    //const double dGammaBB = 0.36E+02;
    
    // expected value
    const double zk = -0.284301452788E+01;
    const double vRhoA = -0.116897854637E+01;
    //const double vRhoB = -0.116897854637E+01;
    const double vGammaAA = -0.127654157835E-02;
    const double vGammaAB =  0.568120340362E-04;
    //const double vGammaBB = -0.127654157835E-02;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dGammaAA);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB;
    f.getDerivativeFunctional(dRhoA, dGammaAA,
                              &dRoundF_roundRhoA, &dRoundF_roundGammaAA, &dRoundF_roundGammaAB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    //EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    //EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


// test4
//  rhoa= 0.88E-01 rhob= 0.88E-01 sigmaaa= 0.87E-01 sigmaab= 0.87E-01 sigmabb= 0.87E-01
//  zk            = -0.734765385895E-01
//  vrhoa         = -0.471179307798E+00
//  vrhob         = -0.471179307798E+00
//  vsigmaaa      = -0.335031909041E-01
//  vsigmaab      =  0.709874878725E-02
//  vsigmabb      = -0.335031909041E-01

//  v2rhoa2       = -0.119087356581E+01
//  v2rhoab       = -0.461994975007E+00
//  v2rhob2       = -0.119087356581E+01

//  v2rhoasigmaaa = -0.123500815668E-01
//  v2rhoasigmaab = -0.654451937611E-01
//  v2rhoasigmabb =  0.868364651756E-01
//  v2rhobsigmaaa =  0.868364651756E-01
//  v2rhobsigmaab = -0.654451937611E-01
//  v2rhobsigmabb = -0.123500815668E-01

//  v2sigmaaa2    =  0.159379207669E+00
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.159379207669E+00
TEST(DfFunctional_B3LYP, pointwise4)
{
    // input
    const double dRhoA = 0.88E-01;
    const double dRhoB = 0.88E-01;
    const double dGammaAA = 0.87E-01;
    const double dGammaAB = 0.87E-01;
    const double dGammaBB = 0.87E-01;
    
    // expected value
    const double zk = -0.734765385895E-01;
    const double vRhoA = -0.471179307798E+00;
    const double vRhoB = -0.471179307798E+00;
    const double vGammaAA = -0.335031909041E-01;
    const double vGammaAB =  0.709874878725E-02;
    const double vGammaBB = -0.335031909041E-01;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA, dRoundF_roundRhoB;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB, dRoundF_roundGammaBB;
    f.getDerivativeFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB,
                              &dRoundF_roundRhoA, &dRoundF_roundRhoB,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB, &dRoundF_roundGammaBB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


TEST(DfFunctional_B3LYP, pointwise4_RKS)
{
    // input
    const double dRhoA = 0.88E-01;
    //const double dRhoB = 0.88E-01;
    const double dGammaAA = 0.87E-01;
    //const double dGammaAB = 0.87E-01;
    //const double dGammaBB = 0.87E-01;
    
    // expected value
    const double zk = -0.734765385895E-01;
    const double vRhoA = -0.471179307798E+00;
    //const double vRhoB = -0.471179307798E+00;
    const double vGammaAA = -0.335031909041E-01;
    const double vGammaAB =  0.709874878725E-02;
    //const double vGammaBB = -0.335031909041E-01;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dGammaAA);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB;
    f.getDerivativeFunctional(dRhoA, dGammaAA,
                              &dRoundF_roundRhoA, &dRoundF_roundGammaAA, &dRoundF_roundGammaAB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    //EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    //EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


//  rhoa= 0.18E+04 rhob= 0.18E+04 sigmaaa= 0.55E+00 sigmaab= 0.55E+00 sigmabb= 0.55E+00
//  zk            = -0.329091638490E+05
//  vrhoa         = -0.121623185279E+02
//  vrhob         = -0.121623185279E+02
//  vsigmaaa      = -0.137670025981E-06
//  vsigmaab      =  0.211226609041E-09
//  vsigmabb      = -0.137670025981E-06

//  v2rhoa2       = -0.221830902630E-02
//  v2rhoab       = -0.188590666589E-04
//  v2rhob2       = -0.221830902630E-02

//  v2rhoasigmaaa =  0.101015443866E-09
//  v2rhoasigmaab = -0.101154717316E-12
//  v2rhoasigmabb =  0.880291835026E-12
//  v2rhobsigmaaa =  0.880291835026E-12
//  v2rhobsigmaab = -0.101154717316E-12
//  v2rhobsigmabb =  0.101015443866E-09

//  v2sigmaaa2    =  0.145185185089E-16
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.145185185089E-16

//  rhoa= 0.18E+04 rhob= 0.18E+04 sigmaaa= 0.86E+04 sigmaab= 0.86E+04 sigmabb= 0.86E+04
//  zk            = -0.329091662150E+05
//  vrhoa         = -0.121623176525E+02
//  vrhob         = -0.121623176525E+02
//  vsigmaaa      = -0.137669901131E-06
//  vsigmaab      =  0.211226609041E-09
//  vsigmabb      = -0.137669901131E-06

//  v2rhoa2       = -0.221831014637E-02
//  v2rhoab       = -0.188590804811E-04
//  v2rhob2       = -0.221831014637E-02

//  v2rhoasigmaaa =  0.101015166421E-09
//  v2rhoasigmaab = -0.101154717316E-12
//  v2rhoasigmabb =  0.880291835026E-12
//  v2rhobsigmaaa =  0.880291835026E-12
//  v2rhobsigmaab = -0.101154717316E-12
//  v2rhobsigmabb =  0.101015166421E-09

//  v2sigmaaa2    =  0.145183686163E-16
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.145183686163E-16

//  rhoa= 0.16E+04 rhob= 0.16E+04 sigmaaa= 0.37E+10 sigmaab= 0.37E+10 sigmabb= 0.37E+10
//  zk            = -0.291662692953E+05
//  vrhoa         = -0.113556508147E+02
//  vrhob         = -0.113556508147E+02
//  vsigmaaa      = -0.125212508392E-06
//  vsigmaab      =  0.258807836033E-09
//  vsigmabb      = -0.125212508392E-06

//  v2rhoa2       = -0.274482776000E-02
//  v2rhoab       = -0.302896659912E-04
//  v2rhob2       = -0.274482776000E-02

//  v2rhoasigmaaa =  0.681575421953E-10
//  v2rhoasigmaab = -0.139564478099E-12
//  v2rhoasigmabb =  0.120262598780E-11
//  v2rhobsigmaaa =  0.120262598780E-11
//  v2rhobsigmaab = -0.139564478099E-12
//  v2rhobsigmabb =  0.681575421953E-10

//  v2sigmaaa2    =  0.565478173007E-17
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.565478173007E-17

//  rhoa= 0.26E+00 rhob= 0.26E+00 sigmaaa= 0.28E+00 sigmaab= 0.28E+00 sigmabb= 0.28E+00
//  zk            = -0.281966570096E+00
//  vrhoa         = -0.675950257463E+00
//  vrhob         = -0.675950257463E+00
//  vsigmaaa      = -0.132396547866E-01
//  vsigmaab      =  0.117685000648E-02
//  vsigmabb      = -0.132396547866E-01

//  v2rhoa2       = -0.805530438829E+00
//  v2rhoab       = -0.118106298362E+00
//  v2rhob2       = -0.805530438829E+00

//  v2rhoasigmaaa =  0.363141897451E-01
//  v2rhoasigmaab = -0.382783053810E-02
//  v2rhoasigmabb =  0.670327184932E-02
//  v2rhobsigmaaa =  0.670327184932E-02
//  v2rhobsigmaab = -0.382783053810E-02
//  v2rhobsigmabb =  0.363141897451E-01

//  v2sigmaaa2    =  0.830532973290E-02
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.830532973290E-02

//  rhoa= 0.53E+05 rhob= 0.53E+05 sigmaaa= 0.96E+05 sigmaab= 0.96E+05 sigmabb= 0.96E+05
//  zk            = -0.297394905871E+07
//  vrhoa         = -0.373790914295E+02
//  vrhob         = -0.373790914295E+02
//  vsigmaaa      = -0.151741774296E-08
//  vsigmaab      =  0.661349905989E-12
//  vsigmabb      = -0.151741774296E-08

//  v2rhoa2       = -0.233832366784E-03
//  v2rhoab       = -0.693519389161E-06
//  v2rhob2       = -0.233832366784E-03

//  v2rhoasigmaaa =  0.380539524393E-13
//  v2rhoasigmaab = -0.105409669788E-16
//  v2rhoasigmabb =  0.110312423429E-15
//  v2rhobsigmaaa =  0.110312423429E-15
//  v2rhobsigmaab = -0.105409669788E-16
//  v2rhobsigmabb =  0.380539524393E-13

//  v2sigmaaa2    =  0.193156282710E-22
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.193156282710E-22

//  rhoa= 0.47E+05 rhob= 0.47E+05 sigmaaa= 0.29E+14 sigmaab= 0.29E+14 sigmabb= 0.29E+14
//  zk            = -0.262386652992E+07
//  vrhoa         = -0.348927955181E+02
//  vrhob         = -0.348927955181E+02
//  vsigmaaa      = -0.139622416354E-08
//  vsigmaab      =  0.810227461136E-12
//  vsigmabb      = -0.139622416354E-08

//  v2rhoa2       = -0.289681929857E-03
//  v2rhoab       = -0.109149292582E-05
//  v2rhob2       = -0.289681929857E-03

//  v2rhoasigmaaa =  0.265777258573E-13
//  v2rhoasigmaab = -0.145695984127E-16
//  v2rhoasigmabb =  0.151867150707E-15
//  v2rhobsigmaaa =  0.151867150707E-15
//  v2rhobsigmaab = -0.145695984127E-16
//  v2rhobsigmabb =  0.265777258573E-13

//  v2sigmaaa2    =  0.781950966986E-23
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.781950966986E-23

//  rhoa= 0.15E+00 rhob= 0.15E+00 sigmaaa= 0.16E+00 sigmaab= 0.16E+00 sigmabb= 0.16E+00
//  zk            = -0.141317331691E+00
//  vrhoa         = -0.560497981293E+00
//  vrhob         = -0.560497981293E+00
//  vsigmaaa      = -0.221566363225E-01
//  vsigmaab      =  0.295820080262E-02
//  vsigmabb      = -0.221566363225E-01

//  v2rhoa2       = -0.110705643629E+01
//  v2rhoab       = -0.230182661300E+00
//  v2rhob2       = -0.110705643629E+01

//  v2rhoasigmaaa =  0.561817966912E-01
//  v2rhoasigmaab = -0.163609821297E-01
//  v2rhoasigmabb =  0.248982600284E-01
//  v2rhobsigmaaa =  0.248982600284E-01
//  v2rhobsigmaab = -0.163609821297E-01
//  v2rhobsigmabb =  0.561817966912E-01

//  v2sigmaaa2    =  0.393918647938E-01
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.393918647938E-01

// test12
//  rhoa= 0.35E+01 rhob= 0.00E+00 sigmaaa= 0.46E-10 sigmaab= 0.00E+00 sigmabb= 0.00E+00
//  zk            = -0.399892263115E+01
//  vrhoa         = -0.152022926953E+01
//  vrhob         =  0.000000000000E+00
//  vsigmaaa      = -0.569059565191E-03
//  vsigmaab      =  0.000000000000E+00
//  vsigmabb      =  0.000000000000E+00

//  v2rhoa2       = -0.143793255671E+00
//  v2rhoab       =  0.000000000000E+00
//  v2rhob2       =  0.000000000000E+00

//  v2rhoasigmaaa =  0.000000000000E+00
//  v2rhoasigmaab =  0.000000000000E+00
//  v2rhoasigmabb =  0.000000000000E+00
//  v2rhobsigmaaa =  0.000000000000E+00
//  v2rhobsigmaab =  0.000000000000E+00
//  v2rhobsigmabb =  0.000000000000E+00

//  v2sigmaaa2    =  0.101564081636E-05
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.000000000000E+00
TEST(DfFunctional_B3LYP, pointwise12)
{
    // input
    const double dRhoA = 0.35E+01;
    const double dRhoB = 0.00E+00;
    const double dGammaAA = 0.46E-10;
    const double dGammaAB = 0.00E+00;
    const double dGammaBB = 0.00E+00;
    
    // expected value
    const double zk = -0.399892263115E+01;
    const double vRhoA = -0.152022926953E+01;
    const double vRhoB =  0.000000000000E+00;
    const double vGammaAA = -0.569059565191E-03;
    const double vGammaAB =  0.000000000000E+00;
    const double vGammaBB =  0.000000000000E+00;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA, dRoundF_roundRhoB;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB, dRoundF_roundGammaBB;
    f.getDerivativeFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB,
                              &dRoundF_roundRhoA, &dRoundF_roundRhoB,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB, &dRoundF_roundGammaBB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}

// test13
//  rhoa= 0.35E+01 rhob= 0.00E+00 sigmaaa= 0.34E+01 sigmaab= 0.00E+00 sigmabb= 0.00E+00
//  zk            = -0.400085169220E+01
//  vrhoa         = -0.151949867012E+01
//  vrhob         =  0.000000000000E+00
//  vsigmaaa      = -0.565718309854E-03
//  vsigmaab      =  0.000000000000E+00
//  vsigmabb      =  0.000000000000E+00

//  v2rhoa2       = -0.144275565036E+00
//  v2rhoab       =  0.000000000000E+00
//  v2rhob2       =  0.000000000000E+00

//  v2rhoasigmaaa =  0.000000000000E+00
//  v2rhoasigmaab =  0.000000000000E+00
//  v2rhoasigmabb =  0.000000000000E+00
//  v2rhobsigmaaa =  0.000000000000E+00
//  v2rhobsigmaab =  0.000000000000E+00
//  v2rhobsigmabb =  0.000000000000E+00

//  v2sigmaaa2    =  0.951896165997E-06
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.000000000000E+00
TEST(DfFunctional_B3LYP, pointwise13)
{
    // input
    const double dRhoA = 0.35E+01;
    const double dRhoB = 0.00E+00;
    const double dGammaAA = 0.34E+01;
    const double dGammaAB = 0.00E+00;
    const double dGammaBB = 0.00E+00;
    
    // expected value
    const double zk = -0.400085169220E+01;
    const double vRhoA = -0.151949867012E+01;
    const double vRhoB =  0.000000000000E+00;
    const double vGammaAA = -0.565718309854E-03;
    const double vGammaAB =  0.000000000000E+00;
    const double vGammaBB =  0.000000000000E+00;
    
    // execute test
    DfFunctional_B3LYP f;
    
    double dFunctionalValue = f.getFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB);
    EXPECT_NEAR(zk, dFunctionalValue, EPS);
    
    double dRoundF_roundRhoA, dRoundF_roundRhoB;
    double dRoundF_roundGammaAA, dRoundF_roundGammaAB, dRoundF_roundGammaBB;
    f.getDerivativeFunctional(dRhoA, dRhoB, dGammaAA, dGammaAB, dGammaBB,
                              &dRoundF_roundRhoA, &dRoundF_roundRhoB,
                              &dRoundF_roundGammaAA, &dRoundF_roundGammaAB, &dRoundF_roundGammaBB);
    
    EXPECT_NEAR(vRhoA, dRoundF_roundRhoA, EPS);
    EXPECT_NEAR(vRhoB, dRoundF_roundRhoB, EPS);
    EXPECT_NEAR(vGammaAA, dRoundF_roundGammaAA, EPS);
    EXPECT_NEAR(vGammaAB, dRoundF_roundGammaAB, EPS);
    EXPECT_NEAR(vGammaBB, dRoundF_roundGammaBB, EPS);
}


//  rhoa= 0.30E+01 rhob= 0.00E+00 sigmaaa= 0.20E+03 sigmaab= 0.00E+00 sigmabb= 0.00E+00
//  zk            = -0.337825554312E+01
//  vrhoa         = -0.140181476241E+01
//  vrhob         =  0.000000000000E+00
//  vsigmaaa      = -0.542857473158E-03
//  vsigmaab      =  0.000000000000E+00
//  vsigmabb      =  0.000000000000E+00

//  v2rhoa2       = -0.183150035577E+00
//  v2rhoab       =  0.000000000000E+00
//  v2rhob2       =  0.000000000000E+00

//  v2rhoasigmaaa =  0.000000000000E+00
//  v2rhoasigmaab =  0.000000000000E+00
//  v2rhoasigmabb =  0.000000000000E+00
//  v2rhobsigmaaa =  0.000000000000E+00
//  v2rhobsigmaab =  0.000000000000E+00
//  v2rhobsigmabb =  0.000000000000E+00

//  v2sigmaaa2    =  0.454347701768E-06
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.000000000000E+00

//  rhoa= 0.58E-01 rhob= 0.00E+00 sigmaaa= 0.47E-01 sigmaab= 0.00E+00 sigmabb= 0.00E+00
//  zk            = -0.208918715508E-01
//  vrhoa         = -0.356452444497E+00
//  vrhob         =  0.000000000000E+00
//  vsigmaaa      = -0.563614622931E-01
//  vsigmaab      =  0.000000000000E+00
//  vsigmabb      =  0.000000000000E+00

//  v2rhoa2       = -0.248689706230E+01
//  v2rhoab       =  0.000000000000E+00
//  v2rhob2       =  0.000000000000E+00

//  v2rhoasigmaaa =  0.000000000000E+00
//  v2rhoasigmaab =  0.000000000000E+00
//  v2rhoasigmabb =  0.000000000000E+00
//  v2rhobsigmaaa =  0.000000000000E+00
//  v2rhobsigmaab =  0.000000000000E+00
//  v2rhobsigmabb =  0.000000000000E+00

//  v2sigmaaa2    =  0.497289960388E+00
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.000000000000E+00

//  rhoa= 0.82E+02 rhob= 0.81E+02 sigmaaa= 0.49E+07 sigmaab= 0.49E+07 sigmabb= 0.49E+07
//  zk            = -0.597657791792E+03
//  vrhoa         = -0.410849174687E+01
//  vrhob         = -0.408944405306E+01
//  vsigmaaa      = -0.480994006130E-05
//  vsigmaab      =  0.470705833739E-07
//  vsigmabb      = -0.483282909871E-05

//  v2rhoa2       = -0.199799370665E-01
//  v2rhoab       = -0.975401299409E-03
//  v2rhob2       = -0.200660427641E-01

//  v2rhoasigmaaa =  0.260989083811E-07
//  v2rhoasigmaab = -0.417456492670E-09
//  v2rhoasigmabb =  0.304958928738E-08
//  v2rhobsigmaaa =  0.311332607385E-08
//  v2rhobsigmaab = -0.605117741230E-09
//  v2rhobsigmabb =  0.258390382546E-07

//  v2sigmaaa2    =  0.305802669194E-12
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.311732004758E-12

//  rhoa= 0.39E+02 rhob= 0.38E+02 sigmaaa= 0.81E+06 sigmaab= 0.82E+06 sigmabb= 0.82E+06
//  zk            = -0.224706205597E+03
//  vrhoa         = -0.320409850337E+01
//  vrhob         = -0.317368254169E+01
//  vsigmaaa      = -0.121714301795E-04
//  vsigmaab      =  0.178441892970E-06
//  vsigmabb      = -0.122078926901E-04

//  v2rhoa2       = -0.317800393334E-01
//  v2rhoab       = -0.225886606202E-02
//  v2rhob2       = -0.318134299631E-01

//  v2rhoasigmaaa =  0.112752322775E-06
//  v2rhoasigmaab = -0.271686374867E-08
//  v2rhoasigmabb =  0.213816335145E-07
//  v2rhobsigmaaa =  0.223467723370E-07
//  v2rhobsigmaab = -0.555234667672E-08
//  v2rhobsigmabb =  0.106009545608E-06

//  v2sigmaaa2    =  0.504534758146E-11
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.517448857581E-11

//  rhoa= 0.13E+00 rhob= 0.95E-01 sigmaaa= 0.15E+00 sigmaab= 0.18E+00 sigmabb= 0.22E+00
//  zk            = -0.102364834001E+00
//  vrhoa         = -0.520539483697E+00
//  vrhob         = -0.505870943988E+00
//  vsigmaaa      = -0.258581782652E-01
//  vsigmaab      =  0.549392678215E-02
//  vsigmabb      = -0.197777936291E-01

//  v2rhoa2       = -0.128917089285E+01
//  v2rhoab       = -0.313272349310E+00
//  v2rhob2       = -0.341042522919E+00

//  v2rhoasigmaaa =  0.523813395958E-01
//  v2rhoasigmaab = -0.350608937430E-02
//  v2rhoasigmabb =  0.294447027718E-01
//  v2rhobsigmaaa =  0.593594411251E-01
//  v2rhobsigmaab = -0.879513086335E-01
//  v2rhobsigmabb = -0.101203673067E+00

//  v2sigmaaa2    =  0.534218945785E-01
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.530748327798E-01

//  rhoa= 0.78E-01 rhob= 0.31E-01 sigmaaa= 0.41E-02 sigmaab= 0.38E-02 sigmabb= 0.36E-02
//  zk            = -0.370908948340E-01
//  vrhoa         = -0.451012905270E+00
//  vrhob         = -0.373868193934E+00
//  vsigmaaa      = -0.833833556166E-01
//  vsigmaab      =  0.302842221212E-01
//  vsigmabb      = -0.144390664966E+00

//  v2rhoa2       = -0.176591577637E+01
//  v2rhoab       = -0.519622735771E+00
//  v2rhob2       = -0.244487009873E+01

//  v2rhoasigmaaa =  0.104993242174E+01
//  v2rhoasigmaab = -0.393482201769E-01
//  v2rhoasigmabb = -0.128113035972E+00
//  v2rhobsigmaaa =  0.276123718282E+00
//  v2rhobsigmaab = -0.131376004203E+01
//  v2rhobsigmabb =  0.176232793556E+01

//  v2sigmaaa2    =  0.175699045569E+01
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.152026875550E+02

//  rhoa= 0.50E+02 rhob= 0.49E+02 sigmaaa= 0.11E+06 sigmaab= 0.11E+06 sigmabb= 0.11E+06
//  zk            = -0.281521772558E+03
//  vrhoa         = -0.369507027847E+01
//  vrhob         = -0.367023184597E+01
//  vsigmaaa      = -0.145690716360E-04
//  vsigmaab      =  0.114039548301E-06
//  vsigmabb      = -0.148744723017E-04

//  v2rhoa2       = -0.253527345932E-01
//  v2rhoab       = -0.706072438256E-03
//  v2rhob2       = -0.257381602738E-01

//  v2rhoasigmaaa =  0.311415552822E-06
//  v2rhoasigmaab = -0.147803254770E-08
//  v2rhoasigmabb =  0.111574735738E-07
//  v2rhobsigmaaa =  0.115460511919E-07
//  v2rhobsigmaab = -0.262052086299E-08
//  v2rhobsigmabb =  0.322180557867E-06

//  v2sigmaaa2    =  0.110200710038E-10
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.116700363054E-10

//  rhoa= 0.40E+02 rhob= 0.40E+02 sigmaaa= 0.99E+05 sigmaab= 0.98E+05 sigmabb= 0.98E+05
//  zk            = -0.213622104931E+03
//  vrhoa         = -0.341660378816E+01
//  vrhob         = -0.341704385893E+01
//  vsigmaaa      = -0.186681440156E-04
//  vsigmaab      =  0.166064527957E-06
//  vsigmabb      = -0.186895846674E-04

//  v2rhoa2       = -0.300897507679E-01
//  v2rhoab       = -0.923280211534E-03
//  v2rhob2       = -0.300737760767E-01

//  v2rhoasigmaaa =  0.459163999447E-06
//  v2rhoasigmaab = -0.368764035839E-08
//  v2rhoasigmabb =  0.198031853578E-07
//  v2rhobsigmaaa =  0.198031853578E-07
//  v2rhobsigmaab = -0.368764035839E-08
//  v2rhobsigmabb =  0.460585502466E-06

//  v2sigmaaa2    =  0.213857448925E-10
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.214957808672E-10

//  rhoa= 0.12E+00 rhob= 0.10E+00 sigmaaa= 0.12E+00 sigmaab= 0.13E+00 sigmabb= 0.14E+00
//  zk            = -0.973549787919E-01
//  vrhoa         = -0.513422548189E+00
//  vrhob         = -0.498404166041E+00
//  vsigmaaa      = -0.279927749358E-01
//  vsigmaab      =  0.519372784598E-02
//  vsigmabb      = -0.257633827920E-01

//  v2rhoa2       = -0.127015732306E+01
//  v2rhoab       = -0.343297185496E+00
//  v2rhob2       = -0.957526903923E+00

//  v2rhoasigmaaa =  0.573950472502E-01
//  v2rhoasigmaab = -0.147046387568E-01
//  v2rhoasigmabb =  0.406497802386E-01
//  v2rhobsigmaaa =  0.593213054050E-01
//  v2rhobsigmaab = -0.667542710588E-01
//  v2rhobsigmabb = -0.258248118537E-01

//  v2sigmaaa2    =  0.742126486309E-01
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.832697038526E-01

//  rhoa= 0.48E-01 rhob= 0.25E-01 sigmaaa= 0.46E-02 sigmaab= 0.44E-02 sigmabb= 0.41E-02
//  zk            = -0.224618905609E-01
//  vrhoa         = -0.383393770852E+00
//  vrhob         = -0.343323903582E+00
//  vsigmaaa      = -0.125297829477E+00
//  vsigmaab      =  0.420751297066E-01
//  vsigmabb      = -0.137996683570E+00

//  v2rhoa2       = -0.256288375723E+01
//  v2rhoab       = -0.665869909022E+00
//  v2rhob2       = -0.188075111055E+01

//  v2rhoasigmaaa =  0.158911884007E+01
//  v2rhoasigmaab = -0.437905384553E-01
//  v2rhoasigmabb = -0.204783018941E-02
//  v2rhobsigmaaa =  0.800379663970E+00
//  v2rhobsigmaab = -0.237012133502E+01
//  v2rhobsigmabb = -0.529596660601E+00

//  v2sigmaaa2    =  0.551118679586E+01
//  v2sigmaaaab   =  0.000000000000E+00
//  v2sigmaaabb   =  0.000000000000E+00
//  v2sigmaab2    =  0.000000000000E+00
//  v2sigmaabbb   =  0.000000000000E+00
//  v2sigmabb2    =  0.178736912276E+02

