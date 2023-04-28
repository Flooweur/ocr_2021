#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#include <err.h>
#include "SDL/SDL_image.h"
#include <math.h>

typedef struct MNIST_ImageFileHeader MNIST_ImageFileHeader;
typedef struct MNIST_LabelFileHeader MNIST_LabelFileHeader;
typedef struct Network Network;
typedef struct Layer Layer;
typedef struct Node Node;
typedef struct Vector Vector;
typedef enum ActFctType {SIGMOID, TANH} ActFctType;

typedef enum LayerType {INPUT, HIDDEN, OUTPUT} LayerType;

typedef struct MNIST_Image MNIST_Image;

struct MNIST_Image{
    uint8_t pixel[28*28];
};

typedef uint8_t MNIST_Label;

struct MNIST_ImageFileHeader{
    uint32_t magicNumber;
    uint32_t maxImages;
    uint32_t imgWidth;
    uint32_t imgHeight;
};

struct MNIST_LabelFileHeader{
    uint32_t magicNumber;
    uint32_t maxLabels;
};

struct Node{
    double bias;
    double output;
    int wcount;
    double weights[];
};

struct Layer{
    int ncount;
    struct Node nodes[];
};

struct Network{
    int inputNodeSize;
    int inputLayerSize;
    int hiddenNodeSize;
    int hiddenLayerSize;
    int outputNodeSize;
    int outputLayerSize;
    double learningRate;
    ActFctType hiddenLayerActType;
    ActFctType outputLayerActType;
    struct Layer layers[];
};

struct Vector{
    int size;
    double vals[];
};

Vector *getVectorFromImage(MNIST_Image *img)
{
    Vector *v = (Vector*)malloc(sizeof(Vector) + (28 * 28 * sizeof(double)));

    v->size = 28 * 28;

    for(int i = 0; i < v->size; i++)
        v->vals[i] = img->pixel[i] ? 1 : 0;

    return v;
}

void feedInput(Network *neuNet, Vector *v)
{
    Layer *inputLayer;
    inputLayer = neuNet->layers;

    Node *inputLayerNode;
    inputLayerNode = inputLayer->nodes;

    for(int i = 0; i < v->size; i++)
    {
        inputLayerNode->output = v->vals[i];
        inputLayerNode++;
    }
}

Layer *getLayer(Network *nn, LayerType ltype)
{

    Layer *l;
    switch (ltype)
    {
        case INPUT:
        {
            l = nn->layers;
            break;
        }
        case HIDDEN:
        {
            uint8_t *sbptr = (uint8_t*) nn->layers;
            sbptr += nn->inputLayerSize;
            l = (Layer*)sbptr;
            break;
        }
        default:
        {
            uint8_t *sbptr = (uint8_t*) nn->layers;
            sbptr += nn->inputLayerSize + nn->hiddenLayerSize;
            l = (Layer*)sbptr;
            break;
        }
    }
    return l;
}

Node *getNode(Layer *l, int nodeId)
{
    int nodeSize = sizeof(Node) + (l->nodes[0].wcount * sizeof(double));
    uint8_t *sbptr = (uint8_t*) l->nodes;

    sbptr += nodeId * nodeSize;

    return (Node*) sbptr;
}

double sumexp(struct Network *neuNet)
{
    double sum = 0;
    struct Layer *l = getLayer(neuNet, HIDDEN);
    for(int i = 0; i < neuNet->hiddenLayerSize; i++)
    {
        sum += exp(getNode(l, i)->output);
    }
    return sum;
}

double getActFctDerivative(Network *nn, LayerType ltype, double outVal)
{
    double dVal = 0;
    ActFctType actFct;

    if (ltype==HIDDEN)
        actFct = nn->hiddenLayerActType;
    else
        actFct = nn->outputLayerActType;

    if (actFct==TANH)
        dVal = 1-pow(tanh(outVal),2);
    else
        dVal = outVal * (1-outVal);
    return dVal;
}

void activateNode(Network *neuNet, LayerType ltype, int id)
{
    Layer *l = getLayer(neuNet, ltype);
    Node *n = getNode(l, id);

    ActFctType actFct;

    if (ltype==HIDDEN)
        actFct = neuNet->hiddenLayerActType;
    else
        actFct = neuNet->outputLayerActType;

    if (actFct==TANH)
        n->output = tanh(n->output);
    else
        n->output = 1 / (1 + (exp((double)-n->output)) );
}

