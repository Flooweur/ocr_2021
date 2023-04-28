#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double frontlayer[2];
double hiddenlayer[2];
double outputlayer[2];

double hiddenweights[4];
double outputweights[2];

double biashidden[2];
double biasoutput[2];

double besthiddenweigths[4];
double bestoutputweights[2];
double bestbias[2];
double bestbias2[2];

double dz[2];
double dw[2];

double dz2[2];
double dw2[2];

double targetoutput;

// Forward propagation
int xor(double a, double b)
{
	hiddenlayer[0] = 0;
	hiddenlayer[1] = 0;
	frontlayer[0] = a;
	frontlayer[1] = b;

	for(size_t i = 0; i < 2; i++)
	{
		hiddenlayer[0] += frontlayer[i] * hiddenweights[2*i];  
		hiddenlayer[1] += frontlayer[i] * hiddenweights[2*i + 1];  
	}
	
	//fonction sigmoïde
	hiddenlayer[0] = 1/(1 + exp(-(hiddenlayer[0] + biashidden[0])));
	hiddenlayer[1] = 1/(1 + exp(-(hiddenlayer[1] + biashidden[1])));

	outputlayer[0] = hiddenlayer[0] * outputweights[0];
	outputlayer[1] = hiddenlayer[1] * outputweights[1];

	//fonction softmax
	/*double sumbiasout = 0;
	for (size_t i = 0; i < 2; i++)
		sumbiasout += biasoutput[i];
	outputlayer[0] = exp(biasoutput[0] + outputlayer[0])/exp(sumbiasout);
	outputlayer[1] = exp(biasoutput[1] + outputlayer[1])/exp(sumbiasout);
*/
	//fonction sigmoide
	outputlayer[0] = 1/(1 - exp(-(outputlayer[0] + biasoutput[0])));
	outputlayer[1] = 1/(1 - exp(-(outputlayer[1] + biasoutput[1])));

	printf("%f\n", outputlayer[1]);

	return outputlayer[0] >= outputlayer[1] ? 0 : 1;
}

// Randomize bias and weight values of all hidden neurons
void rdm()
{
	srand((double) time(NULL));
	for (size_t i = 0; i < 2; i++) {
		biashidden[i] = (rand() % 2) - 1; 
		biasoutput[i] = (rand() % 2) - 1;
		outputweights[i] = (rand() % 2) - 1; 
	}
	for (size_t i = 0; i < 4; i++)
		hiddenweights[i] = (rand() % 2) - 1; 
}

// Calculate final Error value (needs to be closest to 0)
double test()
{
	double error = 0;
	targetoutput = 0;
	xor (0, 0);
	double test1 = 0 - outputlayer[1];
	targetoutput = 1;
	xor (0, 1);
	double test2 = 1 - outputlayer[1];
	xor (1, 0);
	double test3 = 1 - outputlayer[1];
	targetoutput = 0;
	xor (1, 1);
	double test4 = 0 - outputlayer[1];

	error += 0.5 * test1 * test1;
	error += 0.5 * test2 * test2;
	error += 0.5 * test3 * test3;
	error += 0.5 * test4 * test4;

	printf("Error = %f\n", error);

	return error;
}


void backpropagation(){
	int j, k;

	// Output layer
	for (j = 0; j < 2; j++) {
		dz2[j] = (outputlayer[j] -  targetoutput)*outputlayer[j]*(1 - outputlayer[j]);

		for (k = 0; k < 2; k++) {
			dw[j] = dz2[j] * hiddenlayer[k];
			outputlayer[k] = outputweights[j] * dz2[j];
		}
		bestbias2[j] = dz2[j];
	}


	// Hidden layer
	for (j = 0; j < 2; j++) {
		dz[j] = hiddenlayer[j] * (1-hiddenlayer[j]);

		for (k = 0; k < 2; k++)
		{
			dw2[j] = dz[j] * frontlayer[k];
			hiddenlayer[k] = hiddenweights[j] * dz2[j];
		}
		bestbias[j] = dz[j];
		/*if (hiddenlayer[j] + biashidden[j] >= 0) {
			dz[j] = 1 / (1 + exp(-(hiddenlayer[j] + biashidden[j])));
		}
		else {
			dz[j] = 0;
		}

		for (k = 0; k < 2; k++) {
			dw2[j] = dz[j] * (1 / (1 + exp(-(hiddenlayer[k] + biashidden[k]))));
		}
		bestbias[j] = dz[j];*/
	}

}

void updateweights(){	
	for (size_t j = 0; j < 2; j++) {
		// Updating weights
		hiddenweights[j] -= dw[j];
		outputweights[j] -= dw2[j];
		biashidden[j] -= bestbias[j];
		biasoutput[j] -= bestbias2[j];
	}
}

void train()
{
	rdm();
	double x = test();
	while (x > 0.1) {
		backpropagation();
		updateweights();
		x = test();
	}
}

int main(int argc, char **argv)
{
	train();
	double a = atoi(argv[1]);
	double b = atoi(argv[2]);
	if (argc != 3)
		printf("usage: ./neuralnetwork int int ");
	if ((a != 0 && a != 1 )||( b != 0 && b != 1))
		printf("arg should be either 0 or 1");
	printf("test 0 0 = %d\n", xor (0, 0));
	printf("test 0 1 = %d\n", xor (0, 1));
	printf("test 1 0 = %d\n", xor (1, 0));
	printf("test 1 1 = %d\n", xor (1, 1));

	printf("%d\n", xor(a , b));
	return 0;
}
