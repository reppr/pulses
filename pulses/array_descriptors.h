/* **************************************************************** */
/*
  array_descriptors.h
  data arrays for scales, jiffles, etc.

  used in menu interface to have a name, type, selected index etc for each array
*/

#define ARRAY_DESCRIPTORS	100	// FIXME: ################

char* noname=(char*) "no name";
char* notype=(char*) "no type";

// void display_jiffletab(unsigned int *);	// see: pulses.ino

/* **************************************************************** */
// store infos about data arrays, like name, type, item size
// (used by the user interface)
struct arr_descriptor {
  unsigned int* pointer=NULL;
  unsigned int len=0;		// bytes
  unsigned int item=0;		// int array indices that form a data item, like 3 for 'multiplier, divisor, count,'
  char* name = noname;
  char* type = notype;	// like scale, jiffle
};

// init an array as array database
// set DB[0] to describe the database
void init_DB(arr_descriptor * DB, unsigned int len, const char* name) {
  // DB[0] describes the database itself
  DB[0].pointer=NULL;	// DB[0].pointer points to selected array
  DB[0].len=len;	// data base length in bytes
  DB[0].item=1;		// DB[0].item is used as registration counter
  DB[0].name=(char*) name;
  DB[0].type=(char*) "DB";
}


int DB_items(arr_descriptor * DB) {
  return DB[0].item - 1;	// returns number of entries, (-1 if not initialised)
}


char* array2name(arr_descriptor * DB, unsigned int* array) {
  // do *not* start at 0,  if the array is selected. you would get DB name ;)
  for(int i=1; i < DB[0].item; i++)
    if (DB[i].pointer == array)
      return DB[i].name;

  return (char*) "";
}

unsigned int* index2pointer(arr_descriptor * DB, unsigned int index) {
  if (index > 0  &&  index < DB[0].item )
    return DB[index].pointer;

  return NULL;	// TIP: use to check if DB has the indexed item
}

int pointer2index(arr_descriptor * DB, unsigned int* array) {
  for (int i=1; i < DB[0].item ; i++)
    if (DB[i].pointer == array)
      return i;

  return ILLEGAL32;	// TIP: use to check if array is in db ;)
}


void select_in(arr_descriptor* DB, unsigned int* array) {
  if (pointer2index(DB, array) != ILLEGAL32)	// TODO: remove debugging code
    DB[0].pointer=array;	// DB[0].pointer points to selected array
  else						// TODO: remove debugging code
    ERROR_ln(F("select_in()"));		// TODO: remove debugging code
}

unsigned int* selected_in(arr_descriptor* DB) {
  return DB[0].pointer;
}

unsigned int selected_length_in(arr_descriptor* DB) {
  return DB[pointer2index(DB, DB[0].pointer)].len;
}

bool register_in_DB(arr_descriptor* DB,	\
	unsigned int* array, unsigned int len, unsigned int item, const char* name, const char* type) {
  // DB[0] describes the database itself
  unsigned int next=DB[0].item;
  if((next * sizeof(arr_descriptor)) < DB[0].len) {
    DB[next].pointer=array;
    DB[next].len=len;
    DB[next].item=item;
    DB[next].name= (char*) name;
    DB[next].type= (char*) type;

    DB[0].item++;	// count registrations in DB[0].item
    return true;
  }

  ERROR_ln(F("could not register array"));
  return false;
} // register_in_DB()

char* selected_name(arr_descriptor* DB) {
  return array2name(DB, selected_in(DB));
}

void DB_info(arr_descriptor* DB) {	// TODO: use
  MENU.out(DB[0].name);
  MENU.tab();
  MENU.out(DB_items(DB));
  MENU.tab();
  MENU.out('*');
  MENU.outln(selected_name(DB));
}

bool include_DB_in_DB(arr_descriptor* DB, arr_descriptor* DB_include, int first) {
  MENU.out("include ");
  MENU.out(DB_include[0].name);
  MENU.out(" in ");
  MENU.out(DB[0].name);
  MENU.space();

  for(int i=first+1; i <= DB_items(DB_include); i++) {
    /*
    MENU.out(i);
    MENU.tab();
    MENU.out(DB_include[i].name);
    MENU.tab();
    MENU.out(DB_include[i].type);
    MENU.ln();
    */
    if (! register_in_DB(DB, DB_include[i].pointer, DB_include[i].len, DB_include[i].item, DB_include[i].name, DB_include[i].type))
      return false;
  }

  MENU.outln(DB_include[0].item - first - 1);
  return true;
}

#define SCALE_DESCRIPTORS	64	// FIXME: ################
arr_descriptor SCALES[SCALE_DESCRIPTORS];

#define JIFFLE_DESCRIPTORS	128	// FIXME: ################
arr_descriptor JIFFLES[JIFFLE_DESCRIPTORS];

#define iCODE_DESCRIPTORS	(64+JIFFLE_DESCRIPTORS)	// FIXME: ################
arr_descriptor iCODEs[iCODE_DESCRIPTORS];


