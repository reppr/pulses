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
  unsigned int len=0;
  unsigned int step=0;
  char* name = noname;
  char* type = notype;	// like scale, jiffle
};

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
  SCALES[registered_scales].len = len;
  SCALES[registered_scales].step = 2;
  SCALES[registered_scales].name = name;
  SCALES[registered_scales].type = "scale";

  registered_scales++;	// FIXME: check ################
  return true;
}
/*
  see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
*/
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define REGISTER_SCALE(X)	register_scale((X), sizeof((X)), STRINGIFY(X))


bool register_jiffle(unsigned int* jiffle, unsigned int len, char* name) {
  static unsigned int registered_jiffles=0;

  JIFFLES[registered_jiffles].pointer = jiffle;
  JIFFLES[registered_jiffles].len = len;
  JIFFLES[registered_jiffles].step = 3;
  JIFFLES[registered_jiffles].name = name;
  JIFFLES[registered_jiffles].type = "jiffle";

  registered_jiffles++;
  return true;
}

#define REGISTER_JIFFLE(X)	register_jiffle((X), sizeof((X)), STRINGIFY(X))
