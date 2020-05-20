

#define S_FUNCTION_NAME  MatrixCalculation
#define S_FUNCTION_LEVEL 2

#include "string.h"
#include <fstream>
#include "simstruc.h"
#include "defines.h"
#include "H:\eigen\eigen3\Eigen\Sparse"
#include "H:\eigen\eigen3\Eigen\Dense"





#ifdef __cplusplus
extern "C" {
#endif

enum InputPort
{
    A_DGL = 0
    , C_DGL = 1
    , A_ALG1 = 2
    , A_ALG2 = 3
    , C_ALG = 4
    , CURRENT_MATRIX = 5
    , CURRENT_SEPARATOR = 6
    , VOLTAGE_MATRIX = 7
    , VOLTAGE_SEPARATOR = 8
    , LAST_INPUT_PORT
};

enum OutputPort
{
    STATES_VECTOR = 0
    , V_VECTOR = 1
    , I_VECTOR = 2
    , TOTAL_V = 3
    , TOTAL_I = 4
    , LAST_OUTPUT_PORT
};

#ifdef __cplusplus
}
#endif





////////////////SPARSE///MATRIX///////////////////////
typedef struct struct_SparseMatrix
{
    Eigen::SparseMatrix<double, Eigen::RowMajor> mMatrix;
} SparseMatrix;

typedef struct struct_Component
{
    size_t mRow;
    size_t mCol;
    double mVal;
} Component;

void FillMatrix(SimStruct *S, SparseMatrix *ret, int input, size_t nRows, size_t nCols, size_t lSep, int rSep)
{
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S, input);
    const size_t n = (rSep < 0) ? (*ssGetInputPortDimensions(S, input) / 3) : rSep;

    std::vector< Eigen::Triplet<double> > tripletList;
    tripletList.reserve(n);
    for(size_t i = lSep; i < n; ++i)
        tripletList.push_back( Eigen::Triplet<double>(static_cast<int>(*uPtrs[i*3] + 0.01), static_cast<int>(*uPtrs[i*3+1] + 0.01), *uPtrs[i*3+2]) );

    ret->mMatrix = Eigen::SparseMatrix<double, Eigen::RowMajor>(static_cast<int>(nRows), static_cast<int>(nCols));
    ret->mMatrix.setFromTriplets(tripletList.begin(), tripletList.end());

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}

void NegMatrix(SparseMatrix *ret, SparseMatrix *rhs)
{
    ret->mMatrix = - rhs->mMatrix;

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}

void AddMatrix(SparseMatrix *ret, SparseMatrix *lhs, SparseMatrix *rhs)
{
    ret->mMatrix = lhs->mMatrix + rhs->mMatrix;

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}

void MulMatrix(SparseMatrix *ret, SparseMatrix *lhs, SparseMatrix *rhs)
{
    ret->mMatrix = lhs->mMatrix * rhs->mMatrix;

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}

double DotProdMatrix(SparseMatrix *lhs, SparseMatrix *rhs)
{
    SparseMatrix ret;
    MulMatrix(&ret, lhs, rhs);
    return ret.mMatrix(0,0);
}

void InvMatrix(SparseMatrix *ret, SparseMatrix *rhs)
{
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> X = rhs->mMatrix;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> inv = X.inverse();
    ret->mMatrix = inv.sparseView();

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}

void AppendMatrix(SparseMatrix *ret, SparseMatrix *upper, SparseMatrix *lower)
{
    ret->mMatrix = Eigen::SparseMatrix<double, Eigen::RowMajor>(upper->mMatrix.rows() + lower->mMatrix.rows(), upper->mMatrix.cols());
    ret->mMatrix.topRows(upper->mMatrix.rows()) = upper->mMatrix;
    ret->mMatrix.bottomRows(lower->mMatrix.rows()) = lower->mMatrix;

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}

