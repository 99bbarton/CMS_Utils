
#include "CutSet.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>

//Default constructor. Set strings to empty strings and count to 0
CutSet::CutSet()
{
    this->name = "";
    this->description = "";
    this->nCuts = 0;
    this->cutStr = "";
}

//Copy the contents of that to this
CutSet::CutSet(CutSet& that)
{
    this->name = that.name;
    this->description = that.description;
    this->nCuts = that.nCuts;
    this->cutStr = that.cutStr;
    for (int i = 0; i < nCuts; i++)
    {
        //TODO
        this->indexMap.insert(std::pair<std::string,int>())
    }
}


CutSet::~Cutset(){} //Nothing to destroy

/* Add a cut to the set. At minimum a name for the cut and a the cut string itself must be provided
name : a string itentifying the cut. e,g, 2018_MuTau_muon_ID_tight See comments at top of file for recommended naming convention 
cutStr : a string of the cut itself e.g. "Muon_pt[0] > 20 && abs(Muon_eta) < 2.4" () around the entire string will be added automatically
op : (optional) The operator which this cut should be joined with the previous cuts with options (&&, ||, xor). default: &&
source : (optional) A string citing the source of the cut. e.g. a POG twiki link
Returns true if a cut is succesfully added and false if not */
//TODO : Handle duplicate keys
bool CutSet::addCut(std::string name, std::string cutStr, std::string op="&&", std::string source="")
{
    if (op == "&&" || op == "||" || op == "xor" || op == "XOR")
    {
        this->indexMap.insert(std::pair<std::string, int>(name, this->nCuts));
        this->cuts.push_back("(" + cutStr + ")");
        this->ops.push_back(op);
        this->sources.push_back(source);
        this->nCuts++;

        if (op == "xor" || op == "XOR")
            this->cutStr = "((" + this->cutStr + "&& !(" + cutStr + ")) || ( !" + this->cutStr + " && (" + cutStr + ")))";
        else
            this->cutStr = "(" + this->cutStr + op + " (" + cutStr + "))";

        return true;
    }
    else
    {
        std::cout << "ERROR: Operator " << op << " is not recognized. Skipping adding a cut... op must be {&&, ||, xor}";
        return false;
    }
}

/*Combine another cutSet object with this one. See addCut() for details of addition. Any duplicate named (with this) cuts are skipped
cutSet : another CutSet object to combine with this one
Returns the number of cuts added*/
int CutSet::addCutSet(CutSet& that)
{
    int nAdded = 0;
    
    for (std::map<std::string, int>::iterator itr=that.indexMap.begin(); itr != that.indexMap.end(); ++itr )
    {
        std::string name = itr->first;
        int idx = itr->second;

        this->addCut(name, that.cuts[idx], that.ops[idx], that.sources[idx]);
        
        nAdded++;
    }

    return nAdded;
}

/*Read cuts and/or CutSet meta information from a file and add them to this CutSet
filepath : a string file path+name to read in. If the file cannot be read, no action is taken
The function looks for "tags" at the beginning of line (NAME, DESCRIPTION, CUT) separated by ":" from the following associated line content
The function further assumes that the line content after the "CUT :" is of the form: cut string,operator,source str corresponding to addCut params
Returns the number of cuts added*/
int CutSet::importFromFile(std::string filepath)
{
    std::ifstream inFile(filepath, std::ifstream::in);

    char lineRaw[1000];
    int nAdded = 0;
    while (inFile.good())
    {
        inFile.getline(lineRaw, 1000);
        std::string line(lineRaw);

        if (line.find("NAME :") != std::string::npos)
            this->name = line.substr(6);
        if (line.find("DESCRIPTION :") != std::string::npos)
            this->description = line.substr(13);
        if (line.find("CUT :") != std::string::npos)
        {
            size_t commaIdx = line.find(",");
            std::string cutName = line.substr(5, commaIdx);
            size_t oldIdx = commaIdx;
            commaIdx = line.find(",", commaIdx);
            std::string cutStr = line.substr(oldIdx + 1, commaIdx);
            oldIdx = commaIdx;
            commaIdx = line.find(",", commaIdx);
            std::string op = line.substr(oldIdx + 1, commaIdx);
            oldIdx = commaIdx;
            commaIdx = line.find(",", commaIdx);
            std::string source = line.substr(oldIdx + 1, commaIdx);
            
            this->addCut(cutName, cutStr, op, source);
            nAdded++;
        }
    }
    
    inFile.close();

    return nAdded;
}

