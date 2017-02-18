/*
Copyright (c) 2017 Oskari Timperi

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/**********************************************************************/
/*                              config.h                              */
/**********************************************************************/

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

#if __STDC_VERSION__ < 199901L
#define MQTT_INLINE __inline
#else
#define MQTT_INLINE inline
#endif

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#endif

/**********************************************************************/
/*                              win32.h                               */
/**********************************************************************/

#ifndef MQTT_WIN32_H
#define MQTT_WIN32_H

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

#endif

/**********************************************************************/
/*                              queue.h                               */
/**********************************************************************/

/*	$NetBSD: queue.h,v 1.68.2.1 2015/12/27 12:10:18 skrll Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)queue.h	8.5 (Berkeley) 8/20/94
 */

#ifndef	_SYS_QUEUE_H_
#define	_SYS_QUEUE_H_

/*
 * This file defines five types of data structures: singly-linked lists,
 * lists, simple queues, tail queues, and circular queues.
 *
 * A singly-linked list is headed by a single forward pointer. The
 * elements are singly linked for minimum space and pointer manipulation
 * overhead at the expense of O(n) removal for arbitrary elements. New
 * elements can be added to the list after an existing element or at the
 * head of the list.  Elements being removed from the head of the list
 * should use the explicit macro for this purpose for optimum
 * efficiency. A singly-linked list may only be traversed in the forward
 * direction.  Singly-linked lists are ideal for applications with large
 * datasets and few or no removals or for implementing a LIFO queue.
 *
 * A list is headed by a single forward pointer (or an array of forward
 * pointers for a hash table header). The elements are doubly linked
 * so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before
 * or after an existing element or at the head of the list. A list
 * may only be traversed in the forward direction.
 *
 * A simple queue is headed by a pair of pointers, one the head of the
 * list and the other to the tail of the list. The elements are singly
 * linked to save space, so elements can only be removed from the
 * head of the list. New elements can be added to the list after
 * an existing element, at the head of the list, or at the end of the
 * list. A simple queue may only be traversed in the forward direction.
 *
 * A tail queue is headed by a pair of pointers, one to the head of the
 * list and the other to the tail of the list. The elements are doubly
 * linked so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before or
 * after an existing element, at the head of the list, or at the end of
 * the list. A tail queue may be traversed in either direction.
 *
 * A circle queue is headed by a pair of pointers, one to the head of the
 * list and the other to the tail of the list. The elements are doubly
 * linked so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before or after
 * an existing element, at the head of the list, or at the end of the list.
 * A circle queue may be traversed in either direction, but has a more
 * complex end of list detection.
 *
 * For details on the use of these macros, see the queue(3) manual page.
 */

/*
 * Include the definition of NULL only on NetBSD because sys/null.h
 * is not available elsewhere.  This conditional makes the header
 * portable and it can simply be dropped verbatim into any system.
 * The caveat is that on other systems some other header
 * must provide NULL before the macros can be used.
 */
#ifdef __NetBSD__
#include <sys/null.h>
#endif

#if defined(QUEUEDEBUG)
# if defined(_KERNEL)
#  define QUEUEDEBUG_ABORT(...) panic(__VA_ARGS__)
# else
#  include <err.h>
#  define QUEUEDEBUG_ABORT(...) err(1, __VA_ARGS__)
# endif
#endif

#if 0
/*
 * Singly-linked List definitions.
 */
#define	SLIST_HEAD(name, type)						\
struct name {								\
	struct type *slh_first;	/* first element */			\
}

#define	SLIST_HEAD_INITIALIZER(head)					\
	{ NULL }

#define	SLIST_ENTRY(type)						\
struct {								\
	struct type *sle_next;	/* next element */			\
}

/*
 * Singly-linked List access methods.
 */
#define	SLIST_FIRST(head)	((head)->slh_first)
#define	SLIST_END(head)		NULL
#define	SLIST_EMPTY(head)	((head)->slh_first == NULL)
#define	SLIST_NEXT(elm, field)	((elm)->field.sle_next)

#define	SLIST_FOREACH(var, head, field)					\
	for((var) = (head)->slh_first;					\
	    (var) != SLIST_END(head);					\
	    (var) = (var)->field.sle_next)

#define	SLIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = SLIST_FIRST((head));				\
	    (var) != SLIST_END(head) &&					\
	    ((tvar) = SLIST_NEXT((var), field), 1);			\
	    (var) = (tvar))

/*
 * Singly-linked List functions.
 */
#define	SLIST_INIT(head) do {						\
	(head)->slh_first = SLIST_END(head);				\
} while (/*CONSTCOND*/0)

#define	SLIST_INSERT_AFTER(slistelm, elm, field) do {			\
	(elm)->field.sle_next = (slistelm)->field.sle_next;		\
	(slistelm)->field.sle_next = (elm);				\
} while (/*CONSTCOND*/0)

#define	SLIST_INSERT_HEAD(head, elm, field) do {			\
	(elm)->field.sle_next = (head)->slh_first;			\
	(head)->slh_first = (elm);					\
} while (/*CONSTCOND*/0)

#define	SLIST_REMOVE_AFTER(slistelm, field) do {			\
	(slistelm)->field.sle_next =					\
	    SLIST_NEXT(SLIST_NEXT((slistelm), field), field);		\
} while (/*CONSTCOND*/0)

#define	SLIST_REMOVE_HEAD(head, field) do {				\
	(head)->slh_first = (head)->slh_first->field.sle_next;		\
} while (/*CONSTCOND*/0)

#define	SLIST_REMOVE(head, elm, type, field) do {			\
	if ((head)->slh_first == (elm)) {				\
		SLIST_REMOVE_HEAD((head), field);			\
	}								\
	else {								\
		struct type *curelm = (head)->slh_first;		\
		while(curelm->field.sle_next != (elm))			\
			curelm = curelm->field.sle_next;		\
		curelm->field.sle_next =				\
		    curelm->field.sle_next->field.sle_next;		\
	}								\
} while (/*CONSTCOND*/0)


/*
 * List definitions.
 */
#define	LIST_HEAD(name, type)						\
struct name {								\
	struct type *lh_first;	/* first element */			\
}

#define	LIST_HEAD_INITIALIZER(head)					\
	{ NULL }

#define	LIST_ENTRY(type)						\
struct {								\
	struct type *le_next;	/* next element */			\
	struct type **le_prev;	/* address of previous next element */	\
}

/*
 * List access methods.
 */
#define	LIST_FIRST(head)		((head)->lh_first)
#define	LIST_END(head)			NULL
#define	LIST_EMPTY(head)		((head)->lh_first == LIST_END(head))
#define	LIST_NEXT(elm, field)		((elm)->field.le_next)

#define	LIST_FOREACH(var, head, field)					\
	for ((var) = ((head)->lh_first);				\
	    (var) != LIST_END(head);					\
	    (var) = ((var)->field.le_next))

#define	LIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = LIST_FIRST((head));				\
	    (var) != LIST_END(head) &&					\
	    ((tvar) = LIST_NEXT((var), field), 1);			\
	    (var) = (tvar))

#define	LIST_MOVE(head1, head2) do {					\
	LIST_INIT((head2));						\
	if (!LIST_EMPTY((head1))) {					\
		(head2)->lh_first = (head1)->lh_first;			\
		LIST_INIT((head1));					\
	}								\
} while (/*CONSTCOND*/0)

/*
 * List functions.
 */
#if defined(QUEUEDEBUG)
#define	QUEUEDEBUG_LIST_INSERT_HEAD(head, elm, field)			\
	if ((head)->lh_first &&						\
	    (head)->lh_first->field.le_prev != &(head)->lh_first)	\
		QUEUEDEBUG_ABORT("LIST_INSERT_HEAD %p %s:%d", (head),	\
		    __FILE__, __LINE__);
#define	QUEUEDEBUG_LIST_OP(elm, field)					\
	if ((elm)->field.le_next &&					\
	    (elm)->field.le_next->field.le_prev !=			\
	    &(elm)->field.le_next)					\
		QUEUEDEBUG_ABORT("LIST_* forw %p %s:%d", (elm),		\
		    __FILE__, __LINE__);				\
	if (*(elm)->field.le_prev != (elm))				\
		QUEUEDEBUG_ABORT("LIST_* back %p %s:%d", (elm),		\
		    __FILE__, __LINE__);
#define	QUEUEDEBUG_LIST_POSTREMOVE(elm, field)				\
	(elm)->field.le_next = (void *)1L;				\
	(elm)->field.le_prev = (void *)1L;
#else
#define	QUEUEDEBUG_LIST_INSERT_HEAD(head, elm, field)
#define	QUEUEDEBUG_LIST_OP(elm, field)
#define	QUEUEDEBUG_LIST_POSTREMOVE(elm, field)
#endif

#define	LIST_INIT(head) do {						\
	(head)->lh_first = LIST_END(head);				\
} while (/*CONSTCOND*/0)

#define	LIST_INSERT_AFTER(listelm, elm, field) do {			\
	QUEUEDEBUG_LIST_OP((listelm), field)				\
	if (((elm)->field.le_next = (listelm)->field.le_next) != 	\
	    LIST_END(head))						\
		(listelm)->field.le_next->field.le_prev =		\
		    &(elm)->field.le_next;				\
	(listelm)->field.le_next = (elm);				\
	(elm)->field.le_prev = &(listelm)->field.le_next;		\
} while (/*CONSTCOND*/0)

#define	LIST_INSERT_BEFORE(listelm, elm, field) do {			\
	QUEUEDEBUG_LIST_OP((listelm), field)				\
	(elm)->field.le_prev = (listelm)->field.le_prev;		\
	(elm)->field.le_next = (listelm);				\
	*(listelm)->field.le_prev = (elm);				\
	(listelm)->field.le_prev = &(elm)->field.le_next;		\
} while (/*CONSTCOND*/0)

#define	LIST_INSERT_HEAD(head, elm, field) do {				\
	QUEUEDEBUG_LIST_INSERT_HEAD((head), (elm), field)		\
	if (((elm)->field.le_next = (head)->lh_first) != LIST_END(head))\
		(head)->lh_first->field.le_prev = &(elm)->field.le_next;\
	(head)->lh_first = (elm);					\
	(elm)->field.le_prev = &(head)->lh_first;			\
} while (/*CONSTCOND*/0)

#define	LIST_REMOVE(elm, field) do {					\
	QUEUEDEBUG_LIST_OP((elm), field)				\
	if ((elm)->field.le_next != NULL)				\
		(elm)->field.le_next->field.le_prev = 			\
		    (elm)->field.le_prev;				\
	*(elm)->field.le_prev = (elm)->field.le_next;			\
	QUEUEDEBUG_LIST_POSTREMOVE((elm), field)			\
} while (/*CONSTCOND*/0)

#define LIST_REPLACE(elm, elm2, field) do {				\
	if (((elm2)->field.le_next = (elm)->field.le_next) != NULL)	\
		(elm2)->field.le_next->field.le_prev =			\
		    &(elm2)->field.le_next;				\
	(elm2)->field.le_prev = (elm)->field.le_prev;			\
	*(elm2)->field.le_prev = (elm2);				\
	QUEUEDEBUG_LIST_POSTREMOVE((elm), field)			\
} while (/*CONSTCOND*/0)
#endif /* 0 */

/*
 * Simple queue definitions.
 */
#define	SIMPLEQ_HEAD(name, type)					\
struct name {								\
	struct type *sqh_first;	/* first element */			\
	struct type **sqh_last;	/* addr of last next element */		\
}

#define	SIMPLEQ_HEAD_INITIALIZER(head)					\
	{ NULL, &(head).sqh_first }

#define	SIMPLEQ_ENTRY(type)						\
struct {								\
	struct type *sqe_next;	/* next element */			\
}

/*
 * Simple queue access methods.
 */
#define	SIMPLEQ_FIRST(head)		((head)->sqh_first)
#define	SIMPLEQ_END(head)		NULL
#define	SIMPLEQ_EMPTY(head)		((head)->sqh_first == SIMPLEQ_END(head))
#define	SIMPLEQ_NEXT(elm, field)	((elm)->field.sqe_next)

#define	SIMPLEQ_FOREACH(var, head, field)				\
	for ((var) = ((head)->sqh_first);				\
	    (var) != SIMPLEQ_END(head);					\
	    (var) = ((var)->field.sqe_next))

#define	SIMPLEQ_FOREACH_SAFE(var, head, field, next)			\
	for ((var) = ((head)->sqh_first);				\
	    (var) != SIMPLEQ_END(head) &&				\
	    ((next = ((var)->field.sqe_next)), 1);			\
	    (var) = (next))

/*
 * Simple queue functions.
 */
#define	SIMPLEQ_INIT(head) do {						\
	(head)->sqh_first = NULL;					\
	(head)->sqh_last = &(head)->sqh_first;				\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_INSERT_HEAD(head, elm, field) do {			\
	if (((elm)->field.sqe_next = (head)->sqh_first) == NULL)	\
		(head)->sqh_last = &(elm)->field.sqe_next;		\
	(head)->sqh_first = (elm);					\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.sqe_next = NULL;					\
	*(head)->sqh_last = (elm);					\
	(head)->sqh_last = &(elm)->field.sqe_next;			\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	if (((elm)->field.sqe_next = (listelm)->field.sqe_next) == NULL)\
		(head)->sqh_last = &(elm)->field.sqe_next;		\
	(listelm)->field.sqe_next = (elm);				\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_REMOVE_HEAD(head, field) do {				\
	if (((head)->sqh_first = (head)->sqh_first->field.sqe_next) == NULL) \
		(head)->sqh_last = &(head)->sqh_first;			\
} while (/*CONSTCOND*/0)

#define SIMPLEQ_REMOVE_AFTER(head, elm, field) do {			\
	if (((elm)->field.sqe_next = (elm)->field.sqe_next->field.sqe_next) \
	    == NULL)							\
		(head)->sqh_last = &(elm)->field.sqe_next;		\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_REMOVE(head, elm, type, field) do {			\
	if ((head)->sqh_first == (elm)) {				\
		SIMPLEQ_REMOVE_HEAD((head), field);			\
	} else {							\
		struct type *curelm = (head)->sqh_first;		\
		while (curelm->field.sqe_next != (elm))			\
			curelm = curelm->field.sqe_next;		\
		if ((curelm->field.sqe_next =				\
			curelm->field.sqe_next->field.sqe_next) == NULL) \
			    (head)->sqh_last = &(curelm)->field.sqe_next; \
	}								\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_CONCAT(head1, head2) do {				\
	if (!SIMPLEQ_EMPTY((head2))) {					\
		*(head1)->sqh_last = (head2)->sqh_first;		\
		(head1)->sqh_last = (head2)->sqh_last;		\
		SIMPLEQ_INIT((head2));					\
	}								\
} while (/*CONSTCOND*/0)

#define	SIMPLEQ_LAST(head, type, field)					\
	(SIMPLEQ_EMPTY((head)) ?						\
		NULL :							\
	        ((struct type *)(void *)				\
		((char *)((head)->sqh_last) - offsetof(struct type, field))))

/*
 * Tail queue definitions.
 */
#define	_TAILQ_HEAD(name, type, qual)					\
struct name {								\
	qual type *tqh_first;		/* first element */		\
	qual type *qual *tqh_last;	/* addr of last next element */	\
}
#define TAILQ_HEAD(name, type)	_TAILQ_HEAD(name, struct type,)

#define	TAILQ_HEAD_INITIALIZER(head)					\
	{ TAILQ_END(head), &(head).tqh_first }

#define	_TAILQ_ENTRY(type, qual)					\
struct {								\
	qual type *tqe_next;		/* next element */		\
	qual type *qual *tqe_prev;	/* address of previous next element */\
}
#define TAILQ_ENTRY(type)	_TAILQ_ENTRY(struct type,)

/*
 * Tail queue access methods.
 */
#define	TAILQ_FIRST(head)		((head)->tqh_first)
#define	TAILQ_END(head)			(NULL)
#define	TAILQ_NEXT(elm, field)		((elm)->field.tqe_next)
#define	TAILQ_LAST(head, headname) \
	(*(((struct headname *)(void *)((head)->tqh_last))->tqh_last))
#define	TAILQ_PREV(elm, headname, field) \
	(*(((struct headname *)(void *)((elm)->field.tqe_prev))->tqh_last))
#define	TAILQ_EMPTY(head)		(TAILQ_FIRST(head) == TAILQ_END(head))


#define	TAILQ_FOREACH(var, head, field)					\
	for ((var) = ((head)->tqh_first);				\
	    (var) != TAILQ_END(head);					\
	    (var) = ((var)->field.tqe_next))

#define	TAILQ_FOREACH_SAFE(var, head, field, next)			\
	for ((var) = ((head)->tqh_first);				\
	    (var) != TAILQ_END(head) &&					\
	    ((next) = TAILQ_NEXT(var, field), 1); (var) = (next))

#define	TAILQ_FOREACH_REVERSE(var, head, headname, field)		\
	for ((var) = TAILQ_LAST((head), headname);			\
	    (var) != TAILQ_END(head);					\
	    (var) = TAILQ_PREV((var), headname, field))

#define	TAILQ_FOREACH_REVERSE_SAFE(var, head, headname, field, prev)	\
	for ((var) = TAILQ_LAST((head), headname);			\
	    (var) != TAILQ_END(head) && 				\
	    ((prev) = TAILQ_PREV((var), headname, field), 1); (var) = (prev))

/*
 * Tail queue functions.
 */
#if defined(QUEUEDEBUG)
#define	QUEUEDEBUG_TAILQ_INSERT_HEAD(head, elm, field)			\
	if ((head)->tqh_first &&					\
	    (head)->tqh_first->field.tqe_prev != &(head)->tqh_first)	\
		QUEUEDEBUG_ABORT("TAILQ_INSERT_HEAD %p %s:%d", (head),	\
		    __FILE__, __LINE__);
#define	QUEUEDEBUG_TAILQ_INSERT_TAIL(head, elm, field)			\
	if (*(head)->tqh_last != NULL)					\
		QUEUEDEBUG_ABORT("TAILQ_INSERT_TAIL %p %s:%d", (head),	\
		    __FILE__, __LINE__);
#define	QUEUEDEBUG_TAILQ_OP(elm, field)					\
	if ((elm)->field.tqe_next &&					\
	    (elm)->field.tqe_next->field.tqe_prev !=			\
	    &(elm)->field.tqe_next)					\
		QUEUEDEBUG_ABORT("TAILQ_* forw %p %s:%d", (elm),	\
		    __FILE__, __LINE__);				\
	if (*(elm)->field.tqe_prev != (elm))				\
		QUEUEDEBUG_ABORT("TAILQ_* back %p %s:%d", (elm),	\
		    __FILE__, __LINE__);
#define	QUEUEDEBUG_TAILQ_PREREMOVE(head, elm, field)			\
	if ((elm)->field.tqe_next == NULL &&				\
	    (head)->tqh_last != &(elm)->field.tqe_next)			\
		QUEUEDEBUG_ABORT("TAILQ_PREREMOVE head %p elm %p %s:%d",\
		    (head), (elm), __FILE__, __LINE__);
#define	QUEUEDEBUG_TAILQ_POSTREMOVE(elm, field)				\
	(elm)->field.tqe_next = (void *)1L;				\
	(elm)->field.tqe_prev = (void *)1L;
#else
#define	QUEUEDEBUG_TAILQ_INSERT_HEAD(head, elm, field)
#define	QUEUEDEBUG_TAILQ_INSERT_TAIL(head, elm, field)
#define	QUEUEDEBUG_TAILQ_OP(elm, field)
#define	QUEUEDEBUG_TAILQ_PREREMOVE(head, elm, field)
#define	QUEUEDEBUG_TAILQ_POSTREMOVE(elm, field)
#endif

#define	TAILQ_INIT(head) do {						\
	(head)->tqh_first = TAILQ_END(head);				\
	(head)->tqh_last = &(head)->tqh_first;				\
} while (/*CONSTCOND*/0)

#define	TAILQ_INSERT_HEAD(head, elm, field) do {			\
	QUEUEDEBUG_TAILQ_INSERT_HEAD((head), (elm), field)		\
	if (((elm)->field.tqe_next = (head)->tqh_first) != TAILQ_END(head))\
		(head)->tqh_first->field.tqe_prev =			\
		    &(elm)->field.tqe_next;				\
	else								\
		(head)->tqh_last = &(elm)->field.tqe_next;		\
	(head)->tqh_first = (elm);					\
	(elm)->field.tqe_prev = &(head)->tqh_first;			\
} while (/*CONSTCOND*/0)

#define	TAILQ_INSERT_TAIL(head, elm, field) do {			\
	QUEUEDEBUG_TAILQ_INSERT_TAIL((head), (elm), field)		\
	(elm)->field.tqe_next = TAILQ_END(head);			\
	(elm)->field.tqe_prev = (head)->tqh_last;			\
	*(head)->tqh_last = (elm);					\
	(head)->tqh_last = &(elm)->field.tqe_next;			\
} while (/*CONSTCOND*/0)

#define	TAILQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	QUEUEDEBUG_TAILQ_OP((listelm), field)				\
	if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != 	\
	    TAILQ_END(head))						\
		(elm)->field.tqe_next->field.tqe_prev = 		\
		    &(elm)->field.tqe_next;				\
	else								\
		(head)->tqh_last = &(elm)->field.tqe_next;		\
	(listelm)->field.tqe_next = (elm);				\
	(elm)->field.tqe_prev = &(listelm)->field.tqe_next;		\
} while (/*CONSTCOND*/0)

#define	TAILQ_INSERT_BEFORE(listelm, elm, field) do {			\
	QUEUEDEBUG_TAILQ_OP((listelm), field)				\
	(elm)->field.tqe_prev = (listelm)->field.tqe_prev;		\
	(elm)->field.tqe_next = (listelm);				\
	*(listelm)->field.tqe_prev = (elm);				\
	(listelm)->field.tqe_prev = &(elm)->field.tqe_next;		\
} while (/*CONSTCOND*/0)

#define	TAILQ_REMOVE(head, elm, field) do {				\
	QUEUEDEBUG_TAILQ_PREREMOVE((head), (elm), field)		\
	QUEUEDEBUG_TAILQ_OP((elm), field)				\
	if (((elm)->field.tqe_next) != TAILQ_END(head))			\
		(elm)->field.tqe_next->field.tqe_prev = 		\
		    (elm)->field.tqe_prev;				\
	else								\
		(head)->tqh_last = (elm)->field.tqe_prev;		\
	*(elm)->field.tqe_prev = (elm)->field.tqe_next;			\
	QUEUEDEBUG_TAILQ_POSTREMOVE((elm), field);			\
} while (/*CONSTCOND*/0)

#define TAILQ_REPLACE(head, elm, elm2, field) do {			\
        if (((elm2)->field.tqe_next = (elm)->field.tqe_next) != 	\
	    TAILQ_END(head))   						\
                (elm2)->field.tqe_next->field.tqe_prev =		\
                    &(elm2)->field.tqe_next;				\
        else								\
                (head)->tqh_last = &(elm2)->field.tqe_next;		\
        (elm2)->field.tqe_prev = (elm)->field.tqe_prev;			\
        *(elm2)->field.tqe_prev = (elm2);				\
	QUEUEDEBUG_TAILQ_POSTREMOVE((elm), field);			\
} while (/*CONSTCOND*/0)

#define	TAILQ_CONCAT(head1, head2, field) do {				\
	if (!TAILQ_EMPTY(head2)) {					\
		*(head1)->tqh_last = (head2)->tqh_first;		\
		(head2)->tqh_first->field.tqe_prev = (head1)->tqh_last;	\
		(head1)->tqh_last = (head2)->tqh_last;			\
		TAILQ_INIT((head2));					\
	}								\
} while (/*CONSTCOND*/0)

#if 0
/*
 * Singly-linked Tail queue declarations.
 */
#define	STAILQ_HEAD(name, type)						\
struct name {								\
	struct type *stqh_first;	/* first element */		\
	struct type **stqh_last;	/* addr of last next element */	\
}

#define	STAILQ_HEAD_INITIALIZER(head)					\
	{ NULL, &(head).stqh_first }

#define	STAILQ_ENTRY(type)						\
struct {								\
	struct type *stqe_next;	/* next element */			\
}

/*
 * Singly-linked Tail queue access methods.
 */
#define	STAILQ_FIRST(head)	((head)->stqh_first)
#define	STAILQ_END(head)	NULL
#define	STAILQ_NEXT(elm, field)	((elm)->field.stqe_next)
#define	STAILQ_EMPTY(head)	(STAILQ_FIRST(head) == STAILQ_END(head))

/*
 * Singly-linked Tail queue functions.
 */
#define	STAILQ_INIT(head) do {						\
	(head)->stqh_first = NULL;					\
	(head)->stqh_last = &(head)->stqh_first;				\
} while (/*CONSTCOND*/0)

#define	STAILQ_INSERT_HEAD(head, elm, field) do {			\
	if (((elm)->field.stqe_next = (head)->stqh_first) == NULL)	\
		(head)->stqh_last = &(elm)->field.stqe_next;		\
	(head)->stqh_first = (elm);					\
} while (/*CONSTCOND*/0)

#define	STAILQ_INSERT_TAIL(head, elm, field) do {			\
	(elm)->field.stqe_next = NULL;					\
	*(head)->stqh_last = (elm);					\
	(head)->stqh_last = &(elm)->field.stqe_next;			\
} while (/*CONSTCOND*/0)

#define	STAILQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	if (((elm)->field.stqe_next = (listelm)->field.stqe_next) == NULL)\
		(head)->stqh_last = &(elm)->field.stqe_next;		\
	(listelm)->field.stqe_next = (elm);				\
} while (/*CONSTCOND*/0)

#define	STAILQ_REMOVE_HEAD(head, field) do {				\
	if (((head)->stqh_first = (head)->stqh_first->field.stqe_next) == NULL) \
		(head)->stqh_last = &(head)->stqh_first;			\
} while (/*CONSTCOND*/0)

#define	STAILQ_REMOVE(head, elm, type, field) do {			\
	if ((head)->stqh_first == (elm)) {				\
		STAILQ_REMOVE_HEAD((head), field);			\
	} else {							\
		struct type *curelm = (head)->stqh_first;		\
		while (curelm->field.stqe_next != (elm))			\
			curelm = curelm->field.stqe_next;		\
		if ((curelm->field.stqe_next =				\
			curelm->field.stqe_next->field.stqe_next) == NULL) \
			    (head)->stqh_last = &(curelm)->field.stqe_next; \
	}								\
} while (/*CONSTCOND*/0)

#define	STAILQ_FOREACH(var, head, field)				\
	for ((var) = ((head)->stqh_first);				\
		(var);							\
		(var) = ((var)->field.stqe_next))

#define	STAILQ_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = STAILQ_FIRST((head));				\
	    (var) && ((tvar) = STAILQ_NEXT((var), field), 1);		\
	    (var) = (tvar))

#define	STAILQ_CONCAT(head1, head2) do {				\
	if (!STAILQ_EMPTY((head2))) {					\
		*(head1)->stqh_last = (head2)->stqh_first;		\
		(head1)->stqh_last = (head2)->stqh_last;		\
		STAILQ_INIT((head2));					\
	}								\
} while (/*CONSTCOND*/0)

#define	STAILQ_LAST(head, type, field)					\
	(STAILQ_EMPTY((head)) ?						\
		NULL :							\
	        ((struct type *)(void *)				\
		((char *)((head)->stqh_last) - offsetof(struct type, field))))


#ifndef _KERNEL
/*
 * Circular queue definitions. Do not use. We still keep the macros
 * for compatibility but because of pointer aliasing issues their use
 * is discouraged!
 */

