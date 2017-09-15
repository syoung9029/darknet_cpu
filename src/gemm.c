#include "gemm.h"
#include <cblas.h>
#include <pthread.h>

#define THREAD_GEMM 3

static int _M;
static int _K;
static float* _A;
static int _lda;
static int _N;
static float* _C;
static int _ldc;
static float* _B;
static int _ldb;

void *gemm_thread(void *ptr)
{
    int i = *((int*) ptr);
    int j,k;
    register int m,n;
    register float A_PART;
    if(i == 0){
        for(i = 0; i < _M/THREAD_GEMM; ++i){
            m = i*_lda;
            for(k = 0; k < _K; ++k){
                A_PART = _A[m+k];
                n = k*_ldb;
                for(j = 0; j < _N; ++j){
                    _C[i*_ldc+j] += A_PART*_B[n+j];
                }
            }
        }
    }else if(i == 1){
        for(i = _M/THREAD_GEMM; i < 2*_M/THREAD_GEMM; ++i){
            m = i*_lda;
            for(k = 0; k < _K; ++k){
                A_PART = _A[m+k];
                n = k*_ldb;
                for(j = 0; j < _N; ++j){
                    _C[i*_ldc+j] += A_PART*_B[n+j];
                }
            }
        }
    }else{
        for(i = 2*_M/THREAD_GEMM; i < _M; ++i){
            m = i*_lda;
            for(k = 0; k < _K; ++k){
                A_PART = _A[m+k];
                n = k*_ldb;
                for(j = 0; j < _N; ++j){
                    _C[i*_ldc+j] += A_PART*_B[n+j];
                }
            }
        }
    }
}

void gemm_nn(int M, int N, int K, float ALPHA,
        float *A, int lda,
        float *B, int ldb,
        float *C, int ldc)
{
    int i,c;
    pthread_t gemm_ids[THREAD_GEMM];
    int indexes[THREAD_GEMM];

    _M=M;
    _K=K;
    _A=A;
    _lda=lda;
    _N=N;
    _C=C;
    _ldc=ldc;
    _B=B;
    _ldb=ldb;

    for (c = 0; c < THREAD_GEMM; ++c) {
        indexes[c] = c;
        pthread_create( &gemm_ids[c], NULL, gemm_thread, (void*)&indexes[c] );
    }

    for(i = 0; i < THREAD_GEMM; ++i ){
        pthread_join( gemm_ids[i], NULL );
    }
}

void gemm_cpu(int M, int N, int K, float ALPHA,
        float *A, int lda,
        float *B, int ldb,
        float *C, int ldc)
{
   gemm_nn(M, N, K, ALPHA,A,lda, B, ldb,C,ldc);
}

void gemm(int M, int N, int K, float ALPHA,
        float *A, int lda,
        float *B, int ldb,
        float *C, int ldc)
{
//    gemm_cpu(M, N, K, ALPHA, A, lda, B, ldb, C, ldc);
    float BETA = 1;
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, M, N, K, ALPHA, A, lda, B, ldb, BETA, C, ldc);
}
