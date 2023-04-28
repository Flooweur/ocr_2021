#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

struct neuronei
{
	int value;
       	int weight;
	int bias;
};
struct layer
{
	struct neurone *neurones;
};
struct reseau
{
	struct layer frontlayer;
	struct layer hiddenlayer;
	struct layer output;
};

#endif
