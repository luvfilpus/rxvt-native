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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/param.h>

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h> /* for XtConfigureWidget and XtResizeWidget */
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Viewport.h>

#include "list.h"
#include "dialogs.h"
#include "edit.h"
#include "rconf.h"

static void create_interface(Interface *);
static void populate_interface(Interface *);
static char *format_line(Entry *);
static void add_cb(Widget, XtPointer, XtPointer);
static void modify_cb(Widget, XtPointer, XtPointer);
static void editok_cb(Widget, XtPointer, XtPointer);
static void editcancel_cb(Widget, XtPointer, XtPointer);
static void delete_cb(Widget, XtPointer, XtPointer);
static void deleteyes_cb(Widget, XtPointer, XtPointer);
static void deleteno_cb(Widget, XtPointer, XtPointer);
static void close_cb(Widget, XtPointer, XtPointer);
static void warning_cb(Widget, XtPointer, XtPointer);
static void error_cb(Widget, XtPointer, XtPointer);
static char *get_text(Widget);

static FILE *open_config(void);
static void read_config(FILE *, List *);
static char *get_line(FILE *);
static char *trim_string(char *);
static int write_config(List *);

static char *defaults[] = {
#include "RConf_ad.h"
NULL
};

int
main(int argc, char *argv[])
{
	char *progname, *ptr;
	XtAppContext app_context;
	Interface ui;
	FILE *fp;

	progname = argv[0];
	if ((ptr = strrchr(progname, '/')))
		progname = ptr + 1;

	argv[0] = "rconf";

	XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

	ui.toplevel = XtAppInitialize(&app_context, "RConf",
		NULL, 0, &argc, argv, defaults, NULL, 0);

	ui.entries_changed = FALSE;
	ui.entries = list_init(NULL);

	create_interface(&ui);

	if ((fp = open_config()) == NULL) {
		message_dialog(ui.dialog, "Unable to create config file", error_cb, NULL);
		XtAppMainLoop(app_context);
	}
	read_config(fp, ui.entries);
	fclose(fp);

	populate_interface(&ui);

	XtAppMainLoop(app_context);

	return 0;
}

FILE *
open_config(void)
{
	char *ptr, path[MAXPATHLEN];
	struct passwd *pwent;
	FILE *fp;

	if ((ptr = getenv("HOME")) == NULL) {
		pwent = getpwuid(getuid());
		ptr = pwent->pw_dir;
	}

	snprintf(path, MAXPATHLEN, "%s/%s", ptr, CONFIGFILE);

	if ((fp = fopen(path, "r+")) == NULL) {
		if ((fp = fopen(path, "w+")) == NULL)
			return NULL;
	}

	return fp;
}

