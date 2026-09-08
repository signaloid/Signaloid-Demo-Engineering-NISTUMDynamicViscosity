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

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "common.h"

typedef enum
{
	kInputDistributionIndexMuC = 0,
	kInputDistributionIndexRhoB,
	kInputDistributionIndexRhoC,
	kInputDistributionIndexRhoM,
	kInputDistributionIndexTc,
	kInputDistributionIndexTm,
	kInputDistributionIndexMax,
} InputDistributionIndex;

typedef enum
{
	kOutputVariableIndexFirstOutput = 0,
	kOutputVariableIndexMax,
} OutputDistributionIndex;

/**
 *	@brief	Holds a set of Monte Carlo samples loaded from a CSV input file,
 *		together with the number of samples loaded.
 */
typedef struct
{
	double *    samples;
	int         count;
} SampleData;

typedef struct
{
	CommonCommandLineArguments  common;
	SampleData                  muCSampleData;
	SampleData                  rhoBSampleData;
	SampleData                  rhoCSampleData;
	SampleData                  rhoMSampleData;
	SampleData                  tCSampleData;
	SampleData                  tMSampleData;
} CommandLineArguments;

/**
 *	@brief	Print out command line usage.
 */
void
printUsage(void);

/**
 *	@brief	Special load method. CSV files have more rows than `kCommonConstantMaxNumberOfInputSamples`,
 *		so this loads the raw samples directly rather than through `readInputDoubleDistributionsFromCSV`.
 *
 *	@param	filename	: file name
 *	@return	SampleData	: Returns the loaded samples and their count.
 */
SampleData
loadDoubleSamplesFromPath(const char * filename);

/**
 *	@brief	Special load method. CSV files have more rows than `kCommonConstantMaxNumberOfInputSamples`.
 *		Loads the raw samples and immediately returns their `UxHwDoubleDistFromSamples` distribution.
 *
 *	@param	filename	: file name
 *	@return	double		: Returns the distribution built from the loaded samples.
 */
double
loadDoubleDistFromPath(const char * filename);

/**
 *	@brief	Get command line arguments.
 *
 *	@param	argc		: argument count from `main()`.
 *	@param	argv		: argument vector from `main()`.
 *	@param	arguments	: Pointer to struct to store arguments.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments);

/*
 *	`determineIndexRangeOfSelectedOutputs()`, `printHumanConsumableOutput()`,
 *	`populateJSONVariableStruct()` and `printJSONFormattedOutput()` are now
 *	provided generically by the `common` submodule (see `common.h`), which
 *	operates on `CommonCommandLineArguments *` and plain `size_t` indices
 *	instead of this demo's local `CommandLineArguments *` /
 *	`OutputDistributionIndex`. This demo previously carried its own
 *	same-named local versions of these functions with incompatible
 *	signatures, which no longer compiles against the current `common`
 *	submodule; `main()` now calls the generic versions directly instead.
 */
