// import the Gamebuino library
#include <Gamebuino-Meta.h>

// here we define our language variable "fox"
// it has two entries, one for english and one for german
const MultiLang fox[] = {
  {LANG_EN, "the fox"}, // english entry
  {LANG_DE, "der Fuchs"}, // german entry
};

// gamebuino setup
void setup() {
  gb.begin();
}

void loop() {
  while(!gb.update());
  
  // clear the previous screen
  gb.display.clear();
  
  // here we get the pointer ot our string, depending on the users language setting
  const char* lang_fox = gb.language.get(fox);
  // aaaaand print it to the screen
  gb.display.println(lang_fox);
  
  // we can also combind the two lines above, letting it become:
  gb.display.println(gb.language.get(fox));
  
  // or, if we want to print to the display, we can even do
  gb.display.println(fox);
}
