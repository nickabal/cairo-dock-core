/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <cairo.h>

#include "cairo-dock-image-buffer.h"
#include "cairo-dock-draw.h"
#include "cairo-dock-icon-factory.h"
#include "cairo-dock-icon-facility.h"  // cairo_dock_icon_get_allocated_width
#include "cairo-dock-module-manager.h"  // cairo_dock_write_active_modules
#include "cairo-dock-module-factory.h"
#include "cairo-dock-surface-factory.h"
#include "cairo-dock-animations.h"
#include "cairo-dock-container.h"
#include "cairo-dock-dock-factory.h"
#include "cairo-dock-log.h"
#include "cairo-dock-applet-factory.h"
#include "cairo-dock-icon-manager.h"  // cairo_dock_search_icon_s_path
#include "cairo-dock-applet-manager.h"


static cairo_surface_t *cairo_dock_create_applet_surface (const gchar *cIconFileName, int iWidth, int iHeight)
{
	cairo_surface_t *pNewSurface;
	if (cIconFileName == NULL)
	{
		pNewSurface = cairo_dock_create_blank_surface (
			iWidth,
			iHeight);
	}
	else
	{
		gchar *cIconPath = cairo_dock_search_icon_s_path (cIconFileName, MAX (iWidth, iHeight));
		pNewSurface = cairo_dock_create_surface_from_image_simple (cIconPath,
			iWidth,
			iHeight);
		g_free (cIconPath);
	}
	return pNewSurface;
}


static void _load_applet (Icon *icon)
{
	int iWidth = cairo_dock_icon_get_allocated_width (icon);
	int iHeight = cairo_dock_icon_get_allocated_height (icon);
	cairo_surface_t *pSurface = cairo_dock_create_applet_surface (icon->cFileName,
		iWidth,
		iHeight);
	if (pSurface == NULL && icon->pModuleInstance != NULL)  // une image inexistante a ete definie en conf => on met l'icone par defaut. Si aucune image n'est definie, alors c'est a l'applet de faire qqch (dessiner qqch, mettre une image par defaut, etc).
	{
		cd_debug ("SET default image: %s", icon->pModuleInstance->pModule->pVisitCard->cIconFilePath);
		pSurface = cairo_dock_create_surface_from_image_simple (icon->pModuleInstance->pModule->pVisitCard->cIconFilePath,
			iWidth,
			iHeight);
	}  // on ne recharge pas myDrawContext ici, mais plutot dans cairo_dock_load_icon_image(), puisqu'elle gere aussi la destruction de la surface.
	cairo_dock_load_image_buffer_from_surface (&icon->image, pSurface, iWidth, iHeight);
}

static gboolean _delete_applet (Icon *icon)
{
	if (icon->pModuleInstance != NULL)
	{
		cairo_dock_deinstanciate_module (icon->pModuleInstance);  // desactive l'instance du module -> n'est plus une applet valide.
		cairo_dock_write_active_modules ();
		return TRUE;
	}
	return FALSE;
}

Icon *cairo_dock_create_icon_for_applet (CairoDockMinimalAppletConfig *pMinimalConfig, CairoDockModuleInstance *pModuleInstance)
{
	//\____________ On cree l'icone.
	Icon *icon = cairo_dock_new_applet_icon (pMinimalConfig, pModuleInstance);
	icon->iface.load_image = _load_applet;
	icon->iface.on_delete = _delete_applet;
	
	return icon;
}