// register_scale(europ_PENTAtonic, sizeof(europ_PENTAtonic), "europ_PENTAtonic");
bool register_scale(unsigned int* scale, unsigned int len, const char* name) {
  return register_in_DB(SCALES, scale, len, 2, name, "scale");
}

#ifndef STRINGIFY2
  // see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
  #define STRINGIFY2(X) #X
  #define STRINGIFY(X) STRINGIFY2(X)
//#define STRINGIFY(X) (char*) STRINGIFY2(X)	// this version avoids the C++ warnings about string constants and char*
#endif

#define REGISTER_SCALE(X)	register_scale((X), sizeof((X)), STRINGIFY(X))


// i.e.  register_jiffle(pentatonic_rise, sizeof(pentatonic_rise), "pentatonic_rise");
bool register_jiffle(unsigned int* jiffle, unsigned int len, const char* name) {
  return register_in_DB(JIFFLES, jiffle, len, 3, name, "jiffle");
}

#ifdef MENU_h
  void display_names(arr_descriptor* DB) {
    unsigned int * selected_array = DB[0].pointer;
    int selected_n=-1;
    int name_len;
    MENU.ln();
    for (int i = 0; i < DB[0].item; i++) {
      if(i) {					// *not for database DB[0]
	if (DB[i].pointer == selected_array) {
	  selected_n = i-1;
	  MENU.out('*');
	} else
	  MENU.space();

	MENU.space();
	MENU.out(i-1);
	MENU.tab();
      }

      MENU.out(DB[i].type);
      MENU.tab();
      MENU.out(DB[i].name);

      if (i==0) {
	MENU.tab();
	MENU.out(DB[0].item - 1);	// number of items in data base
      }

      if(MENU.verbosity > VERBOSITY_LOWEST) {	// maybe show size
	MENU.tab();
	name_len = strlen(DB[i].name);
	if(name_len < 8)
	  MENU.tab();
	if(name_len < 16)
	  MENU.tab();
	MENU.out(F("bytes "));
	MENU.out(DB[i].len);
      }

      MENU.ln();
    }

    extern uint8_t extended_output(char* data, uint8_t col=0, uint8_t row=0, bool force=false);
    extended_output(selected_name(DB));
    MENU.out(F(" ["));
    MENU.out(selected_n);
    MENU.outln(']');
  } // void display_names(arr_descriptor* DB)


// menu helper function: UI to select array from a DB
bool UI_select_from_DB(arr_descriptor* DB) {
  bool return_flag=false;
  unsigned int* p = NULL;
  int input_value;

  if(MENU.peek() == EOF8) {	// no further input:
    display_names(DB);	//    display list
    return false;		//    RETURN
  }

  if(MENU.is_numeric()) {
    input_value=MENU.numeric_input(-1);
    if (input_value > -1) {
      p = index2pointer(DB, input_value + 1);
      if (p == NULL) {
	MENU.outln_invalid();
      } else {
	MENU.outln(array2name(DB, p));
	DB[0].pointer = p;	// selected in DB
	return_flag = true;
      }
    } else {			// negative input_value
      MENU.outln_invalid();
      return false;
    }
  } else {			// *not* numeric input
    while(MENU.peek()=='+' || MENU.peek()=='-' || MENU.peek()=='E' || MENU.peek()=='T') {
      switch(MENU.peek()) {	// '+' | '-'  and 'E' 'T' (for morse)
      case '+': // 'J+' == 'JE'	(for morse) one entry up
      case 'E': // 'J+' == 'JE'	(for morse) one entry up
	MENU.drop_input_token();
	{
	  int i = pointer2index(DB, selected_in(DB));
	  if(i<DB_items(DB)) {
	    select_in(DB,index2pointer(DB, ++i));
	    return_flag = true;
	  }
	}
	break;
      case '-': // 'J-' == 'JT'	(for morse) one entry down
      case 'T': // 'J-' == 'JT'	(for morse) one entry down
	MENU.drop_input_token();
	{
	  int i = pointer2index(DB, selected_in(DB));
	  if(i) {
	    select_in(DB,index2pointer(DB, --i));
	    return_flag = true;
	  }
	}
	break;
      } // '+' '-'  'E' 'T' (for morse)  loop
    } // treat '+' and '-'
  }

  extern uint8_t extended_output(char* data, uint8_t col=0, uint8_t row=0, bool force=false);
  extended_output(selected_name(DB));
  MENU.ln();

  return return_flag;
} // UI_select_from_DB()

#endif // MENU_h

#define REGISTER_JIFFLE(X)	register_jiffle((X), sizeof((X)), STRINGIFY(X))


bool register_iCODE(void* icode, unsigned int len, const char* name) {
  return register_in_DB(iCODEs, (unsigned int *) icode, len, 1, name, "icode");
}

#define REGISTER_iCODE(X)	register_iCODE((X), sizeof((X)), STRINGIFY(X))
