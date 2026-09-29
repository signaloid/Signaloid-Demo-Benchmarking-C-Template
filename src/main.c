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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <uxhw.h>
#include "utilities.h"
#include "kernel.h"
#include "common.h"

#ifdef NO_OS_AVAILABLE

void
returnZeroNoOS(void);
#endif

int
main(int argc, char *  argv[])
{
	CommandLineArguments    arguments = (CommandLineArguments) { 0 };
	double                  inputVariables[kInputDistributionIndexMax];
	const char *            expectedInputHeaders[kInputDistributionIndexMax] = {
		"firstInputVariableName",
		"secondInputVariableName"
	};
	double                  outputVariables[kOutputVariableIndexMax];
	const char *            outputVariableNames[kOutputVariableIndexMax] = {
		"firstOutputVariableName",
		"secondOutputVariableName"
	};
	const char *            outputVariableDescriptions[kOutputVariableIndexMax] = {
		"First output variable",
		"Second output variable"
	};
	int                     outputVariableTypes[kOutputVariableIndexMax] = {
		[kOutputVariableIndexFirstOutput]   = kOutputVariableTypeDistribution,
		[kOutputVariableIndexSecondOutput]  = kOutputVariableTypeScalar,
	};
	const char *            applicationDescription          = "Demo Benchmarking Application";
	double *                monteCarloOutputSamples         = NULL;
	MeanAndVariance         monteCarloOutputMeanAndVariance = { 0 };
	clock_t                 start                   = 0;
	clock_t                 end                     = 0;
	double                  cpuTimeUsedInSeconds    = 0;

	/*
	 *	Get command line arguments.
	 */
	if (getCommandLineArguments(argc, argv, &arguments) != kCommonConstantReturnTypeSuccess)
	{
		return EXIT_FAILURE;
	}

	/*
	 *	Read input distributions from CSV if input from file is enabled.
	 */
	if (arguments.common.isInputFromFileEnabled)
	{
		if (readInputDoubleDistributionsFromCSV(
				arguments.common.inputFilePath,
				expectedInputHeaders,
				inputVariables,
				kInputDistributionIndexMax
		))
		{
			fprintf(stderr, "Error: Could not read from input CSV file \"%s\".\n", arguments.common.inputFilePath);

			return EXIT_FAILURE;
		}
	}

	/*
	 *	Allocate for `monteCarloOutputSamples` if in Monte Carlo mode.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		monteCarloOutputSamples = (double *) checkedMalloc(
			arguments.common.numberOfMonteCarloIterations * sizeof(double),
			__FILE__,
			__LINE__
		);
	}

	/*
	 *	Populate the kernel parameters from the command-line arguments. The
	 *	kernels operate on this struct rather than on `CommandLineArguments`, so
	 *	they do not depend on this application's command-line handling.
	 */
	KernelParameters kernelParameters = { .someDouble = arguments.someDouble };

	for (size_t ii = 0; ii < kInputDistributionIndexMax; ++ii)
	{
		kernelParameters.isInputVariableSet[ii]     = arguments.isInputVariableSet[ii];
		kernelParameters.presetInputVariables[ii]   = arguments.inputVariables[ii];
	}

	/*
	 *	Start timing if timing is enabled.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	/*
	 *	A scalar output collapses to a single number even in Monte Carlo mode
	 *	(see `outputVariableTypes`), so it does not go through the sample-based
	 *	post-processing below and is printed as a scalar.
	 */
	bool isSelectedOutputScalar =
		(arguments.common.outputSelect != kOutputVariableIndexMax) &&
		(outputVariableTypes[arguments.common.outputSelect] == kOutputVariableTypeScalar);

	/*
	 *	Dispatch to the mode-specific kernel.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		calculateOutputMonteCarlo(
			&kernelParameters,
			arguments.common.outputSelect,
			arguments.common.numberOfMonteCarloIterations,
			outputVariables,
			monteCarloOutputSamples
		);

		/*
		 *	For distributional outputs, approximate the cost of the third phase of
		 *	Monte Carlo (post-processing) by calculating the mean and variance, and
		 *	report the mean as the output value. Scalar outputs are already written
		 *	directly to `outputVariables[outputSelect]` by the kernel.
		 */
		if (!isSelectedOutputScalar)
		{
			monteCarloOutputMeanAndVariance = calculateMeanAndVarianceOfDoubleSamples(
				monteCarloOutputSamples,
				arguments.common.numberOfMonteCarloIterations
			);
			outputVariables[arguments.common.outputSelect] = monteCarloOutputMeanAndVariance.mean;
		}
	}
	else
	{
		calculateOutputUxHw(
			&kernelParameters,
			arguments.common.outputSelect,
			arguments.common.isInputFromFileEnabled,
			inputVariables,
			outputVariables
		);
	}

	/*
	 *	Stop timing if timing is enabled.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end = clock();
		cpuTimeUsedInSeconds = ((double) (end - start)) / CLOCKS_PER_SEC;
	}

	/*
	 *	For a scalar output in Monte Carlo mode, present a copy of the common
	 *	arguments with Monte Carlo disabled and iterations set to 1, so the common
	 *	print routines take their scalar code paths instead of computing
	 *	distribution statistics over a single-element array.
	 */
	CommonCommandLineArguments printArguments = arguments.common;

	if (arguments.common.isMonteCarloMode && isSelectedOutputScalar)
	{
		printArguments.isMonteCarloMode             = false;
		printArguments.numberOfMonteCarloIterations = 1;
	}

	/*
	 *	Print json outputs if in JSON output mode.
	 */
	if (arguments.common.isOutputJSONMode)
	{
		printJSONFormattedOutput(
			&printArguments,
			monteCarloOutputSamples,
			outputVariables,
			outputVariableNames,
			kOutputVariableIndexMax,
			applicationDescription
		);
	}
	/*
	 *	Print human-consumable output if not in JSON output mode.
	 */
	else
	{
		printHumanConsumableOutput(
			&printArguments,
			kOutputVariableIndexMax,
			outputVariables,
			outputVariableNames,
			outputVariableDescriptions,
			monteCarloOutputSamples
		);
	}

	/*
	 *	Print timing if timing is enabled.
	 */
	if (arguments.common.isTimingEnabled)
	{
		printf("\nCPU time used: %" SignaloidParticleModifier "lf seconds\n", cpuTimeUsedInSeconds);
	}

	/*
	 *	Save Monte Carlo data to "data.out" if in Monte Carlo mode.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		size_t samplesToSave = isSelectedOutputScalar ? 1 : arguments.common.numberOfMonteCarloIterations;

		saveMonteCarloDoubleDataToDataDotOutFile(
			monteCarloOutputSamples,
			(uint64_t) (cpuTimeUsedInSeconds * 1000000),
			samplesToSave
		);
	}
	/*
	 *	Save outputs to file if not in Monte Carlo mode and write to file is enabled.
	 */
	else
	{
		if (arguments.common.isWriteToFileEnabled)
		{
			if (writeOutputDoubleDistributionsToCSV(
					arguments.common.outputFilePath,
					outputVariables,
					outputVariableNames,
					kOutputVariableIndexMax
			))
			{
				fprintf(stderr, "Error: Could not write to output CSV file \"%s\".\n", arguments.common.outputFilePath);

				return EXIT_FAILURE;
			}
		}
	}

	/*
	 *	Free allocations.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		free(monteCarloOutputSamples);
	}

#ifdef NO_OS_AVAILABLE
	returnZeroNoOS();
#else

	return EXIT_SUCCESS;

#endif
}
