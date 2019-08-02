#ifndef PULSES_H

#error DO WE NEED pulses.h at all?

void do_jiffle(int pulse);
void pulse_info_1line(int pulse);

#ifndef STRINGIFY2
  // see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
  #define STRINGIFY2(X) #X
  #define STRINGIFY(X) STRINGIFY2(X)
#endif

#define PULSES_H
#endif
