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

#include <stddef.h>
#include <stdbool.h>
#include <uxhw.h>
#include "kernel.h"
#include "nist-um-dynamic-viscosity-monte-carlo.h"
#include "utilities.h"

double
dynamicViscositySingleSample(CommandLineArguments * arguments)
{
	double  muC;
	double  rhoB;
	double  rhoC;
	double  rhoM;
	double  tC;
	double  tM;

	/*
	 *	Draw one sample of each input distribution. `UxHwDoubleDistFromSamples()`
	 *	returns a `double` that, in UxHw mode, also has associated with it a
	 *	complete probability distribution built from the loaded samples.
	 */
	muC     = UxHwDoubleDistFromSamples(arguments->muCSampleData.samples, arguments->muCSampleData.count);
	rhoB    = UxHwDoubleDistFromSamples(arguments->rhoBSampleData.samples, arguments->rhoBSampleData.count);
	rhoC    = UxHwDoubleDistFromSamples(arguments->rhoCSampleData.samples, arguments->rhoCSampleData.count);
	rhoM    = UxHwDoubleDistFromSamples(arguments->rhoMSampleData.samples, arguments->rhoMSampleData.count);
	tC      = UxHwDoubleDistFromSamples(arguments->tCSampleData.samples, arguments->tCSampleData.count);
	tM      = UxHwDoubleDistFromSamples(arguments->tMSampleData.samples, arguments->tMSampleData.count);

	/*
	 *	Perform calculations as specified by the NIST Uncertainty Machine example.
	 */
	return muC * ((rhoB - rhoM) / (rhoB - rhoC)) * (tM / tC);
}

double
calculateOutputUxHw(
	CommandLineArguments *  arguments,
	double *                outputVariables,
	double *                monteCarloOutputSamples)
{
	double dynamicViscosity;

	dynamicViscosity            = dynamicViscositySingleSample(arguments);
	monteCarloOutputSamples[0]  = dynamicViscosity;

	return outputVariables[kOutputVariableIndexFirstOutput] = dynamicViscosity;
}

double
calculateOutputMonteCarlo(
	CommandLineArguments *  arguments,
	double *                outputVariables,
	double *                monteCarloOutputSamples)
{
	double dynamicViscosity;

	dynamicViscosity = dynamicViscosityMonteCarlo(
		arguments->common.numberOfMonteCarloIterations,
		arguments,
		monteCarloOutputSamples
	);

	return outputVariables[kOutputVariableIndexFirstOutput] = dynamicViscosity;
}
