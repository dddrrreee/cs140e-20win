### Pre-lab for 0-blink

Read through the below and do any action item before wednesday's lab.
Since this is the first lab and people are missing context, we
over-explain --- the other labs should have less prose.

For this lab, we'll use different variations of a blinky light to check
that parts of your toolchain / understanding are working.

You'll likely forget this link, but if at any point you get confused
about some basic concept, cs107e has a good set of guides on electricity,
shell commands, pi stuff in general:
   - [CS107E repo] (http://cs107e.github.io/guides/)

### Deliverables before the first lab (wed):

Do these on your own (described more in the rest of the document):
   1. Check-out the class git repo.
   2. Figure out how to add a local `~/bin` directory in your home directory to your shell's `PATH` variable.
   3. Have a way to mount a micro-SD card on your laptop.  You may need an adaptor.
      We will have some at the lab, but it's better if you have your own.
   4. Make sure you know how to use common unix commands such as:
      `ls`, `mkdir`, `cd`, `pwd`.
   5. Read through the first lab (`0-blink/README.md`) and look through the code.
   6. Ideally: Install the rest of the software needed by Part 3 and Part 4 of the lab.

### *Turn-in*

The wednesday lab is broken down into five parts:
  1. You'll turn on an LED manually;
  2. Then copy a pre-compiled program to an micro-SD and boot up your pi;
  3. Then use a bootloader to ship the program to the pi directly;
  4. Then install the r/pi tool chain, compile a given assembly
       version and use it;
  5. Then write your own `blink` program, compile and run it;

For turn in answer the following questions:
  1. What is a bug that is easier to find b/c we broke up steps 1 and 2?
  2. What is a bug that is easier to find b/c we broke up steps 2 and 3?
  3. What is a bug that is easier to find b/c we broke up steps 3 and 4?
  4. What is a bug that is easier to find b/c we broke up steps 4 and 5?

Differential debugging: Your pi setup in step 2 is not working. 
Your partner's does.  You plug your setup into their laptop.

  1. Yours works in their laptop: what do you know?
  2. Yours does not work in thier laptop: what do you know?
  
### git and checking out the class repo

   1. Install `git` on your laptop if you don't have it: we use `git`
      to push all assignments and code to you.  

      MacOS seems to ship with `git`, but you might need to install the
      xcode command line tools with:

          % xcode-select --install

      Ubuntu:

          % sudo apt install git

   2. Use `git` to clone (check out) the class repository:

          % git clone git@github.com:dddrrreee/cs140e-win20.git  

      If you don't want to install a key on github, use:

          % git clone https://github.com/dddrrreee/cs140e-win20.git

      Pro tip: run your `git clone` command in some sensible directory
      (e.g., `~/class/`) and remember where you checked your repository
      out to!  Also, know how to find it.  We had a surprising (to me)
      number of people lose work last year because they did not do this.

      Pro tip: ***Do not download the class repository directly as a
      .zip file***!  Direct download will be both tedious and overwrite
      any local changes you do later.

#### micro-SD card stuff

In order to run code on the pi, you will need to be able to write to a
micro-SD card on your laptop:

   1.  Get/bring a micro-SD card reader or adaptor if its not built-in
       to your laptop.  The [CS107E guide on SD
       cards](http://cs107e.github.io/guides) is a great description
       (with pictures!).

   2. Figure out where your SD card is mounted (usually on MacOS it is in
      `/Volumes` and on linux in `/media/yourusername/`).   Figure out
      how to copy files to the SD card from the command line using
      `cp`, which is much much faster and better than using a gui (e.g.,
      you can put it in a
     `Makefile`, or use your shell to redo a command).  For me,

           % cp kernel.img /media/engler/0330-444/
           % sync
 
      will copy the file in `kernel.img` in the current directory to the
      top level directory of the mounted SD card, the `sync` command forces
      the OS to flush out all dirty blocks to all stable media (SD card,
      flash drive, etc).  At this point you can pull the card out.

      Pro tip: ***do not omit either a `sync` or some file manager eject
      action*** if you do, parts of the copied file(s) may in fact not
      be on the SD card.  (Why: writes to stable storage are slow, so
      the OS tends to defer them.)


### change your Path

To save you time, typing and mistakes: tell your shell to look for
executable programs in a `bin` directory located in your home directory.

  1. For whatever shell (`tcsh`, `bash`, etc) you are using, figure
     out how to edit your `PATH` variable so that you can install binary
     programs in a local `~/bin` directory and not have to constantly
     type out the path to them.  E.g.,

           % cd ~
           % mkdir bin
           % cd bin
           % pwd     
           /home/engler/bin
           
     To figure out your shell, you can typically check in `/etc/password`:

           % grep engler /etc/passwd
           engler:x:1000:1000:engler,,,:/home/engler:/usr/bin/tcsh

     Since I use `tcsh`, to add `/home/engler/bin` to my `path` I would
     edit my `~/.tcshrc` file and change:

          set path = ( $path  )

     To:

          set path = ( $path /home/engler/usr/bin )

     and then tell the shell to re-scan the paths as follows:

           % source ~/.tcshrc
     
     Note, you have to run `source` in every open window:  it may be
     easier to just logout and log back in.

     For MacOS users using `bash`, put the path to your `~/bin` directory
     in a `.bashrc` file in your home directory. Just run:

           % vim ~/.bashrc

     When you add your path, make sure you include you default path or
     you may mess up your terminal.  This looks like:

           export PATH="$PATH:your/path/here:your/other/path/here"

      and source it the same way as the `.tcshrc`.

#### Install the rest of the code

Gates wireless can really suck, so it's best if you download the rest
of the code that you need: look in Part 3 (serial drivers) and Part 4
(gcc toolchain) of the lab, and install the software it says.

#### Nice-haves: Extra, very useful things.

You don't need these for the first lab, but they will make
your Stanford journey much easier:

   - Figure out how to "tab complete" in your shell.  This gives you
     a fast(er) way to list out files and run things.  We'll show this,
     but you can google.

   - Figure out how to use tags.  Both `emacs` or `vim` support "tags",
     which use keyboard shortcuts to essentially treat identifiers as urls
     and navigate to the definitions of structures, functions, variables.
     This makes understanding code ***much much*** faster and easier.   [A description for `vim`](https://vim.fandom.com/wiki/Browsing_programs_with_tags).

   - Longer term: eliminate as much mouse usage as you can; ***especially
     eliminate screen switching***.  Ideally you'll have your code in one
     window, the compilation in another window on the same screen and
     switch back and forth between these windows without lifting your
     hands off the keyboard.  You should definitely not be switching
     screens for such tasks.

     The single biggest productivity slowdown I see in stanford undergrads
     (grads too) is the slow, laborious use of taking their hand off
     a keyboard, moving a mouse, clicking, switchin screens, clicking
     on something else, switching back, etc.   You don't realize how
     much this will cripple you until you watch someone who writes code
     well and compare their fluent flow to your plodding agrarian
     lifestyle.  Such switching screens means you lose context
     (which makes fixing even trivial compilation errors painful),
     you move slowly, and in many different ways you will distrupt your
     programming flow state, which is basically the only real way you
     can get significant, effective work done.  Any time you move the
     mouse, you're not doing work.  Do what you can to stop this.

   - Install `gdb` if you can --- it makes things easier in other classes
     (e.g., you can do a backtrace after a crash to figure out where the
     code was) and for this class let you answer simple code questions
     quickly.  For example, what is the decimal value of `01010101011`?

          % gdb
          (gdb) p 0b01010101011
          $1 = 683

     Easy.
