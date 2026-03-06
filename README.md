# hellsh 

> *A shell for the damned.*

hellsh is a fake Unix shell written in C that greets you with a full judgment scene, reads your sins from a file, delivers verdicts on each one, and then drops you into a custom interactive shell where every command is a sentence from hell. You can also beg Adam for mercy. He won't help.

All text renders in red. All signals are blocked. There is no escape.

---

## build & run

```bash
gcc -O2 -o hellsh hellsh.c
./hellsh
```

That's it. No dependencies. Pure C + POSIX.

---

## install as a real shell

hellsh is a fully POSIX-compliant login shell. To register it on your system:

```bash
sudo cp hellsh /usr/local/bin/hellsh
echo /usr/local/bin/hellsh | sudo tee -a /etc/shells
sudo chsh -s /usr/local/bin/hellsh $USER
```

Or just run `./hellsh --register-hint` to print those instructions.

From then on, whenever that user logs in — terminal, SSH, whatever — they get hellsh. No bash. No zsh. Just the judgment.

---

## what happens when you launch it

### 1. the warning screen

```
  HELLSH

  WARNING

  THIS TERMINAL IS RESTRICTED. ALL INPUT IS MONITORED AND LOGGED.
  UNAUTHORIZED ACCESS OR TAMPERING IS STRICTLY PROHIBITED.
  DISCONNECT IMMEDIATELY IF NOT AUTHORIZED.


  Sent to Hell
```

### 2. THE JUDGMENT

