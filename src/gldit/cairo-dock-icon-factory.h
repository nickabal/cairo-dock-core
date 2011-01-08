/*
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

#ifndef __CAIRO_DOCK_ICON_FACTORY__
#define  __CAIRO_DOCK_ICON_FACTORY__

#include <glib.h>

#include "cairo-dock-struct.h"
#include "cairo-dock-notifications.h"
G_BEGIN_DECLS

/**
*@file cairo-dock-icon-factory.h This class defines the items contained in containers : Icons.
* An icon can either be:
* - a launcher (it has a command, a class, and possible an X window ID)
* - an appli (it has a X window ID and a class, no command)
* - an applet (it has a module instance and no command, possibly a class)
* - a container (it has a sub-dock and no class nor command)
* - a class icon (it has a bsub-dock and a class, but no command nor X ID)
* - a separator (it has nothing)
* 
* The class defines the methods used to create a generic Icon and to load its various buffers.
* Specialized Icons are created by the corresponding factory.
*/

/// Definition of the type of icons.
typedef enum {
	CAIRO_DOCK_ICON_TYPE_LAUNCHER = 0,
	CAIRO_DOCK_ICON_TYPE_FILE,
	CAIRO_DOCK_ICON_TYPE_CONTAINER,
	CAIRO_DOCK_ICON_TYPE_SEPARATOR,
	CAIRO_DOCK_ICON_TYPE_CLASS_CONTAINER,
	CAIRO_DOCK_ICON_TYPE_APPLI,
	CAIRO_DOCK_ICON_TYPE_APPLET,
	CAIRO_DOCK_ICON_TYPE_OTHER,
	CAIRO_DOCK_NB_ICON_TYPES
	} CairoDockIconTrueType;

/// Definition of the groups of icons.
typedef enum {
	CAIRO_DOCK_LAUNCHER = 0,
	CAIRO_DOCK_SEPARATOR12,
	CAIRO_DOCK_APPLI,
	CAIRO_DOCK_SEPARATOR23,
	CAIRO_DOCK_APPLET,
	CAIRO_DOCK_NB_GROUPS
	} CairoDockIconGroup;

/// Animation state of an icon, sorted by priority.
typedef enum {
	CAIRO_DOCK_STATE_REST = 0,
	CAIRO_DOCK_STATE_MOUSE_HOVERED,
	CAIRO_DOCK_STATE_CLICKED,
	CAIRO_DOCK_STATE_AVOID_MOUSE,
	CAIRO_DOCK_STATE_FOLLOW_MOUSE,
	CAIRO_DOCK_STATE_REMOVE_INSERT,
	CAIRO_DOCK_NB_STATES
	} CairoDockAnimationState;

/// Icon's interface
struct _IconInterface {
	/// function that loads the icon surface (and optionnally texture).
	void (*load_image) (Icon *icon);
	/// function called when the icon is deleted from the current theme.
	gboolean (*on_delete) (Icon *icon);
	/// function called when the user drag something over the icon for more than 500ms.
	void (*action_on_drag_hover) (Icon *icon);
	};

/// Definition of an Icon.
struct _Icon {
	//\____________ Definition.
	/// list of available notifications.
	GPtrArray *pNotificationsTab;
	/// type of the icon.
	CairoDockIconTrueType iTrueType;
	/// group of the icon.
	CairoDockIconGroup iGroup;
	/// interface
	IconInterface iface;
	gpointer pDataSlot[CAIRO_DOCK_NB_DATA_SLOT];
	
	//\____________ properties.
	// generic.
	/// Name of the icon.
	gchar *cName;
	/// Short info displayed on the icon (few characters).
	gchar *cQuickInfo;
	/// name or path of an image displayed on the icon.
	gchar *cFileName;
	/// Class of application the icon will be bound to.
	gchar *cClass;
	/// name of the dock the icon belongs to (NULL means it's not currently inside a dock).
	gchar *cParentDockName;
	/// Sub-dock the icon is pointing to.
	CairoDock *pSubDock;
	/// Order of the icon amongst the other icons of its group.
	gdouble fOrder;
	