void MakeStatesVector(SimStruct *S, SparseMatrix *ret, SparseMatrix *negA_alg1, SparseMatrix *invA_alg2, int addDim)
{
    //Create dgl states
    std::vector< Eigen::Triplet<double> > tripletList;
    tripletList.reserve(DGL_COUNT);

    for(size_t i = 0; i < DGL_COUNT; ++i)
        if(ssGetContStates(S)[i] != 0)
            tripletList.push_back( Eigen::Triplet<double>(static_cast<int>(i), 0, ssGetContStates(S)[i]) );

    SparseMatrix statesDgl;
    statesDgl.mMatrix = Eigen::SparseMatrix<double, Eigen::RowMajor>(DGL_COUNT, 1);
    statesDgl.mMatrix.setFromTriplets(tripletList.begin(), tripletList.end());

#if ALG_COUNT > 0
    //Create alg states
    SparseMatrix tmpFactor;
    MulMatrix(&tmpFactor, negA_alg1, &statesDgl);

    SparseMatrix c_alg;
    FillMatrix(S, &c_alg, C_ALG, ALG_COUNT, 1, 0, -1);
    SparseMatrix negC_alg;
    NegMatrix(&negC_alg, &c_alg);

    SparseMatrix tmpSum;
    AddMatrix(&tmpSum, &negC_alg, &tmpFactor);

    SparseMatrix x_alg;
    MulMatrix(&x_alg, invA_alg2, &tmpSum);

    //Fuse into total states vector
    SparseMatrix statesMatrix;
    AppendMatrix(&statesMatrix, &statesDgl, &x_alg);
#else
    SparseMatrix statesMatrix = statesDgl;
#endif

    if(addDim > 0)
    {
        SparseMatrix add;
        add.mMatrix = Eigen::SparseMatrix<double, Eigen::RowMajor>(1, 1);
        add.mMatrix(0, 0) = 1.0;
        SparseMatrix statesMatrixP1;
        AppendMatrix(&statesMatrixP1, &statesMatrix, &add);
        ret->mMatrix = statesMatrixP1.mMatrix;
    }
    else
    {
        ret->mMatrix = statesMatrix.mMatrix;
    }

    //std::ofstream file("LastOutput.txt");
    //file << ret->mMatrix;
}
//////////////////////////////////////////////////////





#ifdef __cplusplus
extern "C" {
#endif

//static void mdlStart(SimStruct *S);

static void mdlInitializeSizes(SimStruct *S)
{
    size_t i = 0;

    //Parameters
    ssSetNumSFcnParams(S, 0);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }

    //Input ports
    if (!ssSetNumInputPorts(S, LAST_INPUT_PORT))
        return;
    ssSetInputPortWidth(S, A_ALG1, A_ALG1_DIM);
    ssSetInputPortWidth(S, A_ALG2, A_ALG2_DIM);
    ssSetInputPortWidth(S, C_ALG, C_ALG_DIM);
    ssSetInputPortWidth(S, A_DGL, A_DGL_DIM);
    ssSetInputPortWidth(S, C_DGL, C_DGL_DIM);
    ssSetInputPortWidth(S, CURRENT_MATRIX, CURRENT_MATRIX_DIM);
    ssSetInputPortWidth(S, CURRENT_SEPARATOR, NUMBER_OF_CELL_ELEMENTS + 2);
    ssSetInputPortWidth(S, VOLTAGE_MATRIX, VOLTAGE_MATRIX_DIM);
    ssSetInputPortWidth(S, VOLTAGE_SEPARATOR, NUMBER_OF_CELL_ELEMENTS + 2);

    for(i = 0; i < LAST_INPUT_PORT; ++i)
        ssSetInputPortDirectFeedThrough(S, i, 1);
    //for(i = CURRENT_SEPARATOR; i < LAST_INPUT_PORT; ++i)
    //    ssSetInputPortDirectFeedThrough(S, i, 0);

    //Output ports
    if (!ssSetNumOutputPorts(S, LAST_OUTPUT_PORT)) return;
    ssSetOutputPortVectorDimension(S, STATES_VECTOR, DGL_COUNT + ALG_COUNT);
    ssSetOutputPortVectorDimension(S, V_VECTOR, NUMBER_OF_CELL_ELEMENTS);
    ssSetOutputPortVectorDimension(S, I_VECTOR, NUMBER_OF_CELL_ELEMENTS);
    ssSetOutputPortVectorDimension(S, TOTAL_V, 1);
    ssSetOutputPortVectorDimension(S, TOTAL_I, 1);

    //Work vector
    ssSetNumIWork(S, 1);
    
    //States
    ssSetNumContStates(S, DGL_COUNT + ALG_COUNT);
    ssSetNumDiscStates(S, 0);

    //Sample times
    ssSetNumSampleTimes(S, PORT_BASED_SAMPLE_TIMES);

    //Miscellaneous
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
    ssSetOptions(S, 0);   /* general options (SS_OPTION_xx)        */
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime( S, 0, INHERITED_SAMPLE_TIME );
    ssSetOffsetTime( S, 0, FIXED_IN_MINOR_STEP_OFFSET );
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
}


