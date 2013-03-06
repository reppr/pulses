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


  # ****************************************************************
  mangle_path() {		# X="$(mangle_path "$1")"
      [ -z "${1}" ] && {
	  # echo "${0##*/}  ${red}ERROR: empty path${treset}!"
	  echo "${0##*/}  $(tput setaf 1)ERROR: empty path$(tput sgr0)!"
          # usage
	  exit 1
      }
      NEW_PATH="${1}"
      NEW_PATH="${NEW_PATH#./}"			# no starting ./
      [ "${NEW_PATH}" == '.' ] && NEW_PATH="$(pwd)"	# no '.'
      [ "${NEW_PATH:0:1}" != "/" ] && NEW_PATH="$(pwd)/${NEW_PATH}"
      if [ "${NEW_PATH}" != '/' ] ; then		# accept '/'
	  NEW_PATH="${NEW_PATH%/}"			# no trailing /
      fi
      echo "${NEW_PATH}"
  }


  # ****************************************************************
  #MAIN_GIT="/home/dada/PULSES_i5/pulses"
  MAIN_GIT="$(mangle_path "${1}")"
  MAIN_PAREN="${MAIN_GIT%/*}"
  MAIN_SUBDIR="${MAIN_GIT##*/}"

  #  OTHER_GIT_SRC="/home/dada/ARDUINO-GIT2/softboard"
  #  OTHER_GIT_SRC="/home/dada/arduino-GIT1"
  OTHER_GIT_SRC="$(mangle_path "${2}")"
  OTHER_SUBDIR="${OTHER_GIT_SRC##*/}"

  shift 2

  NEW_MAIN="/tmp/newGIT_${OTHER_SUBDIR}.$$"
  OTHER_TMP="/tmp/OTHER_CLONE.$$"

  # echo "MAIN_GIT	$MAIN_GIT"
  # echo "MAIN_PAREN	$MAIN_PAREN"
  # echo "MAIN_SUBDIR	$MAIN_SUBDIR"
  # echo "OTHER_GIT_SRC	$OTHER_GIT_SRC"
  # echo "OTHER_SUBDIR	$OTHER_SUBDIR"
  # echo "NEW_MAIN	$NEW_MAIN"


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
  
  # ****************************************************************
  # Start all over again...
  rm -rf $OTHER_TMP $NEW_MAIN  # obsolete as long as we use $$
  
  
  # ****************************************************************
  echo "${magenta}Clone ${OTHER_GIT_SRC} to temporary location ${OTHER_TMP}${treset}"
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
  
  git commit || exit gitErrCommit

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
  exit shell when done
    ${blue}<ENTER>${treset}
  "
  read dummy

  PS1="${cyan}\w ${red}\$ ${magenta}"  bash --norc -i
  echo ${treset}
  echo "(back fom sub shell)"


  # ****************************************************************
  echo "${cyan}cp -ap $MAIN_PAREN $NEW_MAIN${treset}"
  mkdir $NEW_MAIN			|| exit 1
  cd ${MAIN_PAREN}			|| exit 1
  cp -ap $MAIN_SUBDIR $NEW_MAIN	|| exit 1
  echo "	${blue}<ENTER>${treset}"
  read dummy

  echo "${cyan}Merge ${OTHER_GIT_SRC} repo into ${NEW_MAIN}${treset}"
  cd ${NEW_MAIN}/${MAIN_SUBDIR}	|| exit 1
  #echo ; pwd ; ls -al ; git status


  # ****************************************************************
  echo "${yellow}  Pull from ${OTHER_TMP}${treset}"

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
  
  Transition: Do not use.
  
  git remote add -f ${otherGit} ${OTHER_TMP}
  git merge --no-commit ${otherGit}/master || exit gitErrMerge
  git commit || exit gitErrCommit

  git pull -s subtree ${otherGit} master
  git remote rm ${otherGit}
  " \
      --log=10000 ${otherGit}/master	|| {
      echo ${red}${?}${treset}
      echo "  'git merge --log=10000 ${otherGit}/master' ${red}FAILED.${treset}"
      exit $gitErrMerge
  }

  echo ${yellow}
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
  echo ${yellow}
  echo "Opening a shell now in $(pwd) to examine result now.${treset}"
  echo "  exit shell when done
    ${blue}<ENTER>${treset}
  "
  read dummy

  PS1="${cyan}\w ${red}\$ ${yellow}"  bash --norc -i
  echo ${treset}

  # ****************************************************************