	gint iSpecificDesktop;
	gint iSubdockViewType;
	/// a hint to indicate the icon should be kept static (no animation like bouncing).
	gboolean bStatic;
	/// a flag that allows the icon to be always visible, even with the dock is hidden.
	gboolean bAlwaysVisible;
	gboolean bIsDemandingAttention;
	
	CairoDataRenderer *pDataRenderer;
	CairoDockTransition *pTransition;
	
	CairoDockAnimationState iAnimationState;
	/// Whether the icon is currently pointed or not.
	gboolean bPointed;
	gdouble fInsertRemoveFactor;
	
	// Launcher.
	gchar *cDesktopFileName;  // nom (et non pas chemin) du fichier .desktop
	gchar *cCommand;
	gchar *cWorkingDirectory;
	gchar *cBaseURI;
	gint iVolumeID;
	
	// Appli.
	Window Xid;
	gboolean bIsHidden;
	gboolean bIsFullScreen;
	gboolean bIsMaximized;
	gboolean bHasIndicator;
	GtkAllocation windowGeometry;
	gint iNumDesktop;
	gint iViewPortX, iViewPortY;
	gint iStackOrder;
	gint iLastCheckTime;
	gchar *cInitialName;
	gchar *cLastAttentionDemand;
	Pixmap iBackingPixmap;
	//Damage iDamageHandle;
	
	// Applet.
	CairoDockModuleInstance *pModuleInstance;
	CairoDockModuleInstance *pAppletOwner;
	
	//\____________ Buffers.
	gdouble fWidth, fHeight;  // taille dans le container.
	gint iImageWidth, iImageHeight;  // taille de la surface/texture telle qu'elle a ete creee.
	cairo_surface_t* pIconBuffer;
	GLuint iIconTexture;
	cairo_surface_t* pReflectionBuffer;
	
	gint iTextWidth, iTextHeight;
	cairo_surface_t* pTextBuffer;
	GLuint iLabelTexture;
	
	gint iQuickInfoWidth, iQuickInfoHeight;
	cairo_surface_t* pQuickInfoBuffer;
	GLuint iQuickInfoTexture;
	
	//\____________ Parametres de dessin, definis par la vue/les animations.
	gdouble fXMin, fXMax;  // Abscisse extremale gauche/droite que the icon atteindra (variable avec la vague).
	gdouble fXAtRest;  // Abscisse de the icon au repos.
	gdouble fPhase;  // Phase de the icon (entre -pi et piconi).
	gdouble fX, fY;  // Abscisse/Ordonnee temporaire du bord haut-gauche de l'image de the icon.
	
	gdouble fScale;
	gdouble fDrawX, fDrawY;
	gdouble fWidthFactor, fHeightFactor;
	gdouble fAlpha;
	gdouble fDeltaYReflection;  // Decalage en ordonnees du reflet (rebond).
	gdouble fOrientation;  // par rapport a la verticale Oz
	gint iRotationX;  // Rotation autour de l'axe Ox
	gint iRotationY;  // Rotation autour de l'axe Oy
	gdouble fReflectShading;
	
	gdouble fGlideOffset;  // decalage pour le glissement des icons.
	gint iGlideDirection;  // direction dans laquelle glisse the icon.
	gdouble fGlideScale;  // echelle d'adaptation au glissement.
	
	gboolean bBeingRemovedByCairo;  // devrait etre dans pDataSlot...
	
	guint iSidRedrawSubdockContent;
	guint iSidLoadImage;
	guint iSidDoubleClickDelay;
	CairoContainer *pContainerForLoad;
	gint iNbDoubleClickListeners;
	
	gchar **pMimeTypes;
	
	gpointer reserved[4];
};

