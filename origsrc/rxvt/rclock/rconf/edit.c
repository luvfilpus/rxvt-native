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

#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Toggle.h>

#include "edit.h"

EditDialog *
create_edit_dialog(Widget parent)
{
	EditDialog *ed;
	Widget textbox, togglebox, buttonbox;

	ed = malloc(sizeof(EditDialog));

	ed->dialog = XtVaCreatePopupShell("edit_dialog", transientShellWidgetClass, parent,
		NULL);

	ed->form = XtVaCreateManagedWidget("edit_form", formWidgetClass, ed->dialog,
		NULL);

	ed->label = XtVaCreateManagedWidget("edit_label", labelWidgetClass, ed->form,
		XtNtop,   XtChainTop,
		XtNleft,  XtChainLeft,
		XtNright, XtChainRight,
		NULL);

	textbox = XtVaCreateManagedWidget("edit_textbox", formWidgetClass, ed->form,
		XtNfromVert, ed->label,
		XtNleft,     XtChainLeft,
		NULL);

	ed->time = XtVaCreateManagedWidget("edit_time", dialogWidgetClass, textbox,
		XtNtop,   XtChainTop,
		XtNleft,  XtChainLeft,
		XtNright, XtChainRight,
		NULL);

	ed->date = XtVaCreateManagedWidget("edit_date", dialogWidgetClass, textbox,
		XtNfromVert, ed->time,
		XtNleft,     XtChainLeft,
		XtNright,    XtChainRight,
		NULL);

	ed->message = XtVaCreateManagedWidget("edit_message", dialogWidgetClass, textbox,
		XtNfromVert, ed->date,
		XtNleft,     XtChainLeft,
		XtNright,    XtChainRight,
		NULL);

	ed->program = XtVaCreateManagedWidget("edit_program", dialogWidgetClass, textbox,
		XtNfromVert, ed->message,
		XtNleft,     XtChainLeft,
		XtNright,    XtChainRight,
		NULL);

	togglebox = XtVaCreateManagedWidget("edit_togglebox", boxWidgetClass, ed->form,
		XtNfromVert,    ed->label,
		XtNfromHoriz,   textbox,
		XtNright,       XtChainRight,
		XtNorientation, XtorientVertical,
		NULL);

	ed->mon = XtVaCreateManagedWidget("edit_mon", toggleWidgetClass, togglebox,
		NULL);
	ed->tue = XtVaCreateManagedWidget("edit_tue", toggleWidgetClass, togglebox,
		NULL);
	ed->wed = XtVaCreateManagedWidget("edit_wed", toggleWidgetClass, togglebox,
		NULL);
	ed->thu = XtVaCreateManagedWidget("edit_thu", toggleWidgetClass, togglebox,
		NULL);
	ed->fri = XtVaCreateManagedWidget("edit_fri", toggleWidgetClass, togglebox,
		NULL);
	ed->sat = XtVaCreateManagedWidget("edit_sat", toggleWidgetClass, togglebox,
		NULL);
	ed->sun = XtVaCreateManagedWidget("edit_sun", toggleWidgetClass, togglebox,
		NULL);

	buttonbox = XtVaCreateManagedWidget("edit_buttonbox", boxWidgetClass, ed->form,
		XtNfromVert,    textbox,
		XtNleft,        XtChainLeft,
		XtNright,       XtChainRight,
		XtNbottom,      XtChainBottom,
		XtNorientation, XtEhorizontal,
		NULL);

	ed->ok = XtVaCreateManagedWidget("edit_ok", commandWidgetClass, buttonbox,
		NULL);
	ed->cancel = XtVaCreateManagedWidget("edit_cancel", commandWidgetClass, buttonbox,
		NULL);

	return ed;
}
