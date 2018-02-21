// displaying and editing unsigned int arrays

unsigned int arr_write_index=0;
unsigned int arr_range_bottom=0;
unsigned int arr_range_top=0;

/*
  struct fraction arr_len(unsigned int *arr, unsigned int step);

  for arrays with multiplier/divisor items starting each data set like scales or jiffletabs
  sums up all multiplier/divisor fractions
  until it sees a divisor==0
  returns struct fraction sum
  0/0 signals an error

  in jiffletabs it will return the *length of the jiffle* relative to the base period
  in scales it gives the length of a melody containing *each note once*

 */
struct fraction arr_len(unsigned int *arr, unsigned int step) {
  struct fraction f = {0,0};	// keeps return value
  if (step < 2)	// the fraction takes the first two integers from each data set
    return f;

  unsigned int multiplier=0;
  unsigned int divisor=0;
  unsigned int count=1;
  struct fraction scratch;

  f.multiplier = 0;
  f.divisor=1;

  for (int i=0;; i += step) {
    multiplier = arr[i];
    divisor = arr[i+1];
    if (divisor==0)
      break;		// stop at divisor==0

    if (step > 2)
      count = arr[i+2];

    MENU.out(multiplier); MENU.tab();     MENU.out(divisor); MENU.tab();    MENU.outln(count);
    scratch.multiplier = multiplier * count;
    scratch.divisor = divisor;
    HARMONICAL.add_fraction(&scratch, &f);
  }

  return f;
}


void display_fraction(struct fraction *f) {
  MENU.out((*f).multiplier);
  MENU.slash();
  MENU.out((*f).divisor);
}


#define ARRAY_ENTRY_UNTIL_ZERO_MODE	1	// menu_mode for unsigned integer data entry, stop at multiple zeros
void display_arr(unsigned int *arr, unsigned int step) {
  struct fraction sum;
  sum.multiplier = 0;
  sum.divisor = 1;
  bool was_zero=false;

  bool edit_mode = (MENU.menu_mode==ARRAY_ENTRY_UNTIL_ZERO_MODE);
//  if (selected_jiffle==ILLEGAL) {
//    MENU.outln(F("(nothing selected)"));
//    return;
//  }

  // first line:
//#ifndef RAM_IS_SCARE
//  MENU.out(jiffle_names[selected_jiffle]); MENU.tab(); MENU.out(F("ID: ")); MENU.out(selected_jiffle); MENU.tab();
//#endif
  MENU.out(F("editing "));
  if (edit_mode) {
    MENU.out(F("on\tclose with '}'"));
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.out(F("\tmove cursor < >\trange borders | \""));
  } else
    MENU.out(F("off\tstart edit with '{'"));

  MENU.ln();

  // second line {data,data, ...}:
  MENU.out("{");
  for (int i=0; ; i++) {
    if ((i % step) == 0)
      MENU.space();
    if (edit_mode) {		// show left markers in edit_mode
      if (i==arr_range_bottom)
	if (arr_range_top)	// no range, no sign
	  MENU.out('|');
      if (i==arr_write_index)
	MENU.out("<");
    }
    MENU.out(arr[i]);
    if (edit_mode) {		// show right markers in edit_mode
      if (i==arr_write_index)
	MENU.out(">");
      if (i==arr_range_top)
	if (arr_range_top)	// no range, no sign
	  MENU.out('"');
    }
    MENU.out(",");
    if (arr[i] == 0) {
      if (was_zero)	// second zero done, stop it
	break;

      was_zero=true;	// first zero, continue *including* both zeroes
    } else
      was_zero=false;
  }

  MENU.out(F(" }  cursor "));
  MENU.out(arr_write_index);

  sum = arr_len(arr, step);
  MENU.tab();
  MENU.out(F("length ")); display_fraction(&sum);
  MENU.ln();
}
