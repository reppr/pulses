/* **************************************************************** */
/*
  array_descriptors.h
  data arrays for scales, jiffles, etc.

  used in menu interface to have a name, type, selected index etc for each array
*/

#define ARRAY_DESCRIPTORS	100	// FIXME: ################

char* noname="no name";
char* notype="no type";

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
void init_arr_DB(arr_descriptor * DB, unsigned int len, char* name) {
  // DB[0] describes the database itself
  DB[0].pointer=NULL;	// DB[0].pointer points to selected array
  DB[0].len=len;	// data base length in bytes
  DB[0].item=1;		// DB[0].item is used as registration counter
  DB[0].name=name;
  DB[0].type="DB";
}


int DB_items(arr_descriptor * DB) {
  return DB[0].item - 1;	// returns number of enties, (-1 if not initialised)
}
char* array2name(arr_descriptor * DB, unsigned int* array) {
  arr_descriptor * arr= DB;

  // do *not* start at 0,  if the array is selected. you would get DB name ;)
  for(int i=1; i < DB[0].item; i++)
    if (DB[i].pointer == array)
      return DB[i].name;

  return "";
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

  return ILLEGAL;	// TIP: use to check if array is in db ;)
}


void select_array_in(arr_descriptor* DB, unsigned int* array) {
  if (pointer2index(DB, array) != ILLEGAL)	// TODO: remove debugging code
    DB[0].pointer=array;	// DB[0].pointer points to selected array
  else						// TODO: remove debugging code
    MENU.error_ln(F("(invalid)"));		// TODO: remove debugging code
}

unsigned int* selected_in(arr_descriptor* DB) {
  return DB[0].pointer;
}

bool register_array_in_DB(arr_descriptor* DB,	\
	unsigned int* array, unsigned int len, unsigned int item, char* name, char* type) {
  // DB[0] describes the database itself
  unsigned int next=DB[0].item;
  if((next * sizeof(arr_descriptor)) < DB[0].len) {
    DB[next].pointer=array;
    DB[next].len=len;
    DB[next].item=item;
    DB[next].name=name;
    DB[next].type=type;

    DB[0].item++;	// count registrations in DB[0].item
    return true;
  }

  return false;
}

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
    if (! register_array_in_DB(DB, DB_include[i].pointer, DB_include[i].len, DB_include[i].item, DB_include[i].name, DB_include[i].type))
      return false;
  }

  MENU.outln(DB_include[0].item - first - 1);
  return true;
}

#define SCALE_DESCRIPTORS	64	// FIXME: ################
arr_descriptor SCALES[SCALE_DESCRIPTORS];

#define JIFFLE_DESCRIPTORS	96	// FIXME: ################
arr_descriptor JIFFLES[JIFFLE_DESCRIPTORS];

#define iCODE_DESCRIPTORS	(64+JIFFLE_DESCRIPTORS)	// FIXME: ################
arr_descriptor iCODEs[iCODE_DESCRIPTORS];


// register_scale(european_PENTAtonic, sizeof(european_PENTAtonic), "european_PENTAtonic");
bool register_scale(unsigned int* scale, unsigned int len, char* name) {
  return register_array_in_DB(SCALES, scale, len, 2, name, "scale");
}

#ifndef STRINGIFY2
  // see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
  #define STRINGIFY2(X) #X
  #define STRINGIFY(X) STRINGIFY2(X)
#endif

#define REGISTER_SCALE(X)	register_scale((X), sizeof((X)), STRINGIFY(X))


// i.e.  register_jiffle(pentatonic_rise, sizeof(pentatonic_rise), "pentatonic_rise");
bool register_jiffle(unsigned int* jiffle, unsigned int len, char* name) {
  return register_array_in_DB(JIFFLES, jiffle, len, 3, name, "jiffle");
}

#ifdef MENU_h
  void display_arr_names(arr_descriptor* DB) {
    unsigned int * selected_array = DB[0].pointer;

    MENU.ln();
    for (int i = 0; i < DB[0].item; i++) {
      if(i) {					// *not for database DB[0]
	if (DB[i].pointer == selected_array)
	  MENU.out('*');
	else
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

      MENU.ln();
    }
  }


// menu helper function: UI to select array from a DB
bool UI_select_from_DB(arr_descriptor* DB) {
  int input_value;
  unsigned int* p = NULL;

  if (MENU.cb_peek() == EOF) {	// no further input:
    display_arr_names(DB);		//   display list
    return false;			//   return
  }

  input_value=MENU.numeric_input(-1);
  if (input_value != -1) {
    p = index2pointer(DB, input_value + 1);
    if (p == NULL) {
      MENU.outln_invalid();
      return false;
    } else {
      MENU.outln(array2name(DB, p));
      DB[0].pointer = p;	// selected in DB
      return true;
    }
  }

  return false;
}

#endif // MENU_h

#define REGISTER_JIFFLE(X)	register_jiffle((X), sizeof((X)), STRINGIFY(X))


bool register_iCODE(int* icode, unsigned int len, char* name) {
  return register_array_in_DB(iCODEs, (unsigned int *) icode, len, 1, name, "icode");
}

#define REGISTER_iCODE(X)	register_iCODE((X), sizeof((X)), STRINGIFY(X))