/*
 * __launder_type():  We use this ugly hack to work around the the compiler
 * noticing that two types may not alias each other and elide tests in code.
 * We hit this in the CIRCLEQ macros when comparing 'struct name *' and
 * 'struct type *' (see CIRCLEQ_HEAD()).  Modern compilers (such as GCC
 * 4.8) declare these comparisons as always false, causing the code to
 * not run as designed.
 *
 * This hack is only to be used for comparisons and thus can be fully const.
 * Do not use for assignment.
 *
 * If we ever choose to change the ABI of the CIRCLEQ macros, we could fix
 * this by changing the head/tail sentinal values, but see the note above
 * this one.
 */
static __inline const void * __launder_type(const void *);
static __inline const void *
__launder_type(const void *__x)
{
	__asm __volatile("" : "+r" (__x));
	return __x;
}

#if defined(QUEUEDEBUG)
#define QUEUEDEBUG_CIRCLEQ_HEAD(head, field)				\
	if ((head)->cqh_first != CIRCLEQ_ENDC(head) &&			\
	    (head)->cqh_first->field.cqe_prev != CIRCLEQ_ENDC(head))	\
		QUEUEDEBUG_ABORT("CIRCLEQ head forw %p %s:%d", (head),	\
		      __FILE__, __LINE__);				\
	if ((head)->cqh_last != CIRCLEQ_ENDC(head) &&			\
	    (head)->cqh_last->field.cqe_next != CIRCLEQ_ENDC(head))	\
		QUEUEDEBUG_ABORT("CIRCLEQ head back %p %s:%d", (head),	\
		      __FILE__, __LINE__);
#define QUEUEDEBUG_CIRCLEQ_ELM(head, elm, field)			\
	if ((elm)->field.cqe_next == CIRCLEQ_ENDC(head)) {		\
		if ((head)->cqh_last != (elm))				\
			QUEUEDEBUG_ABORT("CIRCLEQ elm last %p %s:%d",	\
			    (elm), __FILE__, __LINE__);			\
	} else {							\
		if ((elm)->field.cqe_next->field.cqe_prev != (elm))	\
			QUEUEDEBUG_ABORT("CIRCLEQ elm forw %p %s:%d",	\
			    (elm), __FILE__, __LINE__);			\
	}								\
	if ((elm)->field.cqe_prev == CIRCLEQ_ENDC(head)) {		\
		if ((head)->cqh_first != (elm))				\
			QUEUEDEBUG_ABORT("CIRCLEQ elm first %p %s:%d",	\
			    (elm), __FILE__, __LINE__);			\
	} else {							\
		if ((elm)->field.cqe_prev->field.cqe_next != (elm))	\
			QUEUEDEBUG_ABORT("CIRCLEQ elm prev %p %s:%d",	\
			    (elm), __FILE__, __LINE__);			\
	}
#define QUEUEDEBUG_CIRCLEQ_POSTREMOVE(elm, field)			\
	(elm)->field.cqe_next = (void *)1L;				\
	(elm)->field.cqe_prev = (void *)1L;
#else
#define QUEUEDEBUG_CIRCLEQ_HEAD(head, field)
#define QUEUEDEBUG_CIRCLEQ_ELM(head, elm, field)
#define QUEUEDEBUG_CIRCLEQ_POSTREMOVE(elm, field)
#endif

#define	CIRCLEQ_HEAD(name, type)					\
struct name {								\
	struct type *cqh_first;		/* first element */		\
	struct type *cqh_last;		/* last element */		\
}

#define	CIRCLEQ_HEAD_INITIALIZER(head)					\
	{ CIRCLEQ_END(&head), CIRCLEQ_END(&head) }

#define	CIRCLEQ_ENTRY(type)						\
struct {								\
	struct type *cqe_next;		/* next element */		\
	struct type *cqe_prev;		/* previous element */		\
}

/*
 * Circular queue functions.
 */
#define	CIRCLEQ_INIT(head) do {						\
	(head)->cqh_first = CIRCLEQ_END(head);				\
	(head)->cqh_last = CIRCLEQ_END(head);				\
} while (/*CONSTCOND*/0)

#define	CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {		\
	QUEUEDEBUG_CIRCLEQ_HEAD((head), field)				\
	QUEUEDEBUG_CIRCLEQ_ELM((head), (listelm), field)		\
	(elm)->field.cqe_next = (listelm)->field.cqe_next;		\
	(elm)->field.cqe_prev = (listelm);				\
	if ((listelm)->field.cqe_next == CIRCLEQ_ENDC(head))		\
		(head)->cqh_last = (elm);				\
	else								\
		(listelm)->field.cqe_next->field.cqe_prev = (elm);	\
	(listelm)->field.cqe_next = (elm);				\
} while (/*CONSTCOND*/0)

#define	CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {		\
	QUEUEDEBUG_CIRCLEQ_HEAD((head), field)				\
	QUEUEDEBUG_CIRCLEQ_ELM((head), (listelm), field)		\
	(elm)->field.cqe_next = (listelm);				\
	(elm)->field.cqe_prev = (listelm)->field.cqe_prev;		\
	if ((listelm)->field.cqe_prev == CIRCLEQ_ENDC(head))		\
		(head)->cqh_first = (elm);				\
	else								\
		(listelm)->field.cqe_prev->field.cqe_next = (elm);	\
	(listelm)->field.cqe_prev = (elm);				\
} while (/*CONSTCOND*/0)

#define	CIRCLEQ_INSERT_HEAD(head, elm, field) do {			\
	QUEUEDEBUG_CIRCLEQ_HEAD((head), field)				\
	(elm)->field.cqe_next = (head)->cqh_first;			\
	(elm)->field.cqe_prev = CIRCLEQ_END(head);			\
	if ((head)->cqh_last == CIRCLEQ_ENDC(head))			\
		(head)->cqh_last = (elm);				\
	else								\
		(head)->cqh_first->field.cqe_prev = (elm);		\
	(head)->cqh_first = (elm);					\
} while (/*CONSTCOND*/0)

#define	CIRCLEQ_INSERT_TAIL(head, elm, field) do {			\
	QUEUEDEBUG_CIRCLEQ_HEAD((head), field)				\
	(elm)->field.cqe_next = CIRCLEQ_END(head);			\
	(elm)->field.cqe_prev = (head)->cqh_last;			\
	if ((head)->cqh_first == CIRCLEQ_ENDC(head))			\
		(head)->cqh_first = (elm);				\
	else								\
		(head)->cqh_last->field.cqe_next = (elm);		\
	(head)->cqh_last = (elm);					\
} while (/*CONSTCOND*/0)

#define	CIRCLEQ_REMOVE(head, elm, field) do {				\
	QUEUEDEBUG_CIRCLEQ_HEAD((head), field)				\
	QUEUEDEBUG_CIRCLEQ_ELM((head), (elm), field)			\
	if ((elm)->field.cqe_next == CIRCLEQ_ENDC(head))		\
		(head)->cqh_last = (elm)->field.cqe_prev;		\
	else								\
		(elm)->field.cqe_next->field.cqe_prev =			\
		    (elm)->field.cqe_prev;				\
	if ((elm)->field.cqe_prev == CIRCLEQ_ENDC(head))		\
		(head)->cqh_first = (elm)->field.cqe_next;		\
	else								\
		(elm)->field.cqe_prev->field.cqe_next =			\
		    (elm)->field.cqe_next;				\
	QUEUEDEBUG_CIRCLEQ_POSTREMOVE((elm), field)			\
} while (/*CONSTCOND*/0)

#define	CIRCLEQ_FOREACH(var, head, field)				\
	for ((var) = ((head)->cqh_first);				\
		(var) != CIRCLEQ_ENDC(head);				\
		(var) = ((var)->field.cqe_next))

#define	CIRCLEQ_FOREACH_REVERSE(var, head, field)			\
	for ((var) = ((head)->cqh_last);				\
		(var) != CIRCLEQ_ENDC(head);				\
		(var) = ((var)->field.cqe_prev))

/*
 * Circular queue access methods.
 */
#define	CIRCLEQ_FIRST(head)		((head)->cqh_first)
#define	CIRCLEQ_LAST(head)		((head)->cqh_last)
/* For comparisons */
#define	CIRCLEQ_ENDC(head)		(__launder_type(head))
/* For assignments */
#define	CIRCLEQ_END(head)		((void *)(head))
#define	CIRCLEQ_NEXT(elm, field)	((elm)->field.cqe_next)
#define	CIRCLEQ_PREV(elm, field)	((elm)->field.cqe_prev)
#define	CIRCLEQ_EMPTY(head)						\
    (CIRCLEQ_FIRST(head) == CIRCLEQ_ENDC(head))

#define CIRCLEQ_LOOP_NEXT(head, elm, field)				\
	(((elm)->field.cqe_next == CIRCLEQ_ENDC(head))			\
	    ? ((head)->cqh_first)					\
	    : (elm->field.cqe_next))
#define CIRCLEQ_LOOP_PREV(head, elm, field)				\
	(((elm)->field.cqe_prev == CIRCLEQ_ENDC(head))			\
	    ? ((head)->cqh_last)					\
	    : (elm->field.cqe_prev))
#endif /* !_KERNEL */

#endif /* 0 */

#endif	/* !_SYS_QUEUE_H_ */

/**********************************************************************/
/*                               log.h                                */
/**********************************************************************/

#ifndef LOG_H
#define LOG_H


#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3

#if !defined(LOG_LEVEL)
#define LOG_LEVEL (LOG_LEVEL_ERROR+1)
#else
#include <stdio.h>
#include <string.h>
static MQTT_INLINE const char *log_basename(const char *s)
{
#if !defined(_WIN32)
    const char *p = strrchr(s, '/');
#else
    const char *p = strrchr(s, '\\');
#endif

    if (p)
        return p+1;

    return s;
}
#endif

#define LOG_DOLOG(level, fmt, ...) \
    fprintf(stderr, "%s %s %s:%d " fmt "\n", \
        #level, __FUNCTION__, log_basename(__FILE__), __LINE__, ##__VA_ARGS__)

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define LOG_DEBUG(FMT, ...) LOG_DOLOG(DEBUG, FMT, ##__VA_ARGS__)
#else
#define LOG_DEBUG(FMT, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define LOG_INFO(FMT, ...) LOG_DOLOG(INFO, FMT, ##__VA_ARGS__)
#else
#define LOG_INFO(FMT, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARNING)
#define LOG_WARNING(FMT, ...) LOG_DOLOG(WARNING, FMT, ##__VA_ARGS__)
#else
#define LOG_WARNING(FMT, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define LOG_ERROR(FMT, ...) LOG_DOLOG(ERROR, FMT, ##__VA_ARGS__)
#else
#define LOG_ERROR(FMT, ...)
#endif

#endif

/**********************************************************************/
/*                               misc.h                               */
/**********************************************************************/

#ifndef MISC_H
#define MISC_H


#include <stdint.h>
#include <stdlib.h>

/*
    Returns the current time as milliseconds. The return value can only be
    compared to another return value of the function.
*/
int64_t MqttGetCurrentTime();

/*
    Simple hexdump to stdout.
*/
void DumpHex(const void* data, size_t size);

#endif

/**********************************************************************/
/*                               misc.c                               */
/**********************************************************************/


#include <stdio.h>
#include <time.h>

#if defined(_WIN32)
int64_t MqttGetCurrentTime()
{
    static volatile long once = 0;
    static LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    if (InterlockedCompareExchange(&once, 1, 0) == 0)
    {
        QueryPerformanceFrequency(&frequency);
    }
    QueryPerformanceCounter(&counter);
    counter.QuadPart *= 1000;
    counter.QuadPart /= frequency.QuadPart;
    return counter.QuadPart;
}
#else
int64_t MqttGetCurrentTime()
{
    struct timespec t;

    if (clock_gettime(CLOCK_MONOTONIC, &t) == -1)
        return -1;

    return ((int64_t) t.tv_sec * 1000) + (int64_t) t.tv_nsec / 1000 / 1000;
}
#endif

/* https://gist.github.com/ccbrown/9722406 */
void DumpHex(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

/**********************************************************************/
/*                             bstrlib.h                              */
/**********************************************************************/

/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2015, and is covered by the BSD open source
 * license and the GPL. Refer to the accompanying documentation for details
 * on usage and license.
 */

/*
 * bstrlib.h
 *
 * This file is the interface for the core bstring functions.
 */

#ifndef BSTRLIB_INCLUDE
#define BSTRLIB_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#if !defined (BSTRLIB_VSNP_OK) && !defined (BSTRLIB_NOVSNP)
# if defined (__TURBOC__) && !defined (__BORLANDC__)
#  define BSTRLIB_NOVSNP
# endif
#endif

#define BSTR_ERR (-1)
#define BSTR_OK (0)
#define BSTR_BS_BUFF_LENGTH_GET (0)

typedef struct tagbstring * bstring;
typedef const struct tagbstring * const_bstring;

/* Version */
#define BSTR_VER_MAJOR  1
#define BSTR_VER_MINOR  0
#define BSTR_VER_UPDATE 0

/* Copy functions */
#define cstr2bstr bfromcstr
extern bstring bfromcstr (const char * str);
extern bstring bfromcstralloc (int mlen, const char * str);
extern bstring bfromcstrrangealloc (int minl, int maxl, const char* str);
extern bstring blk2bstr (const void * blk, int len);
extern char * bstr2cstr (const_bstring s, char z);
extern int bcstrfree (char * s);
extern bstring bstrcpy (const_bstring b1);
extern int bassign (bstring a, const_bstring b);
extern int bassignmidstr (bstring a, const_bstring b, int left, int len);
extern int bassigncstr (bstring a, const char * str);
extern int bassignblk (bstring a, const void * s, int len);

/* Destroy function */
extern int bdestroy (bstring b);

/* Space allocation hinting functions */
extern int balloc (bstring s, int len);
extern int ballocmin (bstring b, int len);

/* Substring extraction */
extern bstring bmidstr (const_bstring b, int left, int len);

/* Various standard manipulations */
extern int bconcat (bstring b0, const_bstring b1);
extern int bconchar (bstring b0, char c);
extern int bcatcstr (bstring b, const char * s);
extern int bcatblk (bstring b, const void * s, int len);
extern int binsert (bstring s1, int pos, const_bstring s2, unsigned char fill);
extern int binsertblk (bstring s1, int pos, const void * s2, int len, unsigned char fill);
extern int binsertch (bstring s1, int pos, int len, unsigned char fill);
extern int breplace (bstring b1, int pos, int len, const_bstring b2, unsigned char fill);
extern int bdelete (bstring s1, int pos, int len);
extern int bsetstr (bstring b0, int pos, const_bstring b1, unsigned char fill);
extern int btrunc (bstring b, int n);

/* Scan/search functions */
extern int bstricmp (const_bstring b0, const_bstring b1);
extern int bstrnicmp (const_bstring b0, const_bstring b1, int n);
extern int biseqcaseless (const_bstring b0, const_bstring b1);
extern int biseqcaselessblk (const_bstring b, const void * blk, int len);
extern int bisstemeqcaselessblk (const_bstring b0, const void * blk, int len);
extern int biseq (const_bstring b0, const_bstring b1);
extern int biseqblk (const_bstring b, const void * blk, int len);
extern int bisstemeqblk (const_bstring b0, const void * blk, int len);
extern int biseqcstr (const_bstring b, const char * s);
extern int biseqcstrcaseless (const_bstring b, const char * s);
extern int bstrcmp (const_bstring b0, const_bstring b1);
extern int bstrncmp (const_bstring b0, const_bstring b1, int n);
extern int binstr (const_bstring s1, int pos, const_bstring s2);
extern int binstrr (const_bstring s1, int pos, const_bstring s2);
extern int binstrcaseless (const_bstring s1, int pos, const_bstring s2);
extern int binstrrcaseless (const_bstring s1, int pos, const_bstring s2);
extern int bstrchrp (const_bstring b, int c, int pos);
extern int bstrrchrp (const_bstring b, int c, int pos);
#define bstrchr(b,c) bstrchrp ((b), (c), 0)
#define bstrrchr(b,c) bstrrchrp ((b), (c), blength(b)-1)
extern int binchr (const_bstring b0, int pos, const_bstring b1);
extern int binchrr (const_bstring b0, int pos, const_bstring b1);
extern int bninchr (const_bstring b0, int pos, const_bstring b1);
extern int bninchrr (const_bstring b0, int pos, const_bstring b1);
extern int bfindreplace (bstring b, const_bstring find, const_bstring repl, int pos);
extern int bfindreplacecaseless (bstring b, const_bstring find, const_bstring repl, int pos);

/* List of string container functions */
struct bstrList {
    int qty, mlen;
    bstring * entry;
};
extern struct bstrList * bstrListCreate (void);
extern int bstrListDestroy (struct bstrList * sl);
extern int bstrListAlloc (struct bstrList * sl, int msz);
extern int bstrListAllocMin (struct bstrList * sl, int msz);

/* String split and join functions */
extern struct bstrList * bsplit (const_bstring str, unsigned char splitChar);
extern struct bstrList * bsplits (const_bstring str, const_bstring splitStr);
extern struct bstrList * bsplitstr (const_bstring str, const_bstring splitStr);
extern bstring bjoin (const struct bstrList * bl, const_bstring sep);
extern bstring bjoinblk (const struct bstrList * bl, const void * s, int len);
extern int bsplitcb (const_bstring str, unsigned char splitChar, int pos,
	int (* cb) (void * parm, int ofs, int len), void * parm);
extern int bsplitscb (const_bstring str, const_bstring splitStr, int pos,
	int (* cb) (void * parm, int ofs, int len), void * parm);
extern int bsplitstrcb (const_bstring str, const_bstring splitStr, int pos,
	int (* cb) (void * parm, int ofs, int len), void * parm);

/* Miscellaneous functions */
extern int bpattern (bstring b, int len);
extern int btoupper (bstring b);
extern int btolower (bstring b);
extern int bltrimws (bstring b);
extern int brtrimws (bstring b);
extern int btrimws (bstring b);

#if !defined (BSTRLIB_NOVSNP)
extern bstring bformat (const char * fmt, ...);
extern int bformata (bstring b, const char * fmt, ...);
extern int bassignformat (bstring b, const char * fmt, ...);
extern int bvcformata (bstring b, int count, const char * fmt, va_list arglist);

#define bvformata(ret, b, fmt, lastarg) { \
bstring bstrtmp_b = (b); \
const char * bstrtmp_fmt = (fmt); \
int bstrtmp_r = BSTR_ERR, bstrtmp_sz = 16; \
	for (;;) { \
		va_list bstrtmp_arglist; \
		va_start (bstrtmp_arglist, lastarg); \
		bstrtmp_r = bvcformata (bstrtmp_b, bstrtmp_sz, bstrtmp_fmt, bstrtmp_arglist); \
		va_end (bstrtmp_arglist); \
		if (bstrtmp_r >= 0) { /* Everything went ok */ \
			bstrtmp_r = BSTR_OK; \
			break; \
		} else if (-bstrtmp_r <= bstrtmp_sz) { /* A real error? */ \
			bstrtmp_r = BSTR_ERR; \
			break; \
		} \
		bstrtmp_sz = -bstrtmp_r; /* Doubled or target size */ \
	} \
	ret = bstrtmp_r; \
}

#endif

typedef int (*bNgetc) (void *parm);
typedef size_t (* bNread) (void *buff, size_t elsize, size_t nelem, void *parm);

/* Input functions */
extern bstring bgets (bNgetc getcPtr, void * parm, char terminator);
extern bstring bread (bNread readPtr, void * parm);
extern int bgetsa (bstring b, bNgetc getcPtr, void * parm, char terminator);
extern int bassigngets (bstring b, bNgetc getcPtr, void * parm, char terminator);
extern int breada (bstring b, bNread readPtr, void * parm);

/* Stream functions */
extern struct bStream * bsopen (bNread readPtr, void * parm);
extern void * bsclose (struct bStream * s);
extern int bsbufflength (struct bStream * s, int sz);
extern int bsreadln (bstring b, struct bStream * s, char terminator);
extern int bsreadlns (bstring r, struct bStream * s, const_bstring term);
extern int bsread (bstring b, struct bStream * s, int n);
extern int bsreadlna (bstring b, struct bStream * s, char terminator);
extern int bsreadlnsa (bstring r, struct bStream * s, const_bstring term);
extern int bsreada (bstring b, struct bStream * s, int n);
extern int bsunread (struct bStream * s, const_bstring b);
extern int bspeek (bstring r, const struct bStream * s);
extern int bssplitscb (struct bStream * s, const_bstring splitStr, 
	int (* cb) (void * parm, int ofs, const_bstring entry), void * parm);
extern int bssplitstrcb (struct bStream * s, const_bstring splitStr, 
	int (* cb) (void * parm, int ofs, const_bstring entry), void * parm);
extern int bseof (const struct bStream * s);

struct tagbstring {
	int mlen;
	int slen;
	unsigned char * data;
};

/* Accessor macros */
#define blengthe(b, e)      (((b) == (void *)0 || (b)->slen < 0) ? (int)(e) : ((b)->slen))
#define blength(b)          (blengthe ((b), 0))
#define bdataofse(b, o, e)  (((b) == (void *)0 || (b)->data == (void*)0) ? (char *)(e) : ((char *)(b)->data) + (o))
#define bdataofs(b, o)      (bdataofse ((b), (o), (void *)0))
#define bdatae(b, e)        (bdataofse (b, 0, e))
#define bdata(b)            (bdataofs (b, 0))
#define bchare(b, p, e)     ((((unsigned)(p)) < (unsigned)blength(b)) ? ((b)->data[(p)]) : (e))
#define bchar(b, p)         bchare ((b), (p), '\0')

/* Static constant string initialization macro */
#define bsStaticMlen(q,m)   {(m), (int) sizeof(q)-1, (unsigned char *) ("" q "")}
#if defined(_MSC_VER)
# define bsStatic(q)        bsStaticMlen(q,-32)
#endif
#ifndef bsStatic
# define bsStatic(q)        bsStaticMlen(q,-__LINE__)
#endif

/* Static constant block parameter pair */
#define bsStaticBlkParms(q) ((void *)("" q "")), ((int) sizeof(q)-1)

#define bcatStatic(b,s)     ((bcatblk)((b), bsStaticBlkParms(s)))
#define bfromStatic(s)      ((blk2bstr)(bsStaticBlkParms(s)))
#define bassignStatic(b,s)  ((bassignblk)((b), bsStaticBlkParms(s)))
#define binsertStatic(b,p,s,f) ((binsertblk)((b), (p), bsStaticBlkParms(s), (f)))
#define bjoinStatic(b,s)    ((bjoinblk)((b), bsStaticBlkParms(s)))
#define biseqStatic(b,s)    ((biseqblk)((b), bsStaticBlkParms(s)))
#define bisstemeqStatic(b,s) ((bisstemeqblk)((b), bsStaticBlkParms(s)))
#define biseqcaselessStatic(b,s) ((biseqcaselessblk)((b), bsStaticBlkParms(s)))
#define bisstemeqcaselessStatic(b,s) ((bisstemeqcaselessblk)((b), bsStaticBlkParms(s)))

/* Reference building macros */
#define cstr2tbstr btfromcstr
#define btfromcstr(t,s) {                                            \
    (t).data = (unsigned char *) (s);                                \
    (t).slen = ((t).data) ? ((int) (strlen) ((char *)(t).data)) : 0; \
    (t).mlen = -1;                                                   \
}
#define blk2tbstr(t,s,l) {            \
    (t).data = (unsigned char *) (s); \
    (t).slen = l;                     \
    (t).mlen = -1;                    \
}
#define btfromblk(t,s,l) blk2tbstr(t,s,l)
#define bmid2tbstr(t,b,p,l) {                                                \
    const_bstring bstrtmp_s = (b);                                           \
    if (bstrtmp_s && bstrtmp_s->data && bstrtmp_s->slen >= 0) {              \
        int bstrtmp_left = (p);                                              \
        int bstrtmp_len  = (l);                                              \
        if (bstrtmp_left < 0) {                                              \
            bstrtmp_len += bstrtmp_left;                                     \
            bstrtmp_left = 0;                                                \
        }                                                                    \
        if (bstrtmp_len > bstrtmp_s->slen - bstrtmp_left)                    \
            bstrtmp_len = bstrtmp_s->slen - bstrtmp_left;                    \
        if (bstrtmp_len <= 0) {                                              \
            (t).data = (unsigned char *)"";                                  \
            (t).slen = 0;                                                    \
        } else {                                                             \
            (t).data = bstrtmp_s->data + bstrtmp_left;                       \
            (t).slen = bstrtmp_len;                                          \
        }                                                                    \
    } else {                                                                 \
        (t).data = (unsigned char *)"";                                      \
        (t).slen = 0;                                                        \
    }                                                                        \
    (t).mlen = -__LINE__;                                                    \
}
#define btfromblkltrimws(t,s,l) {                                            \
    int bstrtmp_idx = 0, bstrtmp_len = (l);                                  \
    unsigned char * bstrtmp_s = (s);                                         \
    if (bstrtmp_s && bstrtmp_len >= 0) {                                     \
        for (; bstrtmp_idx < bstrtmp_len; bstrtmp_idx++) {                   \
            if (!isspace (bstrtmp_s[bstrtmp_idx])) break;                    \
        }                                                                    \
    }                                                                        \
    (t).data = bstrtmp_s + bstrtmp_idx;                                      \
    (t).slen = bstrtmp_len - bstrtmp_idx;                                    \
    (t).mlen = -__LINE__;                                                    \
}
#define btfromblkrtrimws(t,s,l) {                                            \
    int bstrtmp_len = (l) - 1;                                               \
    unsigned char * bstrtmp_s = (s);                                         \
    if (bstrtmp_s && bstrtmp_len >= 0) {                                     \
        for (; bstrtmp_len >= 0; bstrtmp_len--) {                            \
            if (!isspace (bstrtmp_s[bstrtmp_len])) break;                    \
        }                                                                    \
    }                                                                        \
    (t).data = bstrtmp_s;                                                    \
    (t).slen = bstrtmp_len + 1;                                              \
    (t).mlen = -__LINE__;                                                    \
}
#define btfromblktrimws(t,s,l) {                                             \
    int bstrtmp_idx = 0, bstrtmp_len = (l) - 1;                              \
    unsigned char * bstrtmp_s = (s);                                         \
    if (bstrtmp_s && bstrtmp_len >= 0) {                                     \
        for (; bstrtmp_idx <= bstrtmp_len; bstrtmp_idx++) {                  \
            if (!isspace (bstrtmp_s[bstrtmp_idx])) break;                    \
        }                                                                    \
        for (; bstrtmp_len >= bstrtmp_idx; bstrtmp_len--) {                  \
            if (!isspace (bstrtmp_s[bstrtmp_len])) break;                    \
        }                                                                    \
    }                                                                        \
    (t).data = bstrtmp_s + bstrtmp_idx;                                      \
    (t).slen = bstrtmp_len + 1 - bstrtmp_idx;                                \
    (t).mlen = -__LINE__;                                                    \
}

/* Write protection macros */
#define bwriteprotect(t)     { if ((t).mlen >=  0) (t).mlen = -1; }
#define bwriteallow(t)       { if ((t).mlen == -1) (t).mlen = (t).slen + ((t).slen == 0); }
#define biswriteprotected(t) ((t).mlen <= 0)

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************/
/*                             bstrlib.c                              */
/**********************************************************************/

/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2015, and is covered by the BSD open source
 * license and the GPL. Refer to the accompanying documentation for details
 * on usage and license.
 */

/*
 * bstrlib.c
 *
 * This file is the core module for implementing the bstring functions.
 */

#if defined (_MSC_VER)
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* Optionally include a mechanism for debugging memory */

#if defined(MEMORY_DEBUG) || defined(BSTRLIB_MEMORY_DEBUG)
#endif

#ifndef bstr__alloc
#if defined (BSTRLIB_TEST_CANARY)
void* bstr__alloc (size_t sz) {
	char* p = (char *) malloc (sz);
	memset (p, 'X', sz);
	return p;
}
#else
#define bstr__alloc(x) malloc (x)
#endif
#endif

