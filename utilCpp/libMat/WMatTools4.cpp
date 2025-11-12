/*
 *------------------------------------------------------------------------
 * WMat<T>Utilities.cpp - untility functions used by all project
 *
 * No  any Corporation or client funds were used to develop this code. 
 * But the numerical receip's SVD decomposition algorithm is adopted.
 *
 * $Id: wmatTools.cpp,v 1.2 2011/06/23 18:41:20 swu Exp $
 * Copyright (c) 2005 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

#include "WMatTools4.h"
namespace app {
	//todo: unsafe for large numbers
	uint32_t wmat_n_choose_k(uint32_t n, uint32_t k)
	{
		if (k > n) return 0;
		if (k * 2 > n) k = n - k;
		if (k == 0) return 1;

		uint32_t result = n;
		for (uint32_t i = 2; i <= k; ++i) {
			result *= (n - i + 1);
			result /= i;
		}
		return result;
	}


	//return the unique element of input $x$
	WMatI32 wmatUnique(const WMatI32& x)
	{
		const int32_t* myints = x.data();
		uint32_t L = x.size();
		std::vector<int32_t> myvector(L);
		std::vector<int32_t>::iterator it;

		// using default comparison:
		it = std::unique_copy(myints, myints + L, myvector.begin());
		std::sort(myvector.begin(), it);

		// using predicate comparison:
		it = std::unique_copy(myvector.begin(), it, myvector.begin());

		int n = it - myvector.begin();

		WMatI32 out(n, 1);
		for (int i = 0; i < n; ++i) {
			out(i) = myvector[i];
		}

		//out.print("out=");

		return out;
	}


	void wmat_n_choose_k_pattern(WMatU32& out, const uint32_t n, const uint32_t k, const std::vector<uint32_t>& in)
	{
		assert(in.size() == k);

		uint32_t m = wmat_n_choose_k(n, k);
		out.resize(m, k);

		std::vector<uint32_t> ints(in);
		uint32_t j = 0;
		do {
			for (uint32_t i = 0; i < k; ++i)
			{
				out(j, i) = ints[i];

				//std::cout << ints[i];
			}
			//std::cout << "\n";

			++j;
		} while (next_combination(ints.begin(), ints.begin() + k, ints.end()));

		assert(m == j);
	}
}
