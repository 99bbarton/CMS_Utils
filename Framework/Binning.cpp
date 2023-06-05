//Implementation of Binning class for storing information on histogram bin limits and labelling

#include "Binning.h"


//Copy constructor
Binning::Binning(Binning& that)
{
    this->name = that.name;
    this->nBins = that.nBins;
    this->isCustom = that.isCustom;
    this->min = that.min;
    this->max = that.max;
    if (that.lowEdges != NULL)
    {
        double lowEdges[this->nBins + 1];
        double labels[this->nBins];
        for (int i = 0; i < nBins; i++)
        {
            lowEdges[i] = that.lowEdges[i];
            labels[i] = that.labels[i];
        }
        lowEdges[this->nBins+1] = that.lowEdges[this->nBins+1];
        this->lowEdges = lowEdges;
        this->labels = labels;
    }
    else
    {
        this->lowEdges = NULL;
        this->labels = NULL;
    }
    
};

//Non-custom binning constructor
Binning::Binning(string name, int nBins, double min, double max)
{
    this->name = name;
    this->nBins = nBins;
    this->min = min;
    this->max = max;
    this->isCustom = false;
    this->lowEdges = NULL;
    this->labels = NULL;
}

//Custom binning constructor
Binning::Binning(string name, int nBins, double lowEdges[], string labels[])
{
    this->name = name;
    this->nBins = nBins;
    this->isCustom = true;

    if (lowEdges != NULL)
    {
        this->min = lowEdges[0];
        this->max = lowEdges[nBins+1]; 
        this->lowEdges = LowEdges;
        this->labels = Labels;
    }
    else
    {
        throw std::invalid_argument;
    }
}

Binning::~Binning()
{
    delete name;

    if (lowEdges != NULL)
    {
        delete lowEdges;
        delete labels;
    }

}