void
read_config(FILE *fp, List *entries)
{
	char *ptr, *line;
	regex_t days_preg, msg_preg, prog_preg;
	regmatch_t pmatch[2];
	Entry *entry;
	int found_time;

	regcomp(&days_preg, "^([mtwrfsu]{1,7}) *", REG_EXTENDED);
	regcomp(&msg_preg, "^ *([^;]+)", REG_EXTENDED);
	regcomp(&prog_preg, "^; *(.+)$", REG_EXTENDED);

	while ((line = get_line(fp))) {
		ptr = trim_string(line);
		if (strlen(ptr) == 0) {
			free(line);
			continue;
		}

#ifdef DEBUG
		printf("\n%s\n", ptr);
#endif /* DEBUG */

		found_time = FALSE;

		entry = calloc(1, sizeof(Entry));

		if (sscanf(ptr, "%d:%d ", &entry->hh, &entry->mm) == 2) {
			found_time = TRUE;
		} else if (sscanf(ptr, "%d:* ", &entry->hh) == 1) {
			entry->mm = -1;
			found_time = TRUE;
		} else if (sscanf(ptr, "*:%d ", &entry->mm) == 1) {
			entry->hh = -1;
			found_time = TRUE;
		} else if (strncmp(ptr, "*:* ", 4) == 0) {
			entry->hh = entry->mm = -1;
			found_time = TRUE;
		} else {
			entry->hh = entry->mm = -1;
		}

		while (*ptr != ' ' && *ptr != ';')
			ptr++;

#ifdef DEBUG
		printf("Time    : '%02d:%02d'\n", entry->hh, entry->mm);
#endif /* DEBUG */

		/* days and date can only be specified if time is specified */
		if (found_time && *ptr != ';') {
			while (*ptr == ' ')
				ptr++;

			if (strncmp(ptr, "* ", 2) == 0) {
				strcpy(entry->days, "*");
				ptr++;
			} else if (regexec(&days_preg, ptr, 2, pmatch, 0) == 0) {
				strncpy(entry->days, ptr + pmatch[1].rm_so,
					pmatch[1].rm_eo - pmatch[1].rm_so);
				entry->days[pmatch[1].rm_eo - pmatch[1].rm_so] = '\0';

				ptr += pmatch[1].rm_eo;
			}
#ifdef DEBUG
			if (strlen(entry->days))
				printf("Days    : '%s'\n", entry->days);
#endif /* DEBUG */

			while (*ptr == ' ')
				ptr++;

			if (sscanf(ptr, "%d/%d/%d ", &entry->month, &entry->day, &entry->year) == 3) {
				/* fully specified date */
			} else if (sscanf(ptr, "*/%d/%d ", &entry->day, &entry->year) == 2) {
				entry->month = -1;
			} else if (sscanf(ptr, "%d/*/%d ", &entry->month, &entry->year) == 2) {
				entry->day = -1;
			} else if (sscanf(ptr, "%d/%d/* ", &entry->month, &entry->day) == 2) {
				entry->year = -1;
			} else if (sscanf(ptr, "%d/*/* ", &entry->month) == 1) {
				entry->day = entry->year = -1;
			} else if (sscanf(ptr, "*/%d/* ", &entry->day) == 1) {
				entry->month = entry->year = -1;
			} else if (sscanf(ptr, "*/*/%d ", &entry->year) == 1) {
				entry->month = entry->day = -1;
			} else if (strncmp(ptr, "* ", 2) == 0) {
				entry->day = entry->month = entry->year = -1;
			} else {
				entry->day = entry->month = entry->year = -1;
			}

#ifdef DEBUG
			printf("Date    : '%02d/%02d/%d'\n", entry->month, entry->day, entry->year);
#endif /* DEBUG */

			while (*ptr != ' ')
				ptr++;
		}

		while (*ptr == ' ')
			ptr++;

		if (regexec(&msg_preg, ptr, 2, pmatch, 0) == 0) {
			entry->message = malloc(pmatch[1].rm_eo - pmatch[1].rm_so + 1);
			strncpy(entry->message, ptr + pmatch[1].rm_so,
				pmatch[1].rm_eo - pmatch[1].rm_so);
			entry->message[pmatch[1].rm_eo - pmatch[1].rm_so] = '\0';

			ptr += pmatch[1].rm_eo;
#ifdef DEBUG
			printf("Message : '%s'\n", entry->message);
#endif /* DEBUG */
		}

		if (regexec(&prog_preg, ptr, 2, pmatch, 0) == 0) {
			entry->program = malloc(pmatch[1].rm_eo - pmatch[1].rm_so + 1);
			strncpy(entry->program, ptr + pmatch[1].rm_so,
				pmatch[1].rm_eo - pmatch[1].rm_so);
			entry->program[pmatch[1].rm_eo - pmatch[1].rm_so] = '\0';
#ifdef DEBUG
			printf("Program : '%s'\n", entry->program);
#endif /* DEBUG */
		}

		/* a message or program must be specified for this entry to be valid */
		if(entry->message || entry->program) {
			if (entry->hh > 23 || entry->hh < -1)
				entry->hh = -1;

			if (entry->mm > 59 || entry->mm < -1)
				entry->mm = -1;

			if (entry->day > 31 || entry->day < 1)
				entry->day = -1;

			if (entry->month > 12 || entry->month < 1)
				entry->month = -1;

			if (entry->year > 9999 || entry->year < 1)
				entry->year = -1;

			list_add(entries, entry);
		} else
			free(entry);

		free(line);
	}

	regfree(&days_preg);
	regfree(&msg_preg);
	regfree(&prog_preg);
}

