// LLVM-MOS Lynx text drawing functions
// JH 2025
extern void TextInit();
extern void TextDrawChar(unsigned char charnum, int x, int y);
extern void TextDrawString(char *text, int x, int y);
extern void TextDrawInt(unsigned int value, int x, int y);
extern void TextDrawHex(unsigned int value, int x, int y);

