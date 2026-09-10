/*
 *	Copyright (c) 2025-2026, Signaloid.
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


void
printUsage(void)
{
	fprintf(stderr, "Example: Template for Signaloid C demos that handles both the Laplace and native MC executions in a single main\n");
	fprintf(stderr, "\n");
	printCommonUsage();
	fprintf(stderr, "\n");

	return;
}

SampleData
loadDoubleSamplesFromPath(const char * filename)
{
	FILE *      inp;
	int         sampleCount;
	double *    samples;
	SampleData  result;

	inp = fopen(filename, "r");

	if (inp == NULL)
	{
		printf("Could not open input file: %s\n", filename);
		exit(EXIT_FAILURE);
	}

	int fscanfResult = fscanf(inp, "%d\n", &sampleCount);

	if (fscanfResult != 1)
	{
		printf("Expected to read one integer at line 0: Read instead %d \n", fscanfResult);
		exit(EXIT_FAILURE);
	}

	samples = calloc(sampleCount, sizeof(double));

	if (samples == NULL)
	{
		perror("error: could not allocate memory for samples");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < sampleCount; i++)
	{
		fscanfResult = fscanf(inp, "%lf\n", &samples[i]);

		if (fscanfResult != 1)
		{
			printf("Expected to read one double at line %d: Read instead %d \n", i, fscanfResult);
			exit(EXIT_FAILURE);
		}
	}

	fclose(inp);

	result.samples  = samples;
	result.count    = sampleCount;

	return result;
}

double
loadDoubleDistFromPath(const char * filename)
{
	FILE *      inp;
	int         sampleCount;
	double *    samples;
	double      returnValue;

	inp = fopen(filename, "r");

	if (inp == NULL)
	{
		printf("Could not open input file: %s\n", filename);
		exit(EXIT_FAILURE);
	}

	int result = fscanf(inp, "%d\n", &sampleCount);

	if (result != 1)
	{
		printf("Expected to read one integer at line 0: Read instead %d \n", result);
		exit(EXIT_FAILURE);
	}

	samples = calloc(sampleCount, sizeof(double));

	if (samples == NULL)
	{
		perror("error: could not allocate memory for samples");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < sampleCount; i++)
	{
		result = fscanf(inp, "%lf\n", &samples[i]);

		if (result != 1)
		{
			printf("Expected to read one double at line %d: Read instead %d \n", i, result);
			exit(EXIT_FAILURE);
		}
	}

	returnValue = UxHwDoubleDistFromSamples(samples, sampleCount);

	if (fclose(inp) != 0)
	{
		perror("warning: could not close input file");
	}

	free(samples);

	return returnValue;
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
		.common = (CommonCommandLineArguments) { 0 },
	};
#pragma GCC diagnostic pop

	return kCommonConstantReturnTypeSuccess;
}

CommonConstantReturnType
getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments)
{
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

	return kCommonConstantReturnTypeSuccess;
}