char *
get_line(FILE *fp)
{
	char *line, *ptr, buf[BUFSIZ];

	if ((line = malloc(BUFSIZ)) == NULL)
		return NULL;

	if ((fgets(line, BUFSIZ, fp)) == NULL) {
		free(line);
		return NULL;
	}

	while (line[strlen(line) - 1] != '\n') {
		if(feof(fp))
			break;
		fgets(buf, BUFSIZ, fp);
		if((ptr = realloc(line, strlen(line) + BUFSIZ)) == NULL) {
			free(line);
			return NULL;
		}
		line = ptr;
		ptr = line + strlen(line);
		strcpy(ptr, buf);
	}

	if (line[strlen(line) - 1] == '\n')
		line[strlen(line) - 1] = '\0';

	return line;
}

char *
trim_string(char *str)
{
	unsigned int n;

	if (str == NULL || *str == '\0')
		return str;

	while (*str && isspace(*str))
		str++;

	for (n = strlen(str) - 1; n >= 0 && isspace(str[n]); n--)
		str[n] = '\0';

	for (n = 0; n < strlen(str); n++) {
		if (str[n] == '#') {
			str[n] = '\0';
			break;
		}
	}

	return str;
}

int
write_config(List *entries)
{
	char *ptr, path[MAXPATHLEN];
	struct passwd *pwent;
	FILE *fp;
	struct list_node *walker;
	Entry *entry;
	int len, i;

	if ((ptr = getenv("HOME")) == NULL) {
		pwent = getpwuid(getuid());
		ptr = pwent->pw_dir;
	}

	snprintf(path, MAXPATHLEN, "%s/%s", ptr, CONFIGFILE);

	if ((fp = fopen(path, "w+")) == NULL)
			return 1;

	for (walker = entries->first; walker; walker = walker->next) {
		entry = (Entry *)walker->data;

		/* write out time */

		if (entry->hh != -1 || entry->mm != -1) {
			if (entry->hh == -1)
				fprintf(fp, " *:");
			else
				fprintf(fp, "%02d:", entry->hh);
			if (entry->mm == -1)
				fprintf(fp, "*  ");
			else
				fprintf(fp, "%02d ", entry->mm);
		}

		/* write out days */

		if ((len = strlen(entry->days))) {
			fprintf(fp, "%s", entry->days);
			for (i = len; i < 8; i++)
				fputc(' ', fp);
		}

		/* write out date */

		if (len && entry->month == -1 && entry->day == -1 && entry->year == -1) {
			fprintf(fp, "*          ");
		} else if (entry->month != -1 || entry->day != -1 || entry->year != -1) {
			if (entry->month == -1)
				fprintf(fp, "*/");
			else
				fprintf(fp, "%02d/", entry->month);
			if (entry->day == -1)
				fprintf(fp, "*/");
			else
				fprintf(fp, "%02d/", entry->day);
			if (entry->year == -1)
				fprintf(fp, "*    ");
			else
				fprintf(fp, "%04d ", entry->year);
			if (entry->month == -1)
				fputc(' ', fp);
			if (entry->day == -1)
				fputc(' ', fp);
		}

		if (entry->message)
			fprintf(fp, "%s", entry->message);

		if (entry->program)
			fprintf(fp, "; %s", entry->program);

		fputc('\n', fp);
	}

	fclose(fp);

	return 0;
}

