## A class to store a collection of cuts, allowing import/export of cuts, searching/subset creation, etc
# Cuts are stored both individually in an OrderedDict (to ensure cut ordering is consisent each use) and in a single combined string
# The dict maps a unique string name to a list of the form [cut string, operator, source] (see addCuts() documentation below)
# It is recommended that names of cuts follow a conistent format so that viewing and searching is most effective
# A recommended conventions is: YEAR_CHANNEL_OBJECT_TYPE_VARIABLE_<VALUE>  e.g. 2018_MuTau_muon_ID_tightId or 2016_ETau_photon_kinematic_pt_20
# Individual cuts may be combined abritrarily via search functions and combined with the AND, OR, XOR logical operators
# The cumulative strings can be used directly as cuts in ROOT


import os
from collections import OrderedDict
import datetime

class CutSet:
    name = "" #A name of the cut set as a whole
    description = "" #An (optional) desciption of cuts
    nCuts = 0 
    cuts = {} #An ordered dict mapping cut names to cut strings and the logical operator which is used to join each cut to the others
    cutStr = "" #A cumulative cut str made up of all of the concatenated cuts in cuts
    
    ## Initialize the cutset object
    # name : A name to identify the cutset by. Default ""
    # description : A desciption of the cutset. Default ""
    def __init__(self, name="", description=""):
        self.name = name
        self.description = description
        self.nCuts = 0
        self.cuts = OrderedDict()

    #--------------------------------------------------------------------------------------------------------------------------------------

    ## Add a cut to the set. At minimum a name for the cut and a the cut string itself must be provided
    # name : a string itentifying the cut. e,g, 2018_MuTau_muon_ID_tight See comments at top of file for recommended naming convention 
    # cutStr : a string of the cut itself e.g. "Muon_pt[0] > 20 && abs(Muon_eta) < 2.4" () around the entire string will be added automatically
    # op : (optional) The operator which this cut should be joined with the previous cuts with options (and, or, xor). default: and
    # source : (optional) A string citing the source of the cut. e.g. a POG twiki link
    # Returns True if a cut is succesfully added and False if not
    def addCut(self, name, cutStr, op="and", source=""):
        if op.lower() == "and" or op.lower() == "or":
            self.cutStr = "(" + self.cutStr + op.lower() + "(" + cutStr + "))"
        elif op.lower == "xor":
            self.cutStr = "((" + self.cutStr + "and not (" + cutStr + ")) or (not " + self.cutStr + " and (" + cutStr + ")))"
        else:
            print("ERROR: Operation " + op + " not recognized. Skippling adding a cut... op must be {and, or, xor}")
            cutstr = ""
            return False
    
        self.cuts[name] = [cutStr, op, source]
        self.nCuts += 1

        return True
    
    #--------------------------------------------------------------------------------------------------------------------------------------

    ## Combine another cutSet object with this one. See addCut() for details of addition. Any duplicate named (with self) cuts are skipped
    # cutSet : another CutSet object to combine with this one
    # Returns the number of cuts added
    def addCutSet(self, cutSet):
        if type(cutSet) not CutSet:
            print("ERROR: Must pass a valid CutSet object to the addCutSet() function. Skipping addition...")
            return 0
        
        nAdded = 0
        for name, cut in enumerate(cutSet.cuts):
            if name not in self.cuts.keys():
                self.addCut(name=name, cutStr=cut[0], op=cut[1], source=cut[2])
            else:
                print("WARNING: Attempting to add duplicate cut called " + name + " . Skipping...")

        return nAdded

    #--------------------------------------------------------------------------------------------------------------------------------------
    
    ## Read cuts and/or CutSet meta information from a file and add them to this CutSet
    # filepath : a string file path+name to read in. If the file cannot be read, no action is taken
    # The function looks for "tags" at the beginning of line (NAME, DESCRIPTION, CUT) separated by ":" from the following associated line content
    # The function further assumes that the line content after the "CUT :" is of the form: cut string,operator,source str corresponding to addCut params
    # Returns the number of cuts added
    def importFromFile(self, filepath):
        if not os.path.isfile(filepath):
            print("ERROR: File " + filepath  + " does not exist or cannot be read")
            return 0
        with open(filepath, "r") as file:
            lines = file.readlines()
            
            addedCount = 0
            for line in lines:
                if line.startswith("#"):
                    continue
                if line.startswith("NAME"):
                    self.name = line.split(":")[1].trim()
                if line.startswith("DESCRIPTION"):
                    self.description = line.split(":")[1].trim()
                if line.startswith("CUT"):
                    cutLine = line.split(":")[1].split(",")
                    self.addCut(cutLine[0].trim(), cutLine[1].trim(), cutLine[2].trim(), cutLine[3].trim())
                    addedCount += 1

        self.nCuts += addedCount
        return addedCount

    #--------------------------------------------------------------------------------------------------------------------------------------

    ## Write the CutSet to a file. File is written in the same form as importFromFile() expects
    # filepath : a file path+name to write the output to. If the file already exists, asks for approval to overwrite the file
    # Returns True if the file is written, false otherwise
    def writeToFile(self, filepath):
        if os.path.isfile(filepath):
            approvCheck = input("ERROR: File " + filepath  + " already exists. Do you wish to overwrite? Enter (Y/N): ")
            if approvCheck.upper() != "Y":
                return False
        
        with open(filepath, "W+") as file:
            file.write("#CutSet dump at " + str(datetime.now()) + "\n\n")
            file.write("NAME : " + self.name + "\n")
            file.write("DESCRIPTION : " + self.description + "\n")
            file.write("CUT : " + self.cutStr + "\n#\n")
            for name, cut in enumerate(self.cuts):
                file.write("CUT : " + name + ", " + cut[0] + ", " + cut[1] + ", " + cut[2] + "\n")
        return True

    #--------------------------------------------------------------------------------------------------------------------------------------

    ## Return the cumulative cut string and list of cuts whose names contain the strings in strsToMatch
    # strsToMatch : A list of strings to search for in the names of the cuts in self.cuts
    # all : Whether or not a cut name must match all or any of strings in strToMatch. default False 
    # Returns the cumulative cutStr and list of cuts matching the above conditions
    def getCutsMatching(self, strsToMatch=[], all=False):
        cutNameList = []
        cutStr = "(1>0)"

        if all:
            for cutName in self.cuts.keys():
                matchesAll = True
                for strToMatch in strsToMatch:
                    if not cutName.find(strToMatch) >= 0:
                        matchesAll = False
                        break
                if matchesAll:
                    cutNameList.append(cutName)
                    cutStr +=  " " + self.cuts[cutName][1] + " " + self.cuts[cutName][0]
        else:
            for strToMatch in strsToMatch:
                for cutName in self.cuts.keys():
                    if cutName.find(strToMatch) >= 0 and cutName not in cutNameList:
                        cutNameList.append(cutName)
                        cutStr +=  " " + self.cuts[cutName][1] + " " + self.cuts[cutName][0]

        return cutStr, cutNameList

    #--------------------------------------------------------------------------------------------------------------------------------------

    ## Return the cumulative cut string and list of cuts whose names contain the strings in strsToMatch
    # strsToExclude : A list of strings to search for in the names of the cuts in self.cuts
    # Returns the cumulative cutStr and list of cuts not matching the the strings in strsToExclude
    def getCutsNotMatching(self, strsToExclude=[]):
        cutNameList = []
        cutStr = "(1>0)"

        for strToExclude in strsToExclude:
            for cutName in self.cuts.keys():
                if cutName.find(strToExclude) >= 0:
                    continue
                else:
                    cutNameList.append(cutName)
                    cutStr +=  " " + self.cuts[cutName][1] + " " + self.cuts[cutName][0]

        return cutStr, cutNameList
    
    #--------------------------------------------------------------------------------------------------------------------------------------

    ## Print the cuts in the cutset in alphanumeric sorted order
    # matchingStr : An optional string to require cuts to match in order to be printed
    def show(self, matchingStr = ""):
        
        sortedCutNames = sorted(self.cuts.keys())
        
        for name in sortedCutNames:
            if len(matchingStr) >= 1:
                if name.find(matchingStr) >= 0:
                    print(name + " : " + self.cuts[name][0])
            else:
                print(name + " : " + self.cuts[name][0])

    #--------------------------------------------------------------------------------------------------------------------------------------