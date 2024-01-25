#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	consoleDebugInit(debugDevice_SVC);

	fprintf(stderr, "Mii selector demo.\n");
	fprintf(stderr, "Press A to bring up Mii selector with default settings.\n");
	fprintf(stderr, "Press B to bring up Mii selector with custom settings.\n");
	fprintf(stderr, "Press START to exit.\n");

	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break;

		bool didit = false;
		static MiiSelectorConf msConf;
		static MiiSelectorReturn msRet;
		static char miiname[36];
		static char miiauthor[30];

		if (kDown & KEY_A)
		{
			didit = true;
			// Ensure that the config is initalized and that all
			// defaults are set properly. (failing to do this to a new
			// MiiSelecorConf at least once before launching the miiSelector
			// can result in undefined behavior)
			miiSelectorInit(&msConf);
			miiSelectorLaunch(&msConf, &msRet);
		}

		if (kDown & KEY_B)
		{
			didit = true;
			miiSelectorInit(&msConf);
			// Sets title of Mii selector.
			miiSelectorSetTitle(&msConf, "This is a custom title!");
			// Choose and option to enable. Any options not listed will be disabled.
			// 0 can be used to select no option.
			miiSelectorSetOptions(&msConf, MIISELECTOR_CANCEL|MIISELECTOR_GUESTS|MIISELECTOR_TOP|MIISELECTOR_GUESTSTART);
			// Start on the Mii with the database index of 1.
			// This is a guest Mii since the option MIISELECTOR_GUESTSTART is enabled.
			// If MIISELECTOR_GUESTSTART was not enabled, this would be a user Mii.
			miiSelectorSetInitialIndex(&msConf, 1);
			// Blacklist main user Mii
			miiSelectorBlacklistUserMii(&msConf, 0);
			miiSelectorLaunch(&msConf, &msRet);
		}

		if (didit)
		{
			// Check that the data in the miiSelector return buffer
			// is correct.
			if (miiSelectorChecksumIsValid(&msRet))
			{
				if (!msRet.no_mii_selected)
				{
					fprintf(stderr, "A Mii was selected.\n");
					miiSelectorReturnGetName(&msRet, miiname, sizeof(miiname));
					miiSelectorReturnGetAuthor(&msRet, miiauthor, sizeof(miiauthor));
					fprintf(stderr, "Name: %s\n", miiname);
					fprintf(stderr, "Author: %s\n", miiauthor);
					fprintf(stderr, "Birthday: Month-%d/Day-%d\n", msRet.mii.mii_details.bday_month, msRet.mii.mii_details.bday_day);
					fprintf(stderr, "Sex: %d\n", msRet.mii.mii_details.sex);
					fprintf(stderr, "Color: %d\n", msRet.mii.mii_details.shirt_color);
					fprintf(stderr, "Favorite: %d\n", msRet.mii.mii_details.favorite);
					// Keep in mind that not all things align with their order in Mii Maker.
					// https://www.3dbrew.org/wiki/Mii#Mii_values
					fprintf(stderr, "Eyebrow: %d\n", msRet.mii.eyebrow_details.style);
					fprintf(stderr, "Eyebrow color: %d\n", msRet.mii.eyebrow_details.color);
					fprintf(stderr, "Nose: %d\n", msRet.mii.nose_details.style);
				} else
					fprintf(stderr, "No Mii was selected.\n");
			} else
				fprintf(stderr, "Return checksum invalid.\n");
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
