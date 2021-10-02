#include <stdio.h>
#include <math.h>
const double pi = M_PI;
const double f_0_1 = 17820;
const double R_1 = 1008;
const double U_in_1 = 0.2;

const double arr_f_1[] = {21809,  22309,  22615,  22871,  22927, 22985, 23070,  23170, 23309,  23527, 23858,  23928,  24422,  24940,  25450};
const double arr_U_1[] = {0.1714, 0.2320, 0.2902, 0.3607, 0.382, 0.4,   0.4322, 0.475, 0.5332, 0.589, 0.5369, 0.5099, 0.3389, 0.2359, 0.1789};

const double arr_f_2[] = {17293, 17455, 17820, 18043,  18376};
const double arr_U_2[] = {0.246, 0.286, 0.351, 0.3307, 0.2536};

const double R_min = 6, R_max = 7, rho_min = 108, rho_max = 110;
const int quantity = 10000;

double U (double R, double rho, double f, double f_0, double U_in)
{
	double I_0 = U_in / R_1;
	double Q = rho / R;
	double omega = 2 * pi * f;
	double omega_0 = 2 * pi * f_0;
	return ((Q * rho * I_0)/(1 + pow ((Q * ((omega/omega_0) - (omega_0/omega))), 2))) * sqrt (pow ((omega/omega_0), 4) + pow (((Q * ((omega/omega_0) - (omega_0/omega))) + (omega_0 / (omega * Q))), 2));
}

double SquareSum (double R, double rho, double *ptr_arr_f, double *ptr_arr_U, int size)
{
	double sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum = sum + pow ((*(ptr_arr_U + i) - U (R, rho, *(ptr_arr_f + i), f_0_1, U_in_1)), 2);
	}
	return sum;
}

double MSS (double R_min, double R_max, double rho_min, double rho_max, int quantity, double *R_optimal, double *rho_optimal, double *ptr_arr_f, double *ptr_arr_U, int size)
{
	double MSS = 10000; //big value
	double SS = 0;
	for (int i = 0; i <= quantity; i++)
	{
		double R = R_min + ((R_max - R_min) * ((double) i / (double) quantity));
		for (int j = 0; j <= quantity; j++)
		{
			double rho = rho_min + ((rho_max - rho_min) * ((double) j / (double) quantity));
			SS = SquareSum (R, rho, ptr_arr_f, ptr_arr_U, size);
			if (SS < MSS)
			{
				MSS = SS;
				*R_optimal = R;
				*rho_optimal = rho;
			}
		}
	}
	return MSS;
}

int main ()
{
	int size = sizeof (arr_f_2) / sizeof (double);
	double R_optimal = 0, rho_optimal = 0;
	
	printf("%s", "MSS = ");
	printf("%.10f\n", MSS (R_min, R_max, rho_min, rho_max, quantity, &R_optimal, &rho_optimal, &arr_f_2[0], &arr_U_2[0], size));
	printf("%s", "R_optimal = ");
	printf("%.10f\n", R_optimal);
	printf("%s", "rho_optimal = ");
	printf("%.10f\n", rho_optimal);
	printf("%s", "Q_optimal = ");
	printf("%.10f\n", rho_optimal / R_optimal);

	/*while (1)
	{
		printf("%s", "Enter R, rho\n");
		scanf ("%lf%lf", &R, &rho);
		printf("%s", "SS = ");
		printf("%lf\n", SquareSum (R, rho, &arr_f_1[0], &arr_U_1[0], size));
	}*/	
}