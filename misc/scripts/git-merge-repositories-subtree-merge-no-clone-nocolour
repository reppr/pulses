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
  echo
  echo "Testing main git repo ${DEST_GIT}"
  cd ${DEST_GIT}		  || exit 1
  cd ${DEST_PAREN}/${DEST_SUBDIR} || exit 1	# should be same ;)
  git status || exit 1
  echo

  echo "Testing other git repo ${OTHER_GIT}"
  cd $OTHER_GIT			  	|| exit 1
  cd ${OTHER_GIT%/*}/${OTHER_SUBDIR}	|| exit 1	# same ;)
  git status || exit 1
  echo
  
  # ****************************************************************
  # Start all over again...
  rm -rf $OTHER_TMP $DEST_COPY_DIR  # obsolete as long as we use $$
  
  
  # ****************************************************************
  echo "Clone ${OTHER_GIT} to temporary location ${OTHER_TMP}"
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
  echo "Remove remote origin in ${OTHER_TMP}/${OTHER_SUBDIR}"
  git remote rm origin || exit 1

  # ****************************************************************
  echo "Include this script in commit message."

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
  echo "  Please edit commit message
  and *do* include this script BY HAND IN COMMIT MESSAGE.
  file:  ${0}
    <ENTER>"
  read dummy

  git commit --allow-empty --amend || exit 1


  # ****************************************************************
  echo "Please check and work now in the cloned ${OTHER_TMP} repository.

  Check everything,
  make commits if needed,
  move files, i.e. README

  OPENING A BASH SHELL NOW
  exit shell when done
    <ENTER>
  "
  read dummy

  PS1="GIT-WORK" bash	################


  # ****************************************************************
  echo "cp -ap $DEST_PAREN $DEST_COPY_DIR"
  mkdir $DEST_COPY_DIR			|| exit 1
  cd ${DEST_PAREN}			|| exit 1
  cp -ap $DEST_SUBDIR $DEST_COPY_DIR	|| exit 1

  echo "Merge ${OTHER_GIT} repo into ${DEST_COPY_DIR}"
  cd ${DEST_COPY_DIR}/${DEST_SUBDIR}	|| exit 1
  #echo ; pwd ; ls -al ; git status


  # ****************************************************************
  echo "  Pull from ${OTHER_TMP}"

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
  
  git pull -s subtree ${otherGit} master	|| exit 1
  git remote rm ${otherGit}			|| exit 1
  #pwd ; ls -Al
  
  
  # ****************************************************************
  echo "New git repo is in ${DEST_COPY_DIR}"
  echo "Check and copy to ${DEST_GIT} by hand."
  sleep 3

  git branch
  sleep 2

  ls -Al
  sleep 3

  git status
  echo "done :)"
  # ****************************************************************
