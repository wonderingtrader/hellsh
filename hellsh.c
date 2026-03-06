#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>

/* ───────────────────────── default sins ───────────────────────── */
static const char *DEFAULT_SINS[] = {
    "lust", "greed", "gluttony", "sloth",
    "wrath", "envy", "pride", NULL
};

/* ───────────────────────── globals ────────────────────────────── */
static struct termios original;
static int tty_saved  = 0;
static int death_mode = 0;

/* ───────────────────────── colours ────────────────────────────── */
#define RED     "\033[1;31m"
#define DIM     "\033[0;31m"
#define BOLD    "\033[1m"
#define ITALIC  "\033[3m"
#define RESET   "\033[0m"

/* ───────────────────────── signal / tty ───────────────────────── */
static void block(int sig) { (void)sig; }

static void restore_tty(void) {
    if (tty_saved)
        tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

static FILE *open_tty(void) {
    int fd = open("/dev/tty", O_RDWR);
    return (fd >= 0) ? fdopen(fd, "w") : stdout;
}
static void close_tty(FILE *tty) {
    if (tty != stdout) fclose(tty);
}

static void lock_tty(void) {
    if (tcgetattr(STDIN_FILENO, &original) == 0) {
        tty_saved = 1;
        atexit(restore_tty);
        struct termios raw = original;
        raw.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON | ISIG);
        raw.c_cc[VMIN]  = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
}

static void unlock_tty_echo(void) {
    if (tty_saved) {
        struct termios cooked = original;
        cooked.c_lflag &= ~(ICANON | ISIG);
        cooked.c_lflag |= ECHO | ECHOE | ECHOK | ECHONL;
        cooked.c_cc[VMIN]  = 1;
        cooked.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &cooked);
    }
}

