#! /bin/bash
# '  ' indentation is for inclusion in git commit messages,
# as lines starting with '#' (like this line) get removed...

  #! /bin/bash
  # ****************************************************************
  # Script used inside a *CLONE* of arduino-git1
  # to prepare merge into pulses:

  # SIMPLIFIED VERSION after preparing 'arduino-git1'

  git status	|| exit 1		# assure we're in a git repository ;)

  echo
  echo "git rm README_arduino-git1"
  git rm README_arduino-git1				|| exit 1
  git commit -m"Avoid double README_arduino-git1."	|| exit 1


  # check github for updates:
  echo
  echo "GHsoftb"
  git remote add -f 'GHsoftb' 'https://github.com/reppr/softboard' || exit 1
  #
  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm GHsoftb				|| exit 1
  git commit --allow-empty \
      -m"Checked with https://github.com/reppr/softboard" || exit 1


  # check local ARDUINO-GIT2/softboard repo for updates:
  echo
  echo "SOFTB"
  git remote add -f 'SOFTB' '~/ARDUINO-GIT2/softboard/' || exit 1
  #
  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm SOFTB				|| exit 1
  #
  git commit --allow-empty \
      -m"Checked with ARDUINO-GIT2/softboard"	|| exit 1


  # check local ARDUINO-GIT2/discharger repo for updates:
  echo
  echo 'discharger'
  git remote add -f 'discharger' '~/ARDUINO-GIT2/discharger/' || exit 1
  #
  #  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm discharger			|| exit 1
  #
  git commit --allow-empty \
      -m"Checked with ARDUINO-GIT2/discharger"	|| exit 1


  # check local ARDUINO-GIT2/periodics repo for updates:
  echo
  echo 'periodics'
  git remote add -f 'periodics' '~/ARDUINO-GIT2/periodics' || exit 1
  #
  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm periodics			|| exit 1
  #
  git commit --allow-empty \
      -m"Checked with ~/ARDUINO-GIT2/periodics"	|| exit 1


  # check local ARDUINO-GIT2/polyphonic_oscillators repo for updates:
  echo
  echo 'polyphonic_oscillators *master'
  git remote add -f 'polyphonicOsc' '~/ARDUINO-GIT2/polyphonic_oscillators' || exit 1
  #
  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm polyphonicOsc			|| exit 1
  #
  git commit --allow-empty \
      -m"Checked with ~/ARDUINO-GIT2/polyphonic_oscillators"	|| exit 1

  echo
  echo "Final commit on this clone, to tell what happened"
  git commit --allow-empty \
      -m"Prepared and updated to ARDUINO-GIT2 level."	|| exit 1

  echo
  echo "Insert ${0} in log message, please.	<RETURN>"
  read dummy
  git commit --allow-empty --amend			|| exit 1

  echo
  echo "${0##*/}:	$(tput sgr0)==> done :)"
  . /etc/profile

  # ****************************************************************
# ****************************************************************