void
create_interface(Interface *ui)
{
	char buf[64];

	ui->edit_dialog = NULL;

	ui->dialog = XtVaCreatePopupShell("dialog", sessionShellWidgetClass, ui->toplevel,
		NULL);

	ui->form = XtVaCreateManagedWidget("form", formWidgetClass, ui->dialog,
		XtNtop,   XtChainTop,
		XtNleft,  XtChainLeft,
		XtNright, XtChainRight,
		NULL);

	ui->label1 = XtVaCreateManagedWidget("label1", labelWidgetClass, ui->form,
		XtNtop,   XtChainTop,
		XtNleft,  XtChainLeft,
		XtNright, XtChainRight,
		NULL);

	snprintf(buf, sizeof(buf), "RConf v%d.%d", MAJOR_VERSION, MINOR_VERSION);
	XtVaSetValues(ui->label1, XtNlabel, buf, NULL);

	ui->label2 = XtVaCreateManagedWidget("label2", labelWidgetClass, ui->form,
		XtNfromVert, ui->label1,
		XtNleft,     XtChainLeft,
		XtNright,    XtChainRight,
		NULL);

	XtVaSetValues(ui->label2, XtNlabel, "(C) Chris Wareham 1999", NULL);

	ui->subform = XtVaCreateManagedWidget("subform", formWidgetClass, ui->form,
		XtNfromVert,  ui->label2,
		XtNleft,      XtChainLeft,
		XtNright,     XtChainRight,
		XtNresizable, True,
		NULL);

	ui->viewport = XtVaCreateManagedWidget("viewport", viewportWidgetClass, ui->subform,
		XtNtop,        XtChainTop,
		XtNleft,       XtChainLeft,
		XtNright,      XtChainRight,
		XtNallowVert,  TRUE,
		XtNallowHoriz, TRUE,
		XtNforceBars,  TRUE,
		NULL);

	ui->list = XtVaCreateManagedWidget("list", listWidgetClass, ui->viewport,
		XtNverticalList,   TRUE,
		XtNdefaultColumns, 1,
		NULL);

	ui->box = XtVaCreateManagedWidget("box", boxWidgetClass, ui->form,
		XtNfromVert,    ui->subform,
		XtNleft,        XtChainLeft,
		XtNright,       XtChainRight,
		XtNbottom,      XtChainBottom,
		XtNorientation, XtEhorizontal,
		NULL);

	ui->add_button = XtVaCreateManagedWidget("add", commandWidgetClass, ui->box,
		NULL);
	XtAddCallback(ui->add_button, XtNcallback, add_cb, ui);

	ui->modify_button = XtVaCreateManagedWidget("modify", commandWidgetClass, ui->box,
		NULL);
	XtAddCallback(ui->modify_button, XtNcallback, modify_cb, ui);

	ui->delete_button = XtVaCreateManagedWidget("delete", commandWidgetClass, ui->box,
		NULL);
	XtAddCallback(ui->delete_button, XtNcallback, delete_cb, ui);

	ui->close_button = XtVaCreateManagedWidget("close", commandWidgetClass, ui->box,
		NULL);
	XtAddCallback(ui->close_button, XtNcallback, close_cb, ui);
}

void
populate_interface(Interface *ui)
{
	Entry *entry;
	struct list_node *walker;
	char **strings;
	int i = 0, len = list_length(ui->entries);
	Dimension x = 0, y = 0, w = 0, h = 0, bw = 0, w2 = 0;

	strings = calloc(len > 0 ? len : 1, sizeof(char *));

	for (walker = ui->entries->first; walker; walker = walker->next) {
		entry = (Entry *)walker->data;

		strings[i] = format_line(entry);
		i++;
	}

	XtRealizeWidget(ui->dialog);

	XtVaSetValues(ui->list,
		XtNlist,          strings,
		XtNnumberStrings, len,
		NULL);

	/* centre align widgets */

	XtVaGetValues(ui->subform,
		XtNwidth,       &w,
		XtNheight,      &h,
		XtNborderWidth, &bw,
		NULL);
	XtVaGetValues(ui->box,
		XtNwidth, &w2,
		NULL);
	if (w2 != w)
		XtResizeWidget(ui->subform, w2, h, bw);

	XtVaGetValues(ui->viewport,
		XtNx,           &x,
		XtNy,           &y,
		XtNheight,      &h,
		XtNborderWidth, &bw,
		NULL);
	XtConfigureWidget(ui->viewport, x, y, w2 - x - x, h, bw);

	XtVaGetValues(ui->label1,
		XtNwidth,       &w,
		XtNheight,      &h,
		XtNborderWidth, &bw,
		NULL);
	if (w2 != w)
		XtResizeWidget(ui->label1, w2, h, bw);

	XtVaGetValues(ui->label2,
		XtNwidth,       &w,
		XtNheight,      &h,
		XtNborderWidth, &bw,
		NULL);
	if (w2 != w)
		XtResizeWidget(ui->label2, w2, h, bw);

	/* put the whole mess onto the screen */

	XtPopup(ui->dialog, XtGrabNone);
	XMapRaised(XtDisplay(ui->dialog), XtWindow(ui->dialog));
}

