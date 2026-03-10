# ============================================================
# Makefile for codexion
# ============================================================
# WHAT  : Compiles all .c files into the 'codexion' binary.
# WHY   : Subject requires Makefile with specific rules and
#         flags: -Wall -Wextra -Werror -pthread
# REMOVE: You'd have to compile manually with long commands.
# ============================================================

# ── Program name (required by subject) ─────────────────────
NAME    = codexion

# ── Compiler and flags ──────────────────────────────────────
# -Wall     : Enable all standard warnings
# -Wextra   : Enable extra warnings beyond -Wall
# -Werror   : Treat ALL warnings as errors (fail build on warn)
# -pthread  : Link the POSIX thread library
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread

# ── Source files (all .c files in coders/ directory) ────────
SRCS    = coders/main.c      \
          coders/args.c      \
          coders/time_utils.c \
          coders/log.c       \
          coders/queue.c     \
          coders/dongle.c    \
          coders/coder.c     \
          coders/monitor.c   \
          coders/sim.c

# ── Object files (same names, .o extension) ─────────────────
# WHY: We compile to .o first so only changed files recompile
#      (avoids unnecessary relinking — required by subject).
OBJS    = $(SRCS:.c=.o)

# ── Include path ─────────────────────────────────────────────
INCLUDE = -I coders/

# ── Default rule: build the program ─────────────────────────
# 'all' is the default target when you run just 'make'
all: $(NAME)

# ── Link rule: combine all .o files into the binary ─────────
# WHY: Only re-links if any .o file changed (no unnecessary relinking)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# ── Compile rule: .c → .o ────────────────────────────────────
# $< = the .c source file
# $@ = the .o output file
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# ── clean: remove object files ───────────────────────────────
# WHY: Keeps the directory clean without removing the binary
clean:
	rm -f $(OBJS)

# ── fclean: remove objects AND the binary ────────────────────
fclean: clean
	rm -f $(NAME)

# ── re: full rebuild from scratch ────────────────────────────
re: fclean all

# ── Prevent make from confusing rules with file names ────────
.PHONY: all clean fclean re
