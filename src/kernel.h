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

#pragma once

#include <stdlib.h>
#include <stdbool.h>


typedef enum
{
	kInputDistributionIndexFirstInput = 0,
	kInputDistributionIndexSecondInput,
	kInputDistributionIndexMax,
} InputDistributionIndex;

typedef enum
{
	kOutputVariableIndexFirstOutput = 0,
	kOutputVariableIndexSecondOutput,
	kOutputVariableIndexMax,
} OutputVariableIndex;

#define kDemoSpecificConstantFirstInputVariableGaussianMean                 (0.0)
#define kDemoSpecificConstantFirstInputVariableGaussianStandardDeviation    (1.0)
#define kDemoSpecificConstantSecondInputVariableGaussianMean                (1.0)
#define kDemoSpecificConstantSecondInputVariableGaussianStandardDeviation   (2.0)
#define kDemoSpecificConstantSomeDoubleValue                                (0.93)

/**
 *	@brief	Model parameters consumed by the kernels. This deliberately mirrors
 *		only the fields the kernels need, decoupling them from the demo's
 *		`CommandLineArguments`. The caller populates it (e.g. from
 *		command-line arguments) before invoking the kernels.
 */
typedef struct
{
	/*
	 *	Demo-specific model coefficient.
	 */
	double someDouble;

	/*
	 *	Whether each input variable has a preset (fixed) value. When false, the
	 *	kernel draws that input from its distribution.
	 */
	bool isInputVariableSet[kInputDistributionIndexMax];

	/*
	 *	Preset values for the input variables that are fixed.
	 */
	double presetInputVariables[kInputDistributionIndexMax];
} KernelParameters;

/**
 *	@brief	Set the input variables, using a preset value where one is provided
 *		in `parameters` and otherwise drawing from the input distribution via
 *		a UxHw call. On the Signaloid platform the UxHw call returns a
 *		distributional `double`; when running natively it returns a single
 *		Monte Carlo sample. Shared by both the UxHw and Monte Carlo kernels.
 *
 *	@param	parameters	: The model parameters.
 *	@param	inputVariables	: The input variables to populate.
 */
void
setInputVariables(
	const KernelParameters *    parameters,
	double *                    inputVariables);

/**
 *	@brief	UxHw calculation kernel. Computes the selected output(s) with a
 *		single distributional evaluation, writing per-output results into
 *		`outputVariables`.
 *
 *	@param	parameters			: The model parameters.
 *	@param	outputSelect			: Which output to compute, or `kOutputVariableIndexMax` for all.
 *	@param	inputVariablesAreProvided	: If true, `inputVariables` already holds the values to use and is not re-sampled.
 *	@param	inputVariables			: Scratch/working buffer for the input variables.
 *	@param	outputVariables			: Array of size `kOutputVariableIndexMax` to fill.
 */
void
calculateOutputUxHw(
	const KernelParameters *    parameters,
	size_t                      outputSelect,
	bool                        inputVariablesAreProvided,
	double *                    inputVariables,
	double *                    outputVariables);

/**
 *	@brief	Monte Carlo calculation kernel. Runs `numberOfMonteCarloIterations`
 *		independent evaluations of the single selected output into
 *		`monteCarloOutputSamples`, and writes the resulting value into
 *		`outputVariables[outputSelect]`.
 *
 *	@param	parameters			: The model parameters.
 *	@param	outputSelect			: Which output to compute.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples to generate.
 *	@param	outputVariables			: Array of size `kOutputVariableIndexMax` to fill.
 *	@param	monteCarloOutputSamples		: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 */
void
calculateOutputMonteCarlo(
	const KernelParameters *    parameters,
	size_t                      outputSelect,
	size_t                      numberOfMonteCarloIterations,
	double *                    outputVariables,
	double *                    monteCarloOutputSamples);
