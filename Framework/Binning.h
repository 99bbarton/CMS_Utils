//Class for storing information regarding histogram bin limits and labelling

#include <string>


class Binning
{    
    public:
    string name; //Name of the binning 
    int nBins; //The number of bins
    bool isCustom; //Whether the binning is custom or evenly spaced
    double min; //Lowest bin edge (inclusive),
    double max; //Highest bin edge (exclusive), 
    double* lowEdges; //if isCustom, Array of length nBins+1 of the lowEdges of the bins. Else NULL
    string* labels; //if isCustom, Array of length nBins of the labels of the bins. Else NULL
    
    Binning(Binning & that); //Copy constructor
    Binning(string name, int nBins, double min, double max); //Non-custom binning constructor
    Binning(string name, int nBins, double lowEdges[], string labels[]); //Custom binning constructor
    
    private:
    ~Binning(); //Destructor

};
