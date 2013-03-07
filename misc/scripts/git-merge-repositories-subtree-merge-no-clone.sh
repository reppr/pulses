#! /bin/bash
# '  ' indentation is for inclusion in git commit messages,
# as lines starting with '#' (like this line) get removed...

  #! /bin/bash

  # ****************************************************************
  # Script used to merge other source directories into an existing one
  # like importing the old c stuff into pulses:
  # '  ' indented to protect #comments when included in commit message.
  # ****************************************************************
  commandline="$*"


  # ****************************************************************
  # Use colours to make output more readable.
  # Get colour codes:
  black=$(tput setaf 0)
  red=$(tput setaf 1)
  green=$(tput setaf 2)
  yellow=$(tput setaf 3)
  blue=$(tput setaf 4)
  magenta=$(tput setaf 5)
  cyan=$(tput setaf 6)
  white=$(tput setaf 7)
  treset=$(tput sgr0)

  # echo "colour test:"
  # echo "${black}black"
  # echo "${red}red"
  # echo "${green}green"
  # echo "${yellow}yellow"
  # echo "${blue}blue"
  # echo "${magenta}magenta"
  # echo "${cyan}cyan"
  # echo "${white}white"
  # echo "${treset}treset"


  # ****************************************************************
  usage() {
    echo "${0##*/}: Merge another git repo into (a copy of) an existing one."
    echo
    echo "Usage:  ${0##*/} main-git-repo repo-to-import"
    echo
  }


  # ****************************************************************
  mangle_path() {		# X="$(mangle_path "$1")"
      [ -z "${1}" ] && {
	  # echo "${0##*/}  ${red}ERROR: empty path${treset}!"
	  echo "${0##*/}  $(tput setaf 1)ERROR: empty path$(tput sgr0)!"
          usage
	  exit 1
      }
      NEW_PATH="${1}"
      NEW_PATH="${NEW_PATH#./}"				# no starting ./
      [ "${NEW_PATH}" == '.' ] && NEW_PATH="$(pwd)"	# no '.'
      [ "${NEW_PATH:0:1}" != "/" ] && NEW_PATH="$(pwd)/${NEW_PATH}"
      if [ "${NEW_PATH}" != '/' ] ; then		# accept '/'
	  NEW_PATH="${NEW_PATH%/}"			# no trailing /
      fi
      echo "${NEW_PATH}"
  }


  # ****************************************************************
  # Commandline parameters:
  [ $# -ne 2 ] && {	# wrong number of parameters
      echo "${red}ERROR: wrong number of parameters${treset}"
      echo
      usage
      exit 1
  }

  #MAIN_GIT="/home/dada/PULSES_i5/pulses"
  MAIN_GIT="$(mangle_path "${1}")"
  MAIN_PAREN="${MAIN_GIT%/*}"
  MAIN_SUBDIR="${MAIN_GIT##*/}"

  #  OTHER_GIT_SRC="/home/dada/ARDUINO-GIT2/softboard"
  #  OTHER_GIT_SRC="/home/dada/arduino-GIT1"
  OTHER_GIT_SRC="$(mangle_path "${2}")"
  OTHER_SUBDIR="${OTHER_GIT_SRC##*/}"

  shift 2

  NEW_MAIN="/tmp/newGIT_${MAIN_SUBDIR}.$$"
  OTHER_TMP="/tmp/${OTHER_SUBDIR:-OTHER_CLONE}.$$"


  # ****************************************************************
  # Debugging:
  # Personal git error return codes:
  gitErrClone=1001
  gitErrAdd=1002
  gitErrStatus=1003
  gitErrCommit=1005

  gitErrMerge=1033

  gitErrPull=1061

  gitErrRemoteAdd=1071
  gitErrRemoteRm=1072


  # ****************************************************************
  echo "${cyan}${0##*/}"
  echo ${yellow}
  echo "Testing main git repo ${MAIN_GIT}${treset}"
  cd ${MAIN_GIT}		  || exit 1
  cd ${MAIN_PAREN}/${MAIN_SUBDIR} || exit 1	# should be same ;)
  git status || {
      echo ${red}${?}${treset}
      echo "  'git status' ${red}FAILED.${treset}"
      exit $gitErrStatus
  }

  echo ${yellow}
  echo "Testing other git repo ${OTHER_GIT_SRC}${treset}"
  cd $OTHER_GIT_SRC			  	|| exit 1
  cd ${OTHER_GIT_SRC%/*}/${OTHER_SUBDIR}	|| exit 1	# same ;)
  git status || {
      echo ${red}${?}${treset}
      echo "  'git status' ${red}FAILED.${treset}"
      exit $gitErrStatus
  }
  echo
  echo "${green}(tests succeeded :)${treset}"
  

  # ****************************************************************
  # Summary of planed actions, ask for confirmation:
  echo 
  echo "${cyan}================================================================"
  echo "${0##*/}${red} PLANED ACTIONS${treset}"
  echo "${cyan}Merge other repository ${OTHER_GIT_SRC} into"
  echo "a ${MAIN_GIT} *COPY*"
  echo "================================================================${treset}"
  echo
  echo "${yellow}First prepare a CLONE of the other repository${treset}:"
  echo "  * clone ${OTHER_GIT_SRC} into ${OTHER_TMP}"
  echo "    The clone will be in ${OTHER_TMP}/${OTHER_SUBDIR}"
  echo
  echo "${yellow}  Then work in the clone${treset}:"
  echo "  * remove remote origin in ${OTHER_SUBDIR}"
  echo "  * prepare commit message and commit --allow-empty"
  echo "    ${cyan}commit --amend${yellow} to edit commit message and include this script."
  echo "  * ${cyan}Give you a shell to work in the CLONE of ${OTHER_SUBDIR}${treset}"
  echo
  echo "${yellow}*COPY* the main repo ${red}(with all files)${yellow} to a new repo${treset}:"
  echo "  * cp -ap ${MAIN_PAREN} ${NEW_MAIN}"
  echo "    The base repo copy will be in ${NEW_MAIN}/${MAIN_SUBDIR}"
  echo "  * merge ${OTHER_SUBDIR} from the clone we have been working on"
  echo "  * ${cyan}commit${treset}"
  echo "  * git pull -s subtree ${OTHER_SUBDIR} master"
  echo
  echo "${cyan}When done give you a shell in ${MAIN_SUBDIR} to check the new repo.${treset}"
  echo
  read -p "Proceed  ${cyan}(Y/n)${treset} ? " input
  # echo "MAIN_PAREN	$MAIN_PAREN"
  # echo "MAIN_SUBDIR	$MAIN_SUBDIR"
  # echo "OTHER_SUBDIR	$OTHER_SUBDIR"
  # echo "NEW_MAIN	$NEW_MAIN"
  case ${input:-Y} in
      Y|y) ;;
      *) echo "${red}nothing done, nothing spoilt${treset} ;)"
	  exit 1
  esac


  # ****************************************************************
  # Start all over again...
  rm -rf $OTHER_TMP $NEW_MAIN  # obsolete as long as we use $$


  # ****************************************************************
  echo "${yellow}Clone ${OTHER_GIT_SRC} to temporary location ${OTHER_TMP}${treset}"
  # git clones into a subdir named after the paren directory of the
  # cloned repository:

  mkdir ${OTHER_TMP} || exit 1
  cd ${OTHER_TMP}    || exit 1

  git clone ${OTHER_GIT_SRC} || {
      echo ${red}${?}${treset}
      echo "  'git clone ${OTHER_GIT_SRC}' ${red}FAILED.${treset}"
      exit $gitErrClone
  }
  #pwd ; ls -Al
  
  cd ${OTHER_TMP}/${OTHER_SUBDIR} || exit 1
  # pwd ; ls -Al
  #
  
  
  # ****************************************************************
  echo "${yellow}Remove remote origin in ${OTHER_TMP}/${OTHER_SUBDIR}${treset}"
  git remote rm origin || {
      echo ${red}${?}${treset}
      echo "  'git remote rm origin' ${red}FAILED.${treset}"
      exit $gitErrRemotrRm
  }


  # ****************************************************************
  echo ${cyan}
  echo "Include this script in commit message.${treset}"

  git commit --allow-empty -m "Prepare merge of ${OTHER_GIT_SRC}
  
  Preparing repository merge: Do not use.
  
  git commit

  [edit commit message], then
  git commit --allow-empty --amend

  ****************************************************************
  ${0##*/} ${commandline}

>>>> >>> >> > INSERT script ${0} HERE < << <<< <<<<

  ****************************************************************
  " || {
      echo ${red}${?}${treset}
      echo "  'git commit --allow-empty -m' ${red}FAILED.${treset}"
      exit $gitErrCommit
  }


  
  # ****************************************************************
  echo ${cyan}
  echo "  Please edit commit message
  and *do* include this script BY HAND IN COMMIT MESSAGE.
  file:  ${0}
    ${blue}<ENTER>${treset}"
  read dummy

  git commit --allow-empty --amend || {
      echo ${red}${?}${treset}
      echo "  'git commit --allow-empty --amend' ${red}FAILED.${treset}"
      exit $gitErrCommit
  }


  # ****************************************************************
  echo ${cyan}
  echo "Please check and work now in the cloned ${OTHER_TMP} repository.
  ${yellow}
  Check everything,
  make commits if needed,
  move files, i.e. README
  ${red}
  OPENING A BASH SHELL NOW
  ${cyan}exit shell when done
    ${blue}<ENTER>${treset}
  "
  read dummy

  PS1="${cyan}${OTHER_SUBDIR}${red}\$ ${magenta}"  bash --norc -i
  echo ${treset}
  echo "(back fom sub shell)"


  # ****************************************************************
  echo "${cyan}cp -ap $MAIN_PAREN $NEW_MAIN${treset}"
  mkdir $NEW_MAIN			|| exit 1
  cd ${MAIN_PAREN}			|| exit 1
  cp -ap $MAIN_SUBDIR $NEW_MAIN	|| exit 1

  # ****************************************************************
  echo
  echo "${yellow}Merge cloned ${OTHER_SUBDIR} repo into ${NEW_MAIN}${treset}"
  cd ${NEW_MAIN}/${MAIN_SUBDIR}	|| exit 1
  #echo ; pwd ; ls -al ; git status



  otherGit="unknownRepo"
  [ -n $OTHER_SUBDIR ] && otherGit=$OTHER_SUBDIR

  git remote add -f ${otherGit} ${OTHER_TMP}/${OTHER_SUBDIR} || {
      echo ${red}${?}${treset}
      echo "  'git remote add -f ${otherGit} ${OTHER_TMP}/${OTHER_SUBDIR}' ${red}FAILED.${treset}"
      exit $gitErrRemoteAdd
  }
  echo "	${blue}<ENTER>${treset}"
  read dummy

  git merge --no-commit -m "Merged ${OTHER_GIT_SRC}
  
  git remote add -f ${otherGit} ${OTHER_TMP}
  git merge --no-commit ${otherGit}/master
  git commit

  git pull -s subtree ${otherGit} master
  git remote rm ${otherGit}
  " \
      --log=10000 ${otherGit}/master	|| {
      echo ${red}${?}${treset}
      echo "  'git merge --log=10000 ${otherGit}/master' ${red}FAILED.${treset}"
      exit $gitErrMerge
  }

  echo ${cyan}
  echo "Edit commit message, please.${treset}"
  echo "	${blue}<ENTER>${treset}"
  read dummy

  git commit --allow-empty || {
      echo ${red}${?}${treset}
      echo "  'git commit --allow-empty -m' ${red}FAILED.${treset}"
      exit $gitErrCommit
  }

  echo ${magenta}
  echo "git pull -s subtree ${otherGit} master${treset}"
  # echo "	${blue}<ENTER>${treset}"
  # read dummy
  git pull -s subtree ${otherGit} master	|| {
      echo ${red}${?}${treset}
      echo "  'git pull -s subtree ${otherGit} master' ${red}FAILED.${treset}"
      exit $gitErrPull
  }



  echo ${magenta}
  echo "git remote rm ${otherGit}${treset}"
  # echo "	${blue}<ENTER>${treset}"
  # read dummy
  git remote rm ${otherGit} || {
      echo ${red}${?}${treset}
      echo "  'git remote rm' ${red}FAILED.${treset}"
      exit $gitErrRemoteRm
  }
  #pwd ; ls -Al
  
  
  # ****************************************************************
  echo
  echo "************************************************"
  echo "${0##*/}  ${cyan}done :)"
  echo "New git repo is in ${treset}${NEW_MAIN}"
  echo "${yellow}Check and copy to ${treset}${MAIN_GIT} ${yellow}by hand.${treset}"
  echo "************************************************"

  echo ${cyan}
  echo "git branch${treset}"
  git branch

  echo ${cyan}
  echo "ls -Al${treset}"
  ls -Al

  echo ${cyan}
  echo "git status${treset}"
  git status


  # ****************************************************************
  # Leave the user on a shell in new repo:
  echo "${cyan}NEW GIT REPOSITORY READY ${yellow}in ${NEW_MAIN}/${MAIN_SUBDIR}"
  echo "Opening a shell there to examine result now.${treset}"
  echo "  ${cyan}exit shell when done
    ${blue}<ENTER>${treset}
  "
  read dummy

  # PS1="${red}\$ ${yellow}"  bash --norc -i
  PS1="${cyan}${MAIN_SUBDIR}${red}\$ ${yellow}"  bash --norc -i
  echo ${treset}

  echo "${green}Left new git repository in ${treset}${NEW_MAIN}/${MAIN_SUBDIR}"

  # ****************************************************************
