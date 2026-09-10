/*
 *	Copyright (c) 2024-2026, Signaloid.
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
 *	@brief	Calculate the dynamic viscosity of a sodium hydroxide solution in
 *		water, as specified by the NIST Uncertainty Machine example, with
 *		one independent draw of each of the six inputs (`muC`, `rhoB`,
 *		`rhoC`, `rhoM`, `tC`, `tM`) per Monte Carlo iteration. Fills
 *		`monteCarloOutputSamples` with one dynamic-viscosity sample per
 *		iteration.
 *
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo iterations to run.
 *	@param	arguments			: Command-line arguments, providing the loaded input sample sets.
 *	@param	monteCarloOutputSamples		: A pointer to an array where the dynamic-viscosity sample for each iteration is stored.
 *	@return	double				: Returns the last element of `monteCarloOutputSamples`.
 */
double
dynamicViscosityMonteCarlo(
	size_t                  numberOfMonteCarloIterations,
	CommandLineArguments *  arguments,
	double *                monteCarloOutputSamples);
