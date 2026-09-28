/*
 *	Copyright (c) 2026, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#include <uxhw.h>
#include "kernel-uxhw.h"

/*
 *	These kernels operate on distributional `double`s: a single evaluation
 *	propagates the complete input distributions through the computation, so the
 *	returned value carries the full output distribution. This is where you would
 *	place any computation that benefits from the UxHw distributional arithmetic
 *	API (e.g. `UxHwDoubleMixture`, `UxHwDoubleQuantile`, ...) for the demo you
 *	build from this template.
 */
double
calculateFirstOutputUxHw(
	const KernelParameters *    parameters,
	const double *              inputVariables)
{
	return (kDemoSpecificConstantSomeDoubleValue + parameters->someDouble) * \
	       (inputVariables[kInputDistributionIndexFirstInput] + inputVariables[kInputDistributionIndexSecondInput]);
}

double
calculateSecondOutputUxHw(
	const KernelParameters *    parameters,
	const double *              inputVariables)
{
	double distribution = (kDemoSpecificConstantSomeDoubleValue + parameters->someDouble) * \
	                      (inputVariables[kInputDistributionIndexFirstInput] - inputVariables[kInputDistributionIndexSecondInput]);

	/*
	 *	The second output is reported as a scalar, so reduce the distribution to
	 *	its mean (first moment). On the Signaloid platform `distribution` carries
	 *	the full probability distribution and `UxHwDoubleNthMoment(x, 1)` extracts
	 *	its mean.
	 */
	return UxHwDoubleNthMoment(distribution, 1);
}
