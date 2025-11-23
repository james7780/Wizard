' ==============
' WIZARD OF WASD
' ==============
' A submission for 2020 10-line BASIC competition
' By Randy Gill (randygill at yahoo dot com)
' Designed for (integer) FastBasic v4.0, for Atari 8-bit computers

' Dimension arrays to hold expanded game data, packed source data, inventory
DIM W(1520), D$(5), V(51)

' Turn off annoying key click sound (on XL series and later)
POKE 731,255

' Initialize address pointer variables
Y = (ADR(W) / 512 + 1) * 512  ' start address of custom glyphs
K = Y + 512   ' start address of entity catalog
M = K + 260   ' start address of map data (skipping 52 entities * 5 bytes)

' Copy character set into RAM for modification
MOVE $E000, Y, 512

' =================================
'    EXPAND COMPRESSED GAME DATA
' =================================

' Show indicator that data is being loaded
? "Load";

' Extract data for custom character glyphs, entity attributes, and game map to
' available memory. Game data is packed in strings with each character repre-
' senting 6 bits (a "sextet"). Data is decoded/expanded as a series of variable-
' length chunks, each prefixed by a two-sextet "header" defining encoding mode
' number (3 bits) and length (9 bits)

' Encoding mode numbers
' ---------------------
' 7 = Octet-Stream: Encodes 3 bytes in 4 printable chars; like Base64 but
'     easier to decode in BASIC (no translation table or 32-bit ints required)
' 4 = Sextet-Stream: Packs a 6-bit value into each char; for values < 64, e.g
'     table of entity relationships
' 3 = Triad-Stream: Packs two 3-bit values (< 8) into each char; useful for
'     base map (having only common entities, 0-7)
' 2 = Sextet-Run: encodes an sextet that repeats a specified # of times (6-bit
'     RLE)
' 1 = Octet-Run: encodes an octet that repeats a specified # of times (8-bit
'     RLE)

' Modes that aren't true encodings
' --------------------------------
' 5 = Header: indicates chunk header is being processed
' 0 = Gap: advances destination pointer by specified count; used to skip char
'    glyphs not being redefined

' The encoding mode numbers (variable E) were carefully chosen so that whether
' the encoding uses a holding sextet, the cycle of when holding sextets appear,
' and whether the encoding is a "run" (vs. a "stream") could be calculated from
' it.  Respectively: ABS(E-3)>1, ABS(E-3+(E=3)), and E<3