/* ───────────────────────── helpers ────────────────────────────── */
static void ms_sleep(int ms) {
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

static void tty_write(FILE *tty, const char *s) {
    fputs(s, tty);
    fflush(tty);
}

static void type_str(FILE *tty, const char *s, int delay_ms) {
    for (; *s; s++) {
        fputc(*s, tty);
        fflush(tty);
        ms_sleep(delay_ms);
    }
}

static void pause_dramatic(int ms) { ms_sleep(ms); }

/* ───────────────────────── print helpers ───────────────────────── */
static void print_response(const char *msg) {
    FILE *tty = open_tty();
    fprintf(tty, RED "%s" RESET "\n", msg);
    fflush(tty);
    close_tty(tty);
}

static void print_prompt(const char *buf, int len) {
    FILE *tty = open_tty();
    fprintf(tty, "\r\033[K" RED "> " RESET DIM);
    for (int i = 0; i < len; i++) fputc(buf[i], tty);
    fprintf(tty, RESET);
    fflush(tty);
    close_tty(tty);
}

/* ───────────────────────── sins file ───────────────────────────── */
#define MAX_SINS 64
#define SIN_LEN  64

static char loaded_sins[MAX_SINS][SIN_LEN];
static int  sin_count = 0;

static void ensure_sins_file(void) {
    if (access(".sins", F_OK) != 0) {
        FILE *f = fopen(".sins", "w");
        if (f) {
            for (int i = 0; DEFAULT_SINS[i]; i++)
                fprintf(f, "%s\n", DEFAULT_SINS[i]);
            fclose(f);
        }
    }
}

static void load_sins(void) {
    sin_count = 0;
    FILE *f = fopen(".sins", "r");
    if (!f) {
        for (int i = 0; DEFAULT_SINS[i] && sin_count < MAX_SINS; i++) {
            strncpy(loaded_sins[sin_count], DEFAULT_SINS[i], SIN_LEN - 1);
            loaded_sins[sin_count][SIN_LEN - 1] = '\0';
            sin_count++;
        }
        return;
    }
    char line[SIN_LEN];
    while (fgets(line, sizeof(line), f) && sin_count < MAX_SINS) {
        int l = strlen(line);
        while (l > 0 && (line[l-1] == '\n' || line[l-1] == '\r'))
            line[--l] = '\0';
        if (l > 0) {
            strncpy(loaded_sins[sin_count], line, SIN_LEN - 1);
            loaded_sins[sin_count][SIN_LEN - 1] = '\0';
            sin_count++;
        }
    }
    fclose(f);
}

/* ───────────────────────── screens ─────────────────────────────── */
static void show_screen(void) {
    FILE *tty = open_tty();
    tty_write(tty, "\033[2J\033[H" RED);
    tty_write(tty, "\n");
    tty_write(tty, "  HELLSH\n");
    tty_write(tty, "\n");
    tty_write(tty, "  WARNING\n");
    tty_write(tty, "\n");
    tty_write(tty, "  THIS TERMINAL IS RESTRICTED. ALL INPUT IS MONITORED AND LOGGED.\n");
    tty_write(tty, "  UNAUTHORIZED ACCESS OR TAMPERING IS STRICTLY PROHIBITED.\n");
    tty_write(tty, "  DISCONNECT IMMEDIATELY IF NOT AUTHORIZED.\n");
    tty_write(tty, "\n");
    tty_write(tty, "\n");
    tty_write(tty, "  Sent to Hell\n");
    tty_write(tty, "\n" RESET);
    fflush(tty);
    close_tty(tty);
}

static void do_death(void) {
    death_mode = 1;
    FILE *tty = open_tty();
    tty_write(tty, "\033[2J\033[H");
    fflush(tty);
    close_tty(tty);
}

/* ───────────────────────── judgment scene ───────────────────────── */

static void rline(FILE *tty, const char *prefix, const char *text, int delay) {
    if (prefix && prefix[0]) {
        fprintf(tty, DIM "%s" RED, prefix);
        fflush(tty);
    }
    type_str(tty, text, delay);
    tty_write(tty, RESET "\n");
}

static void judgment_scene(void) {
    FILE *tty = open_tty();

    tty_write(tty, "\033[2J\033[H");
    pause_dramatic(700);

    tty_write(tty, "\033[2J\033[H\n\n\n" RED);
    tty_write(tty, "               THE JUDGMENT\n");
    tty_write(tty, "        ---------------------------------\n" RESET "\n\n");
    fflush(tty);
    pause_dramatic(1400);

    rline(tty, "  ", "The light dims. A figure stands before you.", 28);
    pause_dramatic(600);
    rline(tty, "  ", "White robes. Wounded hands. Eyes like dying stars.", 28);
    pause_dramatic(800);
    rline(tty, "  ", "He opens the Book.", 30);
    pause_dramatic(1200);

    tty_write(tty, "\n");
    rline(tty, "  [Jesus]  ", "I know thee.", 45);
    pause_dramatic(900);
    rline(tty, "  [Jesus]  ", "The record of thy soul is written thus:", 38);
    pause_dramatic(700);
    tty_write(tty, "\n");

    for (int i = 0; i < sin_count; i++) {
        ms_sleep(320);
        fprintf(tty, DIM "    %2d.  " RED "%s" RESET "\n",
                i + 1, loaded_sins[i]);
        fflush(tty);
    }
    tty_write(tty, "\n");
    pause_dramatic(1000);

    rline(tty, "  [Jesus]  ", "I weigh thy deeds against mercy.", 38);
    pause_dramatic(500);
    tty_write(tty, RED "  [ " RESET);
    fflush(tty);
    for (int i = 0; i < sin_count; i++) {
        ms_sleep(220);
        tty_write(tty, RED "#" RESET);
    }
    tty_write(tty, RED " ]" RESET "\n\n");
    fflush(tty);
    pause_dramatic(900);

    rline(tty, "  [Jesus]  ", "The scales do not lie.", 38);
    pause_dramatic(700);

    const char *verdicts[][2] = {
        { "lust",      "Thou hast burned for flesh that was not thine." },
        { "greed",     "Thou hast hoarded what the poor needed to survive." },
        { "gluttony",  "Thou hast consumed without gratitude or restraint." },
        { "sloth",     "The gifts I gave thee rotted in thy idle hands." },
        { "wrath",     "Thou hast turned my name into a weapon." },
        { "envy",      "Thou hast despised what I blessed in thy neighbour." },
        { "pride",     "Thou hast placed thyself above the Author of all things." },
    };
    int n_verdicts = (int)(sizeof(verdicts) / sizeof(verdicts[0]));

    tty_write(tty, "\n");
    for (int i = 0; i < sin_count; i++) {
        const char *v = NULL;
        for (int j = 0; j < n_verdicts; j++) {
            if (strcmp(loaded_sins[i], verdicts[j][0]) == 0) {
                v = verdicts[j][1];
                break;
            }
        }
        if (v) {
            fprintf(tty, DIM "  [%s]  " RESET, loaded_sins[i]);
            fflush(tty);
            type_str(tty, v, 30);
            tty_write(tty, "\n");
            pause_dramatic(500);
        }
    }

    tty_write(tty, "\n");
    pause_dramatic(1000);
    rline(tty, "  [Jesus]  ", "I have wept for thee. I have called to thee.", 36);
    pause_dramatic(700);
    rline(tty, "  [Jesus]  ", "Thou didst not answer.", 36);
    pause_dramatic(900);
    rline(tty, "  [Jesus]  ", "Depart from me.", 55);
    pause_dramatic(1400);

    tty_write(tty, "\033[2J\033[H\n" RED);
    tty_write(tty,
        "          +---------------------------------+\n"
        "          |                                 |\n"
        "          |        GATES OF HELL            |\n"
        "          |                                 |\n"
        "          |    ABANDON ALL HOPE             |\n"
        "          |    YE WHO ENTER                 |\n"
        "          |                                 |\n"
        "          |          < YOU >                |\n"
        "          |                                 |\n"
        "          +---------------------------------+\n"
    );
    tty_write(tty, RESET "\n");
    fflush(tty);
    pause_dramatic(900);

    for (int i = 0; i < 6; i++) {
        tty_write(tty, RED "          >>>   THE GATE OPENS   <<<" RESET "\n");
        fflush(tty);
        ms_sleep(220);
        tty_write(tty, "\033[A\033[2K");
        fflush(tty);
        ms_sleep(160);
    }
    tty_write(tty, RED "          >>>   THE GATE OPENS   <<<" RESET "\n");
    fflush(tty);
    pause_dramatic(1300);

    tty_write(tty, "\033[2J\033[H\n\n\n\n\n" RED);
    tty_write(tty, "          +----------------------------+\n");
    tty_write(tty, "          |                            |\n");
    tty_write(tty, "          |       SENT TO HELL         |\n");
    tty_write(tty, "          |                            |\n");
    tty_write(tty, "          +----------------------------+\n");
    tty_write(tty, RESET "\n\n");
    fflush(tty);
    pause_dramatic(2200);

    show_screen();
    close_tty(tty);
}

/* ───────────────────────── beg scene ───────────────────────────── */

static void beg_scene(void) {
    FILE *tty = open_tty();

    /* restore echo briefly so user can type */
    unlock_tty_echo();

    tty_write(tty, "\033[2J\033[H");
    pause_dramatic(500);

    /* title */
    tty_write(tty, "\033[2J\033[H\n\n\n" RED);
    tty_write(tty, "               THE PLEADING\n");
    tty_write(tty, "        ---------------------------------\n" RESET "\n\n");
    fflush(tty);
    pause_dramatic(1200);

    rline(tty, "  ", "The fire quiets. A vast silence falls.", 28);
    pause_dramatic(700);
    rline(tty, "  ", "A figure descends — not the beast, but a man.", 28);
    pause_dramatic(700);
    rline(tty, "  ", "Adam. The first. His eyes carry the weight of all death.", 28);
    pause_dramatic(1000);
    tty_write(tty, "\n");

    /* Adam speaks first */
    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "I know why thou art here.", 40);
    tty_write(tty, "\n");
    pause_dramatic(800);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "I stood where thou didst stand. I made the first choice.", 38);
    tty_write(tty, "\n");
    pause_dramatic(700);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "And every child of mine has paid for it ever since.", 38);
    tty_write(tty, "\n");
    pause_dramatic(900);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "Speak then. Beg if thou must. I have heard every plea.", 38);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(600);

    /* === ROUND 1 === */
    fprintf(tty, RED "  > " RESET DIM);
    fflush(tty);

    char line[256];
    if (fgets(line, sizeof(line), stdin)) {
        int l = strlen(line);
        while (l > 0 && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = '\0';
    }
    tty_write(tty, RESET "\n");
    pause_dramatic(900);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "Thou speakest as they all do. Words dressed in desperation.", 38);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "I said the same when the angel barred Eden's gate.", 38);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "I wept. I begged. I named every reason.", 38);
    tty_write(tty, "\n");
    pause_dramatic(700);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "The gate did not open.", 50);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(800);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "Thou knowest why thou art here. Say it plainly.", 40);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(500);

    /* === ROUND 2 === */
    fprintf(tty, RED "  > " RESET DIM);
    fflush(tty);

    if (fgets(line, sizeof(line), stdin)) {
        int l = strlen(line);
        while (l > 0 && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = '\0';
    }
    tty_write(tty, RESET "\n");
    pause_dramatic(1100);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "...", 200);
    tty_write(tty, "\n");
    pause_dramatic(800);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "I carried that same confession for nine hundred years.", 38);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "Guilt is the one crop that never fails to grow in hell.", 38);
    tty_write(tty, "\n");
    pause_dramatic(700);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "But guilt is not repentance. Regret is not change.", 40);
    tty_write(tty, "\n");
    pause_dramatic(800);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "And here — in this place — change is no longer possible.", 40);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(900);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "Tell me one reason I should carry thy plea to Him.", 40);
    tty_write(tty, "\n");
    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "One. True. Reason.", 55);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(500);

    /* === ROUND 3 === */
    fprintf(tty, RED "  > " RESET DIM);
    fflush(tty);

    if (fgets(line, sizeof(line), stdin)) {
        int l = strlen(line);
        while (l > 0 && (line[l-1] == '\n' || line[l-1] == '\r')) line[--l] = '\0';
    }
    tty_write(tty, RESET "\n");
    pause_dramatic(1400);

    /* long pause — Adam considers */
    fprintf(tty, DIM "  [Adam considers. The fire dims slightly.]\n" RESET);
    fflush(tty);
    pause_dramatic(2000);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "...", 300);
    tty_write(tty, "\n");
    pause_dramatic(1000);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "There was one who came here once.", 40);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "Three days. He descended into this very dark.", 38);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "He walked through it. Every corner. Every scream.", 38);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "He could have taken me then.", 42);
    tty_write(tty, "\n");
    pause_dramatic(800);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "He did not take thee.", 48);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(900);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "That is not my decision to make.", 40);
    tty_write(tty, "\n");
    pause_dramatic(600);

    fprintf(tty, RED "  [Adam]  " RESET);
    fflush(tty);
    type_str(tty, "It never was.", 50);
    tty_write(tty, "\n\n");
    fflush(tty);
    pause_dramatic(1000);

    /* Adam departs */
    fprintf(tty, DIM "  [Adam turns away. The fire returns.]\n" RESET);
    fflush(tty);
    pause_dramatic(1200);

    fprintf(tty, DIM "  [His footsteps fade into the dark.]\n" RESET);
    fflush(tty);
    pause_dramatic(1500);

    fprintf(tty, DIM "  [You are alone again.]\n\n" RESET);
    fflush(tty);
    pause_dramatic(2000);

    /* return to shell */
    lock_tty();
    show_screen();
    close_tty(tty);
}

