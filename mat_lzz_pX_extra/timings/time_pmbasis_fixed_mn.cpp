#include <iomanip>
#include <NTL/BasicThreadPool.h>
#include <numeric>
#include <random>

#include "util.h"
#include "mat_lzz_pX_approximant.h"
#include "mat_lzz_pX_interpolant.h"

std::ostream &operator<<(std::ostream &out, const VecLong &s)
{
    out << "[ ";
    for (auto &i: s)
        out << i << " ";
    return out << "]";
}

NTL_CLIENT

/*------------------------------------------------------------*/
/* run one bench for specified rdim,cdim,order                */
/*------------------------------------------------------------*/
void one_bench_pmbasis(long rdim, long cdim, long deg, long order)
{
    VecLong shift(rdim,0);

    double t1,t2;

    long nb_iter=0;

    double t_pmbasis_app=0.0;
    while (t_pmbasis_app<0.2)
    {
        Mat<zz_pX> pmat;
        random(pmat, rdim, cdim, deg+1);

        t1 = GetWallTime();
        Mat<zz_pX> appbas;
        VecLong rdeg(shift);
        pmbasis(appbas,pmat,order,rdeg);
        t2 = GetWallTime();

        t_pmbasis_app += t2-t1;
        ++nb_iter;
    }
    t_pmbasis_app /= nb_iter;

    double t_pmbasis_int=0.0;
    if (order<zz_p::modulus())
    {
        nb_iter=0;
        while (t_pmbasis_int<0.2)
        {
            Mat<zz_pX> pmat;
            random(pmat, rdim, cdim, deg+1);
            Vec<zz_p> pts(INIT_SIZE, order);
            std::iota(pts.begin(), pts.end(), 0);
            std::shuffle(pts.begin(), pts.end(), std::mt19937{std::random_device{}()});
        
            t1 = GetWallTime();
            Mat<zz_pX> intbas;
            VecLong rdeg(shift);
            pmbasis(intbas,pmat,pts,rdeg);
            t2 = GetWallTime();
        
            t_pmbasis_int += t2-t1;
            ++nb_iter;
        }
        t_pmbasis_int /= nb_iter;
    }
    else
        t_pmbasis_int=-1.0;

    double t_pmbasis_intgeom=0.0;
    if (2*order+1 < zz_p::modulus())
    {
        nb_iter=0;
        while (t_pmbasis_intgeom<0.2)
        {
            Mat<zz_pX> pmat;
            random(pmat, rdim, cdim, deg+1);
            // geometric in degree 'order' (bound on degree of intbas) requires an
            // element order at least 2*deg+1
            zz_p r;
            element_of_order(r, 2*order+1); 
            if (IsZero(r))
            {
                t_pmbasis_intgeom=-1.0; nb_iter=1;
                break;
            }

            t1 = GetWallTime();
            Vec<zz_p> pts;
            Mat<zz_pX> intbas;
            VecLong rdeg(shift);
            pmbasis_geometric(intbas,pmat,r,order,rdeg,pts);
            t2 = GetWallTime();
        
            t_pmbasis_intgeom += t2-t1;
            ++nb_iter;
        }
        t_pmbasis_intgeom /= nb_iter;
    }
    else
        t_pmbasis_intgeom=-1.0;

    cout << order;
    cout << "\t" << t_pmbasis_app << "\t" << t_pmbasis_int << "\t" << t_pmbasis_intgeom;

    cout << endl;
}

/*------------------------------------------------------------*/
/* main calls check                                           */
/*------------------------------------------------------------*/
int main(int argc, char ** argv)
{
    std::cout << std::fixed;
    std::cout << std::setprecision(8);

    if (argc!=5)
        throw std::invalid_argument("Usage: ./time_pmbasis nbits fftprime rdim cdim");
    // assume rdim>0 , cdim>0, order>0

    const long nbits = atoi(argv[1]);
    const bool fftprime = (atoi(argv[2])==1);

    SetNumThreads(1);

    if (fftprime)
    {
        cout << "# Bench mbasis, FFT prime p = ";
        if (nbits < 25)
        {
            zz_p::UserFFTInit(786433); // 20 bits
            cout << zz_p::modulus() << ", bit length = " << 20 << endl;
        }
        else if (nbits < 35)
        {
            zz_p::UserFFTInit(2013265921); // 31 bits
            cout << zz_p::modulus() << ", bit length = " << 31 << endl;
        }
        else if (nbits < 45)
        {
            zz_p::UserFFTInit(2748779069441); // 42 bits
            cout << zz_p::modulus() << ", bit length = " << 42 << endl;
        }
        else if (nbits < 61)
        {
            zz_p::FFTInit(0);
            std::cout << zz_p::modulus() << ", bit length = " << NumBits(zz_p::modulus()) << std::endl;
        }
        else
        {
            std::cout << "Asking for FFT prime with too large bitsize (> 60). Exiting." << std::endl;
            return 0;
        }
    }
    else
    {
        cout << "# Bench pmbasis, random prime p = ";
        zz_p::init(NTL::GenPrime_long(nbits));
        cout << zz_p::modulus() << ", bit length = " << nbits << endl;
    }
		    cout << "# d \t \\sigma \t approx \t \t int (general) \t \t int (geometric)" << endl;

    if (argc==5)
    {
        const long rdim = atoi(argv[3]);
        const long cdim = atoi(argv[4]);
        cout << "# rdim = " << rdim << ", cdim = " << cdim << endl;

				for(long order = 1; order <= 4096; order *= 2)
           one_bench_pmbasis(rdim,cdim,order-1,order);
    }
    
    return 0;
}

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
