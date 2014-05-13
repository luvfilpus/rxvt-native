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
#include "list.h"

List *
list_init(void (*delete_func)(void *))
{
	List *list;

	list = calloc(1, sizeof(List));
	list->delete_func = delete_func;

	return list;
}

int
list_length(List *list)
{
	if (list == NULL)
		return 0;

	return list->length;
}

int
list_add(List *list, void *data)
{
	struct list_node *node;

	if (list == NULL || data == NULL)
		return 1;

	node = calloc(1, sizeof(struct list_node));
	if (node == NULL)
		return 1;

	node->data = data;

	if (list->length == 0) {
		list->first = node;
		list->last = node;
	} else {
		list->last->next = node;
		list->last = node;
	}

	list->length++;

	return 0;
}

void *
list_remove(List *list, int i)
{
	struct list_node *curr_node, *next_node;
	void *data;

	if (list == NULL || i >= list->length)
		return NULL;

	if (i == 0) {
		curr_node = list->first;
		list->first = curr_node->next;
		if (list->length == 1)
			list->last = NULL;
	} else {
		for (curr_node = list->first; i > 1; i--)
			curr_node = curr_node->next;
		next_node = curr_node->next;
		curr_node->next = next_node->next;
		if (next_node->next == NULL)
			list->last = curr_node;
		curr_node = next_node;
	}

	data = curr_node->data;
	free(curr_node);

	list->length--;

	return data;
}

void *
list_get(List *list, int i)
{
	struct list_node *node;

	if (list == NULL || i >= list->length)
		return NULL;

	for (node = list->first; i; i--)
		node = node->next;

	return node->data;
}

void
list_free(List *list)
{
	struct list_node *curr_node, *next_node;

	if (list == NULL)
		return;

	curr_node = list->first;
	while (curr_node) {
		next_node = curr_node->next;
		if (list->delete_func != NULL)
			list->delete_func(curr_node->data);
		free(curr_node);
		curr_node = next_node;
	}

	free(list);
}
