/* **************************************************************** */
/*
  array_descriptors.h
  data arrays for scales, jiffles, etc.
  
  used in menu interface to have a name, type, selected index etc for each array
*/
#define ARRAY_DESCRIPTORS	100	// FIXME: ################

char* noname="no name";
char* notype="no type";

// store infos about data arrays, like name, type, step
// (used by the user interface)
struct arr_descriptor {
  unsigned int* pointer=NULL;
  unsigned int len=0;		// bytes
  unsigned int step=0;		// int array indices that form a data item, like 'multiplier, divisor, count,'
  // unsigned int step=0;	// in DATABASE arrays this is also used as registration counter in DB[0]
  char* name = noname;
  char* type = notype;	// like scale, jiffle
};

// init an array as array database
// set DB[0] to describe the database
void init_arr_DB(arr_descriptor * DB, unsigned int len, char* name) {
  // DB[0] describes the database itself
  DB[0].pointer=(unsigned int *) DB;	// DB[0].pointer points to itself
  DB[0].len=len;
  //  DB[0].step=sizeof(arr_descriptor); we don't need that
  DB[0].step=1;	// DB[0].step is used as registration counter
  DB[0].name=name;
  DB[0].type="DB";
}


bool register_array(arr_descriptor* DB, unsigned int* array, unsigned int len, unsigned int step, char* name, char* type) {
  // DB[0] describes the database itself
  // misnamed DB[0].step counts registrations, *DB[0] only*
  unsigned int registered=DB[0].step;
  if((registered * sizeof(arr_descriptor)) < DB[0].len) {
    DB[registered].pointer=array;
    DB[registered].len=len;
    DB[registered].step=step;
    DB[registered].name=name;
    DB[registered].type=type;

    DB[0].step++;	// count registrations in DB[0].step
    return true;
  }

  return false;
}


#define SCALE_DESCRIPTORS	64	// FIXME: ################
arr_descriptor SCALES[SCALE_DESCRIPTORS];

#define JIFFLE_DESCRIPTORS	100	// FIXME: ################
arr_descriptor JIFFLES[JIFFLE_DESCRIPTORS];

/*
  ie:  register_scale(european_pentatonic, sizeof(european_pentatonic), "european_pentatonic");
*/
bool register_scale(unsigned int* scale, unsigned int len, char* name) {
  static unsigned int registered_scales=0;

  SCALES[registered_scales].pointer = scale;
  SCALES[registered_scales].len = len;		// bytes
  SCALES[registered_scales].step = 2;		// int array indices for 'multiplier, divisor' data items
  SCALES[registered_scales].name = name;
  SCALES[registered_scales].type = "scale";

  registered_scales++;	// FIXME: check ################
  return true;
}

/*
  see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
*/
#ifndef STRINGIFY2
  #define STRINGIFY2(X) #X
  #define STRINGIFY(X) STRINGIFY2(X)
#endif

#define REGISTER_SCALE(X)	register_scale((X), sizeof((X)), STRINGIFY(X))


bool register_jiffle(unsigned int* jiffle, unsigned int len, char* name) {
  static unsigned int registered_jiffles=0;

  JIFFLES[registered_jiffles].pointer = jiffle;
  JIFFLES[registered_jiffles].len = len;	// bytes
  JIFFLES[registered_jiffles].step = 3;		// int array indices for 'multiplier, divisor, count' data items
  JIFFLES[registered_jiffles].name = name;
  JIFFLES[registered_jiffles].type = "jiffle";

  registered_jiffles++;
  return true;
}

#define REGISTER_JIFFLE(X)	register_jiffle((X), sizeof((X)), STRINGIFY(X))