void calcNodeOutput(Network *neuNet, LayerType layerType, int id)
{
    Layer *calcLayer = getLayer(neuNet, layerType);
    Node *calcNode = getNode(calcLayer, id);

    Layer *prevLayer;
    int prevLayerNodeSize = 0;

    if (layerType == HIDDEN)
    {
        prevLayer = getLayer(neuNet, INPUT);
        prevLayerNodeSize = neuNet->inputNodeSize;
    }
    else
    {
        prevLayer = getLayer(neuNet, HIDDEN);
        prevLayerNodeSize = neuNet->hiddenNodeSize;
    }

    uint8_t *sbptr = (uint8_t*) prevLayer->nodes;

    calcNode->output = calcNode->bias;

    for(int i = 0; i < prevLayer->ncount; i++)
    {
        Node *prevLayerNode = (Node*)sbptr;
        calcNode->output += prevLayerNode->output * calcNode->weights[i];
        sbptr += prevLayerNodeSize;
    }
}

void calcLayer(Network *neuNet, LayerType layerType)
{
    Layer *l;
    l = getLayer(neuNet, layerType);

    for(int i = 0; i < l->ncount; i++)
    {
        calcNodeOutput(neuNet, layerType, i);
        activateNode(neuNet, layerType, i);
    }
}

void feedForwardNetwork(Network *neuNet)
{
    calcLayer(neuNet, HIDDEN);
    calcLayer(neuNet, OUTPUT);
}

void updateNodeWeights(Network *neuNet, LayerType ltype, int id, double error)
{
    Layer *updateLayer = getLayer(neuNet, ltype);
    Node *updateNode = getNode(updateLayer, id);

    Layer *prevLayer;
    int prevLayerNodeSize = 0;
    if (ltype==HIDDEN) {
        prevLayer = getLayer(neuNet, INPUT);
        prevLayerNodeSize = neuNet->inputNodeSize;
    } else {
        prevLayer = getLayer(neuNet, HIDDEN);
        prevLayerNodeSize = neuNet->hiddenNodeSize;
    }

    uint8_t *sbptr = (uint8_t*) prevLayer->nodes;

    for (int i=0; i<updateNode->wcount; i++){
        Node *prevLayerNode = (Node*)sbptr;
        updateNode->weights[i] += (neuNet->learningRate * prevLayerNode->output * error);
        sbptr += prevLayerNodeSize;
    }

    updateNode->bias += (neuNet->learningRate * 1 * error);
}

void backPropagateOutputLayer(Network *neuNet, int targetClassification)
{
    Layer *outputLayer = getLayer(neuNet, OUTPUT);
    for (int i = 0; i < outputLayer->ncount; i++){
        Node *outputNode = getNode(outputLayer,i);
        int targetOutput = (i == targetClassification)?1:0;
        double errorDelta = targetOutput - outputNode->output;
        double errorSignal = errorDelta * getActFctDerivative(neuNet, OUTPUT, outputNode->output);
        updateNodeWeights(neuNet, OUTPUT, i, errorSignal);
    }
}

void backPropagateHiddenLayer(Network *neuNet, int targetClassification)
{
    Layer *hiddenLayer = getLayer(neuNet, HIDDEN);
    Layer *outputLayer = getLayer(neuNet, OUTPUT);

    for (int j = 0; j < hiddenLayer->ncount; j++)
    {
        double errorSum = 0;
        Node *hiddenNode = getNode(hiddenLayer,j);

        for (int k = 0; k < outputLayer->ncount; k++)
        {
            Node *outputNode = getNode(outputLayer,k);
            int targetOutput = (k == targetClassification)?1:0;
            double errorDelta = targetOutput - outputNode->output;
            double errorSignal = errorDelta * getActFctDerivative(neuNet, OUTPUT, outputNode->output);
            errorSum += errorSignal * outputNode->weights[j];
        }
        double hErrorSignal = errorSum * getActFctDerivative(neuNet, HIDDEN, hiddenNode->output);
        updateNodeWeights(neuNet, HIDDEN, j, hErrorSignal);
    }
}

void backPropagateNetwork(Network *neuNet, int targetClassification)
{
    backPropagateOutputLayer(neuNet, targetClassification);
    backPropagateHiddenLayer(neuNet, targetClassification);
}