/*Write the CutSet to a file. File is written in the same form as importFromFile() expects
filepath : a file path+name to write the output to. 
Returns True if the file is written, false otherwise*/
bool CutSet::writeToFile(std::string filepath)
{
    std::ofstream outFile(filepath, std::ofstream::out);

    std::string outStr = "#CutSet dump at " + currentDateTime() + "\n\n";
    outFile.write(outStr.c_str(), outStr.length());
    outStr = "NAME : " + this->name + "\n";
    outFile.write(outStr.c_str(), outStr.length());
    outStr = "DESCRIPTION : " + this->description + "\n";
    outFile.write(outStr.c_str(), outStr.length());

    for (std::map<std::string, int>::iterator itr=this->indexMap.begin(); itr != this->indexMap.end(); ++itr )
    {
        std::string name = itr->first;
        int idx = itr->second;

        outStr = "CUT : " + name + ", " + this->cuts[idx] + ", " + this->ops[idx] + ", " + this->sources[idx] + "\n";
        outFile.write(outStr.c_str(), outStr.length());
    }

    return true;
}


/*Return the cumulative cut string and list of cuts whose names contain the strings in strsToMatch
strsToMatch : A vector of strings to search for in the names of the cuts in this->cuts
all : Whether or not a cut name must match all or any of strings in strToMatch. default false 
Returns the cumulative cutStr and list of cuts matching the above conditions*/
std::string CutSet::getCutsMatching(std::vector<std::string>& strsToMatch, std::vector<std::string>& matchedCutNames, bool all=false)
{
    std::string cutStr = "(1>0)";

    if (all)
    {
        for (std::map<std::string, int>::iterator itr=this->indexMap.begin(); itr != this->indexMap.end(); ++itr )
        {
            std::string name = itr->first;
            int idx = itr->second;

            bool matchesAll = true;

            for (int sN = 0; sN < strsToMatch.size(); sN++)
            {
                if (name.find(strsToMatch[sN]) == std::string::npos)
                {
                    matchesAll = false;
                    break;
                }
            }

            if (matchesAll)
            {
                matchedCutNames.push_back(name);
                cutStr += " " + this->ops[idx] + " " + this->cuts[idx];
            }
        }
    }
    else
    {
        for (int sN = 0; sN < strsToMatch.size(); sN++)
        {
            for (std::map<std::string, int>::iterator itr=this->indexMap.begin(); itr != this->indexMap.end(); ++itr )
            {
                std::string name = itr->first;
                int idx = itr->second;

                if (name.find(strsToMatch[sN]) != std::string::npos)
                {
                    matchedCutNames.push_back(name);
                    cutStr += " " + this->ops[idx] + " " + this->cuts[idx];
                }
            }
        }
    }

    return cutStr;
}

/* Return the cumulative cut string and list of cuts whose names contain the strings in strsToMatch
strsToExclude : A list of strings to search for in the names of the cuts in this->cuts
Returns the cumulative cutStr and list of cuts not matching the the strings in strsToExclude*/
std::string CutSet::getCutsNotMatching(std::vector<std::string>& strsToExclude, std::vector<std::string>& nonMatchedCutNames)
{   
    std::string cutStr = "(1>0)";

    for (int sN = 0; sN < strsToExclude.size(); sN++)
    {
        for (std::map<std::string, int>::iterator itr=this->indexMap.begin(); itr != this->indexMap.end(); ++itr )
            {
                std::string name = itr->first;
                int idx = itr->second;

                if (name.find(strsToExclude[sN]) == std::string::npos)
                {
                    nonMatchedCutNames.push_back(name);
                    cutStr += " " + this->ops[idx] + " " + this->cuts[idx];
                }
            }
    }

    return cutStr;
}

/*Print the cuts in the cutset
matchingStr : An optional string to require cuts to match in order to be printed*/
//TODO implement sorting of order
void CutSet::show(std::string matchingStr = "")
{
    for (std::map<std::string, int>::iterator itr=this->indexMap.begin(); itr != this->indexMap.end(); ++itr )
        {
            std::string name = itr->first;
            int idx = itr->second;

            if (matchingStr.size() > 0)
            {
                if (name.find(matchingStr) != std::string::npos)
                    std::cout << name + " : " + this->cuts[idx] << std::endl;
            }
            else
                std::cout << name + " : " + this->cuts[idx] << std::endl;
        }
}



// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
// Taken from: https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}