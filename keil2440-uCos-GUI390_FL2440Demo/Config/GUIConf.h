#ifndef GUICONF_H
#define GUICONF_H

#define GUI_OS                    (1)  	/* Compile with multitasking support */
#define GUI_SUPPORT_TOUCH         (1)  	/* Support a touch screen (req. win-manager) */
#define GUI_SUPPORT_UNICODE       (1)  	/* Support mixed ASCII/UNICODE strings */
#define	GUI_SUPPORT_MEMDEV        (1)	/* Memory devices available */	
#define GUI_DEFAULT_FONT          &GUI_Font6x8
#define GUI_WINSUPPORT            (1)  	/* Window manager package available */
#define GUI_SUPPORT_AA            (1)  	/* Anti aliasing available */

#define GUI_SUPPORT_CURSOR        (0)

#define GUI_ALLOC_SIZE            22500  /* Size of dynamic memory ... For WM and memory devices*/


//#define GUI_ALLOC_SIZE             1024 /* (1k) Size of dynamic memory ... For WM and memory devices*/


#endif

