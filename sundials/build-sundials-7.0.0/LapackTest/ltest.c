#define SUNDIALS_LAPACK_FUNC(name,NAME) name ## _
#define dcopy_f77 SUNDIALS_LAPACK_FUNC(dcopy, DCOPY)
#define dgetrf_f77 SUNDIALS_LAPACK_FUNC(dgetrf, DGETRF)
extern void dcopy_f77(int *n, const double *x, const int *inc_x, double *y, const int *inc_y);
extern void dgetrf_f77(const int *m, const int *n, double *a, int *lda, int *ipiv, int *info);
int main(void) {
int n=1;
double x=1.0;
double y=1.0;
dcopy_f77(&n, &x, &n, &y, &n);
dgetrf_f77(&n, &n, &x, &n, &n, &n);
return(0);
}