Layer *createInputLayer(int inputCount)
{
    int inputNodeSize = sizeof(Node);
    int inputLayerSize = sizeof(Layer) + (inputCount * inputNodeSize);

    Layer *inputLayer = malloc(inputLayerSize);
    inputLayer->ncount = inputCount;

    Node inputLayerNode;
    inputLayerNode.bias = 0;
    inputLayerNode.output = 0;
    inputLayerNode.wcount = 0;

    uint8_t *sbptr = (uint8_t*) inputLayer->nodes;

    for (int i = 0; i < inputLayer->ncount; i++)
    {
        memcpy(sbptr, &inputLayerNode, inputNodeSize);
        sbptr += inputNodeSize;
    }

    return inputLayer;
}

Layer *createLayer(int nodeCount, int weightCount)
{
    int nodeSize = sizeof(Node) + (weightCount * sizeof(double));
    Layer *layer = (Layer*)malloc(sizeof(Layer) + (nodeCount * nodeSize));

    layer->ncount = nodeCount;

    Node *defNode = (Node*) malloc(sizeof(Node) + ((weightCount) * sizeof(double)));
    defNode->bias = 0;
    defNode->output = 0;
    defNode->wcount = weightCount;

    for(int i = 0; i < weightCount; i++)
        defNode->weights[i] = 0;

    uint8_t *sbptr = (uint8_t*) layer->nodes;

    for (int j = 0; j < nodeCount; j++)
        memcpy(sbptr + (j * nodeSize), defNode, nodeSize);

    free(defNode);

    return layer;
}

void initWeights(Network *neuNet, LayerType layerType)
{
    int nodeSize = 0;
    if (layerType == HIDDEN)
        nodeSize = neuNet->hiddenNodeSize;
    else
        nodeSize = neuNet->outputNodeSize;

    Layer *layer = getLayer(neuNet, layerType);

    uint8_t *sbptr = (uint8_t*) layer->nodes;

    for (int i = 0; i < layer->ncount; i++)
    {
        Node *node = (Node*)sbptr;

        for(int j = 0; j < node->wcount; j++)
        {
            node->weights[j] = (double)(2*rand()-1)/(double)(RAND_MAX);
        }

        node->bias = (double)(2*rand())/(double)(RAND_MAX);

        sbptr += nodeSize;
    }
}

void initNetwork(Network *neuNet, int inputCount, int hiddenCount, int outputCount)
{
    Layer *inputLayer = createInputLayer(inputCount);
    memcpy(neuNet->layers, inputLayer, neuNet->inputLayerSize);
    free(inputLayer);

    uint8_t *sbptr = (uint8_t*) neuNet->layers;

    sbptr += neuNet->inputLayerSize;

    Layer *hiddenLayer = createLayer(hiddenCount, inputCount);
    memcpy(sbptr, hiddenLayer, neuNet->hiddenLayerSize);
    free(hiddenLayer);

    sbptr += neuNet->hiddenLayerSize;

    Layer *outputLayer = createLayer(outputCount, hiddenCount);
    memcpy(sbptr, outputLayer, neuNet->outputLayerSize);
    free(outputLayer);
}
void setNetworkDefaults(Network *neuNet)
{
    neuNet->hiddenLayerActType = SIGMOID;
    neuNet->outputLayerActType = SIGMOID;

    neuNet->learningRate = 0.004;
    neuNet->learningRate = 0.2;
}

Network *createNetwork(int inputCount, int hiddenCount, int outputCount)
{
    int inputNodeSize = sizeof(Node);
    int inputLayerSize = sizeof(Layer) + (inputCount + inputNodeSize);
    int hiddenWeightsCount = inputCount;
    int hiddenNodeSize =
        sizeof(Node) + (hiddenWeightsCount * sizeof(double));
    int hiddenLayerSize = sizeof(Layer) + (hiddenCount * hiddenNodeSize);
    int outputWeightsCount = hiddenCount;
    int outputNodeSize = sizeof(Node) + (outputWeightsCount * sizeof(double));
    int outputLayerSize = sizeof(Layer) + (outputCount * outputNodeSize);

    Network *neuNet = (Network*) malloc(sizeof(Network) +
            inputLayerSize +
            hiddenLayerSize +
            outputLayerSize);

    neuNet->inputNodeSize = inputNodeSize;
    neuNet->inputLayerSize = inputLayerSize;
    neuNet->hiddenNodeSize = hiddenNodeSize;
    neuNet->hiddenLayerSize = hiddenLayerSize;
    neuNet->outputNodeSize = outputNodeSize;
    neuNet->outputLayerSize = outputLayerSize;

    initNetwork(neuNet, inputCount, hiddenCount, outputCount);
    setNetworkDefaults(neuNet);
    initWeights(neuNet, HIDDEN);
    initWeights(neuNet, OUTPUT);

    return neuNet;
}

