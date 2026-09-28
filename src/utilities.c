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

#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <uxhw.h>
#include <assert.h>
#include "utilities.h"
#include "common.h"

static const char * kDefaultSomeString  = "some-string";
static const int    kDefaultSomeInt     = 1;
static const double kDefaultSomeDouble  = 0.5;

void
printUsage(void)
{
	fprintf(stderr, "Example: Template for Signaloid C demos that handles both the Laplace and native MC executions in a single main\n");
	fprintf(stderr, "\n");
	printCommonUsage();
	fprintf(
		stderr,
		"	[-f, --first-input-variable <first input variable: double (Default: Gauss(%lf, %lf))>]\n"
		"	[-s, --second-input-variable <second input variable: double (Default: Gauss(%lf, %lf))>]\n"
		"	[-c, --long-option-string <path to some file: str (Default: %s)>]\n"
		"	[-e, --long-option-int <some integer variable : int (Default: %d)>]\n"
		"	[-x, --long-option-double <some double variable : double (Default: %lf)>]\n"
		"	[-p, --long-option-bool] (Whatever specifying the bool option does.)\n",
		kDemoSpecificConstantFirstInputVariableGaussianMean,
		kDemoSpecificConstantFirstInputVariableGaussianStandardDeviation,
		kDemoSpecificConstantSecondInputVariableGaussianMean,
		kDemoSpecificConstantSecondInputVariableGaussianStandardDeviation,
		kDefaultSomeString,
		kDefaultSomeInt,
		kDefaultSomeDouble
	);
	fprintf(stderr, "\n");

	return;
}

/**
 *	@brief	Set the default values for the command line arguments.
 *
 *	@param	arguments	: command line arguments pointer.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
static CommonConstantReturnType
setDefaultCommandLineArguments(CommandLineArguments * arguments)
{
	if (arguments == NULL)
	{
		fprintf(stderr, "Error: The provided pointer to arguments is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Older GCC versions have a bug which gives a spurious warning for the C universal zero
	 *	initializer `{0}`. Any workaround makes the code less portable or prevents the common code
	 *	from adding new fields to the `CommonCommandLineArguments` struct. Therefore, we surpress
	 *	this warning.
	 *
	 *	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119.
	 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
	*arguments = (CommandLineArguments) {
		.common     = (CommonCommandLineArguments) { 0 },
		.someString = "",
		.someInt    = kDefaultSomeInt,
		.someDouble = kDefaultSomeDouble,
		.someBool   = false,
	};
#pragma GCC diagnostic pop

	snprintf(
		arguments->someString,
		kCommonConstantMaxCharsPerFilepath,
		"%s",
		(char *) kDefaultSomeString
	);

	return kCommonConstantReturnTypeSuccess;
}

#ifdef NO_OS_AVAILABLE
CommonConstantReturnType
setNoOSCommandLineArguments(CommandLineArguments * arguments)
{
	if (arguments == NULL)
	{
		fprintf(stderr, "Error: The provided pointer to arguments is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Start from the defaults so that every demo-specific field is initialized,
	 *	then override the ones the no-OS build fixes. The `common` sub-struct is
	 *	zeroed by this call, so it is set explicitly below.
	 */
	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	arguments->common.numberOfMonteCarloIterations  = 1;
	arguments->common.outputSelect                  = kOutputVariableIndexMax;
	arguments->common.isTimingEnabled               = false;
	arguments->common.isMonteCarloMode              = false;
	arguments->common.isOutputJSONMode              = false;
	arguments->common.isWriteToFileEnabled          = false;

	return kCommonConstantReturnTypeSuccess;
}
#endif

