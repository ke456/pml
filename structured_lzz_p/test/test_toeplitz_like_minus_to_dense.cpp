#include <NTL/vec_lzz_p.h>
#include <assert.h>

#include "vec_lzz_p_extra.h"
#include "structured_lzz_p.h"

NTL_CLIENT

/*------------------------------------------------------------*/
/* creates toeplitz matrices                                  */
/*------------------------------------------------------------*/
void check(long p)
{
    if (p == 0)
        zz_p::FFTInit(0);
    else
        zz_p::init(p);

    for (long i = 1; i < 100; i += (i < 50 ? 1 : 5))
    {
        long j, alpha;
        Mat<zz_p> G, H, tH, A, R;
        toeplitz_like_minus_lzz_p hl;
        alpha = 3;

        j = i;
        G = random_mat_zz_p(i, alpha);
        H = random_mat_zz_p(j, alpha);
        transpose(tH, H);
        hl = toeplitz_like_minus_lzz_p(G, H);
        A = hl.to_dense();
        R = toeplitz_lzz_p_phi_minus(A);
        assert (R == G*tH);

        j = max(1, i-4);
        G = random_mat_zz_p(i, alpha);
        H = random_mat_zz_p(j, alpha);
        transpose(tH, H);
        hl = toeplitz_like_minus_lzz_p(G, H);
        A = hl.to_dense();
        R = toeplitz_lzz_p_phi_minus(A);
        assert (R == G*tH);

        j = i+4;
        G = random_mat_zz_p(i, alpha);
        H = random_mat_zz_p(j, alpha);
        transpose(tH, H);
        hl = toeplitz_like_minus_lzz_p(G, H);
        A = hl.to_dense();
        R = toeplitz_lzz_p_phi_minus(A);
        assert (R == G*tH);

        j = max(1, i/4);
        G = random_mat_zz_p(i, alpha);
        H = random_mat_zz_p(j, alpha);
        transpose(tH, H);
        hl = toeplitz_like_minus_lzz_p(G, H);
        A = hl.to_dense();
        R = toeplitz_lzz_p_phi_minus(A);
        assert (R == G*tH);

        j = i*4;
        G = random_mat_zz_p(i, alpha);
        H = random_mat_zz_p(j, alpha);
        transpose(tH, H);
        hl = toeplitz_like_minus_lzz_p(G, H);
        A = hl.to_dense();
        R = toeplitz_lzz_p_phi_minus(A);
        assert (R == G*tH);
    }
}

/*------------------------------------------------------------*/
/* main just calls check()                                    */
/*------------------------------------------------------------*/
int main(int argc, char** argv)
{
    check(0);
    check(786433);
    check(288230376151711813);
    return 0;
}

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