int getNetworkClassification(Network *nn)
{
    Layer *l = getLayer(nn, OUTPUT);
    double maxOut = 0;
    int maxInd = 0;
    for (int i=0; i<l->ncount; i++)
    {
        Node *on = getNode(l,i);
        if (on->output > maxOut){
            maxOut = on->output;
            maxInd = i;
        }
    }
    return maxInd;
}

/////////////////////////////////////////////////////////////////
// :)
/////////////////////////////////////////////////////////////////

uint32_t flipBytes(uint32_t n){
    uint32_t b0,b1,b2,b3;
    b0 = (n & 0x000000ff) <<  24u;
    b1 = (n & 0x0000ff00) <<   8u;
    b2 = (n & 0x00ff0000) >>   8u;
    b3 = (n & 0xff000000) >>  24u;
    return (b0 | b1 | b2 | b3);
}




/**
 * @details Read MNIST image file header
 * @see http://yann.lecun.com/exdb/mnist/ for definition details
 */

void readImageFileHeader(FILE *imageFile, MNIST_ImageFileHeader *ifh){
    ifh->magicNumber =0;
    ifh->maxImages   =0;
    ifh->imgWidth    =0;
    ifh->imgHeight   =0;
    fread(&ifh->magicNumber, 4, 1, imageFile);
    ifh->magicNumber = flipBytes(ifh->magicNumber);
    fread(&ifh->maxImages, 4, 1, imageFile);
    ifh->maxImages = flipBytes(ifh->maxImages);
    fread(&ifh->imgWidth, 4, 1, imageFile);
    ifh->imgWidth = flipBytes(ifh->imgWidth);
    fread(&ifh->imgHeight, 4, 1, imageFile);
    ifh->imgHeight = flipBytes(ifh->imgHeight);
}




/**
 * @details Read MNIST label file header
 * @see http://yann.lecun.com/exdb/mnist/ for definition details
 */

void readLabelFileHeader(FILE *imageFile, MNIST_LabelFileHeader *lfh){
    lfh->magicNumber =0;
    lfh->maxLabels   =0;
    fread(&lfh->magicNumber, 4, 1, imageFile);
    lfh->magicNumber = flipBytes(lfh->magicNumber);
    fread(&lfh->maxLabels, 4, 1, imageFile);
    lfh->maxLabels = flipBytes(lfh->maxLabels);
}

FILE *openMNISTImageFile(char *fileName){
 FILE *imageFile;
    imageFile = fopen (fileName, "rb");
    if(imageFile == NULL) {
        printf("Abort! Could not fine MNIST IMAGE file: %s\n",fileName);
        exit(0);
    }

    MNIST_ImageFileHeader imageFileHeader;
    readImageFileHeader(imageFile, &imageFileHeader);
    return imageFile;
}




/**
 * @details Open MNIST label file and read header info
 * by reading the header info, the read pointer
 * is moved to the position of the 1st LABEL
 */

FILE *openMNISTLabelFile(char *fileName){
    FILE *labelFile;
    labelFile = fopen (fileName, "rb");
    if (labelFile == NULL) {
        printf("Abort! Could not find MNIST LABEL file: %s\n",fileName);
        exit(0);
    }

    MNIST_LabelFileHeader labelFileHeader;
    readLabelFileHeader(labelFile, &labelFileHeader);
    return labelFile;
}




/**
 * @details Returns the next image in the given MNIST image file
 */

MNIST_Image getImage(FILE *imageFile){
    MNIST_Image img;
    size_t result;
    result = fread(&img, sizeof(img), 1, imageFile);
    if (result!=1) {
        printf("\nError when reading IMAGE file! Abort!\n");
        exit(1);
    }
    return img;
}




/**
 * @details Returns the next label in the given MNIST label file
 */

MNIST_Label getLabel(FILE *labelFile){
    MNIST_Label lbl;
    size_t result;
    result = fread(&lbl, sizeof(lbl), 1, labelFile);
    if (result!=1) {
        printf("\nError when reading LABEL file! Abort!\n");
        exit(1);
    }
    return lbl;
}