#ifndef bstr__free
#define bstr__free(p) free (p)
#endif

#ifndef bstr__realloc
#define bstr__realloc(p,x) realloc ((p), (x))
#endif

#ifndef bstr__memcpy
#define bstr__memcpy(d,s,l) memcpy ((d), (s), (l))
#endif

#ifndef bstr__memmove
#define bstr__memmove(d,s,l) memmove ((d), (s), (l))
#endif

#ifndef bstr__memset
#define bstr__memset(d,c,l) memset ((d), (c), (l))
#endif

#ifndef bstr__memcmp
#define bstr__memcmp(d,c,l) memcmp ((d), (c), (l))
#endif

#ifndef bstr__memchr
#define bstr__memchr(s,c,l) memchr ((s), (c), (l))
#endif

/* Just a length safe wrapper for memmove. */

#define bBlockCopy(D,S,L) { if ((L) > 0) bstr__memmove ((D),(S),(L)); }

/* Compute the snapped size for a given requested size.  By snapping to powers
   of 2 like this, repeated reallocations are avoided. */
static int snapUpSize (int i) {
	if (i < 8) {
		i = 8;
	} else {
		unsigned int j;
		j = (unsigned int) i;

		j |= (j >>  1);
		j |= (j >>  2);
		j |= (j >>  4);
		j |= (j >>  8);		/* Ok, since int >= 16 bits */
#if (UINT_MAX != 0xffff)
		j |= (j >> 16);		/* For 32 bit int systems */
#if (UINT_MAX > 0xffffffffUL)
		j |= (j >> 32);		/* For 64 bit int systems */
#endif
#endif
		/* Least power of two greater than i */
		j++;
		if ((int) j >= i) i = (int) j;
	}
	return i;
}

/*  int balloc (bstring b, int len)
 *
 *  Increase the size of the memory backing the bstring b to at least len.
 */
int balloc (bstring b, int olen) {
	int len;
	if (b == NULL || b->data == NULL || b->slen < 0 || b->mlen <= 0 ||
	    b->mlen < b->slen || olen <= 0) {
		return BSTR_ERR;
	}

	if (olen >= b->mlen) {
		unsigned char * x;

		if ((len = snapUpSize (olen)) <= b->mlen) return BSTR_OK;

		/* Assume probability of a non-moving realloc is 0.125 */
		if (7 * b->mlen < 8 * b->slen) {

			/* If slen is close to mlen in size then use realloc to reduce
			   the memory defragmentation */

			reallocStrategy:;

			x = (unsigned char *) bstr__realloc (b->data, (size_t) len);
			if (x == NULL) {

				/* Since we failed, try allocating the tighest possible
				   allocation */

				len = olen;
				x = (unsigned char *) bstr__realloc (b->data, (size_t) olen);
				if (NULL == x) {
					return BSTR_ERR;
				}
			}
		} else {

			/* If slen is not close to mlen then avoid the penalty of copying
			   the extra bytes that are allocated, but not considered part of
			   the string */

			if (NULL == (x = (unsigned char *) bstr__alloc ((size_t) len))) {

				/* Perhaps there is no available memory for the two
				   allocations to be in memory at once */

				goto reallocStrategy;

			} else {
				if (b->slen) bstr__memcpy ((char *) x, (char *) b->data,
				                           (size_t) b->slen);
				bstr__free (b->data);
			}
		}
		b->data = x;
		b->mlen = len;
		b->data[b->slen] = (unsigned char) '\0';

#if defined (BSTRLIB_TEST_CANARY)
		if (len > b->slen + 1) {
			memchr (b->data + b->slen + 1, 'X', len - (b->slen + 1));
		}
#endif
	}

	return BSTR_OK;
}

/*  int ballocmin (bstring b, int len)
 *
 *  Set the size of the memory backing the bstring b to len or b->slen+1,
 *  whichever is larger.  Note that repeated use of this function can degrade
 *  performance.
 */
int ballocmin (bstring b, int len) {
	unsigned char * s;

	if (b == NULL || b->data == NULL) return BSTR_ERR;
	if (b->slen >= INT_MAX || b->slen < 0) return BSTR_ERR;
	if (b->mlen <= 0 || b->mlen < b->slen || len <= 0) {
		return BSTR_ERR;
	}

	if (len < b->slen + 1) len = b->slen + 1;

	if (len != b->mlen) {
		s = (unsigned char *) bstr__realloc (b->data, (size_t) len);
		if (NULL == s) return BSTR_ERR;
		s[b->slen] = (unsigned char) '\0';
		b->data = s;
		b->mlen = len;
	}

	return BSTR_OK;
}

/*  bstring bfromcstr (const char * str)
 *
 *  Create a bstring which contains the contents of the '\0' terminated char *
 *  buffer str.
 */
bstring bfromcstr (const char * str) {
bstring b;
int i;
size_t j;

	if (str == NULL) return NULL;
	j = (strlen) (str);
	i = snapUpSize ((int) (j + (2 - (j != 0))));
	if (i <= (int) j) return NULL;

	b = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (NULL == b) return NULL;
	b->slen = (int) j;
	if (NULL == (b->data = (unsigned char *) bstr__alloc (b->mlen = i))) {
		bstr__free (b);
		return NULL;
	}

	bstr__memcpy (b->data, str, j+1);
	return b;
}

/*  bstring bfromcstrrangealloc (int minl, int maxl, const char* str)
 *
 *  Create a bstring which contains the contents of the '\0' terminated
 *  char* buffer str.  The memory buffer backing the string is at least
 *  minl characters in length, but an attempt is made to allocate up to
 *  maxl characters.
 */
bstring bfromcstrrangealloc (int minl, int maxl, const char* str) {
bstring b;
int i;
size_t j;

	/* Bad parameters? */
	if (str == NULL) return NULL;
	if (maxl < minl || minl < 0) return NULL;

	/* Adjust lengths */
	j = (strlen) (str);
	if ((size_t) minl < (j+1)) minl = (int) (j+1);
	if (maxl < minl) maxl = minl;
	i = maxl;

	b = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (b == NULL) return NULL;
	b->slen = (int) j;

	while (NULL == (b->data = (unsigned char *) bstr__alloc (b->mlen = i))) {
		int k = (i >> 1) + (minl >> 1);
		if (i == k || i < minl) {
			bstr__free (b);
			return NULL;
		}
		i = k;
	}

	bstr__memcpy (b->data, str, j+1);
	return b;
}

/*  bstring bfromcstralloc (int mlen, const char * str)
 *
 *  Create a bstring which contains the contents of the '\0' terminated
 *  char* buffer str.  The memory buffer backing the string is at least
 *  mlen characters in length.
 */
bstring bfromcstralloc (int mlen, const char * str) {
	return bfromcstrrangealloc (mlen, mlen, str);
}

/*  bstring blk2bstr (const void * blk, int len)
 *
 *  Create a bstring which contains the content of the block blk of length
 *  len.
 */
bstring blk2bstr (const void * blk, int len) {
bstring b;
int i;

	if (blk == NULL || len < 0) return NULL;
	b = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (b == NULL) return NULL;
	b->slen = len;

	i = len + (2 - (len != 0));
	i = snapUpSize (i);

	b->mlen = i;

	b->data = (unsigned char *) bstr__alloc ((size_t) b->mlen);
	if (b->data == NULL) {
		bstr__free (b);
		return NULL;
	}

	if (len > 0) bstr__memcpy (b->data, blk, (size_t) len);
	b->data[len] = (unsigned char) '\0';

	return b;
}

/*  char * bstr2cstr (const_bstring s, char z)
 *
 *  Create a '\0' terminated char * buffer which is equal to the contents of
 *  the bstring s, except that any contained '\0' characters are converted
 *  to the character in z. This returned value should be freed with a
 *  bcstrfree () call, by the calling application.
 */
char * bstr2cstr (const_bstring b, char z) {
int i, l;
char * r;

	if (b == NULL || b->slen < 0 || b->data == NULL) return NULL;
	l = b->slen;
	r = (char *) bstr__alloc ((size_t) (l + 1));
	if (r == NULL) return r;

	for (i=0; i < l; i ++) {
		r[i] = (char) ((b->data[i] == '\0') ? z : (char) (b->data[i]));
	}

	r[l] = (unsigned char) '\0';

	return r;
}

/*  int bcstrfree (char * s)
 *
 *  Frees a C-string generated by bstr2cstr ().  This is normally unnecessary
 *  since it just wraps a call to bstr__free (), however, if bstr__alloc ()
 *  and bstr__free () have been redefined as a macros within the bstrlib
 *  module (via defining them in memdbg.h after defining
 *  BSTRLIB_MEMORY_DEBUG) with some difference in behaviour from the std
 *  library functions, then this allows a correct way of freeing the memory
 *  that allows higher level code to be independent from these macro
 *  redefinitions.
 */
int bcstrfree (char * s) {
	if (s) {
		bstr__free (s);
		return BSTR_OK;
	}
	return BSTR_ERR;
}

/*  int bconcat (bstring b0, const_bstring b1)
 *
 *  Concatenate the bstring b1 to the bstring b0.
 */
int bconcat (bstring b0, const_bstring b1) {
int len, d;
bstring aux = (bstring) b1;

	if (b0 == NULL || b1 == NULL || b0->data == NULL || b1->data == NULL)
		return BSTR_ERR;

	d = b0->slen;
	len = b1->slen;
	if ((d | (b0->mlen - d) | len | (d + len)) < 0) return BSTR_ERR;

	if (b0->mlen <= d + len + 1) {
		ptrdiff_t pd = b1->data - b0->data;
		if (0 <= pd && pd < b0->mlen) {
			if (NULL == (aux = bstrcpy (b1))) return BSTR_ERR;
		}
		if (balloc (b0, d + len + 1) != BSTR_OK) {
			if (aux != b1) bdestroy (aux);
			return BSTR_ERR;
		}
	}

	bBlockCopy (&b0->data[d], &aux->data[0], (size_t) len);
	b0->data[d + len] = (unsigned char) '\0';
	b0->slen = d + len;
	if (aux != b1) bdestroy (aux);
	return BSTR_OK;
}

/*  int bconchar (bstring b, char c)
 *
 *  Concatenate the single character c to the bstring b.
 */
int bconchar (bstring b, char c) {
int d;

	if (b == NULL) return BSTR_ERR;
	d = b->slen;
	if ((d | (b->mlen - d)) < 0 || balloc (b, d + 2) != BSTR_OK)
		return BSTR_ERR;
	b->data[d] = (unsigned char) c;
	b->data[d + 1] = (unsigned char) '\0';
	b->slen++;
	return BSTR_OK;
}

/*  int bcatcstr (bstring b, const char * s)
 *
 *  Concatenate a char * string to a bstring.
 */
int bcatcstr (bstring b, const char * s) {
char * d;
int i, l;

	if (b == NULL || b->data == NULL || b->slen < 0 || b->mlen < b->slen
	 || b->mlen <= 0 || s == NULL) return BSTR_ERR;

	/* Optimistically concatenate directly */
	l = b->mlen - b->slen;
	d = (char *) &b->data[b->slen];
	for (i=0; i < l; i++) {
		if ((*d++ = *s++) == '\0') {
			b->slen += i;
			return BSTR_OK;
		}
	}
	b->slen += i;

	/* Need to explicitely resize and concatenate tail */
	return bcatblk (b, (const void *) s, (int) strlen (s));
}

/*  int bcatblk (bstring b, const void * s, int len)
 *
 *  Concatenate a fixed length buffer to a bstring.
 */
int bcatblk (bstring b, const void * s, int len) {
int nl;

	if (b == NULL || b->data == NULL || b->slen < 0 || b->mlen < b->slen
	 || b->mlen <= 0 || s == NULL || len < 0) return BSTR_ERR;

	if (0 > (nl = b->slen + len)) return BSTR_ERR; /* Overflow? */
	if (b->mlen <= nl && 0 > balloc (b, nl + 1)) return BSTR_ERR;

	bBlockCopy (&b->data[b->slen], s, (size_t) len);
	b->slen = nl;
	b->data[nl] = (unsigned char) '\0';
	return BSTR_OK;
}

/*  bstring bstrcpy (const_bstring b)
 *
 *  Create a copy of the bstring b.
 */
bstring bstrcpy (const_bstring b) {
bstring b0;
int i,j;

	/* Attempted to copy an invalid string? */
	if (b == NULL || b->slen < 0 || b->data == NULL) return NULL;

	b0 = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (b0 == NULL) {
		/* Unable to allocate memory for string header */
		return NULL;
	}

	i = b->slen;
	j = snapUpSize (i + 1);

	b0->data = (unsigned char *) bstr__alloc (j);
	if (b0->data == NULL) {
		j = i + 1;
		b0->data = (unsigned char *) bstr__alloc (j);
		if (b0->data == NULL) {
			/* Unable to allocate memory for string data */
			bstr__free (b0);
			return NULL;
		}
	}

	b0->mlen = j;
	b0->slen = i;

	if (i) bstr__memcpy ((char *) b0->data, (char *) b->data, i);
	b0->data[b0->slen] = (unsigned char) '\0';

	return b0;
}

/*  int bassign (bstring a, const_bstring b)
 *
 *  Overwrite the string a with the contents of string b.
 */
int bassign (bstring a, const_bstring b) {
	if (b == NULL || b->data == NULL || b->slen < 0)
		return BSTR_ERR;
	if (b->slen != 0) {
		if (balloc (a, b->slen) != BSTR_OK) return BSTR_ERR;
		bstr__memmove (a->data, b->data, b->slen);
	} else {
		if (a == NULL || a->data == NULL || a->mlen < a->slen ||
		    a->slen < 0 || a->mlen == 0)
			return BSTR_ERR;
	}
	a->data[b->slen] = (unsigned char) '\0';
	a->slen = b->slen;
	return BSTR_OK;
}

/*  int bassignmidstr (bstring a, const_bstring b, int left, int len)
 *
 *  Overwrite the string a with the middle of contents of string b
 *  starting from position left and running for a length len.  left and
 *  len are clamped to the ends of b as with the function bmidstr.
 */
int bassignmidstr (bstring a, const_bstring b, int left, int len) {
	if (b == NULL || b->data == NULL || b->slen < 0)
		return BSTR_ERR;

	if (left < 0) {
		len += left;
		left = 0;
	}

	if (len > b->slen - left) len = b->slen - left;

	if (a == NULL || a->data == NULL || a->mlen < a->slen ||
	    a->slen < 0 || a->mlen == 0)
		return BSTR_ERR;

	if (len > 0) {
		if (balloc (a, len) != BSTR_OK) return BSTR_ERR;
		bstr__memmove (a->data, b->data + left, len);
		a->slen = len;
	} else {
		a->slen = 0;
	}
	a->data[a->slen] = (unsigned char) '\0';
	return BSTR_OK;
}

/*  int bassigncstr (bstring a, const char * str)
 *
 *  Overwrite the string a with the contents of char * string str.  Note that
 *  the bstring a must be a well defined and writable bstring.  If an error
 *  occurs BSTR_ERR is returned however a may be partially overwritten.
 */
int bassigncstr (bstring a, const char * str) {
int i;
size_t len;
	if (a == NULL || a->data == NULL || a->mlen < a->slen ||
	    a->slen < 0 || a->mlen == 0 || NULL == str)
		return BSTR_ERR;

	for (i=0; i < a->mlen; i++) {
		if ('\0' == (a->data[i] = str[i])) {
			a->slen = i;
			return BSTR_OK;
		}
	}

	a->slen = i;
	len = strlen (str + i);
	if (len + 1 > (size_t) INT_MAX - i ||
	    0 > balloc (a, (int) (i + len + 1))) return BSTR_ERR;
	bBlockCopy (a->data + i, str + i, (size_t) len + 1);
	a->slen += (int) len;
	return BSTR_OK;
}

/*  int bassignblk (bstring a, const void * s, int len)
 *
 *  Overwrite the string a with the contents of the block (s, len).  Note that
 *  the bstring a must be a well defined and writable bstring.  If an error
 *  occurs BSTR_ERR is returned and a is not overwritten.
 */
int bassignblk (bstring a, const void * s, int len) {
	if (a == NULL || a->data == NULL || a->mlen < a->slen ||
	    a->slen < 0 || a->mlen == 0 || NULL == s || len < 0 || len >= INT_MAX)
		return BSTR_ERR;
	if (len + 1 > a->mlen && 0 > balloc (a, len + 1)) return BSTR_ERR;
	bBlockCopy (a->data, s, (size_t) len);
	a->data[len] = (unsigned char) '\0';
	a->slen = len;
	return BSTR_OK;
}

/*  int btrunc (bstring b, int n)
 *
 *  Truncate the bstring to at most n characters.
 */
