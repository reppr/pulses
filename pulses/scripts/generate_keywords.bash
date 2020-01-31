#!/bin/bash

# some small adaptions of
# see: https://github.com/crankyoldgit/IRremoteESP8266/blob/master/tools/mkkeywords
# LICENCE please see:
# https://github.com/crankyoldgit/IRremoteESP8266/blob/master/LICENSE.txt

# Generate a keywords.txt for the library that is suitable for the Arduino IDE.
# Expects to run from the top directory of the library.

# Set the locale for sorting
export LC_ALL=C

cat << EndOfTextEndOfTextEndOfText
#######################################
# Datatypes & Classes (KEYWORD1)
#######################################

EndOfTextEndOfTextEndOfText

CLASSES=$(egrep -h "^ *((enum|class) |} [a-zA-Z0-9_]+_t;$)" *.h |
          sed 's/^ *//;s/enum class//;s/\;$//' | cut -d' ' -f2 | sort -u)
for i in ${CLASSES}; do
  echo -e "${i}\tKEYWORD1"
done | sort -du

cat << EndOfTextEndOfTextEndOfText

#######################################
# Methods and Functions (KEYWORD2)
#######################################

EndOfTextEndOfTextEndOfText
CTYPES="u?int(8|16|32|64)?(_t)?|void|bool|char|float|long|double|String|static"
OURTYPES="match_result_t|state_t|decode_type_t"
METHODS=$(egrep -h "^[ ]{0,2}(${CTYPES}|${OURTYPES})\*? [^ ]*\(" *.cpp |
          sed 's/^  //' | cut -d' ' -f2 | sed 's/^\([^:]*::\| *\* *\)//' |
          cut -d'(' -f1 | sort -u | grep -v RAM_ATTR)
for i in ${METHODS}; do
  echo -e "${i}\tKEYWORD2"
done | sort -u

cat << EndOfTextEndOfTextEndOfText

#######################################
# Constants (LITERAL1)
#######################################

EndOfTextEndOfTextEndOfText
LITERALS=$(grep "^#define [A-Z]" *.cpp *.h |
           while read ignore define ignore; do
             echo ${define};
           done | sort -u |
           grep -v [\(\)] | grep -v ^_ | grep -v _\$ | grep -v VIRTUAL)
CONSTS=$(grep "^const " *.cpp *.h |
         sed 's/\[.*\] =.*//;s/ =.*//;s/^.* k/k/')
ENUMS=$(cat *.h | while read a b; do
          if [[ ${a} == "};" ]]; then
            ENUM=0;
          fi;
          if [[ ${ENUM} -eq 1 ]]; then
            echo $a | sed 's/,//g';
          fi;
          if [[ ${a} == "enum" ]]; then
            ENUM=1;
          fi;
        done)
for i in ${LITERALS} ${CONSTS} ${ENUMS}; do
  echo -e "${i}\tLITERAL1"
done | sort -u
