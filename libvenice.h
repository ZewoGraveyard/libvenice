/*

  Copyright (c) 2015 Martin Sustrik

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

#ifndef LIBMILL_H_INCLUDED
#define LIBMILL_H_INCLUDED

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/******************************************************************************/
/*  ABI versioning support                                                    */
/******************************************************************************/

/*  Don't change this unless you know exactly what you're doing and have      */
/*  read and understand the following documents:                              */
/*  www.gnu.org/software/libtool/manual/html_node/Libtool-versioning.html     */
/*  www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html  */

/*  The current interface version. */
#define MILL_VERSION_CURRENT 13

/*  The latest revision of the current interface. */
#define MILL_VERSION_REVISION 0

/*  How many past interface versions are still supported. */
#define MILL_VERSION_AGE 1

/******************************************************************************/
/*  Symbol visibility                                                         */
/******************************************************************************/

#if defined MILL_NO_EXPORTS
#   define MILL_EXPORT
#else
#   if defined _WIN32
#      if defined MILL_EXPORTS
#          define MILL_EXPORT __declspec(dllexport)
#      else
#          define MILL_EXPORT __declspec(dllimport)
#      endif
#   else
#      if defined __SUNPRO_C
#          define MILL_EXPORT __global
#      elif (defined __GNUC__ && __GNUC__ >= 4) || \
             defined __INTEL_COMPILER || defined __clang__
#          define MILL_EXPORT __attribute__ ((visibility("default")))
#      else
#          define MILL_EXPORT
#      endif
#   endif
#endif

/******************************************************************************/
/*  Helpers                                                                   */
/******************************************************************************/

MILL_EXPORT int64_t now(void);

/******************************************************************************/
/*  Coroutines                                                                */
/******************************************************************************/

MILL_EXPORT void co(void* ctx, void (*routine)(void*), const char *created);
MILL_EXPORT size_t mill_clauselen();

MILL_EXPORT void goprepare(int count, size_t stack_size);

MILL_EXPORT extern volatile int mill_unoptimisable1;
MILL_EXPORT extern volatile void *mill_unoptimisable2;

MILL_EXPORT void *mill_go_prologue(const char *created);
MILL_EXPORT void mill_go_epilogue(void);

MILL_EXPORT void mill_yield(const char *current);
MILL_EXPORT void mill_msleep(int64_t deadline, const char *current);

#define mill_string2(x) #x
#define mill_string(x) mill_string2(x)

#define fdwait(fd, events, deadline) mill_fdwait((fd), (events), (deadline), __FILE__ ":" mill_string(__LINE__))

MILL_EXPORT void fdclean(int fd);

#define FDW_IN 1
#define FDW_OUT 2
#define FDW_ERR 4

MILL_EXPORT int mill_fdwait(int fd, int events, int64_t deadline, const char *current);

MILL_EXPORT pid_t mfork(void);
MILL_EXPORT int mill_number_of_cores(void);

/******************************************************************************/
/*  Channels                                                                  */
/******************************************************************************/

typedef struct mill_chan *chan;

#define MILL_CLAUSELEN (sizeof(struct{void *f1; void *f2; void *f3; void *f4; \
    void *f5; int f6; int f7; int f8;}))

MILL_EXPORT chan mill_chmake(size_t bufsz, const char *created);
MILL_EXPORT void mill_chs(chan ch, const char *current);
MILL_EXPORT void mill_chr(chan ch, const char *current);
MILL_EXPORT void mill_chdone(chan ch, const char *current);
MILL_EXPORT void mill_chclose(chan ch, const char *current);

MILL_EXPORT void mill_choose_init(const char *current);
MILL_EXPORT void mill_choose_in(void *clause, chan ch, int idx);
MILL_EXPORT void mill_choose_out(void *clause, chan ch, int idx);
MILL_EXPORT void mill_choose_otherwise(void);
MILL_EXPORT int mill_choose_wait(void);

MILL_EXPORT void mill_panic(const char *text);

/******************************************************************************/
/*  IP address library                                                        */
/******************************************************************************/

#define IPADDR_IPV4 1
#define IPADDR_IPV6 2
#define IPADDR_PREF_IPV4 3
#define IPADDR_PREF_IPV6 4
#define IPADDR_MAXSTRLEN 46

typedef struct {char data[32];} ipaddr;

