#! /bin/bash
# '  ' indentation is for inclusion in git commit messages,
# as lines starting with '#' (like this line) get removed...

  #! /bin/bash
  # ****************************************************************
  # Script used inside a *CLONE* of arduino-git1
  # to prepare merge into pulses:

  git status	|| exit 1		# assure we're in a git repository ;)

  mkdir -p misc/src/arduino-git1		|| exit 1

  git mv README.txt misc/src/arduino-git1/	|| exit 1
  git mv sketchbook/* misc/src/arduino-git1/	|| exit 1

  rmdir sketchbook/				|| exit 1

  # check github for updates:
  git remote add -f 'GHsoftb' 'https://github.com/reppr/softboard' || exit 1
  #
  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm GHsoftb				|| exit 1


  # check local ARDUINO-GIT2/softboard repo for updates:
  git remote add -f 'SOFTB' '~/ARDUINO-GIT2/softboard/' || exit 1
  #
  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm SOFTB				|| exit 1

  git commit					|| exit 1

  # check local ARDUINO-GIT2/accelerometer_3D repo for updates:
  git remote add -f 'A3D' '~/ARDUINO-GIT2/accelerometer_3D/' || exit 1
  #
###  # The merge gives 'Already up-to-date.'
  git merge master				|| exit 1
  #
  git remote rm A3B				|| exit 1

  git commit					|| exit 1

  # ****************************************************************
# ****************************************************************