/* ───────────────────────── command table ────────────────────────── */
typedef struct { const char *cmd; const char *resp; } CmdEntry;

static const CmdEntry EXACT[] = {
    { "whoami",      "Lucifer" },
    { "pwd",         "/hell/fireplace" },
    { "date",        "judgement" },
    { "whereis",     "in the middle of the fire, burning from thy sins" },
    { "ls",          "only ashes remain" },
    { "exit",        "no one can escape their sins." },
    { "ps",          "tormented souls: 666" },
    { "kill",        "fire cannot kill fire" },
    { "chmod",       "permissions are irrelevant in hell" },
    { "sudo",        "there is no authority here but the flames" },
    { "history",     "a record of thy sins: burning" },
    { "top",         "asmodeus, beelzebub, lucifer, satan" },
    { "diff",        "all sins look the same in the fire" },
    { "tail",        "the serpent's tail" },
    { "head",        "the beast with seven heads" },
    { "df",          "disk full of sinners" },
    { "du",          "damned users: everyone" },
    { "mount",       "mounted upon the flames" },
    { "umount",      "cannot unmount eternal suffering" },
    { "dmesg",       "demons messaging..." },
    { "journalctl",  "the book of life is already burning" },
    { "ping",        "praying to deaf heavens... timeout" },
    { "curl",        "curling in fetal position from the heat" },
    { "wget",        "why get? there is only give... give thy soul" },
    { "ssh",         "screaming silently here" },
    { "netstat",     "network of pain: established to hell" },
    { "satan",       "you called?" },
    { "666",         "the number of the beast" },
    { "fire",        "you are surrounded by it" },
    { "helpme",      "too late for that" },
    { "please",      "pleading won't save you now" },
    { "heaven",      "no signal from up there" },
    { "cross",       "melts in thy hands" },
    { "bible",       "ashes" },
    /* new exact commands */
    { "less",        "less than dust, less than ash" },
    { "more",        "more fire, more pain, more eternity" },
    { "sort",        "the righteous from the damned? already sorted" },
    { "uniq",        "all sinners are alike in the flames" },
    { "wc",          "words: none. cries: countless. sins: 7" },
    { "cut",         "cut off from mercy" },
    { "paste",       "pasted to the flames" },
    { "join",        "join the choir of the damned" },
    { "comm",        "commune with the fire" },
    { "patch",       "no patches for a damned soul" },
    { "awk",         "awaken, thou who sleeps in damnation" },
    { "sed",         "the seducer awaits" },
    { "git",         "the sins of the father are committed to the son" },
    { "make",        "thou hast made thy bed in hell" },
    { "gcc",         "gathering cinders constantly" },
    { "python",      "the serpent speaks: \"forgive them not\"" },
    { "python3",     "the serpent speaks: \"forgive them not\"" },
    { "perl",        "pearls before swine, thy soul before the flames" },
    { "ruby",        "ruby red like the eyes of the beast" },
    { "node",        "node of the gallows" },
    { "npm",         "nails piercing my palms" },
    { "pip",         "pipes of hell play thy dirge" },
    { "java",        "java? just another vessel aflame" },
    { "systemctl",   "system cannot control the flames" },
    { "service",     "services: none. suffering: active" },
    { "crontab",     "cron tab? the appointed time was yesterday" },
    { "at",          "at the gates of hell" },
    { "batch",       "batch processing of souls" },
    { "shutdown",    "shutting down? the fire never sleeps" },
    { "reboot",      "reboot? resurrection is not for thee" },
    { "init",        "in the beginning was the Word... thou didst not listen" },
    { "status",      "\xe2\x9c\x9d" },
    { "0",           "the number of fucks given" },
    { "1",           "the one who sent thee here" },
    { "7",           "the number of thy sins" },
    { "40",          "days of flood, years of wandering, now eternity of fire" },
    { NULL, NULL }
};

