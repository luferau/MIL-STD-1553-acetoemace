#ifndef __BUWIN32_H__
#define __BUWIN32_H__

#define BUOS_95     1
#define BUOS_NT     2
#define BUOS_2K     3

#define BEGINOS(x) if (advBuWin32Platform() == x) {
#define ENDOS }

#endif