This plays automatically on launch. Every line types out character by character with dramatic pauses. It reads your sins from `.sins` in the working directory (defaults to the seven deadly sins if the file doesn't exist).

```
               THE JUDGMENT
        ---------------------------------

  The light dims. A figure stands before you.
  White robes. Wounded hands. Eyes like dying stars.
  He opens the Book.

  [Jesus]  I know thee.
  [Jesus]  The record of thy soul is written thus:

     1.  lust
     2.  greed
     3.  gluttony
     4.  sloth
     5.  wrath
     6.  envy
     7.  pride

  [Jesus]  I weigh thy deeds against mercy.
  [ ####### ]

  [Jesus]  The scales do not lie.

  [lust]    Thou hast burned for flesh that was not thine.
  [greed]   Thou hast hoarded what the poor needed to survive.
  [gluttony] Thou hast consumed without gratitude or restraint.
  [sloth]   The gifts I gave thee rotted in thy idle hands.
  [wrath]   Thou hast turned my name into a weapon.
  [envy]    Thou hast despised what I blessed in thy neighbour.
  [pride]   Thou hast placed thyself above the Author of all things.

  [Jesus]  I have wept for thee. I have called to thee.
  [Jesus]  Thou didst not answer.
  [Jesus]  Depart from me.
```

Then the gates:

```
          +---------------------------------+
          |                                 |
          |        GATES OF HELL            |
          |                                 |
          |    ABANDON ALL HOPE             |
          |    YE WHO ENTER                 |
          |                                 |
          |          < YOU >                |
          |                                 |
          +---------------------------------+

          >>>   THE GATE OPENS   <<<
```

*(flickers six times)*

```
          +----------------------------+
          |                            |
          |       SENT TO HELL         |
          |                            |
          +----------------------------+
```

Then it dumps you into the shell.

---

## the shell

The prompt is a red `> `. Ctrl+C, Ctrl+Z, Ctrl+\\ — all blocked. `exit` doesn't work. You're here forever.

### every command and what it says

| command | response |
|---|---|
| `whoami` | Lucifer |
| `pwd` | /hell/fireplace |
| `date` | judgement |
| `whereis` | in the middle of the fire, burning from thy sins |
| `ls` | only ashes remain |
| `exit` | no one can escape their sins. |
| `ps` | tormented souls: 666 |
| `kill` | fire cannot kill fire |
| `chmod` | permissions are irrelevant in hell |
| `sudo` | there is no authority here but the flames |
| `history` | a record of thy sins: burning |
| `top` | asmodeus, beelzebub, lucifer, satan |
| `diff` | all sins look the same in the fire |
| `tail` | the serpent's tail |
| `head` | the beast with seven heads |
| `df` | disk full of sinners |
| `du` | damned users: everyone |
| `mount` | mounted upon the flames |
| `umount` | cannot unmount eternal suffering |
| `dmesg` | demons messaging... |
| `journalctl` | the book of life is already burning |
| `ping` | praying to deaf heavens... timeout |
| `curl` | curling in fetal position from the heat |
| `wget` | why get? there is only give... give thy soul |
| `ssh` | screaming silently here |
| `netstat` | network of pain: established to hell |
| `satan` | you called? |
| `666` | the number of the beast |
| `fire` | you are surrounded by it |
| `helpme` | too late for that |
| `please` | pleading won't save you now |
| `heaven` | no signal from up there |
| `cross` | melts in thy hands |
| `bible` | ashes |
| `less` | less than dust, less than ash |
| `more` | more fire, more pain, more eternity |
| `sort` | the righteous from the damned? already sorted |
| `uniq` | all sinners are alike in the flames |
| `wc` | words: none. cries: countless. sins: 7 |
| `cut` | cut off from mercy |
| `paste` | pasted to the flames |
| `join` | join the choir of the damned |
| `comm` | commune with the fire |
| `patch` | no patches for a damned soul |
| `awk` | awaken, thou who sleeps in damnation |
| `sed` | the seducer awaits |
| `git` | the sins of the father are committed to the son |
| `make` | thou hast made thy bed in hell |
| `gcc` | gathering cinders constantly |
| `python` / `python3` | the serpent speaks: "forgive them not" |
| `perl` | pearls before swine, thy soul before the flames |
| `ruby` | ruby red like the eyes of the beast |
| `node` | node of the gallows |
| `npm` | nails piercing my palms |
| `pip` | pipes of hell play thy dirge |
| `java` | java? just another vessel aflame |
| `systemctl` | system cannot control the flames |
| `service` | services: none. suffering: active |
| `crontab` | cron tab? the appointed time was yesterday |
| `at` | at the gates of hell |
| `batch` | batch processing of souls |
| `shutdown` | shutting down? the fire never sleeps |
| `reboot` | reboot? resurrection is not for thee |
| `init` | in the beginning was the Word... thou didst not listen |
| `status` | ✝ |
| `0` | the number of fucks given |
| `1` | the one who sent thee here |
| `7` | the number of thy sins |
| `40` | days of flood, years of wandering, now eternity of fire |
| `clear` | the smoke clears but the fire remains |
| `death` | blanks the screen entirely, locks input forever |
| *(anything else)* | command not found in the depths of hell |

### prefix commands

These match the command name regardless of arguments, so `cat /etc/passwd`, `cat foo`, `cat` — all the same.

| command | response |
|---|---|
| `touch <anything>` | thy fingers burn |
| `cd <anywhere>` | every path leads to the flame |
| `rm <anything>` | the fire has already taken it |
| `cat <anything>` | the screams of the damned |
| `echo <anything>` | thy cries return unanswered |
| `grep <anything>` | searching through the flames... found: more fire |
| `find <anything>` | wandering through the inferno |

---

## beg

Type `beg` to enter THE PLEADING — a three-round interactive dialogue with Adam, the first man, who has been in hell longer than anyone and has heard every excuse.

```
               THE PLEADING
        ---------------------------------

  The fire quiets. A vast silence falls.
  A figure descends — not the beast, but a man.
  Adam. The first. His eyes carry the weight of all death.

  [Adam]  I know why thou art here.
  [Adam]  I stood where thou didst stand. I made the first choice.
  [Adam]  And every child of mine has paid for it ever since.
  [Adam]  Speak then. Beg if thou must. I have heard every plea.

  > [you type here]

  [Adam]  Thou speakest as they all do. Words dressed in desperation.
  [Adam]  I said the same when the angel barred Eden's gate.
  [Adam]  I wept. I begged. I named every reason.
  [Adam]  The gate did not open.

  [Adam]  Thou knowest why thou art here. Say it plainly.

  > [you type here]

  [Adam]  ...
  [Adam]  I carried that same confession for nine hundred years.
  [Adam]  Guilt is the one crop that never fails to grow in hell.
  [Adam]  But guilt is not repentance. Regret is not change.
  [Adam]  And here — in this place — change is no longer possible.

  [Adam]  Tell me one reason I should carry thy plea to Him.
  [Adam]  One. True. Reason.

  > [you type here]

  [Adam considers. The fire dims slightly.]

  [Adam]  ...
  [Adam]  There was one who came here once.
  [Adam]  Three days. He descended into this very dark.
  [Adam]  He walked through it. Every corner. Every scream.
  [Adam]  He could have taken me then.
  [Adam]  He did not take thee.

  [Adam]  That is not my decision to make.
  [Adam]  It never was.

  [Adam turns away. The fire returns.]
  [His footsteps fade into the dark.]
  [You are alone again.]
```

You get three chances to say something. Adam has seen it all. Then he leaves, and you're back at the prompt.

---

## customising your sins

Edit `.sins` in the working directory — one sin per line. These get read at launch, displayed during the judgment, and individually sentenced. If the file doesn't exist, hellsh creates it with the default seven.

```
lust
greed
gluttony
sloth
wrath
envy
pride
```

You can put whatever you want in there. Custom sins that don't have a pre-written verdict just don't get a verdict line. The judgment bar still counts them.

---

## how it works

- raw terminal mode from the start — no canonical input, no echo, no signals
- Ctrl+C, Ctrl+Z, Ctrl+\\, SIGHUP, SIGTERM — all swallowed
- `setsid()` on startup, so it's its own process group
- reads directly from `/dev/tty` so it works even when stdin is redirected
- `atexit()` handler restores the terminal on any exit path
- `beg` temporarily re-enables echo so you can actually type your plea, then re-locks when Adam leaves
- POSIX login shell compliant: handles the `-` prefix in argv[0]

---

## license

do whatever you want with it. you're already in hell.
