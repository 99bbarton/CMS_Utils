#Useful aliased bash commands when working on LPC

## Safety aliases
alias rm='rm -i' #By default, rm will now ask for permission to delete each file unless -f is specified

## Editors and packages
alias root='root -l' # No graphical loading screen when opening root
alias emacs='emacs -nw' # emacs is only support in terminal (not x window) on LPC. Specifying -nw makes opening emacs faster

## Utilities 
alias clntmps='rm \#* *~' #Clean temporary/backup files produced by text editors
alias voms='voms-proxy-init --voms cms' #Shortened command to request authentication with your grid certificate

## Navigation
#alias cd13='cd ~/nobackup/CMSSW_10_6_13/src/' #Short cut to navigating to commonly used directories
#alias cd23='cd ~/nobackup/CMSSW_10_6_23/src/'
#alias cdts='cd ~/nobackup/TauStar/'