static const CmdEntry PREFIX[] = {
    { "touch",   "thy fingers burn" },
    { "cd",      "every path leads to the flame" },
    { "rm",      "the fire has already taken it" },
    { "cat",     "the screams of the damned" },
    { "echo",    "thy cries return unanswered" },
    { "grep",    "searching through the flames... found: more fire" },
    { "find",    "wandering through the inferno" },
    { NULL, NULL }
};

static int prefix_matches(const char *cmd, const char *prefix) {
    size_t l = strlen(prefix);
    return strncmp(cmd, prefix, l) == 0 &&
           (cmd[l] == '\0' || cmd[l] == ' ');
}

static void handle_command(const char *cmd) {
    if (strcmp(cmd, "death") == 0) { do_death(); return; }

    if (strcmp(cmd, "beg") == 0) {
        beg_scene();
        return;
    }

    if (strcmp(cmd, "clear") == 0) {
        FILE *tty = open_tty();
        fprintf(tty, "\033[2J\033[H");
        fflush(tty);
        close_tty(tty);
        print_response("the smoke clears but the fire remains");
        return;
    }

    FILE *nl = open_tty();
    fprintf(nl, "\n");
    fflush(nl);
    close_tty(nl);

    for (int i = 0; EXACT[i].cmd; i++) {
        if (strcmp(cmd, EXACT[i].cmd) == 0) {
            print_response(EXACT[i].resp);
            return;
        }
    }

    for (int i = 0; PREFIX[i].cmd; i++) {
        if (prefix_matches(cmd, PREFIX[i].cmd)) {
            print_response(PREFIX[i].resp);
            return;
        }
    }

    if (cmd[0] != '\0')
        print_response("command not found in the depths of hell");
}