CommonConstantReturnType
getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments)
{
#ifdef NO_OS_AVAILABLE
	/*
	 *	The no-OS build has no command line to parse, so ignore `argc` and `argv`
	 *	and use the hard-coded configuration instead.
	 */
	(void) argc;
	(void) argv;

	puts("Using hard coded command line arguments");

	return setNoOSCommandLineArguments(arguments);

#else
	const char *    firstInputVariableArg   = NULL;
	const char *    secondInputVariableArg  = NULL;
	const char *    someStringArg           = NULL;
	const char *    someIntArg              = NULL;
	const char *    someDoubleArg           = NULL;

	if (arguments == NULL)
	{
		fprintf(stderr, "Error: The provided pointer to arguments is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	DemoOption options[] = {
		{ .opt = "f", .optAlternative = "first-input-variable",  .hasArg = true,  .foundArg = &firstInputVariableArg,  .foundOpt = NULL                 },
		{ .opt = "s", .optAlternative = "second-input-variable", .hasArg = true,  .foundArg = &secondInputVariableArg, .foundOpt = NULL                 },
		{ .opt = "c", .optAlternative = "long-option-string",    .hasArg = true,  .foundArg = &someStringArg,          .foundOpt = NULL                 },
		{ .opt = "e", .optAlternative = "long-option-int",       .hasArg = true,  .foundArg = &someIntArg,             .foundOpt = NULL                 },
		{ .opt = "x", .optAlternative = "long-option-double",    .hasArg = true,  .foundArg = &someDoubleArg,          .foundOpt = NULL                 },
		{ .opt = "p", .optAlternative = "long-option-bool",      .hasArg = false, .foundArg = NULL,                    .foundOpt = &arguments->someBool },
		{ 0 },
	};

	if (parseArgs(argc, argv, &arguments->common, options) != kCommonConstantReturnTypeSuccess)
	{
		fprintf(stderr, "Error: Parsing command line arguments failed.\n");
		printUsage();

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isHelpEnabled)
	{
		printUsage();

		exit(EXIT_SUCCESS);
	}

	/*
	 *	If no output is selected, set `outputSelect` to `kOutputVariableIndexMax`.
	 *	This triggers the demo to compute all outputs.
	 */
	if (!arguments->common.isOutputSelected)
	{
		arguments->common.outputSelect = kOutputVariableIndexMax;
	}

	/*
	 *	When `outputSelect` is set to `kOutputVariableIndexMax`, we cannot be
	 *	in benchmarking mode or Monte Carlo mode.
	 */
	if (arguments->common.outputSelect == kOutputVariableIndexMax)
	{
		if ((arguments->common.isBenchmarkingMode) || (arguments->common.isMonteCarloMode))
		{
			fprintf(stderr, "Error: Please select a single output when in benchmarking mode or Monte Carlo mode.\n");

			return kCommonConstantReturnTypeError;
		}
	}
	/*
	 *	Selected output can never be greater than `kOutputVariableIndexMax`.
	 */
	else if (arguments->common.outputSelect > kOutputVariableIndexMax)
	{
		fprintf(stderr, "Error: Wrong output selection.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Monte Carlo mode does not support input from file.
	 */
	if ((arguments->common.isMonteCarloMode) && (arguments->common.isInputFromFileEnabled))
	{
		fprintf(stderr, "Error: Monte Carlo mode does not support input from file.\n");

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isVerbose)
	{
		fprintf(stderr, "Warning: Verbose mode not supported. Continuing in non-verbose mode.\n");
	}

	if (firstInputVariableArg != NULL)
	{
		double firstInputVariable;

		if (parseDoubleChecked(firstInputVariableArg, &firstInputVariable) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The some double must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (firstInputVariable <= 0.0)
		{
			fprintf(stderr, "Error: The first input variable must be positive.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->inputVariables[kInputDistributionIndexFirstInput]        = firstInputVariable;
		arguments->isInputVariableSet[kInputDistributionIndexFirstInput]    = true;
	}

	if (secondInputVariableArg != NULL)
	{
		double secondInputVariable;

		if (parseDoubleChecked(secondInputVariableArg, &secondInputVariable) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The some double must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (secondInputVariable <= 0.0)
		{
			fprintf(stderr, "Error: The second input variable must be positive.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->inputVariables[kInputDistributionIndexSecondInput]       = secondInputVariable;
		arguments->isInputVariableSet[kInputDistributionIndexSecondInput]   = true;
	}

	if (someStringArg != NULL)
	{
		int ret = snprintf(arguments->someString, kCommonConstantMaxCharsPerFilepath, "%s", someStringArg);

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerFilepath))
		{
			fprintf(stderr, "Error: Could not read the some file path from command line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
	}

	if (someIntArg != NULL)
	{
		int someInt;

		if (parseIntChecked(someIntArg, &someInt) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The some int must be an integer.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (someInt < 0)
		{
			fprintf(stderr, "Error: The some int must be non-negative.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->someInt = someInt;
	}

	if (someDoubleArg != NULL)
	{
		double someDouble;

		if (parseDoubleChecked(someDoubleArg, &someDouble) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The some double must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (someDouble <= 0.0)
		{
			fprintf(stderr, "Error: The some double must be positive.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->someDouble = someDouble;
	}

	return kCommonConstantReturnTypeSuccess;

#endif
}
