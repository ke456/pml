#ifndef __LZZ_PX_EXTRA__H
#define __LZZ_PX_EXTRA__H

/** \brief Additional functions for univariate polynomials over `zz_p`
 *
 * \file lzz_pX_extra.h
 * \author Seung Gyu Hyun, Vincent Neiger, Eric Schost
 * \version 0.1
 * \date 2018-12-15
 *
 */

#include <memory> // for unique_ptr
#include <NTL/lzz_pX.h>
#include <stdexcept>
#include <cmath>
#include "../mat_lzz_pX_extra/mat_lzz_pX_utils.h" // TODO probably to be removed, here for new FFT things, isn't it?

NTL_CLIENT

/** Returns `true` if the polynomial `a` is monic, and `false` otherwise */
inline bool is_monic(const zz_pX & a)
{ return (a.rep.length() > 0 && IsOne(a.rep[deg(a)])); }


/** @name Power series division
 * \anchor PowerSeriesDivision
 *
 *  Given two polynomials `a` and `b` and an integer `d`, with `a` invertible
 *  modulo `x^d` (that is, invertible constant coefficient), these functions
 *  compute the truncated power series division `f = b * a^{-1} mod x^d`.
 */
//@{

/** Computes `f = b * a^{-1} mod x^d`, requiring that the constant coefficient
 * of `a` is invertible (see @ref PowerSeriesDivision). Uses threshold to
 * choose between the naive algorithm and Newton iteration. The OUT parameter
 * `f` may alias the IN parameters `a` or `b`. */
void InvTruncMul(zz_pX & f, const zz_pX & b, const zz_pX & a, long d);

/** Computes and returns `b * a^{-1} mod x^d`, requiring that the constant
 * coefficient of `a` is invertible (see @ref PowerSeriesDivision). Uses
 * threshold to choose between the naive algorithm and Newton iteration. */
inline zz_pX InvTruncMul(const zz_pX & b, const zz_pX & a, long d)
{ zz_pX f; InvTruncMul(f, b, a, d); return f; }

//@} // doxygen group: Power series division

/** An abstract class for Taylor shift */
class zz_pX_shift
{
public:
    /** Empty constructor sets the bound to `-1` */
    zz_pX_shift() : d(-1) { };
    /** Constructor for a given bound */
    zz_pX_shift(long d) : d(d) { };

    /** Destructor */
    virtual ~zz_pX_shift(){}

    /** Computes the Taylor shift `g = f(x+c)` (`c` will be an attribute in
     * derived classes); `g` may alias `f` */
    virtual void shift(zz_pX & g, const zz_pX & f) const = 0;

protected:
    long d; /**< non-strict upper bound on the degree of the polynomials `f` to be shifted */
};


/** A class for Taylor shift with a divide and conquer algorithm, making no
 * assumption on the base ring `zz_p` */
class zz_pX_shift_DAC : public zz_pX_shift
{
public:  
    /** Empty constructor, sets the degree bound to `-1` */
    zz_pX_shift_DAC() { };

    /** Main constructor. Initializes the attributes `d` and `c` with the given
     * parameters, and performs some pre-computations stored in the other
     * attributes `cc`, `c3`, `precomp` defined as indicated in their
     * description */
    zz_pX_shift_DAC(long d, const zz_p& c);

    /** Computes the Taylor shift `g = f(x+c)`. The OUT parameter `g` may alias
     * the IN parameter `f`
     *
     * \todo speed-up : precompute for repeated right-multiplication by `precomp[idx]`
     */
    void shift(zz_pX& g, const zz_pX& f) const;

protected:
    Vec<zz_pX> precomp; /**< contains `(x+c)^{2**k}` for all integers `k` such
                          that `2**k` is at least `4` and strictly less than
                          `(d+1)/2` */
    zz_p c; /**< the shifting parameter */
    zz_p cc; /**< `cc = c*c`, the square of `c` */
    zz_p c3; /**< `c3 = 3*c` */
};

/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/* a class that does Taylor shift                             */
/* requires 1,...,d units mod p                               */
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/** A class for Taylor shift with a divide and conquer algorithm, assuming that
 * `2,...,d` are units in `zz_p` */
