/*
  Copyright (C) 2010  Charles Pence <charles@charlespence.net>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <aformula.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//
// This is the basic expression test for correct solutions.
// Tests every operator and function on every backend.
//

AFormula::Formula *f;
double x, y, z;

void CHECK_FORMULA (const char *formula, double value)
{
	double ret;

	f->errorString ();

	if (!f->setExpression (formula))
	{
		fprintf (stderr, "FAIL: Could not set expression to %s\n", formula);
		exit (1);
	}

	ret = f->evaluate ();

	if (f->errorString () != "")
	{
		fprintf (stderr, "FAIL: Could not evaluate %s\n", formula);
		exit (1);
	}

	if (fabs (ret - value) > 0.001f)
	{
		fprintf (stderr, "FAIL: Evaluated %s and expected %f, got %f\n",
		         formula, value, ret);
		exit (1);
	}

	fprintf (stdout, "PASS: %s => %f\n", formula, value);
}

void CHECK_VARIABLE_STR (const char *varname, double variable, double value)
{
	if (fabs (variable - value) > 0.001f)
	{
		fprintf (stderr, "FAIL: Variable %s (%f) should be %f\n",
		         varname, variable, value);
		exit (1);
	}

	fprintf (stdout, "PASS: %s = %f\n", varname, value);
}


#define CHECK_VARIABLE(variable, value) CHECK_VARIABLE_STR (#variable, variable, value)

AFormula::Formula *makeFormula (int backend)
{
	AFormula::Formula *f = AFormula::Formula::createFormula (backend);
	if (!f)
		return NULL;

	f->setVariable ("x", &x);
	f->setVariable ("y", &y);
	f->setVariable ("z", &z);

	return f;
}


int main (int argc, char *argv[])
{
	for (int i = 1 ; i < AFormula::Formula::NUM_BACKENDS ; i++)
	{
		f = makeFormula (i);

		x = 1.0;
		y = 2.0;
		z = 3.0;
		
		// Assignment
		CHECK_FORMULA ("x = y", 2.0);
		CHECK_VARIABLE (x, 2.0);
		CHECK_FORMULA ("x = 1.0", 1.0);
		CHECK_VARIABLE (x, 1.0);
		CHECK_FORMULA ("x = y * z", 6.0);
		CHECK_VARIABLE (x, 6.0);
		CHECK_FORMULA ("x = x / 6.0", 1.0);
		CHECK_VARIABLE (x, 1.0);
		
		// Comparison
		CHECK_FORMULA ("1.0 < 2.0", 1);
		CHECK_FORMULA ("1.0 < 0.5", 0);
		CHECK_FORMULA ("x * y > 1.0", 1);
		CHECK_FORMULA ("1.0 > 2.0", 0);
		CHECK_FORMULA ("1.0 <= 1.0", 1);
		CHECK_FORMULA ("1.1 <= 1.0", 0);
		CHECK_FORMULA ("x >= x", 1);
		CHECK_FORMULA ("x >= y", 0);
		CHECK_FORMULA ("x == x", 1);
		CHECK_FORMULA ("x == y", 0);
		CHECK_FORMULA ("x != x", 0);
		CHECK_FORMULA ("y != x", 1);
		
		// Addition / Subtraction
		CHECK_FORMULA ("x + y", 3.0);
		CHECK_FORMULA ("x + y + z", 6.0);
		CHECK_FORMULA ("x + y - z", 0.0);
		CHECK_FORMULA ("x - z", -2.0);
		CHECK_FORMULA ("2+2", 4.0);
		
		// Multiplication / Division
		CHECK_FORMULA ("x * y", 2.0);
		CHECK_FORMULA ("y*y", 4.0);
		CHECK_FORMULA ("2.0 * z", 6.0);
		CHECK_FORMULA ("-3.5 * 2", -7.0);
		
		// Exponentiation
		CHECK_FORMULA ("2^2", 4.0);
		CHECK_FORMULA ("100^0", 1.0);
		CHECK_FORMULA ("z^x", 3.0);
		CHECK_FORMULA ("100^0.5", 10);

		// Constants
		CHECK_FORMULA ("2*pi", 6.28318531);
		CHECK_FORMULA ("e/4", 0.679570457);
		
		// Order of Operations
		CHECK_FORMULA ("1 + 2 * 3", 7.0);
		CHECK_FORMULA ("3 + 2^2", 7.0);
		CHECK_FORMULA ("(3 + 2)^2", 25.0);
		CHECK_FORMULA ("(1 + 2) * 3", 9.0);
		CHECK_FORMULA ("z = 1 + x^2", 2.0);
		CHECK_FORMULA ("y^2/4", 1.0);

		// Functions
		CHECK_FORMULA ("sin(pi)", 0.0);
		CHECK_FORMULA ("sin(pi/2)", 1.0);
		CHECK_FORMULA ("cos(pi/4)", 0.707106781);
		CHECK_FORMULA ("cos(-3*pi/5)", -0.309016994);
		CHECK_FORMULA ("tan(pi/3)", 1.73205081);
		CHECK_FORMULA ("asin(0.5)", 0.523598776);
		CHECK_FORMULA ("acos(0.5)", 1.04719755);
		CHECK_FORMULA ("atan(0.5)", 0.463647609);
		
		CHECK_FORMULA ("sinh(0.5)", 0.521095305);
		CHECK_FORMULA ("cosh(0.5)", 1.12762597);
		CHECK_FORMULA ("tanh(0.5)", 0.462117157);
		CHECK_FORMULA ("asinh(1)", 0.88137358702);
		CHECK_FORMULA ("acosh(2)", 1.31695789692);
		CHECK_FORMULA ("atanh(0.2)", 0.202732554054);

		CHECK_FORMULA ("log2(256)", 8.0);
		CHECK_FORMULA ("ln(10)", 2.30258509);
		CHECK_FORMULA ("log(4)", 0.602059991);
		CHECK_FORMULA ("log10(4)", 0.602059991);

		CHECK_FORMULA ("exp(2.30258509)", 10.0);
		CHECK_FORMULA ("sqrt(2)", 1.41421356);

		CHECK_FORMULA ("sign(3.0)", 1.0);
		CHECK_FORMULA ("sign(-13.0)", -1.0);
		CHECK_FORMULA ("rint(1.5)", 2.0);
		CHECK_FORMULA ("if(y == 2.0,3.5,1.2)", 3.5);
				
		delete f;
	}
	
	return 0;
}