typedef void (*CairoIconContainerLoadFunc) (void);
typedef void (*CairoIconContainerUnloadFunc) (void);
typedef void (*CairoIconContainerRenderFunc) (Icon *pIcon, CairoContainer *pContainer, int w, int h, cairo_t *pCairoContext);
typedef void (*CairoIconContainerRenderOpenGLFunc) (Icon *pIcon, CairoContainer *pContainer, int w, int h);

/// Definition of an Icon container (= an icon holding a sub-dock) renderer.
struct _CairoIconContainerRenderer {
	CairoIconContainerLoadFunc load;
	CairoIconContainerUnloadFunc unload;
	CairoIconContainerRenderFunc render;
	CairoIconContainerRenderOpenGLFunc render_opengl;
};


#define CAIRO_DOCK_ICON_TYPE_IS_LAUNCHER(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_LAUNCHER)
#define CAIRO_DOCK_ICON_TYPE_IS_FILE(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_FILE)
#define CAIRO_DOCK_ICON_TYPE_IS_CONTAINER(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_CONTAINER)
#define CAIRO_DOCK_ICON_TYPE_IS_SEPARATOR(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_SEPARATOR)
#define CAIRO_DOCK_ICON_TYPE_IS_CLASS_CONTAINER(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_CLASS_CONTAINER)
#define CAIRO_DOCK_ICON_TYPE_IS_APPLI(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_APPLI)
#define CAIRO_DOCK_ICON_TYPE_IS_APPLET(icon) (icon != NULL && (icon)->iTrueType == CAIRO_DOCK_ICON_TYPE_APPLET)

/* TRUE if the icon is a launcher (a real launcher with a command or an URI, or a container icon, or even a fake launcher).
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_LAUNCHER(icon) (icon != NULL && ((icon)->cCommand != NULL || (icon)->cBaseURI != NULL || ((icon)->pSubDock != NULL && (icon)->pModuleInstance == NULL)))

/** TRUE if the icon holds a window.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_APPLI(icon) (icon != NULL && (icon)->Xid != 0)

/** TRUE if the icon holds an instance of a module.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_APPLET(icon) (icon != NULL && (icon)->pModuleInstance != NULL)

/* TRUE if the icon is an icon of separator.
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_SEPARATOR(icon) (icon != NULL && (icon)->pModuleInstance == NULL && (icon)->Xid == 0 && (icon)->pSubDock == NULL && (icon)->cCommand == NULL && (icon)->cBaseURI == NULL)

/* TRUE if the icon is an icon of launcher launching something.
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_NORMAL_LAUNCHER(icon) (CAIRO_DOCK_IS_LAUNCHER (icon) && (icon)->cCommand != NULL)

/* TRUE if the icon is an icon of launcher defined by a .desktop file.
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_STORED_LAUNCHER(icon) (CAIRO_DOCK_IS_LAUNCHER (icon) && (icon)->cDesktopFileName != NULL)

/* TRUE if the icon is an icon representing an URI.
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_URI_LAUNCHER(icon) (CAIRO_DOCK_IS_LAUNCHER (icon) && (icon)->cBaseURI != NULL)

/* TRUE if the icon is a fake icon (for class groupment).
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_FAKE_LAUNCHER(icon) (icon != NULL && (icon)->cCommand == NULL && (icon)->cClass != NULL && (icon)->pModuleInstance == NULL && (icon)->pSubDock != NULL && (icon)->cDesktopFileName == NULL)

/* TRUE if the icon is a container icon.
*@param icon an icon.
*/
//#define CAIRO_DOCK_IS_CONTAINER_LAUNCHER(icon) (CAIRO_DOCK_IS_LAUNCHER (icon) && (icon)->cCommand == NULL && (icon)->cBaseURI == NULL && icon->cClass == NULL)

