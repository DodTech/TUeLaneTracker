//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: run_Init_State_emxutil.cpp
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 28-Jan-2017 01:24:11
//

// Include Files
#include "rt_nonfinite.h"
#include "run_Init_State.h"
#include "run_Init_State_emxutil.h"

// Function Declarations
static void emxInit_real_T1(emxArray_real_T **pEmxArray, int numDimensions);

// Function Definitions

//
// Arguments    : emxArray_real_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
static void emxInit_real_T1(emxArray_real_T **pEmxArray, int numDimensions)
{
  emxArray_real_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real_T *)malloc(sizeof(emxArray_real_T));
  emxArray = *pEmxArray;
  emxArray->data = (double *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : MatlabStruct_laneFilter *pStruct
// Return Type  : void
//
void c_emxFreeStruct_MatlabStruct_la(MatlabStruct_laneFilter *pStruct)
{
  emxFree_real_T(&pStruct->mFilter);
  emxFree_real_T(&pStruct->mPrior);
}

//
// Arguments    : MatlabStruct_likelihoods *pStruct
// Return Type  : void
//
void c_emxFreeStruct_MatlabStruct_li(MatlabStruct_likelihoods *pStruct)
{
  emxFree_real_T(&pStruct->TOT_P_ALL);
  emxFree_real_T(&pStruct->DIR_ALL);
  emxFree_real_T(&pStruct->MASK_FOC_TOT_P);
  emxFree_real_T(&pStruct->TOT_P);
  emxFree_real_T(&pStruct->FOC_TOT_P);
  emxFree_real_T(&pStruct->AVG_DIR_TOT_P);
  emxFree_real_T(&pStruct->TOT_P_ALL_BACK_UP);
  emxFree_real_T(&pStruct->DIR_ALL_BACK_UP);
}

//
// Arguments    : MatlabStruct_templates *pStruct
// Return Type  : void
//
void c_emxFreeStruct_MatlabStruct_te(MatlabStruct_templates *pStruct)
{
  emxFree_real_T(&pStruct->ROOT_DIR_TEMPLATE);
  emxFree_real_T(&pStruct->ROOT_PROB_TEMPLATE);
  emxFree_real_T(&pStruct->ROOT_DEPTH_TEMPLATE);
  emxFree_real_T(&pStruct->SEGMENT);
}

//
// Arguments    : MatlabStruct_vpFilter *pStruct
// Return Type  : void
//
void c_emxFreeStruct_MatlabStruct_vp(MatlabStruct_vpFilter *pStruct)
{
  emxFree_real_T(&pStruct->mFilter);
  emxFree_real_T(&pStruct->mPrior);
}

//
// Arguments    : MatlabStruct_laneFilter *pStruct
// Return Type  : void
//
void c_emxInitStruct_MatlabStruct_la(MatlabStruct_laneFilter *pStruct)
{
  emxInit_real_T(&pStruct->mFilter, 2);
  emxInit_real_T(&pStruct->mPrior, 2);
}

//
// Arguments    : MatlabStruct_likelihoods *pStruct
// Return Type  : void
//
void c_emxInitStruct_MatlabStruct_li(MatlabStruct_likelihoods *pStruct)
{
  emxInit_real_T1(&pStruct->TOT_P_ALL, 3);
  emxInit_real_T1(&pStruct->DIR_ALL, 3);
  emxInit_real_T(&pStruct->MASK_FOC_TOT_P, 2);
  emxInit_real_T(&pStruct->TOT_P, 2);
  emxInit_real_T(&pStruct->FOC_TOT_P, 2);
  emxInit_real_T(&pStruct->AVG_DIR_TOT_P, 2);
  emxInit_real_T1(&pStruct->TOT_P_ALL_BACK_UP, 3);
  emxInit_real_T1(&pStruct->DIR_ALL_BACK_UP, 3);
}

//
// Arguments    : MatlabStruct_templates *pStruct
// Return Type  : void
//
void c_emxInitStruct_MatlabStruct_te(MatlabStruct_templates *pStruct)
{
  emxInit_real_T(&pStruct->ROOT_DIR_TEMPLATE, 2);
  emxInit_real_T(&pStruct->ROOT_PROB_TEMPLATE, 2);
  emxInit_real_T(&pStruct->ROOT_DEPTH_TEMPLATE, 2);
  emxInit_real_T(&pStruct->SEGMENT, 2);
}

//
// Arguments    : MatlabStruct_vpFilter *pStruct
// Return Type  : void
//
void c_emxInitStruct_MatlabStruct_vp(MatlabStruct_vpFilter *pStruct)
{
  emxInit_real_T(&pStruct->mFilter, 2);
  emxInit_real_T(&pStruct->mPrior, 2);
}

//
// Arguments    : emxArray__common *emxArray
//                int oldNumel
//                int elementSize
// Return Type  : void
//
void emxEnsureCapacity(emxArray__common *emxArray, int oldNumel, int elementSize)
{
  int newNumel;
  int i;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }

  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i <<= 1;
      }
    }

    newData = calloc((unsigned int)i, (unsigned int)elementSize);
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, (unsigned int)(elementSize * oldNumel));
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }

    emxArray->data = newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

