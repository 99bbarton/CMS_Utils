# A basic bash_profile for the LPC which is sourced each time you connect vis ssh

## Source .bash_aliases file to get access to useful aliased commands
if [ -f ~/.bash_aliases ]; then
    . ~/.bash_aliases
fi

## CMS environment variable setup
source /cvmfs/cms.cern.ch/cmsset_default.sh
export CMSSW_GIT_REFERENCE=/cvmfs/cms.cern.ch/cmssw.git.daily

## Useful environment variables
export XRDURL=root://cmseos.fnal.gov/ #When copying to/from LPC EOS area, this URL is needed before the /store/<...> filepath 
export ROOTURL=root://cmsxrootd.fnal.gov/ #Accessing .root files located on LPC EOS should have this URL in front of the /store/<...> filepath