#define MDL_START
#if defined(MDL_START)
static void mdlStart(SimStruct *S)
{
    //Initializing needed
    ssGetIWork(S)[0] = 1;
    
    // Set stateVector to zero
    int_T stateDimension = ssGetNumContStates(S);
    memset(ssGetContStates(S), 0, sizeof(real_T) * (DGL_COUNT + ALG_COUNT));

    //Set outputVector to zero
    for(int i = 0; i < LAST_OUTPUT_PORT; ++i)
    {
        int_T *outputPortDimension = ssGetOutputPortDimensions(S, i);
        memset(ssGetOutputPortRealSignal(S, i), 0, sizeof(real_T) * (*outputPortDimension));
    }
}
#endif // MDL_START

static void mdlOutputs(SimStruct *S, int_T tid)
{
    try {
    //Initializing needed
    if(ssGetIWork(S)[0])
    {
        // Set stateVector to zero
        int_T stateDimension = ssGetNumContStates(S);
        memset(ssGetContStates(S), 0, sizeof(real_T) * (DGL_COUNT + ALG_COUNT));
    
        //Set initial states
#if ALG_COUNT > 0
        SparseMatrix a_alg2;
        SparseMatrix c_alg;
        FillMatrix(S, &a_alg2, A_ALG2, ALG_COUNT, ALG_COUNT, 0, -1);
        FillMatrix(S, &c_alg, C_ALG, ALG_COUNT, 1, 0, -1);

        SparseMatrix negC_alg;
        SparseMatrix invA_alg2;
        SparseMatrix statesMatrix;
        NegMatrix(&negC_alg, &c_alg);
        InvMatrix(&invA_alg2, &a_alg2);
        MulMatrix(&statesMatrix, &invA_alg2, &negC_alg);

        for(size_t i = DGL_COUNT; i < DGL_COUNT + ALG_COUNT; ++i)
            ssGetContStates(S)[i] = statesMatrix.mMatrix(i - DGL_COUNT, 0);
#endif

        for(size_t i = 0; i < DGL_COUNT; ++i)
            ssGetContStates(S)[i] = 0.0;
        
        //Initializing no longer needed, because it has been done
        ssGetIWork(S)[0] = 0;
    }


    // Set outputVector to zero
    for(int i = 0; i < LAST_OUTPUT_PORT; ++i)
    {
        int_T *outputPortDimension = ssGetOutputPortDimensions(S, i);
        memset(ssGetOutputPortRealSignal(S, i), 0, sizeof(real_T) * (*outputPortDimension));
     }

    //Get states vector
#if ALG_COUNT > 0
    SparseMatrix a_alg1;
    SparseMatrix a_alg2;
    FillMatrix(S, &a_alg1, A_ALG1, ALG_COUNT, DGL_COUNT, 0, -1);
    FillMatrix(S, &a_alg2, A_ALG2, ALG_COUNT, ALG_COUNT, 0, -1);
    SparseMatrix negA_alg1;
    SparseMatrix invA_alg2;
    NegMatrix(&negA_alg1, &a_alg1);
    InvMatrix(&invA_alg2, &a_alg2);
    SparseMatrix statesMatrix;
    MakeStatesVector(S, &statesMatrix, &negA_alg1, &invA_alg2, 1);
#else
    SparseMatrix statesMatrix;
    MakeStatesVector(S, &statesMatrix, 0, 0, 1);
#endif

    //Set outputVector
    for(size_t i = 0; i < NUMBER_OF_CELL_ELEMENTS + 1; ++i)
    {
        SparseMatrix currentMatrix;
        size_t lSep = static_cast<size_t>(*ssGetInputPortRealSignalPtrs(S, CURRENT_SEPARATOR)[i] + 0.01);
        int rSep = static_cast<int>(*ssGetInputPortRealSignalPtrs(S, CURRENT_SEPARATOR)[i+1] + 0.01);
        FillMatrix(S, &currentMatrix, CURRENT_MATRIX, 1, DGL_COUNT + ALG_COUNT + 1, lSep, rSep);

        SparseMatrix voltageMatrix;
        lSep = static_cast<size_t>(*ssGetInputPortRealSignalPtrs(S, VOLTAGE_SEPARATOR)[i] + 0.01);
        rSep = static_cast<int>(*ssGetInputPortRealSignalPtrs(S, VOLTAGE_SEPARATOR)[i+1] + 0.01);
        FillMatrix(S, &voltageMatrix, VOLTAGE_MATRIX, 1, DGL_COUNT + ALG_COUNT + 1, lSep, rSep);

        if(i < NUMBER_OF_CELL_ELEMENTS)
        {
            ssGetOutputPortRealSignal(S, I_VECTOR)[i] = DotProdMatrix(&currentMatrix, &statesMatrix);
            ssGetOutputPortRealSignal(S, V_VECTOR)[i] = DotProdMatrix(&voltageMatrix, &statesMatrix);
        }
        else
        {
            ssGetOutputPortRealSignal(S, TOTAL_I)[i] = DotProdMatrix(&currentMatrix, &statesMatrix);
            ssGetOutputPortRealSignal(S, TOTAL_V)[i] = DotProdMatrix(&voltageMatrix, &statesMatrix);
        }
    }
    for(size_t i = 0; i < DGL_COUNT + ALG_COUNT; ++i)
        ssGetOutputPortRealSignal(S, STATES_VECTOR)[i] = ssGetContStates(S)[i];
    } catch (...) {
        int i = 0;
    }
}

