typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_Header;

typedef struct {
	PSF1_Header* psf1_Header;
	void* glyphBuffer;
} PSF1_Font;

PSF1_Font* getDefaultFont();
void setActiveFont(PSF1_Font *av);
PSF1_Font *getActiveFont();
void drawCharacter(PSF1_Font *font,char character,unsigned int colour,unsigned long xOff,unsigned long yOff);