char *
format_line(Entry *entry)
{
	int size, i;
	char *line;

	/* length based on:
	 *
	 * 7  bytes for time and subsequent whitespace
	 * 12 bytes for date and subsequent whitespace
	 * 9  bytes for days and subsequent whitespace
	 * n  bytes for message and program
	 * 2  bytes for whitespace between message and program
	 * 1  byte for terminating null
	 */

	size = 31 + (entry->message ? strlen(entry->message) : 0) +
		(entry->program ? strlen(entry->program) : 0);

	line = calloc(size, 1);

	snprintf(line, size, "%02d:%02d  %02d/%02d/%04d  %s", entry->hh, entry->mm,
		entry->month, entry->day, entry->year, entry->days);

	/* time */

	if (entry->hh == -1)
		memset(line, '*', 2);

	if (entry->mm == -1)
		memset(line + 3, '*', 2);

	/* date */

	if (entry->month == -1)
		memset(line + 7, '*', 2);

	if (entry->day == -1)
		memset(line + 10, '*', 2);

	if (entry->year == -1)
		memset(line + 13, '*', 4);

	/* days */

	for (i = strlen(entry->days); i < 7; i++)
		line[19 + i] = ' ';

	/* message */

	if (entry->message && strlen(entry->message)) {
		strcat(line, "  ");
		strcat(line, entry->message);
	}

	/* program */

	if (entry->program && strlen(entry->program)) {
		strcat(line, "  ");
		strcat(line, entry->program);
	}

	return line;
}

void
add_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Interface *ui = (Interface *)client_data;
	int pw, w, h, bw;

	/* if it's the first time through, create the dialog and callbacks */

	if (!ui->edit_dialog) {
		ui->edit_dialog = create_edit_dialog(ui->toplevel);

		XtAddCallback(ui->edit_dialog->ok, XtNcallback, editok_cb,
			ui);

		XtAddCallback(ui->edit_dialog->cancel, XtNcallback, editcancel_cb,
			ui->edit_dialog->dialog);
	}

	ui->selected_entry = -1;

	/* setup dialog strings */

	XtVaSetValues(ui->edit_dialog->time,
		XtNvalue, "**:**",
		NULL);
	XtVaSetValues(ui->edit_dialog->date,
		XtNvalue, "**/**/****",
		NULL);
	XtVaSetValues(ui->edit_dialog->message,
		XtNvalue, "",
		NULL);
	XtVaSetValues(ui->edit_dialog->program,
		XtNvalue, "",
		NULL);

	/* unset all toggles */

	XtVaSetValues(ui->edit_dialog->mon, XtNstate, FALSE, NULL);
	XtVaSetValues(ui->edit_dialog->tue, XtNstate, FALSE, NULL);
	XtVaSetValues(ui->edit_dialog->wed, XtNstate, FALSE, NULL);
	XtVaSetValues(ui->edit_dialog->thu, XtNstate, FALSE, NULL);
	XtVaSetValues(ui->edit_dialog->fri, XtNstate, FALSE, NULL);
	XtVaSetValues(ui->edit_dialog->sat, XtNstate, FALSE, NULL);
	XtVaSetValues(ui->edit_dialog->sun, XtNstate, FALSE, NULL);

	/* centre align label */

	XtRealizeWidget(ui->edit_dialog->dialog);

	XtVaGetValues(ui->edit_dialog->form,
		XtNwidth, &pw,
		NULL);

	XtVaGetValues(ui->edit_dialog->label,
		XtNwidth,       &w,
		XtNheight,      &h,
		XtNborderWidth, &bw,
		NULL);

	if (w != pw)
		XtResizeWidget(ui->edit_dialog->label, pw, h, bw);

	/* put the whole mess onto the screen */

	XtPopup(ui->edit_dialog->dialog, XtGrabExclusive);
	XMapRaised(XtDisplay(ui->edit_dialog->dialog),
		XtWindow(ui->edit_dialog->dialog));
}

