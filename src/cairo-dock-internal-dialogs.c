/*********************************************************************************

This file is a part of the cairo-dock program,
released under the terms of the GNU General Public License.

Written by Fabrice Rey (for any bug report, please mail me to fabounet@users.berlios.de)

*********************************************************************************/

#include <string.h>
#include "cairo-dock-dialogs.h"
#define _INTERNAL_MODULE_
#include "cairo-dock-internal-dialogs.h"

extern CairoDock *g_pMainDock;
extern CairoDockLabelDescription g_iconTextDescription;

CairoConfigDialogs myDialogs;

static gboolean get_config (GKeyFile *pKeyFile, CairoConfigDialogs *pDialogs)
{
	gboolean bFlushConfFileNeeded = FALSE;
	
	pDialogs->cButtonOkImage = cairo_dock_get_string_key_value (pKeyFile, "Dialogs", "button_ok image", &bFlushConfFileNeeded, NULL, NULL, NULL);
	pDialogs->cButtonCancelImage = cairo_dock_get_string_key_value (pKeyFile, "Dialogs", "button_cancel image", &bFlushConfFileNeeded, NULL, NULL, NULL);

	pDialogs->iDialogButtonWidth = cairo_dock_get_integer_key_value (pKeyFile, "Dialogs", "button width", &bFlushConfFileNeeded, 48, NULL, NULL);
	pDialogs->iDialogButtonHeight = cairo_dock_get_integer_key_value (pKeyFile, "Dialogs", "button height", &bFlushConfFileNeeded, 32, NULL, NULL);

	double couleur_bulle[4] = {1.0, 1.0, 1.0, 0.7};
	cairo_dock_get_double_list_key_value (pKeyFile, "Dialogs", "background color", &bFlushConfFileNeeded, pDialogs->fDialogColor, 4, couleur_bulle, NULL, NULL);

	pDialogs->iDialogIconSize = cairo_dock_get_integer_key_value (pKeyFile, "Dialogs", "icon size", &bFlushConfFileNeeded, 48, NULL, NULL);

	if (cairo_dock_get_boolean_key_value (pKeyFile, "Dialogs", "homogeneous text", &bFlushConfFileNeeded, TRUE, NULL, NULL))
	{
		pDialogs->dialogTextDescription.iSize = g_iconTextDescription.iSize;
		if (pDialogs->dialogTextDescription.iSize == 0)
			pDialogs->dialogTextDescription.iSize = 14;
		pDialogs->dialogTextDescription.cFont = g_strdup (g_iconTextDescription.cFont);
		pDialogs->dialogTextDescription.iWeight = g_iconTextDescription.iWeight;
		pDialogs->dialogTextDescription.iStyle = g_iconTextDescription.iStyle;
	}
	else
	{
		pDialogs->dialogTextDescription.cFont = cairo_dock_get_string_key_value (pKeyFile, "Dialogs", "message police", &bFlushConfFileNeeded, "sans", NULL, NULL);
		pDialogs->dialogTextDescription.iSize = cairo_dock_get_integer_key_value (pKeyFile, "Dialogs", "message size", &bFlushConfFileNeeded, 14, NULL, NULL);
		int iLabelWeight = cairo_dock_get_integer_key_value (pKeyFile, "Dialogs", "message weight", &bFlushConfFileNeeded, 5, NULL, NULL);
		pDialogs->dialogTextDescription.iWeight = ((PANGO_WEIGHT_HEAVY - PANGO_WEIGHT_ULTRALIGHT) * iLabelWeight + 9 * PANGO_WEIGHT_ULTRALIGHT - PANGO_WEIGHT_HEAVY) / 8;  // on se ramene aux intervalles definit par Pango.
		if (cairo_dock_get_boolean_key_value (pKeyFile, "Dialogs", "message italic", &bFlushConfFileNeeded, FALSE, NULL, NULL))
			pDialogs->dialogTextDescription.iStyle = PANGO_STYLE_ITALIC;
		else
			pDialogs->dialogTextDescription.iStyle = PANGO_STYLE_NORMAL;
	}
	
	double couleur_dtext[3] = {0., 0., 0.};
	cairo_dock_get_double_list_key_value (pKeyFile, "Dialogs", "text color", &bFlushConfFileNeeded, pDialogs->dialogTextDescription.fColorStart, 3, couleur_dtext, NULL, NULL);
	memcpy (&pDialogs->dialogTextDescription.fColorStop, &pDialogs->dialogTextDescription.fColorStart, 3*sizeof (double));
	
	return bFlushConfFileNeeded;
}


static void reset_config (CairoConfigDialogs *pDialogs)
{
	g_free (pDialogs->cButtonOkImage);
	g_free (pDialogs->cButtonCancelImage);
}


static void reload (CairoConfigDialogs *pPrevDialogs, CairoConfigDialogs *pDialogs)
{
	CairoDock *pDock = g_pMainDock;
	
	if (cairo_dock_strings_differ (pPrevDialogs->cButtonOkImage, pDialogs->cButtonOkImage) || cairo_dock_strings_differ (pPrevDialogs->cButtonCancelImage, pDialogs->cButtonCancelImage))
		cairo_dock_load_dialog_buttons (CAIRO_CONTAINER (pDock), pDialogs->cButtonOkImage, pDialogs->cButtonCancelImage);
}


DEFINE_PRE_INIT (Dialogs)
{
	pModule->cModuleName = "Dialogs";
	pModule->cTitle = "Dialogs";
	pModule->cIcon = "gtk-dialog-question";
	pModule->cDescription = "Let the dock talk to you.";
	pModule->iCategory = CAIRO_DOCK_CATEGORY_THEME;
	pModule->iSizeOfConfig = sizeof (CairoConfigDialogs);
	pModule->iSizeOfData = 0;
	
	pModule->reload = (CairoDockInternalModuleReloadFunc) reload;
	pModule->get_config = (CairoDockInternalModuleGetConfigFunc) get_config;
	pModule->reset_config = (CairoDockInternalModuleResetConfigFunc) reset_config;
	pModule->reset_data = NULL;
	
	pModule->pConfig = (CairoInternalModuleConfigPtr) &myDialogs;
	pModule->pData = NULL;
}