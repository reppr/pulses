#! /bin/sh
# '  ' indentation is for inclusion in git commit messages,
# as lines starting with '#' (like this line) get removed...

  #! /bin/sh

  # ****************************************************************
  # Script used to merge other source directories into an existing one
  # like importing the old c stuff into pulses:
  # '  ' indented to protect #comments when included in commit message.
  # ****************************************************************
  commandline="$*"

  DEST_GIT="/home/dada/PULSES_i5/pulses"

#  OTHER_GIT="/home/dada/ARDUINO-GIT2/softboard"
  OTHER_GIT="/home/dada/arduino-GIT1"

  DEST_COPY_DIR="/tmp/GIT-new.$$"
  OTHER_TMP="/tmp/GIT-OTHER.$$"

  DEST_PAREN="${DEST_GIT%/*}"
  DEST_SUBDIR="${DEST_GIT##*/}"
  OTHER_SUBDIR="${OTHER_GIT##*/}"


  # ****************************************************************
  # Use colours to make output more readable
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
  echo "${cyan}${0##*/}"
  echo ${yellow}
  echo "Testing main git repo ${DEST_GIT}${treset}"
  cd ${DEST_GIT}		  || exit 1
  cd ${DEST_PAREN}/${DEST_SUBDIR} || exit 1	# should be same ;)
  git status || exit 1

  echo ${yellow}
  echo "Testing other git repo ${OTHER_GIT}${treset}"
  cd $OTHER_GIT			  	|| exit 1
  cd ${OTHER_GIT%/*}/${OTHER_SUBDIR}	|| exit 1	# same ;)
  git status || exit 1
  echo
  
  # ****************************************************************
  # Start all over again...
  rm -rf $OTHER_TMP $DEST_COPY_DIR  # obsolete as long as we use $$
  
  
  # ****************************************************************
  echo "${blue}Clone ${OTHER_GIT} to temporary location ${OTHER_TMP}${treset}"
  # git clones into a subdir named after the paren directory of the
  # cloned repository:
  OTHER_SUBDIR="${OTHER_GIT##*/}"

  mkdir ${OTHER_TMP} || exit 1
  cd ${OTHER_TMP}    || exit 1

  git clone ${OTHER_GIT} || exit 1
  #pwd ; ls -Al
  
  cd ${OTHER_TMP}/${OTHER_SUBDIR} || exit 1
  # pwd ; ls -Al
  #
  
  
  # ****************************************************************
  echo "${yellow}Remove remote origin in ${OTHER_TMP}/${OTHER_SUBDIR}${treset}"
  git remote rm origin || exit 1

  # ****************************************************************
  echo ${cyan}
  echo "Include this script in commit message.${treset}"

  git commit --allow-empty -m "Prepare merge of ${OTHER_GIT}
  
  Preparing repository merge: Do not use.
  
  git commit || exit 1

  [edit commit message], then
  git commit --allow-empty --amend

  ****************************************************************
  ${0##*/} ${commandline}

>>>> >>> >> > INSERT script ${0} HERE < << <<< <<<<

  ****************************************************************
  " || exit 1

  
  # ****************************************************************
  echo ${cyan}
  echo "  Please edit commit message
  and *do* include this script BY HAND IN COMMIT MESSAGE.
  file:  ${0}
    <ENTER>${treset}"
  read dummy

  git commit --allow-empty --amend || exit 1


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
    ${blue}<ENTER>
  "
  read dummy

  PS1='\$' bash -l	################


  # ****************************************************************
  echo "${cyan}cp -ap $DEST_PAREN $DEST_COPY_DIR${treset}"
  mkdir $DEST_COPY_DIR			|| exit 1
  cd ${DEST_PAREN}			|| exit 1
  cp -ap $DEST_SUBDIR $DEST_COPY_DIR	|| exit 1

  echo "${cyan}Merge ${OTHER_GIT} repo into ${DEST_COPY_DIR}${treset}"
  cd ${DEST_COPY_DIR}/${DEST_SUBDIR}	|| exit 1
  #echo ; pwd ; ls -al ; git status


  # ****************************************************************
  echo "${yellow}  Pull from ${OTHER_TMP}${treset}"

  otherGit="unknownRepo"
  [ -n $OTHER_SUBDIR ] && otherGit=$OTHER_SUBDIR

  git remote add -f ${otherGit} ${OTHER_TMP}/${OTHER_SUBDIR} || exit 1
  git merge --log=10000 ${otherGit}/master	|| exit 1

  git commit -m "Merged ${OTHER_GIT}
  
  Transition: Do not use.
  
  git remote add -f ${otherGit} ${OTHER_TMP}
  git merge ${otherGit}/master
  git commit || exit 1

  git pull -s subtree ${otherGit} master
  git remote rm ${otherGit}
  " || exit 1
  
  echo ${blue}
  echo "git pull -s subtree ${otherGit} master${treset}"
  git pull -s subtree ${otherGit} master	|| exit 1

  echo ${blue}
  echo "git remote rm ${otherGit}${treset}"
  git remote rm ${otherGit}			|| exit 1
  #pwd ; ls -Al
  
  
  # ****************************************************************
  echo ${cyan}
  echo "New git repo is in ${DEST_COPY_DIR}"
  echo "Check and copy to ${DEST_GIT} by hand.${treset}"
  sleep 3

  git branch
  sleep 2

  ls -Al
  sleep 3

  git status
  echo " ${cyan}done :) ${treset}"
  # ****************************************************************
