#include "sizet.h"
#include "boot.h"
#include "graphics.h"

PSF1_Font* getDefaultFont();
void setActiveFont(PSF1_Font *av);
PSF1_Font *getActiveFont();
void drawCharacter(PSF1_Font *font,char character,unsigned int colour,unsigned long xOff,unsigned long yOff);
void drawCharacterAtBuffer(GraphicsInfo *gi, PSF1_Font *font,char character,unsigned int colour,unsigned long xOff,unsigned long yOff);