MILL_EXPORT ipaddr iplocal(const char *name, int port, int mode);
MILL_EXPORT ipaddr ipremote(const char *name, int port, int mode, int64_t deadline);
MILL_EXPORT const char *ipaddrstr(ipaddr addr, char *ipstr);

/******************************************************************************/
/*  TCP library                                                               */
/******************************************************************************/

typedef struct mill_tcpsock *tcpsock;

MILL_EXPORT tcpsock tcplisten(ipaddr addr, int backlog, int reuseport);
MILL_EXPORT int tcpport(tcpsock s);
MILL_EXPORT tcpsock tcpaccept(tcpsock s, int64_t deadline);
MILL_EXPORT ipaddr tcpaddr(tcpsock s);
MILL_EXPORT tcpsock tcpconnect(ipaddr addr, int64_t deadline);
MILL_EXPORT size_t tcpsend(tcpsock s, const void *buf, size_t len, int64_t deadline);
MILL_EXPORT void tcpflush(tcpsock s, int64_t deadline);
MILL_EXPORT size_t tcprecv(tcpsock s, void *buf, size_t len, int64_t deadline);
MILL_EXPORT size_t tcprecvlh(tcpsock s, void *buf, size_t lowwater, size_t highwater, int64_t deadline);
MILL_EXPORT size_t tcprecvuntil(tcpsock s, void *buf, size_t len, const char *delims, size_t delimcount, int64_t deadline);
MILL_EXPORT void tcpclose(tcpsock s);
MILL_EXPORT tcpsock tcpattach(int fd, int listening);
MILL_EXPORT int tcpdetach(tcpsock s);

/******************************************************************************/
/*  UDP library                                                               */
/******************************************************************************/

typedef struct mill_udpsock *udpsock;

MILL_EXPORT udpsock udplisten(ipaddr addr);
MILL_EXPORT int udpport(udpsock s);
MILL_EXPORT void udpsend(udpsock s, ipaddr addr, const void *buf, size_t len);
MILL_EXPORT size_t udprecv(udpsock s, ipaddr *addr,
                           void *buf, size_t len, int64_t deadline);
MILL_EXPORT void udpclose(udpsock s);
MILL_EXPORT udpsock udpattach(int fd);
MILL_EXPORT int udpdetach(udpsock s);

/******************************************************************************/
/*  UNIX library                                                              */
/******************************************************************************/

typedef struct mill_unixsock *unixsock;

MILL_EXPORT unixsock unixlisten(const char *addr, int backlog);
MILL_EXPORT unixsock unixaccept(unixsock s, int64_t deadline);
MILL_EXPORT unixsock unixconnect(const char *addr);
MILL_EXPORT void unixpair(unixsock *a, unixsock *b);
MILL_EXPORT size_t unixsend(unixsock s, const void *buf, size_t len,
                            int64_t deadline);
MILL_EXPORT void unixflush(unixsock s, int64_t deadline);
MILL_EXPORT size_t unixrecv(unixsock s, void *buf, size_t len,
                            int64_t deadline);
MILL_EXPORT size_t unixrecvuntil(unixsock s, void *buf, size_t len,
                                 const char *delims, size_t delimcount, int64_t deadline);
MILL_EXPORT void unixclose(unixsock s);
MILL_EXPORT unixsock unixattach(int fd, int listening);
MILL_EXPORT int unixdetach(unixsock s);

/******************************************************************************/
/*  File library                                                              */
/******************************************************************************/

typedef struct mill_file *mfile;
MILL_EXPORT mfile fileopen(const char *pathname, int flags, mode_t mode);
MILL_EXPORT size_t filewrite(mfile f, const void *buf, size_t len, int64_t deadline);
MILL_EXPORT void fileflush(mfile f, int64_t deadline);
MILL_EXPORT size_t fileread(mfile f, void *buf, size_t len, int64_t deadline);
MILL_EXPORT size_t filereadlh(mfile f, void *buf, size_t lowwater, size_t highwater, int64_t deadline);
MILL_EXPORT void fileclose(mfile f);
MILL_EXPORT mfile fileattach(int fd);
MILL_EXPORT int filedetach(mfile f);
MILL_EXPORT off_t filetell(mfile f);
MILL_EXPORT off_t fileseek(mfile f, off_t offset);
MILL_EXPORT int fileeof(mfile f);

/******************************************************************************/
/*  Debugging                                                                 */
/******************************************************************************/

MILL_EXPORT void goredump(void);
MILL_EXPORT void gotrace(int level);

#endif

