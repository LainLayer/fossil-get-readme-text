#ifndef LOGKA_H
#define LOGKA_H

// assume user included stdio.h

// Cyan
#define DEBUG_LABEL "[\033[36mDEBUG\033[0m]: "

// Yellow
#define WARN_LABEL  "[\033[33m WARN\033[0m]: "

// Red
#define ERROR_LABEL "[\033[31mERROR\033[0m]: "

// Blue
#define INFO_LABEL  "[\033[34m INFO\033[0m]: "

// Green
#define OK_LABEL    "[\033[32m   OK\033[0m]: "

#ifdef SILENT
	#define debug(format, ...) ((void)0)
	#define  warn(format, ...) ((void)0)
	#define error(format, ...) ((void)0)
	#define  info(format, ...) ((void)0)
	#define    ok(format, ...) ((void)0)
#else
	#define debug(format, ...) fprintf(stderr, DEBUG_LABEL format "\n", ##__VA_ARGS__)
	#define  warn(format, ...) fprintf(stderr, WARN_LABEL  format "\n", ##__VA_ARGS__)
	#define error(format, ...) fprintf(stderr, ERROR_LABEL format "\n", ##__VA_ARGS__)
	#define  info(format, ...) fprintf(stderr, INFO_LABEL  format "\n", ##__VA_ARGS__)
	#define    ok(format, ...) fprintf(stderr, OK_LABEL    format "\n", ##__VA_ARGS__)
#endif

#ifdef RELEASE
	#undef  debug
	#define debug(format, ...) ((void)0)
#endif

#endif // LOGKA_H
