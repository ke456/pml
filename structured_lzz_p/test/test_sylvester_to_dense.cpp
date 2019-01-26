#include <NTL/vec_lzz_p.h>
#include <assert.h>

#include "vec_lzz_p_extra.h"
#include "structured_lzz_p.h"

NTL_CLIENT

/*------------------------------------------------------------*/
/* creates sylvester matrices                                 */
/*------------------------------------------------------------*/
void check(long p)
{
    if (p == 0)
        zz_p::FFTInit(0);
    else
        zz_p::init(p);

    for (long i = 1; i < 100; i += 3)
    {
        for (long j = 2; j < 100; j += 3)
        {
            zz_pX F = random_zz_pX(i);
            zz_pX G = random_zz_pX(j);
            sylvester_lzz_p S;
            if (F != 0 && G != 0)
            {
                S = sylvester_lzz_p(F, G);
                Mat<zz_p> M = S.to_dense();
                if (p != 0 && p < (1L << 30) && (i + j) < 10)
                {
                    cout << "i=" << i << ", j=" << i << endl;
                    cout << "F=" << F << endl;
                    cout << "G=" << G << endl;
                    cout << M << endl;
                }
            }
        }
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