' Initialize variables, including D$(n) which holds compressed game data
' Note that the odd ordering is to better pack statements into 256-char lines
E = 5   ' encoding mode (initialized to "header" encoding, which prefaces a chunk)
Z = Y   ' data destination pointer (set to starting address of custom char glyphs)
D$(0)="08?hoooWo33;_G[@1@`@0842@1h\ZZZZjZZnThlj_ooj>Zn0In1N4P`P0P371>lHlX40oooWo333?33H`LHo1nlH5V@DhbkoOofVdRbj3hR
WJ_?P5lZ8088888IS=fH14RdN5hUU6Ioi"
I = 0   ' index of current element in source data string array $D()
D$(1)="=mm00HHl1JHTEVVgeg4dOoFl8@7@=00P0D>OP?70=Pl6d<io1n183o7P00H0@l0nQn0X5hX`0842T1nmNIWnDZDn00lHDlnJ1nlH@78FAjF
MOZJ9^90PGL;0035LF05:L00Lc62L00;<E0?L21<9d56_h097W0?7<03751?7M1?7`1<?P18?226_O2;7d2;7f2?M[53M05>_l46_b50_254_T5=_j
45_Z53_457_\57Mn3;M549"
P = 0   ' pointer to source data character (RAM addr within $D(I))
D$(2)="L46?Mm5;MB53M;57Ml63600;5;504006JN442C10N0043V58;<34PG62=@63OV51KS6;1d249`5;k=388>1770o0o01o20o39S\en7<Hm@m
CmMmNmOmDm?m>m8mBmAm9m=mFmEmHmGmJmImLmKm:m;m<mQmPmSmRn0n0n2n0n2n0n0n0[TT0U0V0WAXYY0ZV:F43999TB42A43;AB9BB42@43=9B9
9B]42>43>9A:=AZU2=43B9"
L = 0   ' limit (ending) RAM address of current source data element $D(I)
D$(3)="9Z]AZ]9Q2;43AAZ=9B]E942943B9Z]99Z]:Q2943@AZ=Q9]E:2;43?AZ=Q]EB12<43>AZ:T]BR2<43C9AB9TERT:42843C9B:9QDTAB4284
3CABY=9T<BB42843A9:]]99<>22:43B99]]=9<9Q2943C99]]=Q<9942843B9<YY]9T9Q2943>9T99A:Q2=43?9T<9BB942<4369Q<262740111042
;4369QA2627501150542;4"
R = 1   ' remaining bytes in current chunk or header
D$(4)="3B99BBB9]=Q2943699A26136]=Q2:43@999T99=Q:F47106:E671002807306062703Cfff``ff`f0276710026671002603Y6666006fff
`ff`````f`02663L00`0````0``ff62963;````f02663^``00`0``00`ff666f66f6f62863:00```2707306062763C`ff`f``f`028671002:03
;600606:F63@0`0`0`0`2;"
C = 0   ' count within decoding cycle (significant for header and octet encodings)
D$(5)="63`0`0`0`0`f0`ff60606060f612663E0`0`0`0`f802663F0`0`0`0`f0`:c63;0`fhh02>63a```f00`f99aff606fn0hfVLT62963B`f
07`fTSd2663E00f600f>9962G6710O:F600"

REPEAT
  IF P = L                ' source data element ran dry (or 1st time through)
    P = ADR(D$(I)) + 1    ' calculate source data pointer
    L = P + LEN(D$(I))    ' ending address of source data array element
    INC I                 ' next time it runs dry, it will use next element of D$
  ENDIF
  S = PEEK(P) - 48        ' set current sextet, subtracting the character offset
  IF C = 0 AND ABS(E - 3) > 1   ' first byte of a multi-byte encoding becomes the "holding" sextet
    H = S                 ' set the holding sextet
  ELIF E = 5              ' "header" encoding; two-character sequence that defines following chunk
    ? ".";                ' print periods to show it is busy working
    E = H & 7             ' get the next encoding id number from 3 low bits of holding sextet
    R = H & 56 * 8 + S    ' high 3 bits from the holding sextet plus 6 from current form (9-bit) chunk length
    H = 0                 ' clear holding sextet for next use
    C = -1                ' set cycle count so it will be 0 next iteration (after incrementing)
    IF E = 0              ' "gap" encoding
      Z = Z + R           ' advance the destination pointer Z
      E = 5               ' back to header encoding for next chunk
    ENDIF
  ELSE
    IF E < 3              ' sextet-run or octet-run encoding
      MSET Z, R, H * 64 + S     ' fill R bytes starting at Z. Note that for sextet, H will be 0
      Z = Z + R           ' skip over range it just filled
      R = 1               ' set to 1 because < 1 would signal prog to stop decoding altogether
    ELSE
      IF E = 3            ' triad-stream encoding
        POKE Z, S & 7     ' put low-order triad at destination address
        IF R > 1          ' if there are remaining chars in the chunk
          INC Z
          POKE Z, S & 56 / 8    ' put high-order triad at (next) destination address
          DEC R           ' decrement for high-order triad (decrement for low-order happens 8 lines down)
        ENDIF
      ELSE                ' sextet-stream or octet-stream encoding
        POKE Z, H & 3 * 64 + S  ' for octet, bottom 2 bits of holding sextet become top bits of destination byte
        H = H / 4         ' shift bits of the holding sextet, in preparation to decode next byte
      ENDIF
      INC Z               ' increment to next destination address
    ENDIF
    DEC R                 ' decrement remaining chars (considering it just processed one)
    IF R < 1              ' done with this chunk, get ready for next
      E = 5               ' set encoding to header encoding
      C = -1              ' so cycle count will become 0 once incremented
      R = 1               ' set to 1 because < 1 would signal prog to stop decoding altogether
    ENDIF
  ENDIF
  C = (C + 1) MOD ABS(E - 3 + (E = 3))   ' increment cycle count
  INC P                   ' point to next character in data source string D$(I)
UNTIL R < 1               ' chunk with R = 0 tells decoder it is done

' ===========================
'    MAP DISPLAY PROCEDURE
' ===========================
' Refreshes the display window of the world map
' Parameters:
'   K - Starting address of entity catalog in RAM
'   P - Current player position (cell #) on map
'   Z - Starting address of screen (display) RAM

PROC M
  FOR R = 0 TO 6              ' map display is 7 rows tall
    O = P - 84 + H * R        ' Precompute map source
    B = Z + 81 + 20 * R       ' and screen destination
    FOR C = 0 TO 6            ' map display is 7 columns wide
      IF C - 3 OR R - 3       ' not the spot where the player glyph always appears, so get map glyph instead
        POKE B + C, PEEK(K + PEEK(O + C) * 3)   ' look up entity in map cell, then glyph for entity
      ENDIF
    NEXT
  NEXT
ENDPROC

' =====================
'    GAME REPLAY LOOP
' =====================
' Now that one-time setup is complete, perform initialization for each game

' Main concepts of game code
' --------------------------
' "Map" contains entire world (surface and underground); conceptually 27 x 65,
' but implemented as one-dimensional array of 1755 cells
' Each map cell contains id of an "entity"
' Entity is anything that can appear on the map (except player) and/or in
' inventory
' Entities are all one of five types, some common (can appear in multiple map
' cells), some unique (can only be in one map cell):
'   - Passible (common): open areas, grass, forest, etc.
'   - Impassible (common): walls, mountains, water
'   - Item (unique): taken into inventory when stepped on
'   - Obstacle (unique): needs appropriate item to defeat
'   - Link (unique): warps to another location on map
' "Entity catalog" has 4 attributes of each of the 52 entities:
'   - "Requirement" entity id: for obstacles, this is entity id of item that
'     defeats it; impassibles have their own entity id (which can never be
'     in inventory); other types identified by pseudo-entities 61 (link),
'     62 (item), or 63 (passable)
'   - "Transition" entity id: entity that replaces this one when obstacle is
'     defeated or item is taken; for links this is entity id it links to
'   - Appearance (ATASCII char code): in graphics 18 implies glyph and color
'   - Position (cell# on map): where initially placed, for unique entities only

H = 27   ' Halt indicator (Escape char)
REPEAT
  ' Loop through entity ids, reset the inventory of them to empty, and copy the
  ' entity ids to their default location on the map; start at 8 because 0-7 are
  ' the common entities that are not inventoried, and are already on the map
  FOR I = 8 TO 51
    V(I) = 0
    POKE M + DPEEK(K + 3 * I + 1), I
  NEXT

  ' Configure graphics
  GRAPHICS 18         ' Text mode having 20 cols x 12 rows, with 4 colors + black background
  POKE 756, Y / 256   ' Tells Atari OS to use program's custom character set
  Z = DPEEK(88)       ' Start address of screen RAM
  SETCOLOR 3,0,15     ' Make color 3 white, defaults ok for others (blue, green, burnt orange)

  ' Display game title and draw frame around map view
  POSITION 7,0
  ? #6,"$D7$C9$DA$C1$D2$C4"  ' "WIZARD"
  POSITION 8,1
  ? #6,"$CF$C6 $F7"          ' "OF W"
  POSITION 10,2
  ? #6,"$E1$F3$E4"           ' "  ASD"
  COLOR 142           ' Player glyph, in white color
  PLOT 4,7
  COLOR 63            ' Skull glyph, in orange color
  PLOT 0,3
  DRAWTO 8,3
  DRAWTO 8,11
  DRAWTO 0,11
  DRAWTO 0,3

  P = M + 464         ' Player position on map
  T = 999             ' Time (moves) remaining

' ========================
'    MAIN GAMEPLAY LOOP
' ========================

  REPEAT
    EXEC M            ' Display map
    POSITION 10,4
    ? #6,"$D4";T;" "  ' Display time remaining
    GET O             ' Get movement key

    ' Calculate new "trial" position N, using algebraic expression instead of
    ' chained IFs for brevity.  In (AT)ASCII: W=87 A=65 S=83 D=68
    N = P + (O = 68) - ( O = 65) + H * ((O = 83) - (O = 87))

    E = PEEK(N)              ' Look up entity at new map position
    A = K + 156 + 2 * E      ' Calculate address of entity's "requirement entity id" in entity catalog
    R = PEEK(A)              ' Get requirement entity id
    IF N - P AND E - R       ' If new pos different from existing (moving) and destination not impassable
                             ' (Note above uses "-" as shorter alternative to "<>")
      X = PEEK(A + 1)        ' Get entity's "transition entity id", which follows requirement id
      IF R < 61              ' Entity has an inventory requirement, hence an obstacle (or impassable)
        IF V(R)              ' If required item is in inventory
          POKE N, R          ' Show the item (briefly), indicating it is working to defeat obstacle
          V(R) = 0           ' Remove item from inventory
          POKE Z + 154 + R, 0   ' Clear item from inventory display
          EXEC M             ' Refresh map display
          I = 6              ' Sound effect step increment
          U = 0              ' Sound effect step duration
          EXEC S             ' Play sound effect
          POKE N, X          ' Replace map cell with the transition-to entity
          O = H * (E = 45)   ' Set exit indicator if winning condition met (i.e. defeated obstacle is wizard)
        ENDIF
      ELSE                   ' This is an unobstructed entity (passable, item, or link)
        P = N                ' Move player to destination cell
        IF R = 62            ' This is an "item" -- something that can be taken
          V(E) = 1           ' Set slot in inventory array corresponding to this item to true
          POKE Z + 154 + E, PEEK(K + E * 3)   ' Look up glyph for item and show on screen
          POKE P, X          ' Replace item on map with its transition-to entity
        ELIF R = 61          ' This is a "link" (portal, entryway, stairs, ship, etc.)
          EXEC M             ' refresh map to show user on top of link cell, before they teleport away
          PAUSE 9
          P = M + DPEEK(K + 3 * X + 1)  ' Look up map address of the destination link, and set position to it
        ENDIF
        DEC T                ' Decrement timer
        O = H * (T < 0)      ' Sets O to 27 (exit indicator) if out of time
      ENDIF
    ENDIF
  UNTIL O = H   ' User pressed Escape, or game was won or lost

  ' Play the final sound.  If E is 45 (Blue Wizard) then player won, and the
  ' sound increment will be set to (positive) 6, causing the fanfare sound to
  ' ascend in pitch.  Otherwise (user lost by running out of time or pressing
  ' Esc) sound increment will be -6, causing descending fanfare.  Either way
  ' the duration of each note will be 12 "jiffies"
  I = (E = 45) * 12 - 6   ' Sound increment 6 if won, -6 if lost
  U = 12                  ' Sound duration
  IF I < 0                ' If player lost...
    PLOT 4,7              ' Overwrite player glyph w/orange skull (active "color" from earlier)
  ENDIF
  EXEC S                  ' Play sound effect

  '  Show user they can start new game by typing "N"
  POSITION 10,6
  ? #6,"$EE$C5$D7";   ' "New"
  GET O
UNTIL O - 78          ' 78 is (AT)ASCII for "N"

' ============================
'    SOUND EFFECT PROCEDURE
' ============================
' Plays an ascending or descending sound.
' Parameters:
'   I - Sound increment; amount pitch rises (or falls if negative) at each step
'   U - Sound duration; time each step is played, in "jiffies"

PROC S
  J = 80
  FOR S = 0 TO 8
    J = J - I
    SOUND 2,J,10,6
    PAUSE U
  NEXT S
  SOUND
ENDPROC