/** TRUE if the icon is an icon pointing on the sub-dock of a class.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_MULTI_APPLI(icon) ((CAIRO_DOCK_ICON_TYPE_IS_LAUNCHER (icon) || CAIRO_DOCK_ICON_TYPE_IS_CLASS_CONTAINER (icon)) && icon->pSubDock != NULL)

/** TRUE if the icon is an automatic separator.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_AUTOMATIC_SEPARATOR(icon) (CAIRO_DOCK_ICON_TYPE_IS_SEPARATOR (icon) && (icon)->cDesktopFileName == NULL)

/** TRUE if the icon is a separator added by the user.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_USER_SEPARATOR(icon) (CAIRO_DOCK_ICON_TYPE_IS_SEPARATOR (icon) && (icon)->cDesktopFileName != NULL)
/**
*TRUE if the icon is an icon d'appli only.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_NORMAL_APPLI(icon) (CAIRO_DOCK_IS_APPLI (icon) && CAIRO_DOCK_ICON_TYPE_IS_APPLI (icon))
/**
*TRUE if the icon is an icon d'applet detachable en desklet.
*@param icon an icon.
*/
#define CAIRO_DOCK_IS_DETACHABLE_APPLET(icon) (CAIRO_DOCK_IS_APPLET (icon) && (icon)->pModuleInstance->bCanDetach)


/** Create an empty icon.
*@return the newly allocated icon object.
*/
Icon *cairo_dock_new_icon (void);

void cairo_dock_free_icon_buffers (Icon *icon);


/** Search the path of an icon into the defined icons themes. It also handles the '~' caracter in paths.
 * @param cFileName name of the icon file.
 * @return the complete path of the icon, or NULL if not found.
 */
gchar *cairo_dock_search_icon_s_path (const gchar *cFileName);

/* Cree la surface de reflection d'une icone (pour cairo).
*@param pIcon l'icone.
*@param pContainer le container de l'icone.
*/
void cairo_dock_add_reflection_to_icon (Icon *pIcon, CairoContainer *pContainer);

void cairo_dock_set_icon_size (CairoContainer *pContainer, Icon *icon);

/**Fill the image buffer (surface & texture) of a given icon, according to its type. Set its size if necessary, and fills the reflection buffer for cairo.
*@param icon the icon.
*@param pContainer its container.
*/
void cairo_dock_load_icon_image (Icon *icon, CairoContainer *pContainer);

/**Fill the label buffer (surface & texture) of a given icon, according to a text description.
*@param icon the icon.
*@param pTextDescription desctiption of the text rendering.
*/
void cairo_dock_load_icon_text (Icon *icon, CairoDockLabelDescription *pTextDescription);

/**Fill the quick-info buffer (surface & texture) of a given icon, according to a text description.
*@param icon the icon.
*@param pTextDescription desctiption of the text rendering.
*@param fMaxScale maximum zoom.
*/
void cairo_dock_load_icon_quickinfo (Icon *icon, CairoDockLabelDescription *pTextDescription, double fMaxScale);

/** Fill all the buffers (surfaces & textures) of a given icon, according to its type. Set its size accordingly, and fills the reflection buffer for cairo. Label and quick-info are loaded with the current global text description.
*@param pIcon the icon.
*@param pContainer its container.
*/
void cairo_dock_load_icon_buffers (Icon *pIcon, CairoContainer *pContainer);

void cairo_dock_trigger_load_icon_buffers (Icon *pIcon, CairoContainer *pContainer);

void cairo_dock_reload_buffers_in_dock (CairoDock *pDock, gboolean bReloadAppletsToo, gboolean bRecursive);
#define cairo_dock_load_buffers_in_one_dock(pDock) cairo_dock_reload_buffers_in_dock (pDock, TRUE, FALSE)

void cairo_dock_reload_icon_image (Icon *icon, CairoContainer *pContainer);


void cairo_dock_draw_subdock_content_on_icon (Icon *pIcon, CairoDock *pDock);


G_END_DECLS
#endif
