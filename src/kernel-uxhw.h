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

#include "kernel.h"

/**
 *	@brief	UxHw single-evaluation kernel for the first output. Operates on the
 *		distributional input variables directly (one evaluation covers the
 *		full input distributions), returning a distributional result.
 *
 *	@param	parameters	: The model parameters.
 *	@param	inputVariables	: The (distributional) input variables.
 *	@return	double		: Returns the first output.
 */
double
calculateFirstOutputUxHw(
	const KernelParameters *    parameters,
	const double *              inputVariables);

/**
 *	@brief	UxHw single-evaluation kernel for the second output. Operates on the
 *		distributional input variables directly (one evaluation covers the
 *		full input distributions) and reduces the result to its mean.
 *
 *	@param	parameters	: The model parameters.
 *	@param	inputVariables	: The (distributional) input variables.
 *	@return	double		: Returns the second output.
 */
double
calculateSecondOutputUxHw(
	const KernelParameters *    parameters,
	const double *              inputVariables);