#define MDL_DERIVATIVES
static void mdlDerivatives(SimStruct *S)
{
    try {
    //Set derivatives to zero
    int_T stateDimension = ssGetNumContStates(S);
    memset(ssGetdX(S), 0, sizeof(real_T) * (DGL_COUNT + ALG_COUNT));

    //Set derivatives
    SparseMatrix a_dgl;
    SparseMatrix c_dgl;
    FillMatrix(S, &a_dgl, A_DGL, DGL_COUNT, DGL_COUNT + ALG_COUNT, 0, -1);
    FillMatrix(S, &c_dgl, C_DGL, DGL_COUNT, 1, 0, -1);

#if ALG_COUNT > 0
    SparseMatrix a_alg1;
    SparseMatrix a_alg2;
    FillMatrix(S, &a_alg1, A_ALG1, ALG_COUNT, DGL_COUNT, 0, -1);
    FillMatrix(S, &a_alg2, A_ALG2, ALG_COUNT, ALG_COUNT, 0, -1);

    SparseMatrix factor;
    SparseMatrix negA_alg1;
    SparseMatrix invA_alg2;
    NegMatrix(&negA_alg1, &a_alg1);
    InvMatrix(&invA_alg2, &a_alg2);
    MulMatrix(&factor, &invA_alg2, &negA_alg1);

    SparseMatrix a_2;
    SparseMatrix c_2;
    MulMatrix(&a_2, &factor, &a_dgl);
    MulMatrix(&c_2, &factor, &c_dgl);

    SparseMatrix a;
    SparseMatrix c;
    AppendMatrix(&a, &a_dgl, &a_2);
    AppendMatrix(&c, &c_dgl, &c_2);
#else
    SparseMatrix &a = a_dgl;
    SparseMatrix &c = c_dgl;
#endif

    SparseMatrix freePart;
    SparseMatrix statesMatrix;
    SparseMatrix dxdtMatrix;
#if ALG_COUNT > 0
    MakeStatesVector(S, &statesMatrix, &negA_alg1, &invA_alg2, 0);
#else
    MakeStatesVector(S, &statesMatrix, 0, 0, 0);
#endif

    MulMatrix(&freePart, &a, &statesMatrix);
    AddMatrix(&dxdtMatrix, &freePart, &c);

    for(size_t i = 0; i < DGL_COUNT + ALG_COUNT; ++i)
        ssGetdX(S)[i] = dxdtMatrix.mMatrix(i, 0);
    } catch (...) {
        int i = 0;
    }
}

static void mdlTerminate(SimStruct *S)
{}

#ifdef __cplusplus
}
#endif

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif