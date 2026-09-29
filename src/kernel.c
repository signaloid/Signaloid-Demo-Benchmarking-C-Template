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

#include <stdbool.h>
#include <uxhw.h>
#include "kernel.h"
#include "kernel-uxhw.h"
#include "kernel-monte-carlo.h"
#include "common.h"

void
setInputVariables(
	const KernelParameters *    parameters,
	double *                    inputVariables)
{
	if (parameters->isInputVariableSet[kInputDistributionIndexFirstInput])
	{
		inputVariables[kInputDistributionIndexFirstInput] = parameters->presetInputVariables[kInputDistributionIndexFirstInput];
	}
	else
	{
		inputVariables[kInputDistributionIndexFirstInput] = UxHwDoubleGaussDist(
			kDemoSpecificConstantFirstInputVariableGaussianMean,
			kDemoSpecificConstantFirstInputVariableGaussianStandardDeviation
		);
	}

	if (parameters->isInputVariableSet[kInputDistributionIndexSecondInput])
	{
		inputVariables[kInputDistributionIndexSecondInput] = parameters->presetInputVariables[kInputDistributionIndexSecondInput];
	}
	else
	{
		inputVariables[kInputDistributionIndexSecondInput] = UxHwDoubleGaussDist(
			kDemoSpecificConstantSecondInputVariableGaussianMean,
			kDemoSpecificConstantSecondInputVariableGaussianStandardDeviation
		);
	}

	return;
}

void
calculateOutputUxHw(
	const KernelParameters *    parameters,
	size_t                      outputSelect,
	bool                        inputVariablesAreProvided,
	double *                    inputVariables,
	double *                    outputVariables)
{
	bool calculateAllOutputs = (outputSelect == kOutputVariableIndexMax);

	/*
	 *	Draw the (distributional) input variables once. When the inputs are
	 *	provided by the caller (e.g. read from a file), `inputVariables` is
	 *	already populated.
	 */
	if (!inputVariablesAreProvided)
	{
		setInputVariables(parameters, inputVariables);
	}

	if (calculateAllOutputs || (outputSelect == kOutputVariableIndexFirstOutput))
	{
		outputVariables[kOutputVariableIndexFirstOutput] = calculateFirstOutputUxHw(parameters, inputVariables);
	}

	if (calculateAllOutputs || (outputSelect == kOutputVariableIndexSecondOutput))
	{
		outputVariables[kOutputVariableIndexSecondOutput] = calculateSecondOutputUxHw(parameters, inputVariables);
	}

	return;
}

void
calculateOutputMonteCarlo(
	const KernelParameters *    parameters,
	size_t                      outputSelect,
	size_t                      numberOfMonteCarloIterations,
	double *                    outputVariables,
	double *                    monteCarloOutputSamples)
{
	/*
	 *	Monte Carlo mode always selects exactly one output (enforced by the
	 *	caller), so we dispatch to the matching per-output Monte Carlo kernel,
	 *	which runs the iteration loop internally.
	 */
	if (outputSelect == kOutputVariableIndexFirstOutput)
	{
		/*
		 *	The first output is a distribution: keep the full sample buffer.
		 */
		outputVariables[kOutputVariableIndexFirstOutput] = calculateFirstOutputMonteCarlo(
			parameters,
			monteCarloOutputSamples,
			numberOfMonteCarloIterations
		);
	}
	else if (outputSelect == kOutputVariableIndexSecondOutput)
	{
		/*
		 *	The second output is a scalar: generate the per-sample values, then
		 *	reduce them to (for example) their mean. Store the scalar in both
		 *	`outputVariables` and `monteCarloOutputSamples[0]`.
		 */
		MeanAndVariance meanAndVariance;

		calculateSecondOutputMonteCarlo(
			parameters,
			monteCarloOutputSamples,
			numberOfMonteCarloIterations
		);

		meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(
			monteCarloOutputSamples,
			numberOfMonteCarloIterations
		);

		monteCarloOutputSamples[0] = meanAndVariance.mean;
		outputVariables[kOutputVariableIndexSecondOutput] = meanAndVariance.mean;
	}

	return;
}
