/*
 *	Copyright (c) 2023–2025, Signaloid.
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
#include "utilities.h"

/**
 *	@brief	Draw one sample of each of the six NIST Uncertainty Machine input
 *		distributions (`muC`, `rhoB`, `rhoC`, `rhoM`, `tC`, `tM`) from
 *		`arguments` via `UxHwDoubleDistFromSamples`, and evaluate the
 *		dynamic viscosity formula on them. In UxHw mode
 *		`UxHwDoubleDistFromSamples` returns the full distribution built
 *		from the input samples, yielding a distributional dynamic
 *		viscosity; in Monte Carlo mode it returns a single random sample
 *		from the input, yielding one Monte Carlo sample of dynamic
 *		viscosity.
 *
 *	@param	arguments	: Command-line arguments, providing the loaded input sample sets.
 *	@return	double		: Returns the dynamic viscosity.
 */
double
dynamicViscositySingleSample(CommandLineArguments * arguments);

/**
 *	@brief	UxHw-mode calculation kernel. Computes the dynamic viscosity output
 *		using distributional arithmetic on a single draw of each input.
 *		Writes the result into `outputVariables` and the single
 *		distributional dynamic viscosity into `monteCarloOutputSamples[0]`.
 *
 *	@param	arguments		: Command-line arguments.
 *	@param	outputVariables		: Array of size `kOutputVariableIndexMax` to fill.
 *	@param	monteCarloOutputSamples	: Single-element array for the distributional result.
 *	@return	double			: Returns the value of the dynamic viscosity output.
 */
double
calculateOutputUxHw(
	CommandLineArguments *  arguments,
	double *                outputVariables,
	double *                monteCarloOutputSamples);

/**
 *	@brief	Monte Carlo calculation kernel. Runs
 *		`arguments->common.numberOfMonteCarloIterations` independent draws
 *		into `monteCarloOutputSamples` (no UxHw distributional API calls
 *		beyond the per-sample draw shared with UxHw mode) and writes the
 *		last sample into `outputVariables`.
 *
 *	@param	arguments		: Command-line arguments.
 *	@param	outputVariables		: Array of size `kOutputVariableIndexMax` to fill.
 *	@param	monteCarloOutputSamples	: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@return	double			: Returns the value of the dynamic viscosity output.
 */
double
calculateOutputMonteCarlo(
	CommandLineArguments *  arguments,
	double *                outputVariables,
	double *                monteCarloOutputSamples);