void displayTrainingProgress(int imgCount, int errCount){
    double progress = (double)(imgCount+1)/(double)(60000)*100;

    printf("1: TRAINING: Reading image No. %5d of %5d images [%3d%%]  ",(imgCount+1),60000,(int)progress);

    double accuracy = 1 - ((double)errCount/(double)(imgCount+1));

    printf("Result: Correct=%5d  Incorrect=%5d  Accuracy=%5.4f%% \n",imgCount+1-errCount, errCount, accuracy*100);
}



void displayTestingProgress(int imgCount, int errCount){
    double progress = (double)(imgCount+1)/(double)(10000)*100;
    printf("2: TESTING:  Reading image No. %5d of %5d images [%3d%%]  ",(imgCount+1),10000 ,(int)progress);

    double accuracy = 1 - ((double)errCount/(double)(imgCount+1));

    printf("Result: Correct=%5d  Incorrect=%5d  Accuracy=%5.4f%% \n",imgCount+1-errCount, errCount, accuracy*100);
}


void trainNetwork(Network *nn){
    // open MNIST files
    FILE *imageFile, *labelFile;
    imageFile = openMNISTImageFile("train-images-idx3-ubyte");
    labelFile = openMNISTLabelFile("train-labels-idx1-ubyte");
    int errCount = 0;

    // Loop through all images in the file
    for (int imgCount=0; imgCount<60000; imgCount++){
        // Reading next image and its corresponding label
        MNIST_Image img = getImage(imageFile);
        MNIST_Label lbl = getLabel(labelFile);
        // Convert the MNIST image to a standardized vector format and feed into the network
        Vector *inpVector = getVectorFromImage(&img);
        feedInput(nn, inpVector);
        // Feed forward all layers (from input to hidden to output) calculating all nodes' output
        feedForwardNetwork(nn);
        // Back propagate the error and adjust weights in all layers accordingly
        backPropagateNetwork(nn, lbl);
        // Classify image by choosing output cell with highest output
        int classification = getNetworkClassification(nn);
        if (classification!=lbl) errCount++;
    }
    // Close files
    fclose(imageFile);
    fclose(labelFile);
}




/**
 * @brief Testing the trained network by processing the MNIST testing set WITHOUT updating weights
 * @param nn A pointer to the NN
 */

void testNetwork(struct Network *nn){
    // open MNIST files
    FILE *imageFile, *labelFile;
    imageFile = openMNISTImageFile("t10k-images-idx3-ubyte");
    labelFile = openMNISTLabelFile("t10k-labels-idx1-ubyte");
    int errCount = 0;
    // Loop through all images in the file
    for (int imgCount=0; imgCount<10000; imgCount++){
        // Reading next image and its corresponding label
        MNIST_Image img = getImage(imageFile);
        MNIST_Label lbl = getLabel(labelFile);
        // Convert the MNIST image to a standardized vector format and feed into the network
        Vector *inpVector = getVectorFromImage(&img);
        feedInput(nn, inpVector);
        // Feed forward all layers (from input to hidden to output) calculating all nodes' output
        feedForwardNetwork(nn);
        // Classify image by choosing output cell with highest output
        int classification = getNetworkClassification(nn);
        if (classification!=lbl) errCount++;
        displayTestingProgress(imgCount, errCount);
    }
    // Close files
    fclose(imageFile);
    fclose(labelFile);
}

int* guessCharacters(struct Network *nn, int *listToSolve)
{
    int *listFilled = listToSolve;
    FILE *imageFile;
    imageFile = openMNISTImageFile("gridexport");

    for (int imgCount=0; imgCount<81; imgCount++)
    {
        MNIST_Image img = getImage(imageFile);

        Vector *inpVector = getVectorFromImage(&img);
        feedInput(nn, inpVector);
        feedForwardNetwork(nn);
        int classification = getNetworkClassification(nn);
        if(listToSolve[imgCount] != 0)
            listFilled[imgCount] = classification;
    }
    fclose(imageFile);
    return listFilled;
}



/**
 * @details Main function to run MNIST-1LNN
 */

int *NeuralNetwork(int* listToSolve) {
    Network *nn = createNetwork(28*28, 128, 10);
//    displayNetworkWeightsForDebugging(nn);
//    exit(1);
    // Training the network by adjusting the weights based on error using the  TRAINING dataset
    trainNetwork(nn);
    // Testing the during training derived network using the TESTING dataset
    int* listFilled = guessCharacters(nn, listToSolve);
    //testNetwork(nn);
    //
    // Free the manually allocated memory for this network
    free(nn);
    // Calculate and print the program's total execution time
    return listFilled;
}
