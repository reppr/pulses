/* **************************************************************** */
/*
  array_descriptors.h
  data arrays for scales, jiffles, etc.

  used in menu interface to have a name, type, selected index etc for each array
*/

#define ARRAY_DESCRIPTORS	100	// FIXME: ################

char* noname="no name";
char* notype="no type";

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
  DB[0].type="database";
}

void select_array_in_DB(arr_descriptor* DB, unsigned int* array) {
  DB[0].pointer=array;	// DB[0].pointer points to selected array
}

bool register_array_in_DB(arr_descriptor* DB, \
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

char* array2name(arr_descriptor * DB, unsigned int* array) {
  arr_descriptor * arr= DB;
  for(int i=0; i < DB[0].item; i++)
    if (DB[i].pointer == array)
      return DB[i].name;

  return "";
}


#define SCALE_DESCRIPTORS	64	// FIXME: ################
arr_descriptor SCALES[SCALE_DESCRIPTORS];

#define JIFFLE_DESCRIPTORS	100	// FIXME: ################
arr_descriptor JIFFLES[JIFFLE_DESCRIPTORS];


// register_scale(european_pentatonic, sizeof(european_pentatonic), "european_pentatonic");
bool register_scale(unsigned int* scale, unsigned int len, char* name) {
  return register_array_in_DB(SCALES, scale, len, 2, name, "scale");
}

#ifndef STRINGIFY2
  // see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
  #define STRINGIFY2(X) #X
  #define STRINGIFY(X) STRINGIFY2(X)
#endif

#define REGISTER_SCALE(X)	register_scale((X), sizeof((X)), STRINGIFY(X))


// register_jiffle(european_pentatonic, sizeof(european_pentatonic), "european_pentatonic");
bool register_jiffle(unsigned int* jiffle, unsigned int len, char* name) {
  return register_array_in_DB(JIFFLES, jiffle, len, 3, name, "jiffle");
}

#ifdef MENU_h
  void display_arr_names(arr_descriptor* DB) {
    unsigned int * selected_array = DB[0].pointer;

    MENU.ln();
    for (int i = 0; i < DB[0].item; i++) {
      if (i && (DB[i].pointer == selected_array))	// ignore DB[0] array DB descriptor
        MENU.out('*');
      else
        MENU.space();

      MENU.space();
      MENU.out(i);
      MENU.tab();
      MENU.out(DB[i].type);
      MENU.tab();
      MENU.out(DB[i].name);
      if (i==0) {
	MENU.tab();
	MENU.out(DB[0].item);	// number of items in data base
      }
      MENU.ln();
    }
  }
#endif // MENU_h

#define REGISTER_JIFFLE(X)	register_jiffle((X), sizeof((X)), STRINGIFY(X))
