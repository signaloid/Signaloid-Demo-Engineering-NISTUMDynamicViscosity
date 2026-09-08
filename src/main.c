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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <uxhw.h>
#include "common.h"
#include "utilities.h"
#include "kernel.h"



int
main(int argc, char *  argv[])
{
	CommandLineArguments    arguments       = (CommandLineArguments) { 0 };
	const char *            kMuCFilePath    = "samples-gaussian-dv_muC.csv";
	const char *            kRhoBFilePath   = "samples-gaussian-dv_rhoB.csv";
	const char *            kRhoCFilePath   = "samples-gaussian-dv_rhoC.csv";
	const char *            kRhoMFilePath   = "samples-gaussian-dv_rhoM.csv";
	const char *            kTcFilePath     = "samples-gaussian-dv_tC.csv";
	const char *            kTmFilePath     = "samples-gaussian-dv_tM.csv";

	double                      output;
	double                      outputVariables[kOutputVariableIndexMax];
	const char *                applicationDescription = "NIST Uncertainty Machine Dynamic Viscosity";
	const char *                outputVariableNames[kOutputVariableIndexMax] = {
		"Dynamic viscosity",
	};
	const char *                outputVariableDescriptions[kOutputVariableIndexMax] = {
		"Dynamic viscosity of a sodium hydroxide solution in water",
	};
	kOutputVariableTypeIndex    outputVariableTypes[kOutputVariableIndexMax] = {
		[kOutputVariableIndexFirstOutput] = kOutputVariableTypeDistribution,
	};
	double *                    monteCarloOutputSamples         = NULL;
	MeanAndVariance             monteCarloOutputMeanAndVariance = { 0 };
	clock_t                     start                   = 0;
	clock_t                     end                     = 0;
	double                      cpuTimeUsedInSeconds    = 0.0;

	/*
	 *	Get command line arguments.
	 */
	if (getCommandLineArguments(argc, argv, &arguments) != kCommonConstantReturnTypeSuccess)
	{
		return EXIT_FAILURE;
	}

	/*
	 *	MonteCarlo output samples are used even in the UxHw use case to store
	 *	the result of the single distributional draw.
	 */
	monteCarloOutputSamples =
		(double *) checkedMalloc(
			arguments.common.numberOfMonteCarloIterations * sizeof(double),
			__FILE__,
			__LINE__
		);

	/*
	 *	Read input distributions from CSV using preexisting method.
	 *	The CSV files have more rows than kCommonConstantMaxNumberOfInputSamples
	 */
	arguments.muCSampleData     = loadDoubleSamplesFromPath(kMuCFilePath);
	arguments.rhoBSampleData    = loadDoubleSamplesFromPath(kRhoBFilePath);
	arguments.rhoCSampleData    = loadDoubleSamplesFromPath(kRhoCFilePath);
	arguments.rhoMSampleData    = loadDoubleSamplesFromPath(kRhoMFilePath);
	arguments.tCSampleData      = loadDoubleSamplesFromPath(kTcFilePath);
	arguments.tMSampleData      = loadDoubleSamplesFromPath(kTmFilePath);

	/*
	 *	Start timing if timing is enabled or in benchmarking mode.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	bool isSelectedOutputScalar = (arguments.common.outputSelect != kOutputVariableIndexMax) &&
	                              (outputVariableTypes[arguments.common.outputSelect] == kOutputVariableTypeScalar);

	if (arguments.common.isMonteCarloMode)
	{
		output = calculateOutputMonteCarlo(&arguments, outputVariables, monteCarloOutputSamples);

		/*
		 *	If not doing UxHw version, then approximate the cost of the third phase of
		 *	Monte Carlo (post-processing), by calculating the mean and variance.
		 */
		if (!isSelectedOutputScalar)
		{
			monteCarloOutputMeanAndVariance = calculateMeanAndVarianceOfDoubleSamples(monteCarloOutputSamples, arguments.common.numberOfMonteCarloIterations);
			output = outputVariables[arguments.common.outputSelect] = monteCarloOutputMeanAndVariance.mean;
		}
	}
	else
	{
		output = calculateOutputUxHw(&arguments, outputVariables, monteCarloOutputSamples);
	}


	CommonCommandLineArguments printArguments = arguments.common;

	if (arguments.common.isMonteCarloMode && isSelectedOutputScalar)
	{
		printArguments.isMonteCarloMode             = false;
		printArguments.numberOfMonteCarloIterations = 1;
	}

	/*
	 *	Free samples arrays
	 */
	free(arguments.muCSampleData.samples);
	free(arguments.rhoBSampleData.samples);
	free(arguments.rhoCSampleData.samples);
	free(arguments.rhoMSampleData.samples);
	free(arguments.tCSampleData.samples);
	free(arguments.tMSampleData.samples);

	/*
	 *	Stop timing if timing is enabled or in benchmarking mode.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end = clock();
		cpuTimeUsedInSeconds = ((double) (end - start)) / CLOCKS_PER_SEC;
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
			outputVariableDescriptions,
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
		size_t samplesToSave = isSelectedOutputScalar
		                ? 1
		                : arguments.common.numberOfMonteCarloIterations;

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

				free(monteCarloOutputSamples);

				return EXIT_FAILURE;
			}
		}
	}

	/*
	 *	Free allocations.
	 */
	free(monteCarloOutputSamples);

	return EXIT_SUCCESS;
}