//
// Arguments    : emxArray_boolean_T **pEmxArray
// Return Type  : void
//
void emxFree_boolean_T(emxArray_boolean_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_boolean_T *)NULL) {
    if (((*pEmxArray)->data != (boolean_T *)NULL) && (*pEmxArray)->canFreeData)
    {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_boolean_T *)NULL;
  }
}

//
// Arguments    : emxArray_int32_T **pEmxArray
// Return Type  : void
//
void emxFree_int32_T(emxArray_int32_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_int32_T *)NULL) {
    if (((*pEmxArray)->data != (int *)NULL) && (*pEmxArray)->canFreeData) {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_int32_T *)NULL;
  }
}

//
// Arguments    : emxArray_real32_T **pEmxArray
// Return Type  : void
//
void emxFree_real32_T(emxArray_real32_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_real32_T *)NULL) {
    if (((*pEmxArray)->data != (float *)NULL) && (*pEmxArray)->canFreeData) {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_real32_T *)NULL;
  }
}

//
// Arguments    : emxArray_real_T **pEmxArray
// Return Type  : void
//
void emxFree_real_T(emxArray_real_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_real_T *)NULL) {
    if (((*pEmxArray)->data != (double *)NULL) && (*pEmxArray)->canFreeData) {
      free((void *)(*pEmxArray)->data);
    }

    free((void *)(*pEmxArray)->size);
    free((void *)*pEmxArray);
    *pEmxArray = (emxArray_real_T *)NULL;
  }
}

//
// Arguments    : emxArray_boolean_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_boolean_T(emxArray_boolean_T **pEmxArray, int numDimensions)
{
  emxArray_boolean_T *emxArray;
  int i;
  *pEmxArray = (emxArray_boolean_T *)malloc(sizeof(emxArray_boolean_T));
  emxArray = *pEmxArray;
  emxArray->data = (boolean_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_int32_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_int32_T(emxArray_int32_T **pEmxArray, int numDimensions)
{
  emxArray_int32_T *emxArray;
  int i;
  *pEmxArray = (emxArray_int32_T *)malloc(sizeof(emxArray_int32_T));
  emxArray = *pEmxArray;
  emxArray->data = (int *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_real32_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_real32_T(emxArray_real32_T **pEmxArray, int numDimensions)
{
  emxArray_real32_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real32_T *)malloc(sizeof(emxArray_real32_T));
  emxArray = *pEmxArray;
  emxArray->data = (float *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_real_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions)
{
  emxArray_real_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real_T *)malloc(sizeof(emxArray_real_T));
  emxArray = *pEmxArray;
  emxArray->data = (double *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// Arguments    : emxArray_real_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInit_real_T2(emxArray_real_T **pEmxArray, int numDimensions)
{
  emxArray_real_T *emxArray;
  int i;
  *pEmxArray = (emxArray_real_T *)malloc(sizeof(emxArray_real_T));
  emxArray = *pEmxArray;
  emxArray->data = (double *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int *)malloc((unsigned int)(sizeof(int) * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

//
// File trailer for run_Init_State_emxutil.cpp
//
// [EOF]
//