int btrunc (bstring b, int n) {
	if (n < 0 || b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;
	if (b->slen > n) {
		b->slen = n;
		b->data[n] = (unsigned char) '\0';
	}
	return BSTR_OK;
}

#define   upcase(c) (toupper ((unsigned char) c))
#define downcase(c) (tolower ((unsigned char) c))
#define   wspace(c) (isspace ((unsigned char) c))

/*  int btoupper (bstring b)
 *
 *  Convert contents of bstring to upper case.
 */
int btoupper (bstring b) {
int i, len;
	if (b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;
	for (i=0, len = b->slen; i < len; i++) {
		b->data[i] = (unsigned char) upcase (b->data[i]);
	}
	return BSTR_OK;
}

/*  int btolower (bstring b)
 *
 *  Convert contents of bstring to lower case.
 */
int btolower (bstring b) {
int i, len;
	if (b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;
	for (i=0, len = b->slen; i < len; i++) {
		b->data[i] = (unsigned char) downcase (b->data[i]);
	}
	return BSTR_OK;
}

/*  int bstricmp (const_bstring b0, const_bstring b1)
 *
 *  Compare two strings without differentiating between case.  The return
 *  value is the difference of the values of the characters where the two
 *  strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal.  If the lengths are
 *  different, then a difference from 0 is given, but if the first extra
 *  character is '\0', then it is taken to be the value UCHAR_MAX+1.
 */
int bstricmp (const_bstring b0, const_bstring b1) {
int i, v, n;

	if (bdata (b0) == NULL || b0->slen < 0 ||
	    bdata (b1) == NULL || b1->slen < 0) return SHRT_MIN;
	if ((n = b0->slen) > b1->slen) n = b1->slen;
	else if (b0->slen == b1->slen && b0->data == b1->data) return BSTR_OK;

	for (i = 0; i < n; i ++) {
		v  = (char) downcase (b0->data[i])
		   - (char) downcase (b1->data[i]);
		if (0 != v) return v;
	}

	if (b0->slen > n) {
		v = (char) downcase (b0->data[n]);
		if (v) return v;
		return UCHAR_MAX + 1;
	}
	if (b1->slen > n) {
		v = - (char) downcase (b1->data[n]);
		if (v) return v;
		return - (int) (UCHAR_MAX + 1);
	}
	return BSTR_OK;
}

/*  int bstrnicmp (const_bstring b0, const_bstring b1, int n)
 *
 *  Compare two strings without differentiating between case for at most n
 *  characters.  If the position where the two strings first differ is
 *  before the nth position, the return value is the difference of the values
 *  of the characters, otherwise 0 is returned.  If the lengths are different
 *  and less than n characters, then a difference from 0 is given, but if the
 *  first extra character is '\0', then it is taken to be the value
 *  UCHAR_MAX+1.
 */
int bstrnicmp (const_bstring b0, const_bstring b1, int n) {
int i, v, m;

	if (bdata (b0) == NULL || b0->slen < 0 ||
	    bdata (b1) == NULL || b1->slen < 0 || n < 0) return SHRT_MIN;
	m = n;
	if (m > b0->slen) m = b0->slen;
	if (m > b1->slen) m = b1->slen;

	if (b0->data != b1->data) {
		for (i = 0; i < m; i ++) {
			v  = (char) downcase (b0->data[i]);
			v -= (char) downcase (b1->data[i]);
			if (v != 0) return b0->data[i] - b1->data[i];
		}
	}

	if (n == m || b0->slen == b1->slen) return BSTR_OK;

	if (b0->slen > m) {
		v = (char) downcase (b0->data[m]);
		if (v) return v;
		return UCHAR_MAX + 1;
	}

	v = - (char) downcase (b1->data[m]);
	if (v) return v;
	return - (int) (UCHAR_MAX + 1);
}

/*  int biseqcaselessblk (const_bstring b, const void * blk, int len)
 *
 *  Compare content of b and the array of bytes in blk for length len for
 *  equality without differentiating between character case.  If the content
 *  differs other than in case, 0 is returned, if, ignoring case, the content
 *  is the same, 1 is returned, if there is an error, -1 is returned.  If the
 *  length of the strings are different, this function is O(1).  '\0'
 *  characters are not treated in any special way.
 */
int biseqcaselessblk (const_bstring b, const void * blk, int len) {
int i;

	if (bdata (b) == NULL || b->slen < 0 ||
	    blk == NULL || len < 0) return BSTR_ERR;
	if (b->slen != len) return 0;
	if (len == 0 || b->data == blk) return 1;
	for (i=0; i < len; i++) {
		if (b->data[i] != ((unsigned char*)blk)[i]) {
			unsigned char c = (unsigned char) downcase (b->data[i]);
			if (c != (unsigned char) downcase (((unsigned char*)blk)[i]))
				return 0;
		}
	}
	return 1;
}


/*  int biseqcaseless (const_bstring b0, const_bstring b1)
 *
 *  Compare two strings for equality without differentiating between case.
 *  If the strings differ other than in case, 0 is returned, if the strings
 *  are the same, 1 is returned, if there is an error, -1 is returned.  If
 *  the length of the strings are different, this function is O(1).  '\0'
 *  termination characters are not treated in any special way.
 */
int biseqcaseless (const_bstring b0, const_bstring b1) {
	if (NULL == b1) return BSTR_ERR;
	return biseqcaselessblk (b0, b1->data, b1->slen);
}

/*  int bisstemeqcaselessblk (const_bstring b0, const void * blk, int len)
 *
 *  Compare beginning of string b0 with a block of memory of length len
 *  without differentiating between case for equality.  If the beginning of b0
 *  differs from the memory block other than in case (or if b0 is too short),
 *  0 is returned, if the strings are the same, 1 is returned, if there is an
 *  error, -1 is returned.  '\0' characters are not treated in any special
 *  way.
 */
int bisstemeqcaselessblk (const_bstring b0, const void * blk, int len) {
int i;

	if (bdata (b0) == NULL || b0->slen < 0 || NULL == blk || len < 0)
		return BSTR_ERR;
	if (b0->slen < len) return BSTR_OK;
	if (b0->data == (const unsigned char *) blk || len == 0) return 1;

	for (i = 0; i < len; i ++) {
		if (b0->data[i] != ((const unsigned char *) blk)[i]) {
			if (downcase (b0->data[i]) !=
			    downcase (((const unsigned char *) blk)[i])) return 0;
		}
	}
	return 1;
}

/*
 * int bltrimws (bstring b)
 *
 * Delete whitespace contiguous from the left end of the string.
 */
int bltrimws (bstring b) {
int i, len;

	if (b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;

	for (len = b->slen, i = 0; i < len; i++) {
		if (!wspace (b->data[i])) {
			return bdelete (b, 0, i);
		}
	}

	b->data[0] = (unsigned char) '\0';
	b->slen = 0;
	return BSTR_OK;
}

/*
 * int brtrimws (bstring b)
 *
 * Delete whitespace contiguous from the right end of the string.
 */
int brtrimws (bstring b) {
int i;

	if (b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;

	for (i = b->slen - 1; i >= 0; i--) {
		if (!wspace (b->data[i])) {
			if (b->mlen > i) b->data[i+1] = (unsigned char) '\0';
			b->slen = i + 1;
			return BSTR_OK;
		}
	}

	b->data[0] = (unsigned char) '\0';
	b->slen = 0;
	return BSTR_OK;
}

/*
 * int btrimws (bstring b)
 *
 * Delete whitespace contiguous from both ends of the string.
 */
int btrimws (bstring b) {
int i, j;

	if (b == NULL || b->data == NULL || b->mlen < b->slen ||
	    b->slen < 0 || b->mlen <= 0) return BSTR_ERR;

	for (i = b->slen - 1; i >= 0; i--) {
		if (!wspace (b->data[i])) {
			if (b->mlen > i) b->data[i+1] = (unsigned char) '\0';
			b->slen = i + 1;
			for (j = 0; wspace (b->data[j]); j++) {}
			return bdelete (b, 0, j);
		}
	}

	b->data[0] = (unsigned char) '\0';
	b->slen = 0;
	return BSTR_OK;
}

/*  int biseqblk (const_bstring b, const void * blk, int len)
 *
 *  Compare the string b with the character block blk of length len.  If the
 *  content differs, 0 is returned, if the content is the same, 1 is returned,
 *  if there is an error, -1 is returned.  If the length of the strings are
 *  different, this function is O(1).  '\0' characters are not treated in any
 *  special way.
 */
int biseqblk (const_bstring b, const void * blk, int len) {
	if (len < 0 || b == NULL || blk == NULL || b->data == NULL || b->slen < 0)
		return BSTR_ERR;
	if (b->slen != len) return 0;
	if (len == 0 || b->data == blk) return 1;
	return !bstr__memcmp (b->data, blk, len);
}

/*  int biseq (const_bstring b0, const_bstring b1)
 *
 *  Compare the string b0 and b1.  If the strings differ, 0 is returned, if
 *  the strings are the same, 1 is returned, if there is an error, -1 is
 *  returned.  If the length of the strings are different, this function is
 *  O(1).  '\0' termination characters are not treated in any special way.
 */
int biseq (const_bstring b0, const_bstring b1) {
	if (b0 == NULL || b1 == NULL || b0->data == NULL || b1->data == NULL ||
		b0->slen < 0 || b1->slen < 0) return BSTR_ERR;
	if (b0->slen != b1->slen) return BSTR_OK;
	if (b0->data == b1->data || b0->slen == 0) return 1;
	return !bstr__memcmp (b0->data, b1->data, b0->slen);
}

/*  int bisstemeqblk (const_bstring b0, const void * blk, int len)
 *
 *  Compare beginning of string b0 with a block of memory of length len for
 *  equality.  If the beginning of b0 differs from the memory block (or if b0
 *  is too short), 0 is returned, if the strings are the same, 1 is returned,
 *  if there is an error, -1 is returned.  '\0' characters are not treated in
 *  any special way.
 */
int bisstemeqblk (const_bstring b0, const void * blk, int len) {
int i;

	if (bdata (b0) == NULL || b0->slen < 0 || NULL == blk || len < 0)
		return BSTR_ERR;
	if (b0->slen < len) return BSTR_OK;
	if (b0->data == (const unsigned char *) blk || len == 0) return 1;

	for (i = 0; i < len; i ++) {
		if (b0->data[i] != ((const unsigned char *) blk)[i]) return BSTR_OK;
	}
	return 1;
}

/*  int biseqcstr (const_bstring b, const char *s)
 *
 *  Compare the bstring b and char * string s.  The C string s must be '\0'
 *  terminated at exactly the length of the bstring b, and the contents
 *  between the two must be identical with the bstring b with no '\0'
 *  characters for the two contents to be considered equal.  This is
 *  equivalent to the condition that their current contents will be always be
 *  equal when comparing them in the same format after converting one or the
 *  other.  If the strings are equal 1 is returned, if they are unequal 0 is
 *  returned and if there is a detectable error BSTR_ERR is returned.
 */
int biseqcstr (const_bstring b, const char * s) {
int i;
	if (b == NULL || s == NULL || b->data == NULL || b->slen < 0)
		return BSTR_ERR;
	for (i=0; i < b->slen; i++) {
		if (s[i] == '\0' || b->data[i] != (unsigned char) s[i])
			return BSTR_OK;
	}
	return s[i] == '\0';
}

/*  int biseqcstrcaseless (const_bstring b, const char *s)
 *
 *  Compare the bstring b and char * string s.  The C string s must be '\0'
 *  terminated at exactly the length of the bstring b, and the contents
 *  between the two must be identical except for case with the bstring b with
 *  no '\0' characters for the two contents to be considered equal.  This is
 *  equivalent to the condition that their current contents will be always be
 *  equal ignoring case when comparing them in the same format after
 *  converting one or the other.  If the strings are equal, except for case,
 *  1 is returned, if they are unequal regardless of case 0 is returned and
 *  if there is a detectable error BSTR_ERR is returned.
 */
int biseqcstrcaseless (const_bstring b, const char * s) {
int i;
	if (b == NULL || s == NULL || b->data == NULL || b->slen < 0)
		return BSTR_ERR;
	for (i=0; i < b->slen; i++) {
		if (s[i] == '\0' ||
		    (b->data[i] != (unsigned char) s[i] &&
		     downcase (b->data[i]) != (unsigned char) downcase (s[i])))
			return BSTR_OK;
	}
	return s[i] == '\0';
}

/*  int bstrcmp (const_bstring b0, const_bstring b1)
 *
 *  Compare the string b0 and b1.  If there is an error, SHRT_MIN is returned,
 *  otherwise a value less than or greater than zero, indicating that the
 *  string pointed to by b0 is lexicographically less than or greater than
 *  the string pointed to by b1 is returned.  If the the string lengths are
 *  unequal but the characters up until the length of the shorter are equal
 *  then a value less than, or greater than zero, indicating that the string
 *  pointed to by b0 is shorter or longer than the string pointed to by b1 is
 *  returned.  0 is returned if and only if the two strings are the same.  If
 *  the length of the strings are different, this function is O(n).  Like its
 *  standard C library counter part strcmp, the comparison does not proceed
 *  past any '\0' termination characters encountered.
 */
int bstrcmp (const_bstring b0, const_bstring b1) {
int i, v, n;

	if (b0 == NULL || b1 == NULL || b0->data == NULL || b1->data == NULL ||
		b0->slen < 0 || b1->slen < 0) return SHRT_MIN;
	n = b0->slen; if (n > b1->slen) n = b1->slen;
	if (b0->slen == b1->slen && (b0->data == b1->data || b0->slen == 0))
		return BSTR_OK;

	for (i = 0; i < n; i ++) {
		v = ((char) b0->data[i]) - ((char) b1->data[i]);
		if (v != 0) return v;
		if (b0->data[i] == (unsigned char) '\0') return BSTR_OK;
	}

	if (b0->slen > n) return 1;
	if (b1->slen > n) return -1;
	return BSTR_OK;
}

/*  int bstrncmp (const_bstring b0, const_bstring b1, int n)
 *
 *  Compare the string b0 and b1 for at most n characters.  If there is an
 *  error, SHRT_MIN is returned, otherwise a value is returned as if b0 and
 *  b1 were first truncated to at most n characters then bstrcmp was called
 *  with these new strings are paremeters.  If the length of the strings are
 *  different, this function is O(n).  Like its standard C library counter
 *  part strcmp, the comparison does not proceed past any '\0' termination
 *  characters encountered.
 */
int bstrncmp (const_bstring b0, const_bstring b1, int n) {
int i, v, m;

	if (b0 == NULL || b1 == NULL || b0->data == NULL || b1->data == NULL ||
		b0->slen < 0 || b1->slen < 0) return SHRT_MIN;
	m = n;
	if (m > b0->slen) m = b0->slen;
	if (m > b1->slen) m = b1->slen;

	if (b0->data != b1->data) {
		for (i = 0; i < m; i ++) {
			v = ((char) b0->data[i]) - ((char) b1->data[i]);
			if (v != 0) return v;
			if (b0->data[i] == (unsigned char) '\0') return BSTR_OK;
		}
	}

	if (n == m || b0->slen == b1->slen) return BSTR_OK;

	if (b0->slen > m) return 1;
	return -1;
}

/*  bstring bmidstr (const_bstring b, int left, int len)
 *
 *  Create a bstring which is the substring of b starting from position left
 *  and running for a length len (clamped by the end of the bstring b.)  If
 *  b is detectably invalid, then NULL is returned.  The section described
 *  by (left, len) is clamped to the boundaries of b.
 */
bstring bmidstr (const_bstring b, int left, int len) {

	if (b == NULL || b->slen < 0 || b->data == NULL) return NULL;

	if (left < 0) {
		len += left;
		left = 0;
	}

	if (len > b->slen - left) len = b->slen - left;

	if (len <= 0) return bfromcstr ("");
	return blk2bstr (b->data + left, len);
}

/*  int bdelete (bstring b, int pos, int len)
 *
 *  Removes characters from pos to pos+len-1 inclusive and shifts the tail of
 *  the bstring starting from pos+len to pos.  len must be positive for this
 *  call to have any effect.  The section of the string described by (pos,
 *  len) is clamped to boundaries of the bstring b.
 */
int bdelete (bstring b, int pos, int len) {
	/* Clamp to left side of bstring */
	if (pos < 0) {
		len += pos;
		pos = 0;
	}

	if (len < 0 || b == NULL || b->data == NULL || b->slen < 0 ||
	    b->mlen < b->slen || b->mlen <= 0)
		return BSTR_ERR;
	if (len > 0 && pos < b->slen) {
		if (pos + len >= b->slen) {
			b->slen = pos;
		} else {
			bBlockCopy ((char *) (b->data + pos),
			            (char *) (b->data + pos + len),
			            b->slen - (pos+len));
			b->slen -= len;
		}
		b->data[b->slen] = (unsigned char) '\0';
	}
	return BSTR_OK;
}

/*  int bdestroy (bstring b)
 *
 *  Free up the bstring.  Note that if b is detectably invalid or not writable
 *  then no action is performed and BSTR_ERR is returned.  Like a freed memory
 *  allocation, dereferences, writes or any other action on b after it has
 *  been bdestroyed is undefined.
 */
int bdestroy (bstring b) {
	if (b == NULL || b->slen < 0 || b->mlen <= 0 || b->mlen < b->slen ||
	    b->data == NULL)
		return BSTR_ERR;

	bstr__free (b->data);

	/* In case there is any stale usage, there is one more chance to
	   notice this error. */

	b->slen = -1;
	b->mlen = -__LINE__;
	b->data = NULL;

	bstr__free (b);
	return BSTR_OK;
}

/*  int binstr (const_bstring b1, int pos, const_bstring b2)
 *
 *  Search for the bstring b2 in b1 starting from position pos, and searching
 *  forward.  If it is found then return with the first position where it is
 *  found, otherwise return BSTR_ERR.  Note that this is just a brute force
 *  string searcher that does not attempt clever things like the Boyer-Moore
 *  search algorithm.  Because of this there are many degenerate cases where
 *  this can take much longer than it needs to.
 */
int binstr (const_bstring b1, int pos, const_bstring b2) {
int j, ii, ll, lf;
unsigned char * d0;
unsigned char c0;
register unsigned char * d1;
register unsigned char c1;
register int i;

	if (b1 == NULL || b1->data == NULL || b1->slen < 0 ||
	    b2 == NULL || b2->data == NULL || b2->slen < 0) return BSTR_ERR;
	if (b1->slen == pos) return (b2->slen == 0)?pos:BSTR_ERR;
	if (b1->slen < pos || pos < 0) return BSTR_ERR;
	if (b2->slen == 0) return pos;

	/* No space to find such a string? */
	if ((lf = b1->slen - b2->slen + 1) <= pos) return BSTR_ERR;

	/* An obvious alias case */
	if (b1->data == b2->data && pos == 0) return 0;

	i = pos;

	d0 = b2->data;
	d1 = b1->data;
	ll = b2->slen;

	/* Peel off the b2->slen == 1 case */
	c0 = d0[0];
	if (1 == ll) {
		for (;i < lf; i++) if (c0 == d1[i]) return i;
		return BSTR_ERR;
	}

	c1 = c0;
	j = 0;
	lf = b1->slen - 1;

	ii = -1;
	if (i < lf) do {
		/* Unrolled current character test */
		if (c1 != d1[i]) {
			if (c1 != d1[1+i]) {
				i += 2;
				continue;
			}
			i++;
		}

		/* Take note if this is the start of a potential match */
		if (0 == j) ii = i;

		/* Shift the test character down by one */
		j++;
		i++;

		/* If this isn't past the last character continue */
		if (j < ll) {
			c1 = d0[j];
			continue;
		}

		N0:;

		/* If no characters mismatched, then we matched */
		if (i == ii+j) return ii;

		/* Shift back to the beginning */
		i -= j;
		j  = 0;
		c1 = c0;
	} while (i < lf);

	/* Deal with last case if unrolling caused a misalignment */
	if (i == lf && ll == j+1 && c1 == d1[i]) goto N0;

	return BSTR_ERR;
}

/*  int binstrr (const_bstring b1, int pos, const_bstring b2)
 *
 *  Search for the bstring b2 in b1 starting from position pos, and searching
 *  backward.  If it is found then return with the first position where it is
 *  found, otherwise return BSTR_ERR.  Note that this is just a brute force
 *  string searcher that does not attempt clever things like the Boyer-Moore
 *  search algorithm.  Because of this there are many degenerate cases where
 *  this can take much longer than it needs to.
 */
int binstrr (const_bstring b1, int pos, const_bstring b2) {
int j, i, l;
unsigned char * d0, * d1;

	if (b1 == NULL || b1->data == NULL || b1->slen < 0 ||
	    b2 == NULL || b2->data == NULL || b2->slen < 0) return BSTR_ERR;
	if (b1->slen == pos && b2->slen == 0) return pos;
	if (b1->slen < pos || pos < 0) return BSTR_ERR;
	if (b2->slen == 0) return pos;

	/* Obvious alias case */
	if (b1->data == b2->data && pos == 0 && b2->slen <= b1->slen) return 0;

	i = pos;
	if ((l = b1->slen - b2->slen) < 0) return BSTR_ERR;

	/* If no space to find such a string then snap back */
	if (l + 1 <= i) i = l;
	j = 0;

	d0 = b2->data;
	d1 = b1->data;
	l  = b2->slen;

	for (;;) {
		if (d0[j] == d1[i + j]) {
			j ++;
			if (j >= l) return i;
		} else {
			i --;
			if (i < 0) break;
			j=0;
		}
	}

	return BSTR_ERR;
}

/*  int binstrcaseless (const_bstring b1, int pos, const_bstring b2)
 *
 *  Search for the bstring b2 in b1 starting from position pos, and searching
 *  forward but without regard to case.  If it is found then return with the
 *  first position where it is found, otherwise return BSTR_ERR.  Note that
 *  this is just a brute force string searcher that does not attempt clever
 *  things like the Boyer-Moore search algorithm.  Because of this there are
 *  many degenerate cases where this can take much longer than it needs to.
 */
int binstrcaseless (const_bstring b1, int pos, const_bstring b2) {
int j, i, l, ll;
unsigned char * d0, * d1;

	if (b1 == NULL || b1->data == NULL || b1->slen < 0 ||
	    b2 == NULL || b2->data == NULL || b2->slen < 0) return BSTR_ERR;
	if (b1->slen == pos) return (b2->slen == 0)?pos:BSTR_ERR;
	if (b1->slen < pos || pos < 0) return BSTR_ERR;
	if (b2->slen == 0) return pos;

	l = b1->slen - b2->slen + 1;

	/* No space to find such a string? */
	if (l <= pos) return BSTR_ERR;

	/* An obvious alias case */
	if (b1->data == b2->data && pos == 0) return BSTR_OK;

	i = pos;
	j = 0;

	d0 = b2->data;
	d1 = b1->data;
	ll = b2->slen;

	for (;;) {
		if (d0[j] == d1[i + j] || downcase (d0[j]) == downcase (d1[i + j])) {
			j ++;
			if (j >= ll) return i;
		} else {
			i ++;
			if (i >= l) break;
			j=0;
		}
	}

	return BSTR_ERR;
}

/*  int binstrrcaseless (const_bstring b1, int pos, const_bstring b2)
 *
 *  Search for the bstring b2 in b1 starting from position pos, and searching
 *  backward but without regard to case.  If it is found then return with the
 *  first position where it is found, otherwise return BSTR_ERR.  Note that
 *  this is just a brute force string searcher that does not attempt clever
 *  things like the Boyer-Moore search algorithm.  Because of this there are
 *  many degenerate cases where this can take much longer than it needs to.
 */
int binstrrcaseless (const_bstring b1, int pos, const_bstring b2) {
int j, i, l;
unsigned char * d0, * d1;

	if (b1 == NULL || b1->data == NULL || b1->slen < 0 ||
	    b2 == NULL || b2->data == NULL || b2->slen < 0) return BSTR_ERR;
	if (b1->slen == pos && b2->slen == 0) return pos;
	if (b1->slen < pos || pos < 0) return BSTR_ERR;
	if (b2->slen == 0) return pos;

	/* Obvious alias case */
	if (b1->data == b2->data && pos == 0 && b2->slen <= b1->slen)
		return BSTR_OK;

	i = pos;
	if ((l = b1->slen - b2->slen) < 0) return BSTR_ERR;

	/* If no space to find such a string then snap back */
	if (l + 1 <= i) i = l;
	j = 0;

	d0 = b2->data;
	d1 = b1->data;
	l  = b2->slen;

	for (;;) {
		if (d0[j] == d1[i + j] || downcase (d0[j]) == downcase (d1[i + j])) {
			j ++;
			if (j >= l) return i;
		} else {
			i --;
			if (i < 0) break;
			j=0;
		}
	}

	return BSTR_ERR;
}


/*  int bstrchrp (const_bstring b, int c, int pos)
 *
 *  Search for the character c in b forwards from the position pos
 *  (inclusive).
 */
int bstrchrp (const_bstring b, int c, int pos) {
unsigned char * p;

	if (b == NULL || b->data == NULL || b->slen <= pos || pos < 0)
		return BSTR_ERR;
	p = (unsigned char *) bstr__memchr ((b->data + pos), (unsigned char) c,
		                                (b->slen - pos));
	if (p) return (int) (p - b->data);
	return BSTR_ERR;
}

/*  int bstrrchrp (const_bstring b, int c, int pos)
 *
 *  Search for the character c in b backwards from the position pos in string
 *  (inclusive).
 */
int bstrrchrp (const_bstring b, int c, int pos) {
int i;

	if (b == NULL || b->data == NULL || b->slen <= pos || pos < 0)
		return BSTR_ERR;
	for (i=pos; i >= 0; i--) {
		if (b->data[i] == (unsigned char) c) return i;
	}
	return BSTR_ERR;
}

#if !defined (BSTRLIB_AGGRESSIVE_MEMORY_FOR_SPEED_TRADEOFF)
#define LONG_LOG_BITS_QTY (3)
#define LONG_BITS_QTY (1 << LONG_LOG_BITS_QTY)
#define LONG_TYPE unsigned char

#define CFCLEN ((1 << CHAR_BIT) / LONG_BITS_QTY)
struct charField { LONG_TYPE content[CFCLEN]; };
#define testInCharField(cf,c) ((cf)->content[(c) >> LONG_LOG_BITS_QTY] & \
	                           (((long)1) << ((c) & (LONG_BITS_QTY-1))))
#define setInCharField(cf,idx) { \
	unsigned int c = (unsigned int) (idx); \
	(cf)->content[c >> LONG_LOG_BITS_QTY] |= \
		(LONG_TYPE) (1ul << (c & (LONG_BITS_QTY-1))); \
}

#else

#define CFCLEN (1 << CHAR_BIT)
struct charField { unsigned char content[CFCLEN]; };
#define testInCharField(cf,c) ((cf)->content[(unsigned char) (c)])
#define setInCharField(cf,idx) (cf)->content[(unsigned int) (idx)] = ~0

#endif

/* Convert a bstring to charField */
static int buildCharField (struct charField * cf, const_bstring b) {
int i;
	if (b == NULL || b->data == NULL || b->slen <= 0) return BSTR_ERR;
	memset ((void *) cf->content, 0, sizeof (struct charField));
	for (i=0; i < b->slen; i++) {
		setInCharField (cf, b->data[i]);
	}
	return BSTR_OK;
}

static void invertCharField (struct charField * cf) {
int i;
	for (i=0; i < CFCLEN; i++) cf->content[i] = ~cf->content[i];
}

/* Inner engine for binchr */
static int binchrCF (const unsigned char * data, int len, int pos,
					 const struct charField * cf) {
int i;
	for (i=pos; i < len; i++) {
		unsigned char c = (unsigned char) data[i];
		if (testInCharField (cf, c)) return i;
	}
	return BSTR_ERR;
}

/*  int binchr (const_bstring b0, int pos, const_bstring b1);
 *
 *  Search for the first position in b0 starting from pos or after, in which
 *  one of the characters in b1 is found and return it.  If such a position
 *  does not exist in b0, then BSTR_ERR is returned.
 */
int binchr (const_bstring b0, int pos, const_bstring b1) {
struct charField chrs;
	if (pos < 0 || b0 == NULL || b0->data == NULL ||
	    b0->slen <= pos) return BSTR_ERR;
	if (1 == b1->slen) return bstrchrp (b0, b1->data[0], pos);
	if (0 > buildCharField (&chrs, b1)) return BSTR_ERR;
	return binchrCF (b0->data, b0->slen, pos, &chrs);
}

/* Inner engine for binchrr */
static int binchrrCF (const unsigned char * data, int pos,
                      const struct charField * cf) {
int i;
	for (i=pos; i >= 0; i--) {
		unsigned int c = (unsigned int) data[i];
		if (testInCharField (cf, c)) return i;
	}
	return BSTR_ERR;
}

/*  int binchrr (const_bstring b0, int pos, const_bstring b1);
 *
 *  Search for the last position in b0 no greater than pos, in which one of
 *  the characters in b1 is found and return it.  If such a position does not
 *  exist in b0, then BSTR_ERR is returned.
 */
int binchrr (const_bstring b0, int pos, const_bstring b1) {
struct charField chrs;
	if (pos < 0 || b0 == NULL || b0->data == NULL || b1 == NULL ||
	    b0->slen < pos) return BSTR_ERR;
	if (pos == b0->slen) pos--;
	if (1 == b1->slen) return bstrrchrp (b0, b1->data[0], pos);
	if (0 > buildCharField (&chrs, b1)) return BSTR_ERR;
	return binchrrCF (b0->data, pos, &chrs);
}

/*  int bninchr (const_bstring b0, int pos, const_bstring b1);
 *
 *  Search for the first position in b0 starting from pos or after, in which
 *  none of the characters in b1 is found and return it.  If such a position
 *  does not exist in b0, then BSTR_ERR is returned.
 */
int bninchr (const_bstring b0, int pos, const_bstring b1) {
struct charField chrs;
	if (pos < 0 || b0 == NULL || b0->data == NULL ||
	    b0->slen <= pos) return BSTR_ERR;
	if (buildCharField (&chrs, b1) < 0) return BSTR_ERR;
	invertCharField (&chrs);
	return binchrCF (b0->data, b0->slen, pos, &chrs);
}

/*  int bninchrr (const_bstring b0, int pos, const_bstring b1);
 *
 *  Search for the last position in b0 no greater than pos, in which none of
 *  the characters in b1 is found and return it.  If such a position does not
 *  exist in b0, then BSTR_ERR is returned.
 */
int bninchrr (const_bstring b0, int pos, const_bstring b1) {
struct charField chrs;
	if (pos < 0 || b0 == NULL || b0->data == NULL ||
	    b0->slen < pos) return BSTR_ERR;
	if (pos == b0->slen) pos--;
	if (buildCharField (&chrs, b1) < 0) return BSTR_ERR;
	invertCharField (&chrs);
	return binchrrCF (b0->data, pos, &chrs);
}

/*  int bsetstr (bstring b0, int pos, bstring b1, unsigned char fill)
 *
 *  Overwrite the string b0 starting at position pos with the string b1. If
 *  the position pos is past the end of b0, then the character "fill" is
 *  appended as necessary to make up the gap between the end of b0 and pos.
 *  If b1 is NULL, it behaves as if it were a 0-length string.
 */
int bsetstr (bstring b0, int pos, const_bstring b1, unsigned char fill) {
int d, newlen;
ptrdiff_t pd;
bstring aux = (bstring) b1;

	if (pos < 0 || b0 == NULL || b0->slen < 0 || NULL == b0->data ||
	    b0->mlen < b0->slen || b0->mlen <= 0) return BSTR_ERR;
	if (b1 != NULL && (b1->slen < 0 || b1->data == NULL)) return BSTR_ERR;

	d = pos;

	/* Aliasing case */
	if (NULL != aux) {
		if ((pd = (ptrdiff_t) (b1->data - b0->data)) >= 0 &&
		    pd < (ptrdiff_t) b0->mlen) {
			if (NULL == (aux = bstrcpy (b1))) return BSTR_ERR;
		}
		d += aux->slen;
	}

	/* Increase memory size if necessary */
	if (balloc (b0, d + 1) != BSTR_OK) {
		if (aux != b1) bdestroy (aux);
		return BSTR_ERR;
	}

	newlen = b0->slen;

	/* Fill in "fill" character as necessary */
	if (pos > newlen) {
		bstr__memset (b0->data + b0->slen, (int) fill,
		              (size_t) (pos - b0->slen));
		newlen = pos;
	}

	/* Copy b1 to position pos in b0. */
	if (aux != NULL) {
		bBlockCopy ((char *) (b0->data + pos), (char *) aux->data, aux->slen);
		if (aux != b1) bdestroy (aux);
	}

	/* Indicate the potentially increased size of b0 */
	if (d > newlen) newlen = d;

	b0->slen = newlen;
	b0->data[newlen] = (unsigned char) '\0';

	return BSTR_OK;
}

/*  int binsertblk (bstring b, int pos, const void * blk, int len,
 *                  unsigned char fill)
 *
 *  Inserts the block of characters at blk with length len into b at position
 *  pos.  If the position pos is past the end of b, then the character "fill"
 *  is appended as necessary to make up the gap between the end of b1 and pos.
 *  Unlike bsetstr, binsert does not allow b2 to be NULL.
 */
int binsertblk (bstring b, int pos, const void * blk, int len,
                unsigned char fill) {
int d, l;
unsigned char* aux = (unsigned char*) blk;

	if (b == NULL || blk == NULL || pos < 0 || len < 0 || b->slen < 0 ||
	    b->mlen <= 0 || b->mlen < b->slen) return BSTR_ERR;

	/* Compute the two possible end pointers */
	d = b->slen + len;
	l = pos + len;
	if ((d|l) < 0) return BSTR_ERR; /* Integer wrap around. */

	/* Aliasing case */
	if (((size_t) ((unsigned char*) blk + len)) >= ((size_t) b->data) &&
		((size_t) blk) < ((size_t) (b->data + b->mlen))) {
		if (NULL == (aux = (unsigned char*) bstr__alloc (len)))
			return BSTR_ERR;
		bstr__memcpy (aux, blk, len);
	}

	if (l > d) {
		/* Inserting past the end of the string */
		if (balloc (b, l + 1) != BSTR_OK) {
			if (aux != (unsigned char*) blk) bstr__free (aux);
			return BSTR_ERR;
		}
		bstr__memset (b->data + b->slen, (int) fill,
		              (size_t) (pos - b->slen));
		b->slen = l;
	} else {
		/* Inserting in the middle of the string */
		if (balloc (b, d + 1) != BSTR_OK) {
			if (aux != (unsigned char*) blk) bstr__free (aux);
			return BSTR_ERR;
		}
		bBlockCopy (b->data + l, b->data + pos, d - l);
		b->slen = d;
	}
	bBlockCopy (b->data + pos, aux, len);
	b->data[b->slen] = (unsigned char) '\0';
	if (aux != (unsigned char*) blk) bstr__free (aux);
	return BSTR_OK;
}

/*  int binsert (bstring b1, int pos, const_bstring b2, unsigned char fill)
 *
 *  Inserts the string b2 into b1 at position pos.  If the position pos is
 *  past the end of b1, then the character "fill" is appended as necessary to
 *  make up the gap between the end of b1 and pos.  Unlike bsetstr, binsert
 *  does not allow b2 to be NULL.
 */
int binsert (bstring b1, int pos, const_bstring b2, unsigned char fill) {
	if (NULL == b2 || (b2->mlen > 0 && b2->slen > b2->mlen)) return BSTR_ERR;
	return binsertblk (b1, pos, b2->data, b2->slen, fill);
}

/*  int breplace (bstring b1, int pos, int len, bstring b2,
 *                unsigned char fill)
 *
 *  Replace a section of a string from pos for a length len with the string
 *  b2. fill is used is pos > b1->slen.
 */
int breplace (bstring b1, int pos, int len, const_bstring b2,
              unsigned char fill) {
int pl, ret;
ptrdiff_t pd;
bstring aux = (bstring) b2;

	if (pos < 0 || len < 0) return BSTR_ERR;
	if (pos > INT_MAX - len) return BSTR_ERR; /* Overflow */
	pl = pos + len;
	if (b1 == NULL || b2 == NULL || b1->data == NULL || b2->data == NULL ||
	    b1->slen < 0 || b2->slen < 0 || b1->mlen < b1->slen ||
	    b1->mlen <= 0) return BSTR_ERR;

	/* Straddles the end? */
	if (pl >= b1->slen) {
		if ((ret = bsetstr (b1, pos, b2, fill)) < 0) return ret;
		if (pos + b2->slen < b1->slen) {
			b1->slen = pos + b2->slen;
			b1->data[b1->slen] = (unsigned char) '\0';
		}
		return ret;
	}

	/* Aliasing case */
	if ((pd = (ptrdiff_t) (b2->data - b1->data)) >= 0 &&
	    pd < (ptrdiff_t) b1->slen) {
		if (NULL == (aux = bstrcpy (b2))) return BSTR_ERR;
	}

	if (aux->slen > len) {
		if (balloc (b1, b1->slen + aux->slen - len) != BSTR_OK) {
			if (aux != b2) bdestroy (aux);
			return BSTR_ERR;
		}
	}

	if (aux->slen != len) bstr__memmove (b1->data + pos + aux->slen,
	                                     b1->data + pos + len,
	                                     b1->slen - (pos + len));
	bstr__memcpy (b1->data + pos, aux->data, aux->slen);
	b1->slen += aux->slen - len;
	b1->data[b1->slen] = (unsigned char) '\0';
	if (aux != b2) bdestroy (aux);
	return BSTR_OK;
}

/*
 *  findreplaceengine is used to implement bfindreplace and
 *  bfindreplacecaseless. It works by breaking the three cases of
 *  expansion, reduction and replacement, and solving each of these
 *  in the most efficient way possible.
 */

typedef int (*instr_fnptr) (const_bstring s1, int pos, const_bstring s2);

#define INITIAL_STATIC_FIND_INDEX_COUNT 32

static int findreplaceengine (bstring b, const_bstring find,
                              const_bstring repl, int pos,
                              instr_fnptr instr) {
int i, ret, slen, mlen, delta, acc;
int * d;
int static_d[INITIAL_STATIC_FIND_INDEX_COUNT+1]; /* This +1 is for LINT. */
ptrdiff_t pd;
bstring auxf = (bstring) find;
bstring auxr = (bstring) repl;

	if (b == NULL || b->data == NULL || find == NULL ||
		find->data == NULL || repl == NULL || repl->data == NULL ||
		pos < 0 || find->slen <= 0 || b->mlen <= 0 || b->slen > b->mlen ||
		b->slen < 0 || repl->slen < 0) return BSTR_ERR;
	if (pos > b->slen - find->slen) return BSTR_OK;

	/* Alias with find string */
	pd = (ptrdiff_t) (find->data - b->data);
	if ((ptrdiff_t) (pos - find->slen) < pd && pd < (ptrdiff_t) b->slen) {
		if (NULL == (auxf = bstrcpy (find))) return BSTR_ERR;
	}

	/* Alias with repl string */
	pd = (ptrdiff_t) (repl->data - b->data);
	if ((ptrdiff_t) (pos - repl->slen) < pd && pd < (ptrdiff_t) b->slen) {
		if (NULL == (auxr = bstrcpy (repl))) {
			if (auxf != find) bdestroy (auxf);
			return BSTR_ERR;
		}
	}

	delta = auxf->slen - auxr->slen;

	/* in-place replacement since find and replace strings are of equal
	   length */
	if (delta == 0) {
		while ((pos = instr (b, pos, auxf)) >= 0) {
			bstr__memcpy (b->data + pos, auxr->data, auxr->slen);
			pos += auxf->slen;
		}
		if (auxf != find) bdestroy (auxf);
		if (auxr != repl) bdestroy (auxr);
		return BSTR_OK;
	}

	/* shrinking replacement since auxf->slen > auxr->slen */
	if (delta > 0) {
		acc = 0;

		while ((i = instr (b, pos, auxf)) >= 0) {
			if (acc && i > pos)
				bstr__memmove (b->data + pos - acc, b->data + pos, i - pos);
			if (auxr->slen)
				bstr__memcpy (b->data + i - acc, auxr->data, auxr->slen);
			acc += delta;
			pos = i + auxf->slen;
		}

		if (acc) {
			i = b->slen;
			if (i > pos)
				bstr__memmove (b->data + pos - acc, b->data + pos, i - pos);
			b->slen -= acc;
			b->data[b->slen] = (unsigned char) '\0';
		}

		if (auxf != find) bdestroy (auxf);
		if (auxr != repl) bdestroy (auxr);
		return BSTR_OK;
	}

	/* expanding replacement since find->slen < repl->slen.  Its a lot
	   more complicated.  This works by first finding all the matches and
	   storing them to a growable array, then doing at most one resize of
	   the destination bstring and then performing the direct memory transfers
	   of the string segment pieces to form the final result. The growable
	   array of matches uses a deferred doubling reallocing strategy.  What
	   this means is that it starts as a reasonably fixed sized auto array in
	   the hopes that many if not most cases will never need to grow this
	   array.  But it switches as soon as the bounds of the array will be
	   exceeded.  An extra find result is always appended to this array that
	   corresponds to the end of the destination string, so slen is checked
	   against mlen - 1 rather than mlen before resizing.
	*/

	mlen = INITIAL_STATIC_FIND_INDEX_COUNT;
	d = (int *) static_d; /* Avoid malloc for trivial/initial cases */
	acc = slen = 0;

	while ((pos = instr (b, pos, auxf)) >= 0) {
		if (slen >= mlen - 1) {
			int *t;
			int sl;
			/* Overflow */
			if (mlen > (INT_MAX / sizeof(int *)) / 2) {
				ret = BSTR_ERR;
				goto done;
			}
			mlen += mlen;
			sl = sizeof (int *) * mlen;
			if (static_d == d) d = NULL; /* static_d cannot be realloced */
			if (NULL == (t = (int *) bstr__realloc (d, sl))) {
				ret = BSTR_ERR;
				goto done;
			}
			if (NULL == d) bstr__memcpy (t, static_d, sizeof (static_d));
			d = t;
		}
		d[slen] = pos;
		slen++;
		acc -= delta;
		pos += auxf->slen;
		if (pos < 0 || acc < 0) {
			ret = BSTR_ERR;
			goto done;
		}
	}

	/* slen <= INITIAL_STATIC_INDEX_COUNT-1 or mlen-1 here. */
	d[slen] = b->slen;

	if (BSTR_OK == (ret = balloc (b, b->slen + acc + 1))) {
		b->slen += acc;
		for (i = slen-1; i >= 0; i--) {
			int s, l;
			s = d[i] + auxf->slen;
			l = d[i+1] - s; /* d[slen] may be accessed here. */
			if (l) {
				bstr__memmove (b->data + s + acc, b->data + s, l);
			}
			if (auxr->slen) {
				bstr__memmove (b->data + s + acc - auxr->slen,
				               auxr->data, auxr->slen);
			}
			acc += delta;
		}
		b->data[b->slen] = (unsigned char) '\0';
	}

	done:;
	if (static_d != d) bstr__free (d);
	if (auxf != find) bdestroy (auxf);
	if (auxr != repl) bdestroy (auxr);
	return ret;
}

/*  int bfindreplace (bstring b, const_bstring find, const_bstring repl,
 *                    int pos)
 *
 *  Replace all occurrences of a find string with a replace string after a
 *  given point in a bstring.
 */
int bfindreplace (bstring b, const_bstring find, const_bstring repl,
                  int pos) {
	return findreplaceengine (b, find, repl, pos, binstr);
}

/*  int bfindreplacecaseless (bstring b, const_bstring find,
 *                            const_bstring repl, int pos)
 *
 *  Replace all occurrences of a find string, ignoring case, with a replace
 *  string after a given point in a bstring.
 */
int bfindreplacecaseless (bstring b, const_bstring find, const_bstring repl,
                          int pos) {
	return findreplaceengine (b, find, repl, pos, binstrcaseless);
}

/*  int binsertch (bstring b, int pos, int len, unsigned char fill)
 *
 *  Inserts the character fill repeatedly into b at position pos for a
 *  length len.  If the position pos is past the end of b, then the
 *  character "fill" is appended as necessary to make up the gap between the
 *  end of b and the position pos + len.
 */
int binsertch (bstring b, int pos, int len, unsigned char fill) {
int d, l, i;

	if (pos < 0 || b == NULL || b->slen < 0 || b->mlen < b->slen ||
	    b->mlen <= 0 || len < 0) return BSTR_ERR;

	/* Compute the two possible end pointers */
	d = b->slen + len;
	l = pos + len;
	if ((d|l) < 0) return BSTR_ERR;

	if (l > d) {
		/* Inserting past the end of the string */
		if (balloc (b, l + 1) != BSTR_OK) return BSTR_ERR;
		pos = b->slen;
		b->slen = l;
	} else {
		/* Inserting in the middle of the string */
		if (balloc (b, d + 1) != BSTR_OK) return BSTR_ERR;
		for (i = d - 1; i >= l; i--) {
			b->data[i] = b->data[i - len];
		}
		b->slen = d;
	}

	for (i=pos; i < l; i++) b->data[i] = fill;
	b->data[b->slen] = (unsigned char) '\0';
	return BSTR_OK;
}

/*  int bpattern (bstring b, int len)
 *
 *  Replicate the bstring, b in place, end to end repeatedly until it
 *  surpasses len characters, then chop the result to exactly len characters.
 *  This function operates in-place.  The function will return with BSTR_ERR
 *  if b is NULL or of length 0, otherwise BSTR_OK is returned.
 */
int bpattern (bstring b, int len) {
int i, d;

	d = blength (b);
	if (d <= 0 || len < 0 || balloc (b, len + 1) != BSTR_OK) return BSTR_ERR;
	if (len > 0) {
		if (d == 1) return bsetstr (b, len, NULL, b->data[0]);
		for (i = d; i < len; i++) b->data[i] = b->data[i - d];
	}
	b->data[len] = (unsigned char) '\0';
	b->slen = len;
	return BSTR_OK;
}

#define BS_BUFF_SZ (1024)

/*  int breada (bstring b, bNread readPtr, void * parm)
 *
 *  Use a finite buffer fread-like function readPtr to concatenate to the
 *  bstring b the entire contents of file-like source data in a roughly
 *  efficient way.
 */
int breada (bstring b, bNread readPtr, void * parm) {
int i, l, n;

	if (b == NULL || b->mlen <= 0 || b->slen < 0 || b->mlen < b->slen ||
	    readPtr == NULL) return BSTR_ERR;

	i = b->slen;
	for (n=i+16; ; n += ((n < BS_BUFF_SZ) ? n : BS_BUFF_SZ)) {
		if (BSTR_OK != balloc (b, n + 1)) return BSTR_ERR;
		l = (int) readPtr ((void *) (b->data + i), 1, n - i, parm);
		i += l;
		b->slen = i;
		if (i < n) break;
	}

	b->data[i] = (unsigned char) '\0';
	return BSTR_OK;
}

/*  bstring bread (bNread readPtr, void * parm)
 *
 *  Use a finite buffer fread-like function readPtr to create a bstring
 *  filled with the entire contents of file-like source data in a roughly
 *  efficient way.
 */
bstring bread (bNread readPtr, void * parm) {
bstring buff;

	if (0 > breada (buff = bfromcstr (""), readPtr, parm)) {
		bdestroy (buff);
		return NULL;
	}
	return buff;
}

/*  int bassigngets (bstring b, bNgetc getcPtr, void * parm, char terminator)
 *
 *  Use an fgetc-like single character stream reading function (getcPtr) to
 *  obtain a sequence of characters which are concatenated to the end of the
 *  bstring b.  The stream read is terminated by the passed in terminator
 *  parameter.
 *
 *  If getcPtr returns with a negative number, or the terminator character
 *  (which is appended) is read, then the stream reading is halted and the
 *  function returns with a partial result in b.  If there is an empty partial
 *  result, 1 is returned.  If no characters are read, or there is some other
 *  detectable error, BSTR_ERR is returned.
 */
int bassigngets (bstring b, bNgetc getcPtr, void * parm, char terminator) {
int c, d, e;

	if (b == NULL || b->mlen <= 0 || b->slen < 0 || b->mlen < b->slen ||
	    getcPtr == NULL) return BSTR_ERR;
	d = 0;
	e = b->mlen - 2;

	while ((c = getcPtr (parm)) >= 0) {
		if (d > e) {
			b->slen = d;
			if (balloc (b, d + 2) != BSTR_OK) return BSTR_ERR;
			e = b->mlen - 2;
		}
		b->data[d] = (unsigned char) c;
		d++;
		if (c == terminator) break;
	}

	b->data[d] = (unsigned char) '\0';
	b->slen = d;

	return d == 0 && c < 0;
}

/*  int bgetsa (bstring b, bNgetc getcPtr, void * parm, char terminator)
 *
 *  Use an fgetc-like single character stream reading function (getcPtr) to
 *  obtain a sequence of characters which are concatenated to the end of the
 *  bstring b.  The stream read is terminated by the passed in terminator
 *  parameter.
 *
 *  If getcPtr returns with a negative number, or the terminator character
 *  (which is appended) is read, then the stream reading is halted and the
 *  function returns with a partial result concatentated to b.  If there is
 *  an empty partial result, 1 is returned.  If no characters are read, or
 *  there is some other detectable error, BSTR_ERR is returned.
 */
int bgetsa (bstring b, bNgetc getcPtr, void * parm, char terminator) {
int c, d, e;

	if (b == NULL || b->mlen <= 0 || b->slen < 0 || b->mlen < b->slen ||
	    getcPtr == NULL) return BSTR_ERR;
	d = b->slen;
	e = b->mlen - 2;

	while ((c = getcPtr (parm)) >= 0) {
		if (d > e) {
			b->slen = d;
			if (balloc (b, d + 2) != BSTR_OK) return BSTR_ERR;
			e = b->mlen - 2;
		}
		b->data[d] = (unsigned char) c;
		d++;
		if (c == terminator) break;
	}

	b->data[d] = (unsigned char) '\0';
	b->slen = d;

	return d == 0 && c < 0;
}

/*  bstring bgets (bNgetc getcPtr, void * parm, char terminator)
 *
 *  Use an fgetc-like single character stream reading function (getcPtr) to
 *  obtain a sequence of characters which are concatenated into a bstring.
 *  The stream read is terminated by the passed in terminator function.
 *
 *  If getcPtr returns with a negative number, or the terminator character
 *  (which is appended) is read, then the stream reading is halted and the
 *  result obtained thus far is returned.  If no characters are read, or
 *  there is some other detectable error, NULL is returned.
 */
bstring bgets (bNgetc getcPtr, void * parm, char terminator) {
bstring buff;

	if (0 > bgetsa (buff = bfromcstr (""), getcPtr, parm, terminator) ||
	    0 >= buff->slen) {
		bdestroy (buff);
		buff = NULL;
	}
	return buff;
}

struct bStream {
	bstring buff;		/* Buffer for over-reads */
	void * parm;		/* The stream handle for core stream */
	bNread readFnPtr;	/* fread compatible fnptr for core stream */
	int isEOF;			/* track file's EOF state */
	int maxBuffSz;
};

/*  struct bStream * bsopen (bNread readPtr, void * parm)
 *
 *  Wrap a given open stream (described by a fread compatible function
 *  pointer and stream handle) into an open bStream suitable for the bstring
 *  library streaming functions.
 */
struct bStream * bsopen (bNread readPtr, void * parm) {
struct bStream * s;

	if (readPtr == NULL) return NULL;
	s = (struct bStream *) bstr__alloc (sizeof (struct bStream));
	if (s == NULL) return NULL;
	s->parm = parm;
	s->buff = bfromcstr ("");
	s->readFnPtr = readPtr;
	s->maxBuffSz = BS_BUFF_SZ;
	s->isEOF = 0;
	return s;
}

/*  int bsbufflength (struct bStream * s, int sz)
 *
 *  Set the length of the buffer used by the bStream.  If sz is zero, the
 *  length is not set.  This function returns with the previous length.
 */
int bsbufflength (struct bStream * s, int sz) {
int oldSz;
	if (s == NULL || sz < 0) return BSTR_ERR;
	oldSz = s->maxBuffSz;
	if (sz > 0) s->maxBuffSz = sz;
	return oldSz;
}

int bseof (const struct bStream * s) {
	if (s == NULL || s->readFnPtr == NULL) return BSTR_ERR;
	return s->isEOF && (s->buff->slen == 0);
}

/*  void * bsclose (struct bStream * s)
 *
 *  Close the bStream, and return the handle to the stream that was originally
 *  used to open the given stream.
 */
void * bsclose (struct bStream * s) {
void * parm;
	if (s == NULL) return NULL;
	s->readFnPtr = NULL;
	if (s->buff) bdestroy (s->buff);
	s->buff = NULL;
	parm = s->parm;
	s->parm = NULL;
	s->isEOF = 1;
	bstr__free (s);
	return parm;
}

/*  int bsreadlna (bstring r, struct bStream * s, char terminator)
 *
 *  Read a bstring terminated by the terminator character or the end of the
 *  stream from the bStream (s) and return it into the parameter r.  This
 *  function may read additional characters from the core stream that are not
 *  returned, but will be retained for subsequent read operations.
 */
int bsreadlna (bstring r, struct bStream * s, char terminator) {
int i, l, ret, rlo;
char * b;
struct tagbstring x;

	if (s == NULL || s->buff == NULL || r == NULL || r->mlen <= 0 ||
	    r->slen < 0 || r->mlen < r->slen) return BSTR_ERR;
	l = s->buff->slen;
	if (BSTR_OK != balloc (s->buff, s->maxBuffSz + 1)) return BSTR_ERR;
	b = (char *) s->buff->data;
	x.data = (unsigned char *) b;

	/* First check if the current buffer holds the terminator */
	b[l] = terminator; /* Set sentinel */
	for (i=0; b[i] != terminator; i++) ;
	if (i < l) {
		x.slen = i + 1;
		ret = bconcat (r, &x);
		s->buff->slen = l;
		if (BSTR_OK == ret) bdelete (s->buff, 0, i + 1);
		return BSTR_OK;
	}

	rlo = r->slen;

	/* If not then just concatenate the entire buffer to the output */
	x.slen = l;
	if (BSTR_OK != bconcat (r, &x)) return BSTR_ERR;

	/* Perform direct in-place reads into the destination to allow for
	   the minimum of data-copies */
	for (;;) {
		if (BSTR_OK != balloc (r, r->slen + s->maxBuffSz + 1))
		    return BSTR_ERR;
		b = (char *) (r->data + r->slen);
		l = (int) s->readFnPtr (b, 1, s->maxBuffSz, s->parm);
		if (l <= 0) {
			r->data[r->slen] = (unsigned char) '\0';
			s->buff->slen = 0;
			s->isEOF = 1;
			/* If nothing was read return with an error message */
			return BSTR_ERR & -(r->slen == rlo);
		}
		b[l] = terminator; /* Set sentinel */
		for (i=0; b[i] != terminator; i++) ;
		if (i < l) break;
		r->slen += l;
	}

	/* Terminator found, push over-read back to buffer */
	i++;
	r->slen += i;
	s->buff->slen = l - i;
	bstr__memcpy (s->buff->data, b + i, l - i);
	r->data[r->slen] = (unsigned char) '\0';
	return BSTR_OK;
}

/*  int bsreadlnsa (bstring r, struct bStream * s, bstring term)
 *
 *  Read a bstring terminated by any character in the term string or the end
 *  of the stream from the bStream (s) and return it into the parameter r.
 *  This function may read additional characters from the core stream that
 *  are not returned, but will be retained for subsequent read operations.
 */
int bsreadlnsa (bstring r, struct bStream * s, const_bstring term) {
int i, l, ret, rlo;
unsigned char * b;
struct tagbstring x;
struct charField cf;

	if (s == NULL || s->buff == NULL || r == NULL || term == NULL ||
	    term->data == NULL || r->mlen <= 0 || r->slen < 0 ||
	    r->mlen < r->slen) return BSTR_ERR;
	if (term->slen == 1) return bsreadlna (r, s, term->data[0]);
	if (term->slen < 1 || buildCharField (&cf, term)) return BSTR_ERR;

	l = s->buff->slen;
	if (BSTR_OK != balloc (s->buff, s->maxBuffSz + 1)) return BSTR_ERR;
	b = (unsigned char *) s->buff->data;
	x.data = b;

	/* First check if the current buffer holds the terminator */
	b[l] = term->data[0]; /* Set sentinel */
	for (i=0; !testInCharField (&cf, b[i]); i++) ;
	if (i < l) {
		x.slen = i + 1;
		ret = bconcat (r, &x);
		s->buff->slen = l;
		if (BSTR_OK == ret) bdelete (s->buff, 0, i + 1);
		return BSTR_OK;
	}

	rlo = r->slen;

	/* If not then just concatenate the entire buffer to the output */
	x.slen = l;
	if (BSTR_OK != bconcat (r, &x)) return BSTR_ERR;

	/* Perform direct in-place reads into the destination to allow for
	   the minimum of data-copies */
	for (;;) {
		if (BSTR_OK != balloc (r, r->slen + s->maxBuffSz + 1))
		    return BSTR_ERR;
		b = (unsigned char *) (r->data + r->slen);
		l = (int) s->readFnPtr (b, 1, s->maxBuffSz, s->parm);
		if (l <= 0) {
			r->data[r->slen] = (unsigned char) '\0';
			s->buff->slen = 0;
			s->isEOF = 1;
			/* If nothing was read return with an error message */
			return BSTR_ERR & -(r->slen == rlo);
		}

		b[l] = term->data[0]; /* Set sentinel */
		for (i=0; !testInCharField (&cf, b[i]); i++) ;
		if (i < l) break;
		r->slen += l;
	}

	/* Terminator found, push over-read back to buffer */
	i++;
	r->slen += i;
	s->buff->slen = l - i;
	bstr__memcpy (s->buff->data, b + i, l - i);
	r->data[r->slen] = (unsigned char) '\0';
	return BSTR_OK;
}

/*  int bsreada (bstring r, struct bStream * s, int n)
 *
 *  Read a bstring of length n (or, if it is fewer, as many bytes as is
 *  remaining) from the bStream.  This function may read additional
 *  characters from the core stream that are not returned, but will be
 *  retained for subsequent read operations.  This function will not read
 *  additional characters from the core stream beyond virtual stream pointer.
 */
int bsreada (bstring r, struct bStream * s, int n) {
int l, ret, orslen;
char * b;
struct tagbstring x;

	if (s == NULL || s->buff == NULL || r == NULL || r->mlen <= 0
	 || r->slen < 0 || r->mlen < r->slen || n <= 0) return BSTR_ERR;

	if (n > INT_MAX - r->slen) return BSTR_ERR;
	n += r->slen;

	l = s->buff->slen;

	orslen = r->slen;

	if (0 == l) {
		if (s->isEOF) return BSTR_ERR;
		if (r->mlen > n) {
			l = (int) s->readFnPtr (r->data + r->slen, 1, n - r->slen,
			                        s->parm);
			if (0 >= l || l > n - r->slen) {
				s->isEOF = 1;
				return BSTR_ERR;
			}
			r->slen += l;
			r->data[r->slen] = (unsigned char) '\0';
			return 0;
		}
	}

	if (BSTR_OK != balloc (s->buff, s->maxBuffSz + 1)) return BSTR_ERR;
	b = (char *) s->buff->data;
	x.data = (unsigned char *) b;

	do {
		if (l + r->slen >= n) {
			x.slen = n - r->slen;
			ret = bconcat (r, &x);
			s->buff->slen = l;
			if (BSTR_OK == ret) bdelete (s->buff, 0, x.slen);
			return BSTR_ERR & -(r->slen == orslen);
		}

		x.slen = l;
		if (BSTR_OK != bconcat (r, &x)) break;

		l = n - r->slen;
		if (l > s->maxBuffSz) l = s->maxBuffSz;

		l = (int) s->readFnPtr (b, 1, l, s->parm);

	} while (l > 0);
	if (l < 0) l = 0;
	if (l == 0) s->isEOF = 1;
	s->buff->slen = l;
	return BSTR_ERR & -(r->slen == orslen);
}

/*  int bsreadln (bstring r, struct bStream * s, char terminator)
 *
 *  Read a bstring terminated by the terminator character or the end of the
 *  stream from the bStream (s) and return it into the parameter r.  This
 *  function may read additional characters from the core stream that are not
 *  returned, but will be retained for subsequent read operations.
 */
int bsreadln (bstring r, struct bStream * s, char terminator) {
	if (s == NULL || s->buff == NULL || r == NULL || r->mlen <= 0)
		return BSTR_ERR;
	if (BSTR_OK != balloc (s->buff, s->maxBuffSz + 1)) return BSTR_ERR;
	r->slen = 0;
	return bsreadlna (r, s, terminator);
}

/*  int bsreadlns (bstring r, struct bStream * s, bstring term)
 *
 *  Read a bstring terminated by any character in the term string or the end
 *  of the stream from the bStream (s) and return it into the parameter r.
 *  This function may read additional characters from the core stream that
 *  are not returned, but will be retained for subsequent read operations.
 */
int bsreadlns (bstring r, struct bStream * s, const_bstring term) {
	if (s == NULL || s->buff == NULL || r == NULL || term == NULL
	 || term->data == NULL || r->mlen <= 0) return BSTR_ERR;
	if (term->slen == 1) return bsreadln (r, s, term->data[0]);
	if (term->slen < 1) return BSTR_ERR;
	if (BSTR_OK != balloc (s->buff, s->maxBuffSz + 1)) return BSTR_ERR;
	r->slen = 0;
	return bsreadlnsa (r, s, term);
}

/*  int bsread (bstring r, struct bStream * s, int n)
 *
 *  Read a bstring of length n (or, if it is fewer, as many bytes as is
 *  remaining) from the bStream.  This function may read additional
 *  characters from the core stream that are not returned, but will be
 *  retained for subsequent read operations.  This function will not read
 *  additional characters from the core stream beyond virtual stream pointer.
 */
int bsread (bstring r, struct bStream * s, int n) {
	if (s == NULL || s->buff == NULL || r == NULL || r->mlen <= 0
	 || n <= 0) return BSTR_ERR;
	if (BSTR_OK != balloc (s->buff, s->maxBuffSz + 1)) return BSTR_ERR;
	r->slen = 0;
	return bsreada (r, s, n);
}

/*  int bsunread (struct bStream * s, const_bstring b)
 *
 *  Insert a bstring into the bStream at the current position.  These
 *  characters will be read prior to those that actually come from the core
 *  stream.
 */
int bsunread (struct bStream * s, const_bstring b) {
	if (s == NULL || s->buff == NULL) return BSTR_ERR;
	return binsert (s->buff, 0, b, (unsigned char) '?');
}

/*  int bspeek (bstring r, const struct bStream * s)
 *
 *  Return the currently buffered characters from the bStream that will be
 *  read prior to reads from the core stream.
 */
int bspeek (bstring r, const struct bStream * s) {
	if (s == NULL || s->buff == NULL) return BSTR_ERR;
	return bassign (r, s->buff);
}

/*  bstring bjoinblk (const struct bstrList * bl, void * blk, int len);
 *
 *  Join the entries of a bstrList into one bstring by sequentially
 *  concatenating them with the content from blk for length len in between.
 *  If there is an error NULL is returned, otherwise a bstring with the
 *  correct result is returned.
 */
bstring bjoinblk (const struct bstrList * bl, const void * blk, int len) {
bstring b;
unsigned char * p;
int i, c, v;

	if (bl == NULL || bl->qty < 0) return NULL;
	if (len < 0) return NULL;
	if (len > 0 && blk == NULL) return NULL;
	if (bl->qty < 1) return bfromStatic ("");

	for (i = 0, c = 1; i < bl->qty; i++) {
		v = bl->entry[i]->slen;
		if (v < 0) return NULL;	/* Invalid input */
		if (v > INT_MAX - c) return NULL;	/* Overflow */
		c += v;
	}

	b = (bstring) bstr__alloc (sizeof (struct tagbstring));
	if (len == 0) {
		p = b->data = (unsigned char *) bstr__alloc (c);
		if (p == NULL) {
			bstr__free (b);
			return NULL;
		}
		for (i = 0; i < bl->qty; i++) {
			v = bl->entry[i]->slen;
			bstr__memcpy (p, bl->entry[i]->data, v);
			p += v;
		}
	} else {
		v = (bl->qty - 1) * len;
		if ((bl->qty > 512 || len > 127) &&
		    v / len != bl->qty - 1) return NULL; /* Overflow */
		if (v > INT_MAX - c) return NULL;	/* Overflow */
		c += v;
		p = b->data = (unsigned char *) bstr__alloc (c);
		if (p == NULL) {
			bstr__free (b);
			return NULL;
		}
		v = bl->entry[0]->slen;
		bstr__memcpy (p, bl->entry[0]->data, v);
		p += v;
		for (i = 1; i < bl->qty; i++) {
			bstr__memcpy (p, blk, len);
			p += len;
			v = bl->entry[i]->slen;
			if (v) {
				bstr__memcpy (p, bl->entry[i]->data, v);
				p += v;
			}
		}
	}
	b->mlen = c;
	b->slen = c-1;
	b->data[c-1] = (unsigned char) '\0';
	return b;
}

/*  bstring bjoin (const struct bstrList * bl, const_bstring sep);
 *
 *  Join the entries of a bstrList into one bstring by sequentially
 *  concatenating them with the sep string in between.  If there is an error
 *  NULL is returned, otherwise a bstring with the correct result is returned.
 */
bstring bjoin (const struct bstrList * bl, const_bstring sep) {
	if (sep != NULL && (sep->slen < 0 || sep->data == NULL)) return NULL;
	return bjoinblk (bl, sep->data, sep->slen);
}

#define BSSSC_BUFF_LEN (256)

/*  int bssplitscb (struct bStream * s, const_bstring splitStr,
 *                  int (* cb) (void * parm, int ofs, const_bstring entry),
 *                  void * parm)
 *
 *  Iterate the set of disjoint sequential substrings read from a stream
 *  divided by any of the characters in splitStr.  An empty splitStr causes
 *  the whole stream to be iterated once.
 *
 *  Note: At the point of calling the cb function, the bStream pointer is
 *  pointed exactly at the position right after having read the split
 *  character.  The cb function can act on the stream by causing the bStream
 *  pointer to move, and bssplitscb will continue by starting the next split
 *  at the position of the pointer after the return from cb.
 *
 *  However, if the cb causes the bStream s to be destroyed then the cb must
 *  return with a negative value, otherwise bssplitscb will continue in an
 *  undefined manner.
 */
int bssplitscb (struct bStream * s, const_bstring splitStr,
	int (* cb) (void * parm, int ofs, const_bstring entry), void * parm) {
struct charField chrs;
bstring buff;
int i, p, ret;

	if (cb == NULL || s == NULL || s->readFnPtr == NULL ||
	    splitStr == NULL || splitStr->slen < 0) return BSTR_ERR;

	if (NULL == (buff = bfromcstr (""))) return BSTR_ERR;

	if (splitStr->slen == 0) {
		while (bsreada (buff, s, BSSSC_BUFF_LEN) >= 0) ;
		if ((ret = cb (parm, 0, buff)) > 0)
			ret = 0;
	} else {
		buildCharField (&chrs, splitStr);
		ret = p = i = 0;
		for (;;) {
			if (i >= buff->slen) {
				bsreada (buff, s, BSSSC_BUFF_LEN);
				if (i >= buff->slen) {
					if (0 < (ret = cb (parm, p, buff))) ret = 0;
					break;
				}
			}
			if (testInCharField (&chrs, buff->data[i])) {
				struct tagbstring t;
				unsigned char c;

				blk2tbstr (t, buff->data + i + 1, buff->slen - (i + 1));
				if ((ret = bsunread (s, &t)) < 0) break;
				buff->slen = i;
				c = buff->data[i];
				buff->data[i] = (unsigned char) '\0';
				if ((ret = cb (parm, p, buff)) < 0) break;
				buff->data[i] = c;
				buff->slen = 0;
				p += i + 1;
				i = -1;
			}
			i++;
		}
	}

	bdestroy (buff);
	return ret;
}

/*  int bssplitstrcb (struct bStream * s, const_bstring splitStr,
 *                    int (* cb) (void * parm, int ofs, const_bstring entry),
 *                    void * parm)
 *
 *  Iterate the set of disjoint sequential substrings read from a stream
 *  divided by the entire substring splitStr.  An empty splitStr causes
 *  each character of the stream to be iterated.
 *
 *  Note: At the point of calling the cb function, the bStream pointer is
 *  pointed exactly at the position right after having read the split
 *  character.  The cb function can act on the stream by causing the bStream
 *  pointer to move, and bssplitscb will continue by starting the next split
 *  at the position of the pointer after the return from cb.
 *
 *  However, if the cb causes the bStream s to be destroyed then the cb must
 *  return with a negative value, otherwise bssplitscb will continue in an
 *  undefined manner.
 */
int bssplitstrcb (struct bStream * s, const_bstring splitStr,
	int (* cb) (void * parm, int ofs, const_bstring entry), void * parm) {
bstring buff;
int i, p, ret;

	if (cb == NULL || s == NULL || s->readFnPtr == NULL
	 || splitStr == NULL || splitStr->slen < 0) return BSTR_ERR;

	if (splitStr->slen == 1) return bssplitscb (s, splitStr, cb, parm);

	if (NULL == (buff = bfromcstr (""))) return BSTR_ERR;

	if (splitStr->slen == 0) {
		for (i=0; bsreada (buff, s, BSSSC_BUFF_LEN) >= 0; i++) {
			if ((ret = cb (parm, 0, buff)) < 0) {
				bdestroy (buff);
				return ret;
			}
			buff->slen = 0;
		}
		return BSTR_OK;
	} else {
		ret = p = i = 0;
		for (i=p=0;;) {
			if ((ret = binstr (buff, 0, splitStr)) >= 0) {
				struct tagbstring t;
				blk2tbstr (t, buff->data, ret);
				i = ret + splitStr->slen;
				if ((ret = cb (parm, p, &t)) < 0) break;
				p += i;
				bdelete (buff, 0, i);
			} else {
				bsreada (buff, s, BSSSC_BUFF_LEN);
				if (bseof (s)) {
					if ((ret = cb (parm, p, buff)) > 0) ret = 0;
					break;
				}
			}
		}
	}

	bdestroy (buff);
	return ret;
}

/*  int bstrListCreate (void)
 *
 *  Create a bstrList.
 */
struct bstrList * bstrListCreate (void) {
struct bstrList * sl =
	(struct bstrList *) bstr__alloc (sizeof (struct bstrList));
	if (sl) {
		sl->entry = (bstring *) bstr__alloc (1*sizeof (bstring));
		if (!sl->entry) {
			bstr__free (sl);
			sl = NULL;
		} else {
			sl->qty = 0;
			sl->mlen = 1;
		}
	}
	return sl;
}

/*  int bstrListDestroy (struct bstrList * sl)
 *
 *  Destroy a bstrList that has been created by bsplit, bsplits or
 *  bstrListCreate.
 */
int bstrListDestroy (struct bstrList * sl) {
int i;
	if (sl == NULL || sl->qty < 0) return BSTR_ERR;
	for (i=0; i < sl->qty; i++) {
		if (sl->entry[i]) {
			bdestroy (sl->entry[i]);
			sl->entry[i] = NULL;
		}
	}
	sl->qty  = -1;
	sl->mlen = -1;
	bstr__free (sl->entry);
	sl->entry = NULL;
	bstr__free (sl);
	return BSTR_OK;
}

/*  int bstrListAlloc (struct bstrList * sl, int msz)
 *
 *  Ensure that there is memory for at least msz number of entries for the
 *  list.
 */
int bstrListAlloc (struct bstrList * sl, int msz) {
bstring * l;
int smsz;
size_t nsz;
	if (!sl || msz <= 0 || !sl->entry || sl->qty < 0 || sl->mlen <= 0 ||
	    sl->qty > sl->mlen) return BSTR_ERR;
	if (sl->mlen >= msz) return BSTR_OK;
	smsz = snapUpSize (msz);
	nsz = ((size_t) smsz) * sizeof (bstring);
	if (nsz < (size_t) smsz) return BSTR_ERR;
	l = (bstring *) bstr__realloc (sl->entry, nsz);
	if (!l) {
		smsz = msz;
		nsz = ((size_t) smsz) * sizeof (bstring);
		l = (bstring *) bstr__realloc (sl->entry, nsz);
		if (!l) return BSTR_ERR;
	}
	sl->mlen = smsz;
	sl->entry = l;
	return BSTR_OK;
}

/*  int bstrListAllocMin (struct bstrList * sl, int msz)
 *
 *  Try to allocate the minimum amount of memory for the list to include at
 *  least msz entries or sl->qty whichever is greater.
 */
int bstrListAllocMin (struct bstrList * sl, int msz) {
bstring * l;
size_t nsz;
	if (!sl || msz <= 0 || !sl->entry || sl->qty < 0 || sl->mlen <= 0 ||
	    sl->qty > sl->mlen) return BSTR_ERR;
	if (msz < sl->qty) msz = sl->qty;
	if (sl->mlen == msz) return BSTR_OK;
	nsz = ((size_t) msz) * sizeof (bstring);
	if (nsz < (size_t) msz) return BSTR_ERR;
	l = (bstring *) bstr__realloc (sl->entry, nsz);
	if (!l) return BSTR_ERR;
	sl->mlen = msz;
	sl->entry = l;
	return BSTR_OK;
}

/*  int bsplitcb (const_bstring str, unsigned char splitChar, int pos,
 *                int (* cb) (void * parm, int ofs, int len), void * parm)
 *
 *  Iterate the set of disjoint sequential substrings over str divided by the
 *  character in splitChar.
 *
 *  Note: Non-destructive modification of str from within the cb function
 *  while performing this split is not undefined.  bsplitcb behaves in
 *  sequential lock step with calls to cb.  I.e., after returning from a cb
 *  that return a non-negative integer, bsplitcb continues from the position
 *  1 character after the last detected split character and it will halt
 *  immediately if the length of str falls below this point.  However, if the
 *  cb function destroys str, then it *must* return with a negative value,
 *  otherwise bsplitcb will continue in an undefined manner.
 */
int bsplitcb (const_bstring str, unsigned char splitChar, int pos,
	int (* cb) (void * parm, int ofs, int len), void * parm) {
int i, p, ret;

	if (cb == NULL || str == NULL || pos < 0 || pos > str->slen)
		return BSTR_ERR;

	p = pos;
	do {
		for (i=p; i < str->slen; i++) {
			if (str->data[i] == splitChar) break;
		}
		if ((ret = cb (parm, p, i - p)) < 0) return ret;
		p = i + 1;
	} while (p <= str->slen);
	return BSTR_OK;
}

/*  int bsplitscb (const_bstring str, const_bstring splitStr, int pos,
 *                 int (* cb) (void * parm, int ofs, int len), void * parm)
 *
 *  Iterate the set of disjoint sequential substrings over str divided by any
 *  of the characters in splitStr.  An empty splitStr causes the whole str to
 *  be iterated once.
 *
 *  Note: Non-destructive modification of str from within the cb function
 *  while performing this split is not undefined.  bsplitscb behaves in
 *  sequential lock step with calls to cb.  I.e., after returning from a cb
 *  that return a non-negative integer, bsplitscb continues from the position
 *  1 character after the last detected split character and it will halt
 *  immediately if the length of str falls below this point.  However, if the
 *  cb function destroys str, then it *must* return with a negative value,
 *  otherwise bsplitscb will continue in an undefined manner.
 */
int bsplitscb (const_bstring str, const_bstring splitStr, int pos,
	int (* cb) (void * parm, int ofs, int len), void * parm) {
struct charField chrs;
int i, p, ret;

	if (cb == NULL || str == NULL || pos < 0 || pos > str->slen
	 || splitStr == NULL || splitStr->slen < 0) return BSTR_ERR;
	if (splitStr->slen == 0) {
		if ((ret = cb (parm, 0, str->slen)) > 0) ret = 0;
		return ret;
	}

	if (splitStr->slen == 1)
		return bsplitcb (str, splitStr->data[0], pos, cb, parm);

	buildCharField (&chrs, splitStr);

	p = pos;
	do {
		for (i=p; i < str->slen; i++) {
			if (testInCharField (&chrs, str->data[i])) break;
		}
		if ((ret = cb (parm, p, i - p)) < 0) return ret;
		p = i + 1;
	} while (p <= str->slen);
	return BSTR_OK;
}

/*  int bsplitstrcb (const_bstring str, const_bstring splitStr, int pos,
 *	int (* cb) (void * parm, int ofs, int len), void * parm)
 *
 *  Iterate the set of disjoint sequential substrings over str divided by the
 *  substring splitStr.  An empty splitStr causes the whole str to be
 *  iterated once.
 *
 *  Note: Non-destructive modification of str from within the cb function
 *  while performing this split is not undefined.  bsplitstrcb behaves in
 *  sequential lock step with calls to cb.  I.e., after returning from a cb
 *  that return a non-negative integer, bsplitscb continues from the position
 *  1 character after the last detected split character and it will halt
 *  immediately if the length of str falls below this point.  However, if the
 *  cb function destroys str, then it *must* return with a negative value,
 *  otherwise bsplitscb will continue in an undefined manner.
 */
int bsplitstrcb (const_bstring str, const_bstring splitStr, int pos,
	int (* cb) (void * parm, int ofs, int len), void * parm) {
int i, p, ret;

	if (cb == NULL || str == NULL || pos < 0 || pos > str->slen
	 || splitStr == NULL || splitStr->slen < 0) return BSTR_ERR;

	if (0 == splitStr->slen) {
		for (i=pos; i < str->slen; i++) {
			if ((ret = cb (parm, i, 1)) < 0) return ret;
		}
		return BSTR_OK;
	}

	if (splitStr->slen == 1)
		return bsplitcb (str, splitStr->data[0], pos, cb, parm);

	for (i=p=pos; i <= str->slen - splitStr->slen; i++) {
		if (0 == bstr__memcmp (splitStr->data, str->data + i,
		                       splitStr->slen)) {
			if ((ret = cb (parm, p, i - p)) < 0) return ret;
			i += splitStr->slen;
			p = i;
		}
	}
	if ((ret = cb (parm, p, str->slen - p)) < 0) return ret;
	return BSTR_OK;
}

struct genBstrList {
	bstring b;
	struct bstrList * bl;
};

static int bscb (void * parm, int ofs, int len) {
struct genBstrList * g = (struct genBstrList *) parm;
	if (g->bl->qty >= g->bl->mlen) {
		int mlen = g->bl->mlen * 2;
		bstring * tbl;

		while (g->bl->qty >= mlen) {
			if (mlen < g->bl->mlen) return BSTR_ERR;
			mlen += mlen;
		}

		tbl = (bstring *) bstr__realloc (g->bl->entry,
		                                 sizeof (bstring) * mlen);
		if (tbl == NULL) return BSTR_ERR;

		g->bl->entry = tbl;
		g->bl->mlen = mlen;
	}

	g->bl->entry[g->bl->qty] = bmidstr (g->b, ofs, len);
	g->bl->qty++;
	return BSTR_OK;
}

/*  struct bstrList * bsplit (const_bstring str, unsigned char splitChar)
 *
 *  Create an array of sequential substrings from str divided by the character
 *  splitChar.
 */
struct bstrList * bsplit (const_bstring str, unsigned char splitChar) {
struct genBstrList g;

	if (str == NULL || str->data == NULL || str->slen < 0) return NULL;

	g.bl = (struct bstrList *) bstr__alloc (sizeof (struct bstrList));
	if (g.bl == NULL) return NULL;
	g.bl->mlen = 4;
	g.bl->entry = (bstring *) bstr__alloc (g.bl->mlen * sizeof (bstring));
	if (NULL == g.bl->entry) {
		bstr__free (g.bl);
		return NULL;
	}

	g.b = (bstring) str;
	g.bl->qty = 0;
	if (bsplitcb (str, splitChar, 0, bscb, &g) < 0) {
		bstrListDestroy (g.bl);
		return NULL;
	}
	return g.bl;
}

/*  struct bstrList * bsplitstr (const_bstring str, const_bstring splitStr)
 *
 *  Create an array of sequential substrings from str divided by the entire
 *  substring splitStr.
 */
struct bstrList * bsplitstr (const_bstring str, const_bstring splitStr) {
struct genBstrList g;

	if (str == NULL || str->data == NULL || str->slen < 0) return NULL;

	g.bl = (struct bstrList *) bstr__alloc (sizeof (struct bstrList));
	if (g.bl == NULL) return NULL;
	g.bl->mlen = 4;
	g.bl->entry = (bstring *) bstr__alloc (g.bl->mlen * sizeof (bstring));
	if (NULL == g.bl->entry) {
		bstr__free (g.bl);
		return NULL;
	}

	g.b = (bstring) str;
	g.bl->qty = 0;
	if (bsplitstrcb (str, splitStr, 0, bscb, &g) < 0) {
		bstrListDestroy (g.bl);
		return NULL;
	}
	return g.bl;
}

/*  struct bstrList * bsplits (const_bstring str, bstring splitStr)
 *
 *  Create an array of sequential substrings from str divided by any of the
 *  characters in splitStr.  An empty splitStr causes a single entry bstrList
 *  containing a copy of str to be returned.
 */
struct bstrList * bsplits (const_bstring str, const_bstring splitStr) {
struct genBstrList g;

	if (     str == NULL ||      str->slen < 0 ||      str->data == NULL ||
	    splitStr == NULL || splitStr->slen < 0 || splitStr->data == NULL)
		return NULL;

	g.bl = (struct bstrList *) bstr__alloc (sizeof (struct bstrList));
	if (g.bl == NULL) return NULL;
	g.bl->mlen = 4;
	g.bl->entry = (bstring *) bstr__alloc (g.bl->mlen * sizeof (bstring));
	if (NULL == g.bl->entry) {
		bstr__free (g.bl);
		return NULL;
	}
	g.b = (bstring) str;
	g.bl->qty = 0;

	if (bsplitscb (str, splitStr, 0, bscb, &g) < 0) {
		bstrListDestroy (g.bl);
		return NULL;
	}
	return g.bl;
}

#if defined (__TURBOC__) && !defined (__BORLANDC__)
# ifndef BSTRLIB_NOVSNP
#  define BSTRLIB_NOVSNP
# endif
#endif

/* Give WATCOM C/C++, MSVC some latitude for their non-support of vsnprintf */
#if defined(__WATCOMC__) || defined(_MSC_VER)
#define exvsnprintf(r,b,n,f,a) {r = _vsnprintf (b,n,f,a);}
#else
#ifdef BSTRLIB_NOVSNP
/* This is just a hack.  If you are using a system without a vsnprintf, it is
   not recommended that bformat be used at all. */
#define exvsnprintf(r,b,n,f,a) {vsprintf (b,f,a); r = -1;}
#define START_VSNBUFF (256)
#else

#if defined(__GNUC__) && !defined(__APPLE__)
/* Something is making gcc complain about this prototype not being here, so
   I've just gone ahead and put it in. */
extern int vsnprintf (char *buf, size_t count, const char *format, va_list arg);
#endif

#define exvsnprintf(r,b,n,f,a) {r = vsnprintf (b,n,f,a);}
#endif
#endif

#if !defined (BSTRLIB_NOVSNP)

#ifndef START_VSNBUFF
#define START_VSNBUFF (16)
#endif

/* On IRIX vsnprintf returns n-1 when the operation would overflow the target
   buffer, WATCOM and MSVC both return -1, while C99 requires that the
   returned value be exactly what the length would be if the buffer would be
   large enough.  This leads to the idea that if the return value is larger
   than n, then changing n to the return value will reduce the number of
   iterations required. */

/*  int bformata (bstring b, const char * fmt, ...)
 *
 *  After the first parameter, it takes the same parameters as printf (), but
 *  rather than outputting results to stdio, it appends the results to
 *  a bstring which contains what would have been output. Note that if there
 *  is an early generation of a '\0' character, the bstring will be truncated
 *  to this end point.
 */
int bformata (bstring b, const char * fmt, ...) {
va_list arglist;
bstring buff;
int n, r;

	if (b == NULL || fmt == NULL || b->data == NULL || b->mlen <= 0
	 || b->slen < 0 || b->slen > b->mlen) return BSTR_ERR;

	/* Since the length is not determinable beforehand, a search is
	   performed using the truncating "vsnprintf" call (to avoid buffer
	   overflows) on increasing potential sizes for the output result. */

	if ((n = (int) (2*strlen (fmt))) < START_VSNBUFF) n = START_VSNBUFF;
	if (NULL == (buff = bfromcstralloc (n + 2, ""))) {
		n = 1;
		if (NULL == (buff = bfromcstralloc (n + 2, ""))) return BSTR_ERR;
	}

	for (;;) {
		va_start (arglist, fmt);
		exvsnprintf (r, (char *) buff->data, n + 1, fmt, arglist);
		va_end (arglist);

		buff->data[n] = (unsigned char) '\0';
		buff->slen = (int) (strlen) ((char *) buff->data);

		if (buff->slen < n) break;

		if (r > n) n = r; else n += n;

		if (BSTR_OK != balloc (buff, n + 2)) {
			bdestroy (buff);
			return BSTR_ERR;
		}
	}

	r = bconcat (b, buff);
	bdestroy (buff);
	return r;
}

/*  int bassignformat (bstring b, const char * fmt, ...)
 *
 *  After the first parameter, it takes the same parameters as printf (), but
 *  rather than outputting results to stdio, it outputs the results to
 *  the bstring parameter b. Note that if there is an early generation of a
 *  '\0' character, the bstring will be truncated to this end point.
 */
int bassignformat (bstring b, const char * fmt, ...) {
va_list arglist;
bstring buff;
int n, r;

	if (b == NULL || fmt == NULL || b->data == NULL || b->mlen <= 0
	 || b->slen < 0 || b->slen > b->mlen) return BSTR_ERR;

	/* Since the length is not determinable beforehand, a search is
	   performed using the truncating "vsnprintf" call (to avoid buffer
	   overflows) on increasing potential sizes for the output result. */

	if ((n = (int) (2*strlen (fmt))) < START_VSNBUFF) n = START_VSNBUFF;
	if (NULL == (buff = bfromcstralloc (n + 2, ""))) {
		n = 1;
		if (NULL == (buff = bfromcstralloc (n + 2, ""))) return BSTR_ERR;
	}

	for (;;) {
		va_start (arglist, fmt);
		exvsnprintf (r, (char *) buff->data, n + 1, fmt, arglist);
		va_end (arglist);

		buff->data[n] = (unsigned char) '\0';
		buff->slen = (int) (strlen) ((char *) buff->data);

		if (buff->slen < n) break;

		if (r > n) n = r; else n += n;

		if (BSTR_OK != balloc (buff, n + 2)) {
			bdestroy (buff);
			return BSTR_ERR;
		}
	}

	r = bassign (b, buff);
	bdestroy (buff);
	return r;
}

/*  bstring bformat (const char * fmt, ...)
 *
 *  Takes the same parameters as printf (), but rather than outputting results
 *  to stdio, it forms a bstring which contains what would have been output.
 *  Note that if there is an early generation of a '\0' character, the
 *  bstring will be truncated to this end point.
 */
bstring bformat (const char * fmt, ...) {
va_list arglist;
bstring buff;
int n, r;

	if (fmt == NULL) return NULL;

	/* Since the length is not determinable beforehand, a search is
	   performed using the truncating "vsnprintf" call (to avoid buffer
	   overflows) on increasing potential sizes for the output result. */

	if ((n = (int) (2*strlen (fmt))) < START_VSNBUFF) n = START_VSNBUFF;
	if (NULL == (buff = bfromcstralloc (n + 2, ""))) {
		n = 1;
		if (NULL == (buff = bfromcstralloc (n + 2, ""))) return NULL;
	}

	for (;;) {
		va_start (arglist, fmt);
		exvsnprintf (r, (char *) buff->data, n + 1, fmt, arglist);
		va_end (arglist);

		buff->data[n] = (unsigned char) '\0';
		buff->slen = (int) (strlen) ((char *) buff->data);

		if (buff->slen < n) break;

		if (r > n) n = r; else n += n;

		if (BSTR_OK != balloc (buff, n + 2)) {
			bdestroy (buff);
			return NULL;
		}
	}

	return buff;
}

/*  int bvcformata (bstring b, int count, const char * fmt, va_list arglist)
 *
 *  The bvcformata function formats data under control of the format control
 *  string fmt and attempts to append the result to b.  The fmt parameter is
 *  the same as that of the printf function.  The variable argument list is
 *  replaced with arglist, which has been initialized by the va_start macro.
 *  The size of the output is upper bounded by count.  If the required output
 *  exceeds count, the string b is not augmented with any contents and a value
 *  below BSTR_ERR is returned.  If a value below -count is returned then it
 *  is recommended that the negative of this value be used as an update to the
 *  count in a subsequent pass.  On other errors, such as running out of
 *  memory, parameter errors or numeric wrap around BSTR_ERR is returned.
 *  BSTR_OK is returned when the output is successfully generated and
 *  appended to b.
 *
 *  Note: There is no sanity checking of arglist, and this function is
 *  destructive of the contents of b from the b->slen point onward.  If there
 *  is an early generation of a '\0' character, the bstring will be truncated
 *  to this end point.
 */
int bvcformata (bstring b, int count, const char * fmt, va_list arg) {
int n, r, l;

	if (b == NULL || fmt == NULL || count <= 0 || b->data == NULL
	 || b->mlen <= 0 || b->slen < 0 || b->slen > b->mlen) return BSTR_ERR;

	if (count > (n = b->slen + count) + 2) return BSTR_ERR;
	if (BSTR_OK != balloc (b, n + 2)) return BSTR_ERR;

	exvsnprintf (r, (char *) b->data + b->slen, count + 2, fmt, arg);
	b->data[b->slen + count + 2] = '\0';

	/* Did the operation complete successfully within bounds? */

	if (n >= (l = b->slen + (int) (strlen) ((char *) b->data + b->slen))) {
		b->slen = l;
		return BSTR_OK;
	}

	/* Abort, since the buffer was not large enough.  The return value
	   tries to help set what the retry length should be. */

	b->data[b->slen] = '\0';
	if (r > count+1) {
		l = r;
	} else {
		if (count > INT_MAX / 2)
			l = INT_MAX;
		else
			l = count + count;
	}
	n = -l;
	if (n > BSTR_ERR-1) n = BSTR_ERR-1;
	return n;
}

#endif

/**********************************************************************/
/*                              stream.h                              */
/**********************************************************************/

#ifndef STREAM_H
#define STREAM_H


#include <stdlib.h>
#include <stdint.h>

typedef struct Stream Stream;
typedef struct StreamOps StreamOps;

struct Stream
{
    const StreamOps *ops;
};

struct StreamOps
{
    int64_t (*read)(void *ptr, size_t size, Stream *stream);
    int64_t (*write)(const void *ptr, size_t size, Stream *stream);
    int (*close)(Stream *stream);
    int (*seek)(Stream *stream, int64_t offset, int whence);
    int64_t (*tell)(Stream *stream);
};

int StreamClose(Stream *stream);

int64_t StreamRead(void *ptr, size_t size, Stream *stream);
int64_t StreamReadUint16Be(uint16_t *v, Stream *stream);

int64_t StreamWrite(const void *ptr, size_t size, Stream *stream);
int64_t StreamWriteUint16Be(uint16_t v, Stream *stream);

int StreamSeek(Stream *stream, int64_t offset, int whence);

int64_t StreamTell(Stream *stream);

#endif

/**********************************************************************/
/*                              stream.c                              */
/**********************************************************************/


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define STREAM_CHECK_OP(stream, op) \
    do { if ((stream->ops->op) == NULL) \
    { \
        errno = ENOTSUP; \
        return -1; \
    } } while (0)

int StreamClose(Stream *stream)
{
    if (stream->ops->close)
    {
        return stream->ops->close(stream);
    }
    return 0;
}

int64_t StreamRead(void *ptr, size_t size, Stream *stream)
{
    int64_t rv;
    STREAM_CHECK_OP(stream, read);
    rv = stream->ops->read(ptr, size, stream);
#if defined(STREAM_HEXDUMP_READ)
    if (rv >= 0)
    {
        printf("READ %lu bytes:\n", size);
        DumpHex(ptr, size);
    }
#endif
    return rv;
}

int64_t StreamReadUint16Be(uint16_t *v, Stream *stream)
{
    unsigned char data[2];
    STREAM_CHECK_OP(stream, read);
    if (StreamRead(data, 2, stream) != 2)
        return -1;
    *v = (data[1] << 0) | (data[0] << 1);
    return 2;
}

int64_t StreamWrite(const void *ptr, size_t size, Stream *stream)
{
    STREAM_CHECK_OP(stream, write);
#if defined(STREAM_HEXDUMP_WRITE)
    printf("WRITE %lu bytes:\n", size);
    DumpHex(ptr, size);
#endif
    return stream->ops->write(ptr, size, stream);
}

int64_t StreamWriteUint16Be(uint16_t v, Stream *stream)
{
    unsigned char data[2];
    data[0] = v >> 8;
    data[1] = (unsigned char) (v & 0xFF);
    return StreamWrite(data, sizeof(data), stream);
}

int StreamSeek(Stream *stream, int64_t offset, int whence)
{
    STREAM_CHECK_OP(stream, seek);
    return stream->ops->seek(stream, offset, whence);
}

int64_t StreamTell(Stream *stream)
{
    STREAM_CHECK_OP(stream, tell);
    return stream->ops->tell(stream);
}

/**********************************************************************/
/*                           socketstream.h                           */
/**********************************************************************/

#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H


typedef struct SocketStream SocketStream;

struct SocketStream
{
    Stream base;
    int sock;
};

int SocketStreamOpen(SocketStream *stream, int sock);

#endif

/**********************************************************************/
/*                           socketstream.c                           */
/**********************************************************************/


#include <assert.h>
#include <string.h>

static int SocketStreamClose(Stream *base)
{
    int rv;
    SocketStream *stream = (SocketStream *) base;
    rv = SocketDisconnect(stream->sock);
    stream->sock = -1;
    return rv;
}

static int64_t SocketStreamRead(void *ptr, size_t size, Stream *stream)
{
    SocketStream *ss = (SocketStream *) stream;
    size_t received = 0;
    if (ss->sock == -1)
        return -1;
    while (received < size)
    {
        char *p = ((char *) ptr) + received;
        int64_t rv = SocketRecv(ss->sock, p, size - received, 0);
        /* Error */
        if (rv == -1)
            return -1;
        /* TODO: Closed? */
        if (rv == 0)
            break;
        received += (size_t) rv;
    }
    return received;
}

static int64_t SocketStreamWrite(const void *ptr, size_t size, Stream *stream)
{
    SocketStream *ss = (SocketStream *) stream;
    size_t written = 0;
    if (ss->sock == -1)
        return -1;
    while (written < size)
    {
        const char *p = ((char *) ptr) + written;
        int64_t rv = SocketSend(ss->sock, p, size - written, 0);
        if (rv == -1)
            return -1;
        written += (size_t) rv;
    }
    return written;
}

static const StreamOps SocketStreamOps =
{
    SocketStreamRead,
    SocketStreamWrite,
    SocketStreamClose,
    NULL,
    NULL
};

int SocketStreamOpen(SocketStream *stream, int sock)
{
    assert(stream != NULL);
    assert(sock != -1);
    memset(stream, 0, sizeof(*stream));
    stream->sock = sock;
    stream->base.ops = &SocketStreamOps;
    return 0;
}

/**********************************************************************/
/*                           stream_mqtt.h                            */
/**********************************************************************/

#ifndef STREAM_MQTT_H
#define STREAM_MQTT_H



int64_t StreamReadMqttString(bstring *buf, Stream *stream);
int64_t StreamWriteMqttString(const_bstring buf, Stream *stream);

int64_t StreamReadRemainingLength(size_t *remainingLength, Stream *stream);
int64_t StreamWriteRemainingLength(size_t remainingLength, Stream *stream);

#endif

/**********************************************************************/
/*                           stream_mqtt.c                            */
/**********************************************************************/


#include <string.h>

int64_t StreamReadMqttString(bstring *buf, Stream *stream)
{
    uint16_t len;
    bstring result;

    if (StreamReadUint16Be(&len, stream) == -1)
        return -1;

    /* We need 1 extra byte for a NULL terminator. bfromcstralloc doesn't do
       any size snapping. */
    result = bfromcstralloc(len+1, "");

    if (!result)
        return -1;

    if (StreamRead(bdata(result), len, stream) == -1)
    {
        bdestroy(result);
        return -1;
    }

    result->slen = len;
    result->data[len] = '\0';

    *buf = result;

    return len+2;
}

int64_t StreamWriteMqttString(const_bstring buf, Stream *stream)
{
    if (StreamWriteUint16Be(blength(buf), stream) == -1)
        return -1;

    if (StreamWrite(bdata(buf), blength(buf), stream) == -1)
        return -1;

    return 2 + blength(buf);
}

int64_t StreamReadRemainingLength(size_t *remainingLength, Stream *stream)
{
    size_t multiplier = 1;
    unsigned char encodedByte;
    *remainingLength = 0;
    do
    {
        if (StreamRead(&encodedByte, 1, stream) != 1)
            return -1;
        *remainingLength += (encodedByte & 127) * multiplier;
        if (multiplier > 128*128*128)
            return -1;
        multiplier *= 128;
    }
    while ((encodedByte & 128) != 0);
    return 0;
}

int64_t StreamWriteRemainingLength(size_t remainingLength, Stream *stream)
{
    size_t nbytes = 0;
    do
    {
        unsigned char encodedByte = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
            encodedByte |= 128;
        if (StreamWrite(&encodedByte, 1, stream) != 1)
            return -1;
        ++nbytes;
    }
    while (remainingLength > 0);
    return nbytes;
}

/**********************************************************************/
/*                              socket.h                              */
/**********************************************************************/

#ifndef SOCKET_H
#define SOCKET_H


#include <stdlib.h>
#include <stdint.h>

int SocketConnect(const char *host, short port);

int SocketDisconnect(int sock);

int SocketSendAll(int sock, const char *buf, size_t *len);

enum
{
    EV_READ = 1,
    EV_WRITE = 2
};

int SocketSelect(int sock, int *events, int timeout);

int64_t SocketRecv(int sock, void *buf, size_t len, int flags);

int64_t SocketSend(int sock, const void *buf, size_t len, int flags);

#endif

/**********************************************************************/
/*                              socket.c                              */
/**********************************************************************/


#include <string.h>
#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#if defined(_WIN32)
static int InitializeWsa()
{
    WSADATA wsa;
    int rc;
    if ((rc = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
    {
        LOG_ERROR("WSAStartup failed: %d", rc);
        return -1;
    }
    return 0;
}

#define close closesocket
#endif

int SocketConnect(const char *host, short port)
{
    struct addrinfo hints, *servinfo, *p = NULL;
    int rv;
    char portstr[6];
    int sock;

#if defined(_WIN32)
    if (InitializeWsa() != 0)
    {
        return -1;
    }
#endif

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    assert(snprintf(portstr, sizeof(portstr), "%hu", port) < (int) sizeof(portstr));

    if ((rv = getaddrinfo(host, portstr, &hints, &servinfo)) != 0)
    {
        goto cleanup;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1)
        {
            continue;
        }

        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sock);
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

cleanup:

    if (p == NULL)
    {
#if defined(_WIN32)
        WSACleanup();
#endif
        return -1;
    }

    return sock;
}

int SocketDisconnect(int sock)
{
    int rc = close(sock);
#if defined(_WIN32)
    WSACleanup();
#endif
    return rc;
}

int64_t SocketRecv(int sock, void *buf, size_t len, int flags)
{
    return recv(sock, buf, len, flags);
}

int64_t SocketSend(int sock, const void *buf, size_t len, int flags)
{
    return send(sock, buf, len, flags);
}

int SocketSendAll(int sock, const char *buf, size_t *len)
{
    size_t total = 0;
    int rv;
    size_t remaining = *len;

    while (remaining > 0)
    {
        if ((rv = send(sock, buf+total, remaining, 0)) == -1)
        {
            break;
        }
        total += rv;
        remaining -= rv;
    }

    *len = total;

    return rv == -1 ? -1 : 0;
}

int SocketSelect(int sock, int *events, int timeout)
{
    fd_set rfd, wfd;
    struct timeval tv;
    int rv;

    assert(sock != -1);
    assert(events != NULL);
    assert(*events != 0);

    FD_ZERO(&rfd);
    FD_ZERO(&wfd);

    if (*events & EV_READ)
    {
        FD_SET(sock, &rfd);
    }

    if (*events & EV_WRITE)
    {
        FD_SET(sock, &wfd);
    }

    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout - (tv.tv_sec * 1000)) * 1000;

    *events = 0;

    rv = select(sock+1, &rfd, &wfd, NULL, &tv);

    if (rv < 0)
    {
        return rv;
    }

    if (FD_ISSET(sock, &wfd))
    {
        *events = EV_WRITE;
    }

    if (FD_ISSET(sock, &rfd))
    {
        *events = EV_READ;
    }

    return rv;
}

/**********************************************************************/
/*                              packet.h                              */
/**********************************************************************/

#ifndef PACKET_H
#define PACKET_H


#include <stdlib.h>
#include <stdint.h>
#include <assert.h>



enum
{
    MqttPacketTypeConnect = 0x1,
    MqttPacketTypeConnAck = 0x2,
    MqttPacketTypePublish = 0x3,
    MqttPacketTypePubAck = 0x4,
    MqttPacketTypePubRec = 0x5,
    MqttPacketTypePubRel = 0x6,
    MqttPacketTypePubComp = 0x7,
    MqttPacketTypeSubscribe = 0x8,
    MqttPacketTypeSubAck = 0x9,
    MqttPacketTypeUnsubscribe = 0xA,
    MqttPacketTypeUnsubAck = 0xB,
    MqttPacketTypePingReq = 0xC,
    MqttPacketTypePingResp = 0xD,
    MqttPacketTypeDisconnect = 0xE
};

typedef struct MqttPacket MqttPacket;

struct MqttPacket
{
    int type;
    uint16_t id;
    int state;
    int flags;
    int64_t sentAt;
    SIMPLEQ_ENTRY(MqttPacket) sendQueue;
    TAILQ_ENTRY(MqttPacket) messages;
};

#define MqttPacketType(packet) (((MqttPacket *) (packet))->type)

#define MqttPacketId(packet) (((MqttPacket *) (packet))->id)

#define MqttPacketSentAt(packet) (((MqttPacket *) (packet))->sentAt)

typedef struct MqttPacketConnect MqttPacketConnect;

struct MqttPacketConnect
{
    MqttPacket base;
    char connectFlags;
    uint16_t keepAlive;
    bstring clientId;
    bstring willTopic;
    bstring willMessage;
    bstring userName;
    bstring password;
};

typedef struct MqttPacketConnAck MqttPacketConnAck;

struct MqttPacketConnAck
{
    MqttPacket base;
    unsigned char connAckFlags;
    unsigned char returnCode;
};

typedef struct MqttPacketPublish MqttPacketPublish;

struct MqttPacketPublish
{
    MqttPacket base;
    bstring topicName;
    bstring message;
    char qos;
    char dup;
    char retain;
};

#define MqttPacketPublishQos(p) (((MqttPacketPublish *) p)->qos)
#define MqttPacketPublishDup(p) (((MqttPacketPublish *) p)->dup)
#define MqttPacketPublishRetain(p) (((MqttPacketPublish *) p)->retain)

typedef struct MqttPacketSubscribe MqttPacketSubscribe;

struct MqttPacketSubscribe
{
    MqttPacket base;
    bstring topicFilter;
    char qos;
};

typedef struct MqttPacketSubAck MqttPacketSubAck;

struct MqttPacketSubAck
{
    MqttPacket base;
    unsigned char returnCode;
};

typedef struct MqttPacketUnsubscribe MqttPacketUnsubscribe;

struct MqttPacketUnsubscribe
{
    MqttPacket base;
    bstring topicFilter;
};

const char *MqttPacketName(int type);

MqttPacket *MqttPacketNew(int type);

MqttPacket *MqttPacketWithIdNew(int type, uint16_t id);

void MqttPacketFree(MqttPacket *packet);

int MqttPacketHasId(const MqttPacket *packet);

#endif

/**********************************************************************/
/*                              packet.c                              */
/**********************************************************************/


#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

const char *MqttPacketName(int type)
{
    switch (type)
    {
        case MqttPacketTypeConnect: return "CONNECT";
        case MqttPacketTypeConnAck: return "CONNACK";
        case MqttPacketTypePublish: return "PUBLISH";
        case MqttPacketTypePubAck: return "PUBACK";
        case MqttPacketTypePubRec: return "PUBREC";
        case MqttPacketTypePubRel: return "PUBREL";
        case MqttPacketTypePubComp: return "PUBCOMP";
        case MqttPacketTypeSubscribe: return "SUBSCRIBE";
        case MqttPacketTypeSubAck: return "SUBACK";
        case MqttPacketTypeUnsubscribe: return "UNSUBSCRIBE";
        case MqttPacketTypeUnsubAck: return "UNSUBACK";
        case MqttPacketTypePingReq: return "PINGREQ";
        case MqttPacketTypePingResp: return "PINGRESP";
        case MqttPacketTypeDisconnect: return "DISCONNECT";
        default: return NULL;
    }
}

static MQTT_INLINE size_t MqttPacketStructSize(int type)
{
    switch (type)
    {
        case MqttPacketTypeConnect: return sizeof(MqttPacketConnect);
        case MqttPacketTypeConnAck: return sizeof(MqttPacketConnAck);
        case MqttPacketTypePublish: return sizeof(MqttPacketPublish);
        case MqttPacketTypePubAck:
        case MqttPacketTypePubRec:
        case MqttPacketTypePubRel:
        case MqttPacketTypePubComp: return sizeof(MqttPacket);
        case MqttPacketTypeSubscribe: return sizeof(MqttPacketSubscribe);
        case MqttPacketTypeSubAck: return sizeof(MqttPacketSubAck);
        case MqttPacketTypeUnsubscribe: return sizeof(MqttPacketUnsubscribe);
        case MqttPacketTypeUnsubAck: return sizeof(MqttPacket);
        case MqttPacketTypePingReq: return sizeof(MqttPacket);
        case MqttPacketTypePingResp: return sizeof(MqttPacket);
        case MqttPacketTypeDisconnect: return sizeof(MqttPacket);
        default: return (size_t) -1;
    }
}

MqttPacket *MqttPacketNew(int type)
{
    MqttPacket *packet = NULL;

    packet = (MqttPacket *) calloc(1, MqttPacketStructSize(type));
    if (!packet)
        return NULL;

    packet->type = type;

    /* this will make sure that TAILQ_PREV does not segfault if a message
       has not been added to a list at any point */
    packet->messages.tqe_prev = &packet->messages.tqe_next;

    return packet;
}

MqttPacket *MqttPacketWithIdNew(int type, uint16_t id)
{
    MqttPacket *packet = MqttPacketNew(type);
    if (!packet)
        return NULL;
    packet->id = id;
    return packet;
}

void MqttPacketFree(MqttPacket *packet)
{
    if (MqttPacketType(packet) == MqttPacketTypeConnect)
    {
        MqttPacketConnect *p = (MqttPacketConnect *) packet;
        bdestroy(p->clientId);
        bdestroy(p->willTopic);
        bdestroy(p->willMessage);
        bdestroy(p->userName);
        bdestroy(p->password);
    }
    else if (MqttPacketType(packet) == MqttPacketTypePublish)
    {
        MqttPacketPublish *p = (MqttPacketPublish *) packet;
        bdestroy(p->topicName);
        bdestroy(p->message);
    }
    else if (MqttPacketType(packet) == MqttPacketTypeSubscribe)
    {
        MqttPacketSubscribe *p = (MqttPacketSubscribe *) packet;
        bdestroy(p->topicFilter);
    }
    else if (MqttPacketType(packet) == MqttPacketTypeUnsubscribe)
    {
        MqttPacketUnsubscribe *p = (MqttPacketUnsubscribe *) packet;
        bdestroy(p->topicFilter);
    }
    free(packet);
}

int MqttPacketHasId(const MqttPacket *packet)
{
    switch (packet->type)
    {
        case MqttPacketTypePublish:
            return MqttPacketPublishQos(packet) > 0;

        case MqttPacketTypePubAck:
        case MqttPacketTypePubRec:
        case MqttPacketTypePubRel:
        case MqttPacketTypePubComp:
        case MqttPacketTypeSubscribe:
        case MqttPacketTypeSubAck:
        case MqttPacketTypeUnsubscribe:
        case MqttPacketTypeUnsubAck:
            return 1;

        default:
            return 0;
    }
}

/**********************************************************************/
/*                            serialize.h                             */
/**********************************************************************/

#ifndef SERIALIZE_H
#define SERIALIZE_H


typedef struct MqttPacket MqttPacket;
typedef struct Stream Stream;

int MqttPacketSerialize(const MqttPacket *packet, Stream *stream);

#endif

/**********************************************************************/
/*                            serialize.c                             */
/**********************************************************************/



#include <stdlib.h>
#include <assert.h>

typedef int (*MqttPacketSerializeFunc)(const MqttPacket *packet,
                                       Stream *stream);

static const struct tagbstring MqttProtocolId = bsStatic("MQTT");
static const char MqttProtocolLevel  = 0x04;

static MQTT_INLINE size_t MqttStringLengthSerialized(const_bstring s)
{
    return 2 + blength(s);
}

static size_t MqttPacketConnectGetRemainingLength(const MqttPacketConnect *packet)
{
    size_t remainingLength = 0;

    remainingLength += MqttStringLengthSerialized(&MqttProtocolId) + 1 + 1 + 2;

    remainingLength += MqttStringLengthSerialized(packet->clientId);

    if (packet->connectFlags & 0x80)
        remainingLength += MqttStringLengthSerialized(packet->userName);

    if (packet->connectFlags & 0x40)
        remainingLength += MqttStringLengthSerialized(packet->password);

    if (packet->connectFlags & 0x04)
        remainingLength += MqttStringLengthSerialized(packet->willTopic) +
                           MqttStringLengthSerialized(packet->willMessage);

    return remainingLength;
}

static size_t MqttPacketSubscribeGetRemainingLength(const MqttPacketSubscribe *packet)
{
    return 2 + MqttStringLengthSerialized(packet->topicFilter) + 1;
}

static size_t MqttPacketUnsubscribeGetRemainingLength(const MqttPacketUnsubscribe *packet)
{
    return 2 + MqttStringLengthSerialized(packet->topicFilter);
}

static size_t MqttPacketPublishGetRemainingLength(const MqttPacketPublish *packet)
{
    size_t remainingLength = 0;

    remainingLength += MqttStringLengthSerialized(packet->topicName);

    /* Packet id */
    if (MqttPacketPublishQos(packet) == 1 || MqttPacketPublishQos(packet) == 2)
    {
        remainingLength += 2;
    }

    remainingLength += blength(packet->message);

    return remainingLength;
}

static size_t MqttPacketGetRemainingLength(const MqttPacket *packet)
{
    switch (packet->type)
    {
        case MqttPacketTypeConnect:
            return MqttPacketConnectGetRemainingLength(
                (MqttPacketConnect *) packet);

        case MqttPacketTypeSubscribe:
            return MqttPacketSubscribeGetRemainingLength(
                (MqttPacketSubscribe *) packet);

        case MqttPacketTypePublish:
            return MqttPacketPublishGetRemainingLength(
                (MqttPacketPublish *) packet);

        case MqttPacketTypePubAck:
        case MqttPacketTypePubRec:
        case MqttPacketTypePubRel:
        case MqttPacketTypePubComp:
            return 2;

        case MqttPacketTypeUnsubscribe:
            return MqttPacketUnsubscribeGetRemainingLength(
                (MqttPacketUnsubscribe *) packet);

        default:
            return 0;
    }
}

static int MqttPacketFlags(const MqttPacket *packet)
{
    switch (packet->type)
    {
        case MqttPacketTypePublish:
            return ((MqttPacketPublishDup(packet) & 1) << 3) |
                   ((MqttPacketPublishQos(packet) & 3) << 1) |
                   (MqttPacketPublishRetain(packet) & 1);

        case MqttPacketTypePubRel:
        case MqttPacketTypeSubscribe:
        case MqttPacketTypeUnsubscribe:
            return 0x2;

        default:
            return 0;
    }
}

static int MqttPacketBaseSerialize(const MqttPacket *packet, Stream *stream)
{
    unsigned char typeAndFlags;
    size_t remainingLength;

    typeAndFlags = ((packet->type & 0x0F) << 4) |
                   (MqttPacketFlags(packet) & 0x0F);
    remainingLength = MqttPacketGetRemainingLength(packet);

    LOG_DEBUG("type:%02X (%s) flags:%02X", packet->type,
        MqttPacketName(packet->type), MqttPacketFlags(packet));

    if (StreamWrite(&typeAndFlags, 1, stream) != 1)
        return -1;

    if (StreamWriteRemainingLength(remainingLength, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketWithIdSerialize(const MqttPacket *packet, Stream *stream)
{
    assert(MqttPacketHasId((const MqttPacket *) packet));

    if (MqttPacketBaseSerialize(packet, stream) == -1)
        return -1;

    if (StreamWriteUint16Be(packet->id, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketConnectSerialize(const MqttPacketConnect *packet, Stream *stream)
{
    if (MqttPacketBaseSerialize(&packet->base, stream) == -1)
        return -1;

    if (StreamWriteMqttString(&MqttProtocolId, stream) == -1)
        return -1;

    if (StreamWrite(&MqttProtocolLevel, 1, stream) != 1)
        return -1;

    if (StreamWrite(&packet->connectFlags, 1, stream) != 1)
        return -1;

    if (StreamWriteUint16Be(packet->keepAlive, stream) == -1)
        return -1;

    if (StreamWriteMqttString(packet->clientId, stream) == -1)
        return -1;

    if (packet->connectFlags & 0x04)
    {
        if (StreamWriteMqttString(packet->willTopic, stream) == -1)
            return -1;

        if (StreamWriteMqttString(packet->willMessage, stream) == -1)
            return -1;
    }

    if (packet->connectFlags & 0x80)
    {
        if (StreamWriteMqttString(packet->userName, stream) == -1)
            return -1;
    }

    if (packet->connectFlags & 0x40)
    {
        if (StreamWriteMqttString(packet->password, stream) == -1)
            return -1;
    }

    return 0;
}

static int MqttPacketSubscribeSerialize(const MqttPacketSubscribe *packet, Stream *stream)
{
    if (MqttPacketWithIdSerialize((const MqttPacket *) packet, stream) == -1)
        return -1;

    if (StreamWriteMqttString(packet->topicFilter, stream) == -1)
        return -1;

    if (StreamWrite(&packet->qos, 1, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketUnsubscribeSerialize(const MqttPacketUnsubscribe *packet, Stream *stream)
{
    if (MqttPacketWithIdSerialize((const MqttPacket *) packet, stream) == -1)
        return -1;

    if (StreamWriteMqttString(packet->topicFilter, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketPublishSerialize(const MqttPacketPublish *packet, Stream *stream)
{
    if (MqttPacketBaseSerialize((const MqttPacket *) packet, stream) == -1)
        return -1;

    if (StreamWriteMqttString(packet->topicName, stream) == -1)
        return -1;

    LOG_DEBUG("qos:%d", MqttPacketPublishQos(packet));

    if (MqttPacketPublishQos(packet) > 0)
    {
        if (StreamWriteUint16Be(packet->base.id, stream) == -1)
            return -1;
    }

    if (StreamWrite(bdata(packet->message), blength(packet->message), stream) == -1)
        return -1;

    return 0;
}

int MqttPacketSerialize(const MqttPacket *packet, Stream *stream)
{
    MqttPacketSerializeFunc f = NULL;

    switch (packet->type)
    {
        case MqttPacketTypeConnect:
            f = (MqttPacketSerializeFunc) MqttPacketConnectSerialize;
            break;

        case MqttPacketTypeConnAck:
            break;

        case MqttPacketTypePublish:
            f = (MqttPacketSerializeFunc) MqttPacketPublishSerialize;
            break;

        case MqttPacketTypePubAck:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypePubRec:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypePubRel:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypePubComp:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypeSubscribe:
            f = (MqttPacketSerializeFunc) MqttPacketSubscribeSerialize;
            break;

        case MqttPacketTypeSubAck:
            break;

        case MqttPacketTypeUnsubscribe:
            f = (MqttPacketSerializeFunc) MqttPacketUnsubscribeSerialize;
            break;

        case MqttPacketTypeUnsubAck:
            break;

        case MqttPacketTypePingReq:
            f = (MqttPacketSerializeFunc) MqttPacketBaseSerialize;
            break;

        case MqttPacketTypePingResp:
            break;

        case MqttPacketTypeDisconnect:
            f = (MqttPacketSerializeFunc) MqttPacketBaseSerialize;
            break;

        default:
            return -1;
    }

    assert(f != NULL && "no serializer");

    return f(packet, stream);
}

/**********************************************************************/
/*                           deserialize.h                            */
/**********************************************************************/

#ifndef DESERIALIZE_H
#define DESERIALIZE_H


typedef struct MqttPacket MqttPacket;
typedef struct Stream Stream;

int MqttPacketDeserialize(MqttPacket **packet, Stream *stream);

#endif

/**********************************************************************/
/*                           deserialize.c                            */
/**********************************************************************/


#include <stdlib.h>
#include <assert.h>

typedef int (*MqttPacketDeserializeFunc)(MqttPacket **packet, Stream *stream);

static int MqttPacketWithIdDeserialize(MqttPacket **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (remainingLength != 2)
        return -1;

    if (StreamReadUint16Be(&(*packet)->id, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketConnAckDeserialize(MqttPacketConnAck **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (remainingLength != 2)
        return -1;

    if (StreamRead(&(*packet)->connAckFlags, 1, stream) != 1)
        return -1;

    if (StreamRead(&(*packet)->returnCode, 1, stream) != 1)
        return -1;

    return 0;
}

static int MqttPacketSubAckDeserialize(MqttPacketSubAck **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    /* 2 bytes for packet id and 1 byte for single return code */
    if (remainingLength != 3)
        return -1;

    if (StreamReadUint16Be(&((*packet)->base.id), stream) == -1)
        return -1;

    if (StreamRead(&((*packet)->returnCode), 1, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketTypeUnsubAckDeserialize(MqttPacket **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (remainingLength != 2)
        return -1;

    if (StreamReadUint16Be(&(*packet)->id, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketPublishDeserialize(MqttPacketPublish **packet, Stream *stream)
{
    size_t remainingLength = 0;
    size_t payloadSize = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (StreamReadMqttString(&(*packet)->topicName, stream) == -1)
        return -1;

    LOG_DEBUG("remainingLength:%lu", remainingLength);

    payloadSize = remainingLength - blength((*packet)->topicName) - 2;

    LOG_DEBUG("qos:%d payloadSize:%lu", MqttPacketPublishQos(*packet),
        payloadSize);

    if (MqttPacketHasId((const MqttPacket *) *packet))
    {
        LOG_DEBUG("packet has id");
        payloadSize -= 2;
        if (StreamReadUint16Be(&((*packet)->base.id), stream) == -1)
        {
            return -1;
        }
    }

    LOG_DEBUG("reading payload payloadSize:%lu\n", payloadSize);

    /* Allocate extra byte for a NULL terminator. If the user tries to print
       the payload directly. */

    (*packet)->message = bfromcstralloc(payloadSize+1, "");

    if (StreamRead(bdata((*packet)->message), payloadSize, stream) == -1)
        return -1;

    (*packet)->message->slen = payloadSize;
    (*packet)->message->data[payloadSize] = '\0';

    return 0;
}

static int MqttPacketGenericDeserializer(MqttPacket **packet, Stream *stream)
{
    size_t remainingLength = 0;
    char buffer[256];

    (void) packet;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    while (remainingLength > 0)
    {
        size_t l = sizeof(buffer);

        if (remainingLength < l)
            l = remainingLength;

        if (StreamRead(buffer, l, stream) != (int64_t) l)
            return -1;

        remainingLength -= l;
    }

    return 0;
}

static int ValidateFlags(int type, int flags)
{
    int rv = 0;

    switch (type)
    {
        case MqttPacketTypePublish:
        {
            int qos = (flags >> 1) & 2;
            if (qos >= 0 && qos <= 2)
                rv = 1;
            break;
        }

        case MqttPacketTypePubRel:
        case MqttPacketTypeSubscribe:
        case MqttPacketTypeUnsubscribe:
            if (flags == 2)
            {
                rv = 1;
            }
            break;

        default:
            if (flags == 0)
            {
                rv = 1;
            }
            break;
    }

    return rv;
}

int MqttPacketDeserialize(MqttPacket **packet, Stream *stream)
{
    MqttPacketDeserializeFunc deserializer = NULL;
    char typeAndFlags;
    int type;
    int flags;
    int rv;

    if (StreamRead(&typeAndFlags, 1, stream) != 1)
        return -1;

    type = (typeAndFlags & 0xF0) >> 4;
    flags = (typeAndFlags & 0x0F);

    if (!ValidateFlags(type, flags))
    {
        return -1;
    }

    switch (type)
    {
        case MqttPacketTypeConnect:
            break;

        case MqttPacketTypeConnAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketConnAckDeserialize;
            break;

        case MqttPacketTypePublish:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketPublishDeserialize;
            break;

        case MqttPacketTypePubAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypePubRec:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypePubRel:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypePubComp:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypeSubscribe:
            break;

        case MqttPacketTypeSubAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketSubAckDeserialize;
            break;

        case MqttPacketTypeUnsubscribe:
            break;

        case MqttPacketTypeUnsubAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketTypeUnsubAckDeserialize;
            break;

        case MqttPacketTypePingReq:
            break;

        case MqttPacketTypePingResp:
            break;

        case MqttPacketTypeDisconnect:
            break;

        default:
            return -1;
    }

    if (!deserializer)
    {
        deserializer = MqttPacketGenericDeserializer;
    }

    *packet = MqttPacketNew(type);

    if (!*packet)
        return -1;

    if (type == MqttPacketTypePublish)
    {
        MqttPacketPublishDup(*packet) = (flags >> 3) & 1;
        MqttPacketPublishQos(*packet) = (flags >> 1) & 3;
        MqttPacketPublishRetain(*packet) = flags & 1;
    }

    rv = deserializer(packet, stream);

    return rv;
}

/**********************************************************************/
/*                              client.c                              */
/**********************************************************************/

#include "mqtt.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>

#if (LOG_LEVEL == LOG_LEVEL_DEBUG) && !defined(PRId64)
#error define PRId64 for your platform
#endif

TAILQ_HEAD(MessageList, MqttPacket);
typedef struct MessageList MessageList;

struct MqttClient
{
    SocketStream stream;
    /* client id, NULL if we want to have server generated id */
    bstring clientId;
    /* set to 1 if we want to have a clean session */
    int cleanSession;
    /* remote host and port */
    bstring host;
    short port;
    /* keepalive interval in seconds */
    int keepAlive;
    /* user specified data, not used by us */
    void *userData;
    /* callback called after connection is made */
    MqttClientOnConnectCallback onConnect;
    /* callback called after subscribe is done */
    MqttClientOnSubscribeCallback onSubscribe;
    /* callback called after subscribe is done */
    MqttClientOnUnsubscribeCallback onUnsubscribe;
    /* callback called when a message is received */
    MqttClientOnMessageCallback onMessage;
    /* callback called after publish is done and acknowledged */
    MqttClientOnPublishCallback onPublish;
    int stopped;
    /* packets waiting to be sent over network */
    SIMPLEQ_HEAD(, MqttPacket) sendQueue;
    /* sent messages that are not done yet */
    MessageList outMessages;
    /* received messages that are not done yet  */
    MessageList inMessages;
    int sessionPresent;
    /* when was the last packet sent */
    int64_t lastPacketSentTime;
    /* next packet id */
    uint16_t packetId;
    /* timeout after which to retry sending messages */
    int retryTimeout;
    /* maximum number of inflight messages (not packets!) */
    int maxInflight;
    /* maximum number of queued messages (not packets!) */
    int maxQueued;
    /* 1 if PINGREQ is sent and we are waiting for PINGRESP, 0 otherwise */
    int pingSent;
    bstring willTopic;
    bstring willMessage;
    int willQos;
    int willRetain;
};

enum MessageState
{
    MessageStateQueued = 100,
    MessageStateSend,
    MessageStateSent
};

static void MqttClientQueuePacket(MqttClient *client, MqttPacket *packet);
static int MqttClientQueueSimplePacket(MqttClient *client, int type);
static int MqttClientSendPacket(MqttClient *client, MqttPacket *packet);
static int MqttClientRecvPacket(MqttClient *client);
static uint16_t MqttClientNextPacketId(MqttClient *client);
static void MqttClientProcessMessageQueue(MqttClient *client);

static MQTT_INLINE int MqttClientInflightMessageCount(MqttClient *client)
{
    MqttPacket *packet;
    int queued = 0;
    int inMessagesCount = 0;
    int outMessagesCount = 0;

    TAILQ_FOREACH(packet, &client->outMessages, messages)
    {
        if (packet->state == MessageStateQueued)
        {
            ++queued;
        }

        ++outMessagesCount;
    }

    TAILQ_FOREACH(packet, &client->inMessages, messages)
    {
        ++inMessagesCount;
    }

    return inMessagesCount + outMessagesCount - queued;
}

MqttClient *MqttClientNew(const char *clientId)
{
    MqttClient *client;

    client = calloc(1, sizeof(*client));

    if (!client)
    {
        return NULL;
    }

    client->clientId = bfromcstr(clientId);

    client->stream.sock = -1;

    client->retryTimeout = 20;

    client->maxQueued = 0;
    client->maxInflight = 20;

    TAILQ_INIT(&client->outMessages);
    TAILQ_INIT(&client->inMessages);
    SIMPLEQ_INIT(&client->sendQueue);

    return client;
}

void MqttClientFree(MqttClient *client)
{
    MqttPacket *packet, *next;

    TAILQ_FOREACH_SAFE(packet, &client->outMessages, messages, next)
    {
        TAILQ_REMOVE(&client->outMessages, packet, messages);
        MqttPacketFree(packet);
    }

    TAILQ_FOREACH_SAFE(packet, &client->inMessages, messages, next)
    {
        TAILQ_REMOVE(&client->inMessages, packet, messages);
        MqttPacketFree(packet);
    }

    bdestroy(client->clientId);
    bdestroy(client->willTopic);
    bdestroy(client->willMessage);
    bdestroy(client->host);

    if (client->stream.sock != -1)
    {
        SocketDisconnect(client->stream.sock);
    }

    free(client);
}

void MqttClientSetUserData(MqttClient *client, void *userData)
{
    assert(client != NULL);
    client->userData = userData;
}

void *MqttClientGetUserData(MqttClient *client)
{
    assert(client != NULL);
    return client->userData;
}

void MqttClientSetOnConnect(MqttClient *client, MqttClientOnConnectCallback cb)
{
    assert(client != NULL);
    client->onConnect = cb;
}

void MqttClientSetOnSubscribe(MqttClient *client,
                              MqttClientOnSubscribeCallback cb)
{
    assert(client != NULL);
    client->onSubscribe = cb;
}

void MqttClientSetOnUnsubscribe(MqttClient *client,
                                MqttClientOnUnsubscribeCallback cb)
{
    assert(client != NULL);
    client->onUnsubscribe = cb;
}

void MqttClientSetOnMessage(MqttClient *client,
                            MqttClientOnMessageCallback cb)
{
    assert(client != NULL);
    client->onMessage = cb;
}

void MqttClientSetOnPublish(MqttClient *client,
                            MqttClientOnPublishCallback cb)
{
    assert(client != NULL);
    client->onPublish = cb;
}

int MqttClientConnect(MqttClient *client, const char *host, short port,
                      int keepAlive, int cleanSession)
{
    int sock;
    MqttPacketConnect *packet;

    assert(client != NULL);
    assert(host != NULL);

    client->host = bfromcstr(host);
    client->port = port;
    client->keepAlive = keepAlive;
    client->cleanSession = cleanSession;

    if (keepAlive < 0)
    {
        LOG_ERROR("invalid keepAlive: %d", keepAlive);
        return -1;
    }

    LOG_DEBUG("connecting");

    if ((sock = SocketConnect(host, port)) == -1)
    {
        LOG_ERROR("SocketConnect failed!");
        return -1;
    }

    if (SocketStreamOpen(&client->stream, sock) == -1)
    {
        return -1;
    }

    packet = (MqttPacketConnect *) MqttPacketNew(MqttPacketTypeConnect);

    if (!packet)
        return -1;

    if (client->cleanSession)
    {
        packet->connectFlags |= 0x02;
    }

    packet->keepAlive = client->keepAlive;

    packet->clientId = bstrcpy(client->clientId);

    if (client->willTopic)
    {
        packet->connectFlags |= 0x04;

        packet->willTopic = bstrcpy(client->willTopic);
        packet->willMessage = bstrcpy(client->willMessage);

        packet->connectFlags |= (client->willQos & 3) << 3;
        packet->connectFlags |= (client->willRetain & 1) << 5;
    }

    MqttClientQueuePacket(client, &packet->base);

    return 0;
}

int MqttClientDisconnect(MqttClient *client)
{
    LOG_DEBUG("disconnecting");
    return MqttClientQueueSimplePacket(client, MqttPacketTypeDisconnect);
}

int MqttClientIsConnected(MqttClient *client)
{
    return client->stream.sock != -1;
}

int MqttClientRunOnce(MqttClient *client, int timeout)
{
    int rv;
    int events;

    assert(client != NULL);

    if (client->stream.sock == -1)
    {
        LOG_ERROR("invalid socket");
        return -1;
    }

    events = EV_READ;

    /* Handle outMessages and inMessages, moving queued messages to sendQueue
       if there are less than maxInflight number of messages in flight */
    MqttClientProcessMessageQueue(client);

    if (SIMPLEQ_EMPTY(&client->sendQueue))
    {
        LOG_DEBUG("nothing to write");
    }
    else
    {
        events |= EV_WRITE;
    }

    LOG_DEBUG("selecting");

    if (timeout < 0)
    {
        timeout = client->keepAlive * 1000;
    }

    rv = SocketSelect(client->stream.sock, &events, timeout);

    if (rv == -1)
    {
        LOG_ERROR("select failed");
        return -1;
    }
    else if (rv)
    {
        LOG_DEBUG("select rv=%d", rv);

        if (events & EV_WRITE)
        {
            MqttPacket *packet;

            LOG_DEBUG("socket writable");

            packet = SIMPLEQ_FIRST(&client->sendQueue);

            if (packet)
            {
                SIMPLEQ_REMOVE_HEAD(&client->sendQueue, sendQueue);

                if (MqttClientSendPacket(client, packet) == -1)
                {
                    LOG_ERROR("MqttClientSendPacket failed");
                    client->stopped = 1;
                }
            }
        }

        if (events & EV_READ)
        {
            LOG_DEBUG("socket readable");

            if (MqttClientRecvPacket(client) == -1)
            {
                LOG_ERROR("MqttClientRecvPacket failed");
                client->stopped = 1;
            }
        }
    }
    else
    {
        LOG_DEBUG("select timeout");

        if (client->pingSent)
        {
            LOG_ERROR("no PINGRESP received in time");
            client->pingSent = 0;
            client->stopped = 1;
        }
        else if (SIMPLEQ_EMPTY(&client->sendQueue))
        {
            int64_t elapsed = MqttGetCurrentTime() - client->lastPacketSentTime;
            if (elapsed/1000 >= client->keepAlive && client->keepAlive > 0)
            {
                MqttClientQueueSimplePacket(client, MqttPacketTypePingReq);
                client->pingSent = 1;
            }
        }
    }

    if (client->stopped)
    {
        SocketDisconnect(client->stream.sock);
        client->stream.sock = -1;
    }

    return 0;
}

int MqttClientRun(MqttClient *client)
{
    assert(client != NULL);

    while (!client->stopped)
    {
        if (MqttClientRunOnce(client, -1) == -1)
            return -1;
    }

    return 0;
}

int MqttClientSubscribe(MqttClient *client, const char *topicFilter,
                        int qos)
{
    MqttPacketSubscribe *packet = NULL;

    assert(client != NULL);
    assert(topicFilter != NULL);
    assert(qos >= 0 && qos <= 2);

    packet = (MqttPacketSubscribe *) MqttPacketWithIdNew(
        MqttPacketTypeSubscribe, MqttClientNextPacketId(client));

    if (!packet)
        return -1;

    packet->topicFilter = bfromcstr(topicFilter);
    packet->qos = qos;

    MqttClientQueuePacket(client, (MqttPacket *) packet);

    TAILQ_INSERT_TAIL(&client->outMessages, (MqttPacket *) packet, messages);

    return MqttPacketId(packet);
}

int MqttClientUnsubscribe(MqttClient *client, const char *topicFilter)
{
    MqttPacketUnsubscribe *packet = NULL;

    assert(client != NULL);
    assert(topicFilter != NULL);

    packet = (MqttPacketUnsubscribe *) MqttPacketWithIdNew(
        MqttPacketTypeUnsubscribe, MqttClientNextPacketId(client));

    packet->topicFilter = bfromcstr(topicFilter);

    MqttClientQueuePacket(client, (MqttPacket *) packet);

    TAILQ_INSERT_TAIL(&client->outMessages, (MqttPacket *) packet, messages);

    return MqttPacketId(packet);
}

static MQTT_INLINE int MqttClientOutMessagesLen(MqttClient *client)
{
    MqttPacket *packet;
    int count = 0;
    TAILQ_FOREACH(packet, &client->outMessages, messages)
    {
        ++count;
    }
    return count;
}

int MqttClientPublish(MqttClient *client, int qos, int retain,
                      const char *topic, const void *data, size_t size)
{
    MqttPacketPublish *packet;

    assert(client != NULL);

    /* first check if the queue is already full */
    if (qos > 0 && client->maxQueued > 0 &&
        MqttClientOutMessagesLen(client) >= client->maxQueued)
    {
        return -1;
    }

    if (qos > 0)
    {
        packet = (MqttPacketPublish *) MqttPacketWithIdNew(
            MqttPacketTypePublish, MqttClientNextPacketId(client));
    }
    else
    {
        packet = (MqttPacketPublish *) MqttPacketNew(MqttPacketTypePublish);
    }

    if (!packet)
        return -1;

    packet->qos = qos;
    packet->retain = retain;
    packet->topicName = bfromcstr(topic);
    packet->message = blk2bstr(data, size);

    if (qos > 0)
    {
        /* check how many messages there are coming in and going out currently
           that are not yet done */
        if (client->maxInflight == 0 ||
            MqttClientInflightMessageCount(client) < client->maxInflight)
        {
            LOG_DEBUG("setting message (%d) state to MessageStateSend",
                      MqttPacketId(packet));
            packet->base.state = MessageStateSend;
        }
        else
        {
            LOG_DEBUG("setting message (%d) state to MessageStateQueued",
                      MqttPacketId(packet));
            packet->base.state = MessageStateQueued;
        }

        /* add the message to the outMessages queue to wait for processing */
        TAILQ_INSERT_TAIL(&client->outMessages, (MqttPacket *) packet,
            messages);
    }
    else
    {
        MqttClientQueuePacket(client, (MqttPacket *) packet);
    }

    if (qos > 0)
        return MqttPacketId(packet);

    return 0;
}

int MqttClientPublishCString(MqttClient *client, int qos, int retain,
                             const char *topic, const char *msg)
{
    return MqttClientPublish(client, qos, retain, topic, msg, strlen(msg));
}

void MqttClientSetPublishRetryTimeout(MqttClient *client, int timeout)
{
    assert(client != NULL);
    client->retryTimeout = timeout;
}

void MqttClientSetMaxMessagesInflight(MqttClient *client, int max)
{
    assert(client != NULL);
    client->maxInflight = max;
}

void MqttClientSetMaxQueuedMessages(MqttClient *client, int max)
{
    assert(client != NULL);
    client->maxQueued = max;
}

int MqttClientSetWill(MqttClient *client, const char *topic, const void *msg,
                      size_t size, int qos, int retain)
{
    assert(client != NULL);

    if (client->stream.sock != -1)
    {
        LOG_ERROR("MqttClientSetWill must be called before MqttClientConnect");
        return -1;
    }

    client->willTopic = bfromcstr(topic);
    client->willMessage = blk2bstr(msg, size);
    client->willQos = qos;
    client->willRetain = retain;

    return 0;
}

static void MqttClientQueuePacket(MqttClient *client, MqttPacket *packet)
{
    assert(client != NULL);
    LOG_DEBUG("queuing packet %s", MqttPacketName(packet->type));
    SIMPLEQ_INSERT_TAIL(&client->sendQueue, packet, sendQueue);
}

static int MqttClientQueueSimplePacket(MqttClient *client, int type)
{
    MqttPacket *packet = MqttPacketNew(type);
    if (!packet)
        return -1;
    MqttClientQueuePacket(client, packet);
    return 0;
}

static int MqttClientSendPacket(MqttClient *client, MqttPacket *packet)
{
    if (MqttPacketSerialize(packet, &client->stream.base) == -1)
        return -1;

    packet->sentAt = MqttGetCurrentTime();
    client->lastPacketSentTime = packet->sentAt;

    if (packet->type == MqttPacketTypeDisconnect)
    {
        client->stopped = 1;
    }

    /* If the packet is not on any message list, it can be removed after
       sending. */
    if (TAILQ_NEXT(packet, messages) == NULL &&
        TAILQ_PREV(packet, MessageList, messages) == NULL &&
        TAILQ_FIRST(&client->inMessages) != packet &&
        TAILQ_FIRST(&client->outMessages) != packet)
    {
        LOG_DEBUG("freeing packet %s after sending",
            MqttPacketName(MqttPacketType(packet)));
        MqttPacketFree(packet);
    }

    return 0;
}

static void MqttClientHandleConnAck(MqttClient *client,
    MqttPacketConnAck *packet)
{
    client->sessionPresent = packet->connAckFlags & 1;

    LOG_DEBUG("sessionPresent:%d", client->sessionPresent);

    if (client->onConnect)
    {
        LOG_DEBUG("calling onConnect rc:%d", packet->returnCode);
        client->onConnect(client, packet->returnCode, client->sessionPresent);
    }
}

static void MqttClientHandlePingResp(MqttClient *client)
{
    LOG_DEBUG("got ping response");
    client->pingSent = 0;
}

static void MqttClientHandleSubAck(MqttClient *client, MqttPacketSubAck *packet)
{
    MqttPacket *sub;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(sub, &client->outMessages, messages)
    {
        if (MqttPacketType(sub) == MqttPacketTypeSubscribe &&
            MqttPacketId(sub) == MqttPacketId(packet))
        {
            break;
        }
    }

    if (!sub)
    {
        LOG_ERROR("SUBSCRIBE with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, sub, messages);
        MqttPacketFree(sub);

        if (client->onSubscribe)
        {
            LOG_DEBUG("calling onSubscribe id:%d rc:%d", MqttPacketId(packet),
                packet->returnCode);
            client->onSubscribe(client, MqttPacketId(packet),
                                packet->returnCode);
        }
    }
}

static void MqttClientHandlePublish(MqttClient *client, MqttPacketPublish *packet)
{
    if (MqttPacketPublishQos(packet) == 2)
    {
        /* Check if we have sent a PUBREC previously with the same id. If we
           have, we have to resend the PUBREC. We must not call the onMessage
           callback again. */

        MqttPacket *pubRec;

        TAILQ_FOREACH(pubRec, &client->inMessages, messages)
        {
            if (MqttPacketId(pubRec) == MqttPacketId(packet) &&
                MqttPacketType(pubRec) == MqttPacketTypePubRec)
            {
                break;
            }
        }

        if (pubRec)
        {
            LOG_DEBUG("resending PUBREC id:%d", MqttPacketId(packet));
            MqttClientQueuePacket(client, pubRec);
            return;
        }
    }

    if (client->onMessage)
    {
        LOG_DEBUG("calling onMessage");
        client->onMessage(client,
            bdata(packet->topicName),
            bdata(packet->message),
            blength(packet->message),
            packet->qos,
            packet->retain);
    }

    if (MqttPacketPublishQos(packet) > 0)
    {
        int type = (MqttPacketPublishQos(packet) == 1) ? MqttPacketTypePubAck :
            MqttPacketTypePubRec;

        MqttPacket *resp = MqttPacketWithIdNew(type, MqttPacketId(packet));

        if (MqttPacketPublishQos(packet) == 2)
        {
            /* append to inMessages as we need a reply to this response */
            TAILQ_INSERT_TAIL(&client->inMessages, resp, messages);
        }

        MqttClientQueuePacket(client, resp);
    }
}

static void MqttClientHandlePubAck(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pub;

    TAILQ_FOREACH(pub, &client->outMessages, messages)
    {
        if (MqttPacketId(pub) == MqttPacketId(packet) &&
            MqttPacketType(pub) == MqttPacketTypePublish)
        {
            break;
        }
    }

    if (!pub)
    {
        LOG_ERROR("PUBLISH with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, pub, messages);
        MqttPacketFree(pub);

        if (client->onPublish)
        {
            client->onPublish(client, MqttPacketId(packet));
        }
    }
}

static void MqttClientHandlePubRec(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pub;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(pub, &client->outMessages, messages)
    {
        if (MqttPacketId(pub) == MqttPacketId(packet) &&
            MqttPacketType(pub) == MqttPacketTypePublish)
        {
            break;
        }
    }

    if (!pub)
    {
        LOG_ERROR("PUBLISH with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        MqttPacket *pubRel;

        TAILQ_REMOVE(&client->outMessages, pub, messages);
        MqttPacketFree(pub);

        pubRel = MqttPacketWithIdNew(MqttPacketTypePubRel, MqttPacketId(packet));
        pubRel->state = MessageStateSend;

        TAILQ_INSERT_TAIL(&client->outMessages, pubRel, messages);
    }
}

static void MqttClientHandlePubRel(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pubRec;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(pubRec, &client->inMessages, messages)
    {
        if (MqttPacketId(pubRec) == MqttPacketId(packet) &&
            MqttPacketType(pubRec) == MqttPacketTypePubRec)
        {
            break;
        }
    }

    if (!pubRec)
    {
        MqttPacket *pubComp;

        TAILQ_FOREACH(pubComp, &client->inMessages, messages)
        {
            if (MqttPacketId(pubComp) == MqttPacketId(packet) &&
                MqttPacketType(pubComp) == MqttPacketTypePubComp)
            {
                break;
            }
        }

        if (pubComp)
        {
            MqttClientQueuePacket(client, pubComp);
        }
        else
        {
            LOG_ERROR("PUBREC with id:%d not found", MqttPacketId(packet));
            client->stopped = 1;
        }
    }
    else
    {
        MqttPacket *pubComp;

        TAILQ_REMOVE(&client->inMessages, pubRec, messages);
        MqttPacketFree(pubRec);

        pubComp = MqttPacketWithIdNew(MqttPacketTypePubComp,
                                      MqttPacketId(packet));

        TAILQ_INSERT_TAIL(&client->inMessages, pubComp, messages);

        MqttClientQueuePacket(client, pubComp);
    }
}

static void MqttClientHandlePubComp(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pubRel;

    TAILQ_FOREACH(pubRel, &client->outMessages, messages)
    {
        if (MqttPacketId(pubRel) == MqttPacketId(packet) &&
            MqttPacketType(pubRel) == MqttPacketTypePubRel)
        {
            break;
        }
    }

    if (!pubRel)
    {
        LOG_ERROR("PUBREL with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, pubRel, messages);
        MqttPacketFree(pubRel);

        if (client->onPublish)
        {
            LOG_DEBUG("calling onPublish id:%d", MqttPacketId(packet));
            client->onPublish(client, MqttPacketId(packet));
        }
    }
}

static void MqttClientHandleUnsubAck(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *sub;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(sub, &client->outMessages, messages)
    {
        if (MqttPacketId(sub) == MqttPacketId(packet) &&
            MqttPacketType(sub) == MqttPacketTypeUnsubscribe)
        {
            break;
        }
    }

    if (!sub)
    {
        LOG_ERROR("UNSUBSCRIBE with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, sub, messages);
        MqttPacketFree(sub);

        if (client->onUnsubscribe)
        {
            LOG_DEBUG("calling onUnsubscribe id:%d", MqttPacketId(packet));
            client->onUnsubscribe(client, MqttPacketId(packet));
        }
    }
}

static int MqttClientRecvPacket(MqttClient *client)
{
    MqttPacket *packet = NULL;

    if (MqttPacketDeserialize(&packet, (Stream *) &client->stream) == -1)
        return -1;

    LOG_DEBUG("received packet %s", MqttPacketName(packet->type));

    switch (MqttPacketType(packet))
    {
        case MqttPacketTypeConnAck:
            MqttClientHandleConnAck(client, (MqttPacketConnAck *) packet);
            break;

        case MqttPacketTypePingResp:
            MqttClientHandlePingResp(client);
            break;

        case MqttPacketTypeSubAck:
            MqttClientHandleSubAck(client, (MqttPacketSubAck *) packet);
            break;

        case MqttPacketTypePublish:
            MqttClientHandlePublish(client, (MqttPacketPublish *) packet);
            break;

        case MqttPacketTypePubAck:
            MqttClientHandlePubAck(client, packet);
            break;

        case MqttPacketTypePubRec:
            MqttClientHandlePubRec(client, packet);
            break;

        case MqttPacketTypePubRel:
            MqttClientHandlePubRel(client, packet);
            break;

        case MqttPacketTypePubComp:
            MqttClientHandlePubComp(client, packet);
            break;

        case MqttPacketTypeUnsubAck:
            MqttClientHandleUnsubAck(client, packet);
            break;

        default:
            LOG_DEBUG("unhandled packet type=%d", MqttPacketType(packet));
            break;
    }

    MqttPacketFree(packet);

    return 0;
}

static uint16_t MqttClientNextPacketId(MqttClient *client)
{
    uint16_t id;
    assert(client != NULL);
    id = ++client->packetId;
    if (id == 0)
        id = ++client->packetId;
    return id;
}

static int64_t MqttPacketTimeSinceSent(MqttPacket *packet)
{
    int64_t now = MqttGetCurrentTime();
    return now - packet->sentAt;
}

static void MqttClientProcessInMessages(MqttClient *client)
{
    MqttPacket *packet, *next;

    LOG_DEBUG("processing inMessages");

    TAILQ_FOREACH_SAFE(packet, &client->inMessages, messages, next)
    {
        LOG_DEBUG("packet type:%s id:%d",
            MqttPacketName(MqttPacketType(packet)),
            MqttPacketId(packet));

        if (MqttPacketType(packet) == MqttPacketTypePubComp)
        {
            int64_t elapsed = MqttPacketTimeSinceSent(packet);
            if (packet->sentAt > 0 &&
                elapsed >= client->retryTimeout*1000)
            {
                LOG_DEBUG("freeing PUBCOMP with id:%d elapsed:%" PRId64,
                    MqttPacketId(packet), elapsed);

                TAILQ_REMOVE(&client->inMessages, packet, messages);

                MqttPacketFree(packet);
            }
        }
    }
}

static int MqttPacketShouldResend(MqttClient *client, MqttPacket *packet)
{
    if (packet->sentAt > 0 &&
        MqttPacketTimeSinceSent(packet) > client->retryTimeout*1000)
    {
        return 1;
    }

    return 0;
}

static void MqttClientProcessOutMessages(MqttClient *client)
{
    MqttPacket *packet, *next;
    int inflight = MqttClientInflightMessageCount(client);

    LOG_DEBUG("processing outMessages inflight:%d", inflight);

    TAILQ_FOREACH_SAFE(packet, &client->outMessages, messages, next)
    {
        LOG_DEBUG("packet type:%s id:%d state:%d",
            MqttPacketName(MqttPacketType(packet)),
            MqttPacketId(packet),
            packet->state);

        switch (packet->state)
        {
            case MessageStateQueued:
                if (inflight >= client->maxInflight)
                {
                    LOG_DEBUG("cannot dequeue %s/%d",
                        MqttPacketName(MqttPacketType(packet)),
                        MqttPacketId(packet));
                    break;
                }
                else
                {
                    /* If there's less than maxInflight messages currently
                       inflight, we can dequeue some messages by falling
                       through to MessageStateSend. */
                    LOG_DEBUG("dequeuing %s (%d)",
                        MqttPacketName(MqttPacketType(packet)),
                        MqttPacketId(packet));
                    ++inflight;
                }

            case MessageStateSend:
                packet->state = MessageStateSent;
                MqttClientQueuePacket(client, packet);
                break;

            case MessageStateSent:
                if (MqttPacketShouldResend(client, packet))
                {
                    packet->state = MessageStateSend;
                }
                break;

            default:
                break;
        }
    }
}

static void MqttClientProcessMessageQueue(MqttClient *client)
{
    MqttClientProcessInMessages(client);
    MqttClientProcessOutMessages(client);
}