class zz_pX_shift_large_characteristic : public zz_pX_shift
{
public:  
    /** Empty constructor, sets the degree bound to `-1` */
    zz_pX_shift_large_characteristic() { };

    /** Main constructor. Initializes the attribute `d` with the given
     * parameters, and uses `c` to do some precomputation stored in protected
     * attributes `fact`, `ifact`, `v` as indicated in their description */
    zz_pX_shift_large_characteristic(long d, const zz_p & c);

    /** Computes the Taylor shift `g = f(x+c)`. The OUT parameter `g` may alias
     * the IN parameter `f` */
    void shift(zz_pX& g, const zz_pX& f) const;

protected:
    Vec<zz_p> fact; /**< `fact` contains the factorials `0!, 1!, 2!, ..., d!` */
    Vec<zz_p> ifact; /**< `ifact` is the entry-wise inverse of `fact`: entry `i` is `1/(i!)` */
    zz_pX v; /**< `v` is the sum of `(c**i / i!) x**i` for `0<=i<=d` */
};

/** Constructs a `zz_pX_shift` object of the right type, and returns a pointer
 * to it; if `2,...,d` are units in `zz_p` then it is a
 * `zz_pX_shift_large_characteristic` object, otherwise it is a
 * `zz_pX_shift_DAC` object */
std::unique_ptr<zz_pX_shift> get_shift(long d, const zz_p & c);

/** Computes the Taylor shift `g = f(x+c)`. The OUT parameter `g` may alias the
 * IN parameter `f`. This creates and discards a `zz_pX_shift` object */
void shift(zz_pX& g, const zz_pX & f, const zz_p & c);

/** Computes and returns the Taylor shift `f(x+c)`. This creates and discards a
 * `zz_pX_shift` object */
inline zz_pX shift(const zz_pX & f, const zz_p & c)
{ zz_pX g; shift(g, f, c); return g; }

/** Computes `c = a + (b << k)`, where the left shift means multiplication by
 * `X^k`. The OUT parameter `c` may alias `a` or `b`. */
void add_LeftShift(zz_pX & c, const zz_pX & a, const zz_pX & b, const long k);


// FFT (including TFT)
class zz_pX_FFT{
	zz_p **wtab = nullptr;
	zz_p **inv_wtab = nullptr;
	long p; // largest power of p

	// in-place algorithms
	// Forward algorithms also truncate, while the inverse does not
	
	// performs forward FT, where l is size of array and p is
	// near exponent st l <= 2^p
	void FFT(Vec<zz_p> &f, const long l, const long p);
	void FFT_t(Vec<zz_p> &f, const long l, const long p);
	
	// full inverse transforms, require size of arrays to be power of 2
	void iFFT(Vec<zz_p> &f, const long p,
		      long start = 0, long end = -1);
	void iFFT_t(Vec<zz_p> &f, const long p, 
			  long start = 0, long end = -1);
	
	// truncated inverse transforms
	void iTFT(Vec<zz_p> &f, long head, long tail, long last, long s,
			  const long p);
	void iTFT_t(Vec<zz_p> &f, long head, long tail, long last, long s,
			  const long p);


public:
	~zz_pX_FFT();

	// initializes with (2^p)-th root of unity
	zz_pX_FFT(const long &prime,const long p);
	
	// transforms
	void forward(Vec<zz_p> &out, const Vec<zz_p> &in);
	void forward_t(Vec<zz_p> &out, const Vec<zz_p> &in);
	
	// full inverse transforms
	void inverse(Vec<zz_p> &out, const Vec<zz_p> &in);
	void inverse_t(Vec<zz_p> &out, const Vec<zz_p> &in);
    
	// polynomial routines
	void mult(zz_pX &res, const zz_pX &a, const zz_pX &b);
	
	// requires deg(b) <= 2*deg(a) + 1
	void middle_prod(zz_pX &res, const zz_pX &a, const zz_pX &b);

	// polynomial matrix routines
	// TODO: move to more appropriate place after tests
	void mat_mult(Mat<zz_pX> &res, const Mat<zz_pX> &A, const Mat<zz_pX> &B);
	void mat_mp(Mat<zz_pX> &res, const Mat<zz_pX> &A, const Mat<zz_pX> &B);
};

#endif

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