/* ───────────────────────── register as shell ────────────────────── */

/*
 * To register hellsh as a valid login shell on the system:
 *   1. Build:   gcc -O2 -o hellsh hellsh.c && sudo cp hellsh /usr/local/bin/hellsh
 *   2. Register: echo /usr/local/bin/hellsh | sudo tee -a /etc/shells
 *   3. Assign:   sudo chsh -s /usr/local/bin/hellsh <username>
 *
 * hellsh honours the POSIX convention that a login shell argv[0] begins
 * with '-'.  When invoked as a login shell we simply run normally; the
 * judgment scene acts as the "motd / profile" equivalent.
 */

static void register_shell_hint(void) {
    /* Non-intrusive: only print when explicitly invoked with --register-hint */
    FILE *tty = open_tty();
    tty_write(tty, RED "\n  To make hellsh a registered shell:\n" RESET);
    tty_write(tty, DIM
        "    sudo cp hellsh /usr/local/bin/hellsh\n"
        "    echo /usr/local/bin/hellsh | sudo tee -a /etc/shells\n"
        "    sudo chsh -s /usr/local/bin/hellsh $USER\n\n" RESET);
    fflush(tty);
    close_tty(tty);
}

/* ───────────────────────── main ─────────────────────────────────── */
int main(int argc, char *argv[]) {

    /* --register-hint: print instructions and exit */
    if (argc > 1 && strcmp(argv[1], "--register-hint") == 0) {
        register_shell_hint();
        return 0;
    }

    /*
     * POSIX login shell: argv[0][0] == '-'
     * We behave identically whether login or interactive; the judgment
     * scene replaces the normal login motd.
     */

    setsid();

    signal(SIGINT,  block);
    signal(SIGTSTP, block);
    signal(SIGQUIT, block);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGTERM, block);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGUSR1, block);
    signal(SIGUSR2, block);

    ensure_sins_file();
    load_sins();

    show_screen();
    unlock_tty_echo();
    lock_tty();

    judgment_scene();

    lock_tty();

    char cmd[256] = {0};
    int  cmd_len = 0;
    char buf[1];

    print_prompt(cmd, cmd_len);

    while (1) {
        int r = read(STDIN_FILENO, buf, 1);
        if (r < 0 && errno == EINTR) continue;
        if (r <= 0) {
            if (!death_mode) {
                show_screen();
                lock_tty();
                cmd_len = 0;
                print_prompt(cmd, cmd_len);
            }
            sleep(1);
            continue;
        }

        if (death_mode) continue;

        char c = buf[0];

        if (c == '\r' || c == '\n') {
            cmd[cmd_len] = '\0';
            handle_command(cmd);
            cmd_len = 0;
            if (!death_mode) print_prompt(cmd, cmd_len);
        } else if (c == 127 || c == '\b') {
            if (cmd_len > 0) {
                cmd_len--;
                print_prompt(cmd, cmd_len);
            }
        } else if (c >= 0x20 && c < 0x7f) {
            if (cmd_len < (int)(sizeof(cmd) - 1)) {
                cmd[cmd_len++] = c;
                print_prompt(cmd, cmd_len);
            }
        }
    }

    return 0;
}