void
modify_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	XawListReturnStruct *list_selection;
	Interface *ui = (Interface *)client_data;
	Entry *entry;
	char time[6], date[11];
	int pw, w, h, bw;

	/* ensure something's selected in the list widget */

	list_selection = XawListShowCurrent(ui->list);
	ui->selected_entry = list_selection->list_index;

	if (ui->selected_entry < 0) {
		message_dialog(ui->dialog, "Please select an entry to modify",
			warning_cb, NULL);
		return;
	}

	entry = list_get(ui->entries, ui->selected_entry);

	/* if it's the first time through, create the dialog and callbacks */

	if (!ui->edit_dialog) {
		ui->edit_dialog = create_edit_dialog(ui->toplevel);

		XtAddCallback(ui->edit_dialog->ok, XtNcallback, editok_cb,
			ui);

		XtAddCallback(ui->edit_dialog->cancel, XtNcallback, editcancel_cb,
			ui->edit_dialog->dialog);
	}

	/* setup dialog strings */

	if (entry->hh == -1 && entry->mm == -1)
		strcpy(time, "**:**");
	else if (entry->hh == -1)
		snprintf(time, sizeof(time), "**:%02d", entry->mm);
	else if (entry->mm == -1)
		snprintf(time, sizeof(time), "%02d:**", entry->hh);
	else
		snprintf(time, sizeof(time), "%02d:%02d", entry->hh, entry->mm);

	if (entry->month == -1 && entry->day == -1 && entry->year == -1)
		strcpy(date, "**/**/****");
	else if (entry->month == -1 && entry->day != -1 && entry->year != -1)
		snprintf(date, sizeof(date), "**/%02d/%04d", entry->day, entry->year);
	else if (entry->month != -1 && entry->day == -1 && entry->year != -1)
		snprintf(date, sizeof(date), "%02d/**/%04d", entry->month, entry->year);
	else if (entry->month != -1 && entry->day != -1 && entry->year == -1)
		snprintf(date, sizeof(date), "%02d/%02d/****", entry->month, entry->day);
	else if (entry->month != -1 && entry->day == -1 && entry->year == -1)
		snprintf(date, sizeof(date), "%02d/**/****", entry->month);
	else if (entry->month == -1 && entry->day != -1 && entry->year == -1)
		snprintf(date, sizeof(date), "**/%02d/****", entry->day);
	else if (entry->month == -1 && entry->day == -1 && entry->year != -1)
		snprintf(date, sizeof(date), "**/**/%04d", entry->year);
	else
		snprintf(date, sizeof(date), "%02d/%02d/%04d", entry->month, entry->day, entry->year);

	XtVaSetValues(ui->edit_dialog->time,
		XtNvalue, time,
		NULL);
	XtVaSetValues(ui->edit_dialog->date,
		XtNvalue, date,
		NULL);
	XtVaSetValues(ui->edit_dialog->message,
		XtNvalue, entry->message ? entry->message : "",
		NULL);
	XtVaSetValues(ui->edit_dialog->program,
		XtNvalue, entry->program ? entry->program : "",
		NULL);

	if (strchr(entry->days, '*')) {
		XtVaSetValues(ui->edit_dialog->mon, XtNstate, TRUE, NULL);
		XtVaSetValues(ui->edit_dialog->tue, XtNstate, TRUE, NULL);
		XtVaSetValues(ui->edit_dialog->wed, XtNstate, TRUE, NULL);
		XtVaSetValues(ui->edit_dialog->thu, XtNstate, TRUE, NULL);
		XtVaSetValues(ui->edit_dialog->fri, XtNstate, TRUE, NULL);
		XtVaSetValues(ui->edit_dialog->sat, XtNstate, TRUE, NULL);
		XtVaSetValues(ui->edit_dialog->sun, XtNstate, TRUE, NULL);
	} else {
		XtVaSetValues(ui->edit_dialog->mon, XtNstate, FALSE, NULL);
		XtVaSetValues(ui->edit_dialog->tue, XtNstate, FALSE, NULL);
		XtVaSetValues(ui->edit_dialog->wed, XtNstate, FALSE, NULL);
		XtVaSetValues(ui->edit_dialog->thu, XtNstate, FALSE, NULL);
		XtVaSetValues(ui->edit_dialog->fri, XtNstate, FALSE, NULL);
		XtVaSetValues(ui->edit_dialog->sat, XtNstate, FALSE, NULL);
		XtVaSetValues(ui->edit_dialog->sun, XtNstate, FALSE, NULL);

		if (strchr(entry->days, 'm'))
			XtVaSetValues(ui->edit_dialog->mon, XtNstate, TRUE, NULL);
		if (strchr(entry->days, 't'))
			XtVaSetValues(ui->edit_dialog->tue, XtNstate, TRUE, NULL);
		if (strchr(entry->days, 'w'))
			XtVaSetValues(ui->edit_dialog->wed, XtNstate, TRUE, NULL);
		if (strchr(entry->days, 'r'))
			XtVaSetValues(ui->edit_dialog->thu, XtNstate, TRUE, NULL);
		if (strchr(entry->days, 'f'))
			XtVaSetValues(ui->edit_dialog->fri, XtNstate, TRUE, NULL);
		if (strchr(entry->days, 's'))
			XtVaSetValues(ui->edit_dialog->sat, XtNstate, TRUE, NULL);
		if (strchr(entry->days, 'u'))
			XtVaSetValues(ui->edit_dialog->sun, XtNstate, TRUE, NULL);
	}

	/* centre align label */

	XtRealizeWidget(ui->edit_dialog->dialog);

	XtVaGetValues(ui->edit_dialog->form,
		XtNwidth, &pw,
		NULL);

	XtVaGetValues(ui->edit_dialog->label,
		XtNwidth,       &w,
		XtNheight,      &h,
		XtNborderWidth, &bw,
		NULL);

	if (w != pw)
		XtResizeWidget(ui->edit_dialog->label, pw, h, bw);

	/* put the whole mess onto the screen */

	XtPopup(ui->edit_dialog->dialog, XtGrabExclusive);
	XMapRaised(XtDisplay(ui->edit_dialog->dialog),
		XtWindow(ui->edit_dialog->dialog));
}

