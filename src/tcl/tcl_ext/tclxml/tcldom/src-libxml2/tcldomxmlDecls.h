/*
 * tcldomxmlDecls.h --
 *
 *	Declarations of functions in the platform independent public TCLDOMXML API.
 *
 */

#ifndef _TCLDOMXMLDECLS
#define _TCLDOMXMLDECLS

/*
 * WARNING: The contents of this file is automatically generated by the
 * genStubs.tcl script. Any modifications to the function declarations
 * below should be made in the tcldomxml.decls script.
 */

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported function declarations:
 */

/* 0 */
EXTERN int		Tcldomxml_Init _ANSI_ARGS_((Tcl_Interp * interp));
/* Slot 1 is reserved */
/* 2 */
EXTERN Tcl_Obj *	TclDOM_NewDoc _ANSI_ARGS_((Tcl_Interp * interp));
/* 3 */
EXTERN Tcl_Obj *	TclDOM_CreateObjFromNode _ANSI_ARGS_((
				xmlNodePtr nodePtr));
/* 4 */
EXTERN Tcl_Obj *	TclDOM_CreateObjFromDoc _ANSI_ARGS_((
				xmlDocPtr docPtr));
/* 5 */
EXTERN int		TclDOM_GetNodeFromObj _ANSI_ARGS_((
				Tcl_Interp * interp, Tcl_Obj * objPtr, 
				xmlNodePtr * nodePtrPtr));
/* 6 */
EXTERN int		TclDOM_GetDocFromObj _ANSI_ARGS_((
				Tcl_Interp * interp, Tcl_Obj * objPtr, 
				xmlDocPtr * docPtrPtr));

typedef struct TcldomxmlStubs {
    int magic;
    struct TcldomxmlStubHooks *hooks;

    int (*tcldomxml_Init) _ANSI_ARGS_((Tcl_Interp * interp)); /* 0 */
    void *reserved1;
    Tcl_Obj * (*tclDOM_NewDoc) _ANSI_ARGS_((Tcl_Interp * interp)); /* 2 */
    Tcl_Obj * (*tclDOM_CreateObjFromNode) _ANSI_ARGS_((xmlNodePtr nodePtr)); /* 3 */
    Tcl_Obj * (*tclDOM_CreateObjFromDoc) _ANSI_ARGS_((xmlDocPtr docPtr)); /* 4 */
    int (*tclDOM_GetNodeFromObj) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Obj * objPtr, xmlNodePtr * nodePtrPtr)); /* 5 */
    int (*tclDOM_GetDocFromObj) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Obj * objPtr, xmlDocPtr * docPtrPtr)); /* 6 */
} TcldomxmlStubs;

#ifdef __cplusplus
extern "C" {
#endif
extern TcldomxmlStubs *tcldomxmlStubsPtr;
#ifdef __cplusplus
}
#endif

#if defined(USE_TCLDOMXML_STUBS) && !defined(USE_TCLDOMXML_STUB_PROCS)

/*
 * Inline function declarations:
 */

#ifndef Tcldomxml_Init
#define Tcldomxml_Init \
	(tcldomxmlStubsPtr->tcldomxml_Init) /* 0 */
#endif
/* Slot 1 is reserved */
#ifndef TclDOM_NewDoc
#define TclDOM_NewDoc \
	(tcldomxmlStubsPtr->tclDOM_NewDoc) /* 2 */
#endif
#ifndef TclDOM_CreateObjFromNode
#define TclDOM_CreateObjFromNode \
	(tcldomxmlStubsPtr->tclDOM_CreateObjFromNode) /* 3 */
#endif
#ifndef TclDOM_CreateObjFromDoc
#define TclDOM_CreateObjFromDoc \
	(tcldomxmlStubsPtr->tclDOM_CreateObjFromDoc) /* 4 */
#endif
#ifndef TclDOM_GetNodeFromObj
#define TclDOM_GetNodeFromObj \
	(tcldomxmlStubsPtr->tclDOM_GetNodeFromObj) /* 5 */
#endif
#ifndef TclDOM_GetDocFromObj
#define TclDOM_GetDocFromObj \
	(tcldomxmlStubsPtr->tclDOM_GetDocFromObj) /* 6 */
#endif

#endif /* defined(USE_TCLDOMXML_STUBS) && !defined(USE_TCLDOMXML_STUB_PROCS) */

/* !END!: Do not edit above this line. */

#endif /* _TCLXMLDECLS */
