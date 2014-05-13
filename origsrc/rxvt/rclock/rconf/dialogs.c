/*
 * Copyright (c) 1999 Chris Wareham.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>

#include "dialogs.h"

void
confirm_dialog(Widget parent, const char *message,
	Callback yes_callback, Callback no_callback, XtPointer callback_data)
{
	Widget dialog, form, box, label, button;

	dialog = XtVaCreatePopupShell("confirm_dialog", transientShellWidgetClass, parent,
		NULL);

	form = XtVaCreateManagedWidget("form", formWidgetClass, dialog,
		NULL);

	label = XtVaCreateManagedWidget("label", labelWidgetClass, form,
		XtNtop,   XtChainTop,
		XtNleft,  XtChainLeft,
		XtNright, XtChainRight,
		XtNlabel, message,
		NULL);

	box = XtVaCreateManagedWidget("box", boxWidgetClass, form,
		XtNfromVert,    label,
		XtNleft,        XtChainLeft,
		XtNright,       XtChainRight,
		XtNbottom,      XtChainBottom,
		XtNorientation, XtEhorizontal,
		NULL);

	button = XtVaCreateManagedWidget("yes", commandWidgetClass, box, NULL);

	XtAddCallback(button, XtNcallback, yes_callback, callback_data);

	button = XtVaCreateManagedWidget("no", commandWidgetClass, box, NULL);

	XtAddCallback(button, XtNcallback, no_callback, callback_data);

	XtRealizeWidget(dialog);

	XtPopup(dialog, XtGrabExclusive);
	XMapRaised(XtDisplay(dialog), XtWindow(dialog));
}

void
message_dialog(Widget parent, const char *message,
	Callback callback, XtPointer callback_data)
{
	Widget dialog, form, label, button;

	dialog = XtVaCreatePopupShell("message_dialog", transientShellWidgetClass, parent,
		NULL);

	form = XtVaCreateManagedWidget("form", formWidgetClass, dialog,
		NULL);

	label = XtVaCreateManagedWidget("label", labelWidgetClass, form,
		XtNtop,   XtChainTop,
		XtNleft,  XtChainLeft,
		XtNright, XtChainRight,
		XtNlabel, message,
		NULL);

	button = XtVaCreateManagedWidget("ok", commandWidgetClass, form,
		XtNfromVert, label,
		XtNleft,     XtChainLeft,
		XtNbottom,   XtChainBottom,
		NULL);

	XtAddCallback(button, XtNcallback, callback, callback_data);

	XtRealizeWidget(dialog);

	XtPopup(dialog, XtGrabExclusive);
	XMapRaised(XtDisplay(dialog), XtWindow(dialog));
}

void
destroy_dialog(Widget widget)
{
	do {
		widget = XtParent(widget);
	} while (XtParent(widget) && XtIsShell(widget) == FALSE);

	XtDestroyWidget(widget);
}
