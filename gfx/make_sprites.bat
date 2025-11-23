rem Convert player sprites from spritesheet
rem Literal, 4 BPP

rem Tile sprites (row 1)
sprpck -s4 -t6 -u -p0 -h -o000000 -S010010 -a000000 spritesheet.bmp waterSpr.h
sprpck -s4 -t6 -u -p0 -h -o010000 -S010010 -a000000 spritesheet.bmp landSpr.h
sprpck -s4 -t6 -u -p0 -h -o020000 -S010010 -a000000 spritesheet.bmp forestSpr.h
sprpck -s4 -t6 -u -p0 -h -o030000 -S010010 -a000000 spritesheet.bmp mountainSpr.h
sprpck -s4 -t6 -u -p0 -h -o040000 -S010010 -a000000 spritesheet.bmp entrance1Spr.h
sprpck -s4 -t6 -u -p0 -h -o050000 -S010010 -a000000 spritesheet.bmp entrance2Spr.h
sprpck -s4 -t6 -u -p0 -h -o060000 -S010010 -a000000 spritesheet.bmp entrance3Spr.h
sprpck -s4 -t6 -u -p0 -h -o070000 -S010010 -a000000 spritesheet.bmp portalSpr.h
sprpck -s4 -t6 -u -p0 -h -o080000 -S010010 -a000000 spritesheet.bmp shipSpr.h
sprpck -s4 -t6 -u -p0 -h -o090000 -S010010 -a000000 spritesheet.bmp wallSpr.h
sprpck -s4 -t6 -u -p0 -h -o100000 -S010010 -a000000 spritesheet.bmp floorSpr.h
sprpck -s4 -t6 -u -p0 -h -o110000 -S010010 -a000000 spritesheet.bmp stairSpr.h
sprpck -s4 -t6 -u -p0 -h -o120000 -S010010 -a000000 spritesheet.bmp bridgeSpr.h
sprpck -s4 -t6 -u -p0 -h -o130000 -S010010 -a000000 spritesheet.bmp rampartSpr.h

rem Tile sprites (row 2)
sprpck -s4 -t6 -u -p0 -h -o000010 -S010010 -a000000 spritesheet.bmp stickSpr.h
sprpck -s4 -t6 -u -p0 -h -o010010 -S010010 -a000000 spritesheet.bmp falconSpr.h
sprpck -s4 -t6 -u -p0 -h -o020010 -S010010 -a000000 spritesheet.bmp cageSpr.h
sprpck -s4 -t6 -u -p0 -h -o030010 -S010010 -a000000 spritesheet.bmp webSpr.h
sprpck -s4 -t6 -u -p0 -h -o040010 -S010010 -a000000 spritesheet.bmp crossSpr.h
sprpck -s4 -t6 -u -p0 -h -o050010 -S010010 -a000000 spritesheet.bmp fireSpr.h
sprpck -s4 -t6 -u -p0 -h -o060010 -S010010 -a000000 spritesheet.bmp hammerSpr.h
sprpck -s4 -t6 -u -p0 -h -o070010 -S010010 -a000000 spritesheet.bmp snakeSpr.h
sprpck -s4 -t6 -u -p0 -h -o080010 -S010010 -a000000 spritesheet.bmp skullSpr.h
sprpck -s4 -t6 -u -p0 -h -o090010 -S010010 -a000000 spritesheet.bmp flaskSpr.h
sprpck -s4 -t6 -u -p0 -h -o100010 -S010010 -a000000 spritesheet.bmp wizardSpr.h
sprpck -s4 -t6 -u -p0 -h -o110010 -S010010 -a000000 spritesheet.bmp daggerSpr.h
sprpck -s4 -t6 -u -p0 -h -o120010 -S010010 -a000000 spritesheet.bmp cagedFalconSpr.h

rem Tile sprites (row 3)
sprpck -s4 -t6 -u -p0 -h -o000020 -S010010 -a000000 spritesheet.bmp playerSpr.h
sprpck -s4 -t6 -u -p0 -h -o010020 -S010010 -a000000 spritesheet.bmp spadeSpr.h
sprpck -s4 -t6 -u -p0 -h -o020020 -S010010 -a000000 spritesheet.bmp torchSpr.h

rem Logo sprite
sprpck -s4 -t6 -u -p0 -h -o000052 -S040027 -a000000 spritesheet.bmp logoSpr.h



rem copy /B pf0.h + pf1.h + pf2.h + pf3.h + pf4.h pfArray.h

rem Title sprite
rem sprpck -s4 -t6 -u -o007014 -S086028 -a000000 title.bmp title.h

rem Clean up
del *.pal

