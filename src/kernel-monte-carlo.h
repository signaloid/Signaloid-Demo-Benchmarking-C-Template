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
#include "kernel.h"

/**
 *	@brief	Monte Carlo kernel for the first output. Runs
 *		`numberOfMonteCarloIterations` independent evaluations, drawing a
 *		fresh set of scalar input samples each iteration and writing the
 *		per-sample first output into `monteCarloOutputSamples`.
 *
 *	@param	parameters			: The model parameters.
 *	@param	monteCarloOutputSamples		: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples to generate.
 *	@return	double				: Returns the last element of `monteCarloOutputSamples`.
 */
double
calculateFirstOutputMonteCarlo(
	const KernelParameters *    parameters,
	double *                    monteCarloOutputSamples,
	size_t                      numberOfMonteCarloIterations);

/**
 *	@brief	Monte Carlo kernel for the second output. Runs
 *		`numberOfMonteCarloIterations` independent evaluations, drawing a
 *		fresh set of scalar input samples each iteration and writing the
 *		per-sample second output into `monteCarloOutputSamples`.
 *
 *	@param	parameters			: The model parameters.
 *	@param	monteCarloOutputSamples		: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples to generate.
 *	@return	double				: Returns the last element of `monteCarloOutputSamples`.
 */
double
calculateSecondOutputMonteCarlo(
	const KernelParameters *    parameters,
	double *                    monteCarloOutputSamples,
	size_t                      numberOfMonteCarloIterations);
