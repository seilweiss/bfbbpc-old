#ifndef ZFMV_H
#define ZFMV_H

enum eFMVFile
{
	eFMVFile_PromoFOP,
	eFMVFile_PromoTak,
	eFMVFile_PromoJN,
	eFMVFile_PromoSB,
	eFMVFile_LogoHeavyIron,
	eFMVFile_LogoNick,
	eFMVFile_LogoRW,
	eFMVFile_LogoTHQ,
	eFMVFile_Demo1,
	eFMVFile_Demo2,
	eFMVFile_Demo3,
	eFMVFile_LogoPixar,
	eFMVFileCount
};

unsigned int zFMVPlay(char *filename, unsigned int buttons, float time, bool skippable,
                      bool lockController);
char *zFMVFileGetName(eFMVFile fileEnum);

#endif