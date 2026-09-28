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

#include <stddef.h>
#include "kernel-monte-carlo.h"
#include "kernel.h"

/*
 *	These kernels do not use the UxHw distributional arithmetic API. Instead they
 *	draw scalar input samples and accumulate one output sample per
 *	iteration into `monteCarloOutputSamples`. This is where you would place any
 *	sample-array computation (e.g. `fmax`-based payoffs, empirical quantiles,
 *	...) for the demo you build from this template.
 */
double
calculateFirstOutputMonteCarlo(
	const KernelParameters *    parameters,
	double *                    monteCarloOutputSamples,
	size_t                      numberOfMonteCarloIterations)
{
	double inputVariables[kInputDistributionIndexMax];

	for (size_t ii = 0; ii < numberOfMonteCarloIterations; ii++)
	{
		setInputVariables(parameters, inputVariables);

		monteCarloOutputSamples[ii] = (kDemoSpecificConstantSomeDoubleValue + parameters->someDouble) * \
		                              (inputVariables[kInputDistributionIndexFirstInput] + inputVariables[kInputDistributionIndexSecondInput]);
	}

	return monteCarloOutputSamples[numberOfMonteCarloIterations - 1];
}

double
calculateSecondOutputMonteCarlo(
	const KernelParameters *    parameters,
	double *                    monteCarloOutputSamples,
	size_t                      numberOfMonteCarloIterations)
{
	double inputVariables[kInputDistributionIndexMax];

	for (size_t ii = 0; ii < numberOfMonteCarloIterations; ii++)
	{
		setInputVariables(parameters, inputVariables);

		monteCarloOutputSamples[ii] = (kDemoSpecificConstantSomeDoubleValue + parameters->someDouble) * \
		                              (inputVariables[kInputDistributionIndexFirstInput] - inputVariables[kInputDistributionIndexSecondInput]);
	}

	return monteCarloOutputSamples[numberOfMonteCarloIterations - 1];
}
