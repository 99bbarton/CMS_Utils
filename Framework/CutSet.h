// A collection of cuts

#include <string>
#include <vector>
#include <map>


class CutSet
{
    public:
        std::string name; //A name of the cut set as a whole
        std::string description; //An (optional) desciption of cuts
        int nCuts;
        std::map<std::string, int> indexMap; //A mapping of cut name to corresponding idx in the vectors below
        std::vector<std::string> cuts; //The actual cut strings
        std::vector<std::string> ops; //The logical operators used to join the cuts        
        std::vector<std::string> sources; //The sources e.g. twiki pages, github links, etc of the cut choices
        std::string cutStr; //A cumulative cutStr composed of all of the cuts in cuts joined by the operators in ops

        CutSet();
        CutSet(CutSet& that);
        CutSet(std::string name, std::string description); //Standard constructor taking a name and description for the cutSet as a whole
        ~CutSet();

        bool addCut(std::string name, std::string cutStr, std::string op="&&", std::string source=""); //Add a new cut to the set
        int addCutSet(CutSet& that); //Copy the cuts in that to this
        int importFromFile(std::string filepath); //Read the cuts and/or cutset parameters from a file and add them to/modify this
        bool writeToFile(std::string filepath); //Write the cutset and cut info to a file in the same format as expected by the importFromFile() method
        //Get a vector of cutnames which contain the string(s) in strsToMatch (matching ALL strings or any). Returns the cumulative sub-cutStr
        std::string getCutsMatching(std::vector<std::string>& strsToMatch, std::vector<std::string>& matchedCutNames, bool all=false); 
        //Same as above but get cuts which do not match string(s) in strsToMatch
        std::string getCutsNotMatching(std::vector<std::string>& strsToMatch, std::vector<std::string>& nonMatchedCutNames);
        void show(std::string matchingStr = ""); //Print the cuts in th ecutset in alphanumeric sorted order

};