void
editok_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Interface *ui = (Interface *)client_data;
	Boolean toggle_state;
	Entry *entry;
	char *ptr, *message, *program, days[8], **strings;
	int found_time, hh, mm, month, day, year, i;
	struct list_node *walker;

	/* validate user input - a program or message is required */

	message = program = NULL;

	if ((ptr = get_text(ui->edit_dialog->message)) && strlen(ptr))
		message = ptr;

	if ((ptr = get_text(ui->edit_dialog->program)) && strlen(ptr))
		program = ptr;

	if (message == NULL && program == NULL) {
		message_dialog(ui->edit_dialog->dialog, "Please enter a message and/or program",
			warning_cb, NULL);
		return;
	}

	/* validate time */

	found_time = FALSE;
	hh = mm = -1;

	if ((ptr = get_text(ui->edit_dialog->time)) && strlen(ptr)) {
		if (strcmp(ptr, "**:**") == 0) {
			hh = mm = -1;
		} else if (sscanf(ptr, "%d:%d", &hh, &mm) == 2) {
			found_time = TRUE;
		} else if (sscanf(ptr, "%d:**", &hh) == 1) {
			found_time = TRUE;
		} else if (sscanf(ptr, "**:%d", &mm) == 1) {
			found_time = TRUE;
		} else {
			message_dialog(ui->edit_dialog->dialog, "Please enter a valid time",
				warning_cb, NULL);
			return;
		}

		if (found_time && (hh > 23 || hh < -1 || mm > 59 || mm < -1)) {
			message_dialog(ui->edit_dialog->dialog, "Please enter a valid time",
				warning_cb, NULL);
			return;
		}
	}

	/* validate date */

	month = day = year = -1;

	if ((ptr = get_text(ui->edit_dialog->date)) && strlen(ptr)) {
		if (sscanf(ptr, "%d/%d/%d ", &month, &day, &year) == 3) {
			/* fully specified date */
		} else if (sscanf(ptr, "**/%d/%d", &day, &year) == 2) {
			month = -1;
		} else if (sscanf(ptr, "%d/**/%d", &month, &year) == 2) {
			day = -1;
		} else if (sscanf(ptr, "%d/%d/**", &month, &day) == 2) {
			year = -1;
		} else if (sscanf(ptr, "%d/**/**", &month) == 1) {
			day = year = -1;
		} else if (sscanf(ptr, "**/%d/**", &day) == 1) {
			month = year = -1;
		} else if (sscanf(ptr, "**/**/%d", &year) == 1) {
			month = day = -1;
		} else if (strcmp(ptr, "**/**/****") == 0) {
			day = month = year = -1;
		} else {
			message_dialog(ui->edit_dialog->dialog, "Please enter a valid date",
				warning_cb, NULL);
			return;
		}

		if (month > 12 || month < -1 || day > 31 || day < -1 || year > 9999 || year < -1) {
			message_dialog(ui->edit_dialog->dialog, "Please enter a valid date",
				warning_cb, NULL);
			return;
		}
	}

	/* validate days */

	i = 0;
	XtVaGetValues(ui->edit_dialog->mon, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 'm';
	XtVaGetValues(ui->edit_dialog->tue, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 't';
	XtVaGetValues(ui->edit_dialog->wed, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 'w';
	XtVaGetValues(ui->edit_dialog->thu, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 'r';
	XtVaGetValues(ui->edit_dialog->fri, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 'f';
	XtVaGetValues(ui->edit_dialog->sat, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 's';
	XtVaGetValues(ui->edit_dialog->sun, XtNstate, &toggle_state, NULL);
	if (toggle_state)
		days[i++] = 'u';
	days[i] = '\0';
	if (strcmp(days, "mtwrfsu") == 0)
		strcpy(days, "*");

#ifdef DEBUG
	printf("\nEDITED ENTRY\n");
	printf("Time    : %02d:%02d\n", hh, mm);
	printf("Days    : %s\n", strlen(days) ? days : "-");
	printf("Date    : %02d:%02d:%d\n", month, day, year);
	printf("Message : %s\n", message);
	printf("Program : %s\n", program);
#endif /* DEBUG */

	if (ui->selected_entry == -1) {
		entry = calloc(1, sizeof(Entry));
		list_add(ui->entries, entry);
	} else {
		entry = list_get(ui->entries, ui->selected_entry);
		if (entry->message)
			free(entry->message);
		if (entry->program)
			free(entry->program);
	}

	entry->hh = hh;
	entry->mm = mm;
	entry->month = month;
	entry->day = day;
	entry->year = year;
	strcpy(entry->days, days);
	entry->message = message ? strdup(message) : NULL;
	entry->program = program ? strdup(program) : NULL;

	strings = calloc(sizeof(char *), list_length(ui->entries));

	for (i = 0, walker = ui->entries->first; walker; walker = walker->next) {
		entry = (Entry *)walker->data;

		strings[i] = format_line(entry);
		i++;
	}

	XawListChange(ui->list, strings, i, 0, FALSE);
	if (ui->selected_entry == -1)
		XawListHighlight(ui->list, list_length(ui->entries) - 1);

	ui->entries_changed = TRUE;

	XtPopdown(ui->edit_dialog->dialog);
}

void
editcancel_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Widget dialog = (Widget)client_data;
	XtPopdown(dialog);
}

void
delete_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	XawListReturnStruct *list_selection;
	Interface *ui = (Interface *)client_data;

	/* ensure something's selected in the list widget */

	list_selection = XawListShowCurrent(ui->list);
	ui->selected_entry = list_selection->list_index;

	if (ui->selected_entry < 0) {
		message_dialog(ui->dialog, "Please select an entry to delete",
			warning_cb, NULL);
		return;
	}

	confirm_dialog(ui->dialog, "Are you sure you want to delete this entry?",
		deleteyes_cb, deleteno_cb, ui);
}

void
deleteyes_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Interface *ui = (Interface *)client_data;
	char **strings;
	int i;
	struct list_node *walker;
	Entry *entry;

	entry = list_remove(ui->entries, ui->selected_entry);

	if (entry->program)
		free(entry->program);
	if (entry->message)
		free(entry->message);

	free(entry);

	strings = calloc(sizeof(char *), list_length(ui->entries));

	for (i = 0, walker = ui->entries->first; walker; walker = walker->next) {
		entry = (Entry *)walker->data;

		strings[i] = format_line(entry);
		i++;
	}

	XawListChange(ui->list, strings, i, 0, FALSE);
	XawListHighlight(ui->list, list_length(ui->entries) - 1);

	ui->entries_changed = TRUE;

	destroy_dialog(widget);
}

void
deleteno_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	destroy_dialog(widget);
}

void
close_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Interface *ui = (Interface *)client_data;
	Entry *entry;
	struct list_node *walker;

	if (ui->entries_changed)
		write_config(ui->entries);

	for (walker = ui->entries->first; walker; walker = walker->next) {
		entry = (Entry *)walker->data;

		if (entry->program)
			free(entry->program);
		if (entry->message)
			free(entry->message);

		free(entry);
	}

	list_free(ui->entries);

	exit(0); 
}

void
warning_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	destroy_dialog(widget);
}

void
error_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	destroy_dialog(widget);
	exit(1);
}

char *
get_text(Widget widget)
{
	char *str;

	if (XtIsSubclass(widget, dialogWidgetClass))
		XtVaGetValues(widget, XtNvalue, &str, NULL);
	else
		str = NULL;

	return str;
}
