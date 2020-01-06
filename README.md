## CS140E: embedded operating systems (Engler, Winter 2020)

This is a lab-based class with no explicit lectures.  We will do two
two-to-three hour labs each week.  You should be able to complete almost
all of the lab in one sitting.  Assuming it's useful I will do some kind
of optional discussion section on Fridays.

By the end of the class you will have built your own simple, clean OS
for the widely-used, ARM-based raspberry pi --- including interrupts,
threads, virtual memory, and a simple file system.  Your OS should serve
as a good base for interesting, real, sensor-based / embedded projects.

We try to work directly with primary-sources (the Broadcom and ARM6
manuals, various datasheets) since learning to understand such prose
is one of the main super-powers of good systems hackers.  It will also
give you the tools to go off on your own after the course and fearlessly
build sensor-based devices using only their datasheets.

This course differs from most OS courses in that it uses real hardware
instead of a fake simulator, and almost all of the code will be written
by you.

After this quarter, you'll know/enact many cool things your peers do not.
You will also have a too-rare concrete understanding of how computation
works on real hardware.   This understanding will serve you in many
other contexts.

### Who should take this class.

You should take this class if:
   1. You write code well OR (you don't yet write code well AND have a lot of time
   to devote to the class);
   2. AND you find these systems topics interesting.

The people that found the class valuable in the past were entirely drawn
from this demographic.  (In particular: I would not take this course
if you were looking for an easier way to satisfy a cs140 requirement.)
We are trying to go as far as possible as quickly as possible with a
group interested in doing so.  

I would say that if you struggle building stuff and are not already
interested in the topic, then it's probably better to take a different
class.

With that said, I set aside the entire quarter to work on this course,
so am happy to help people that need it --- some of the biggest success
stories last year were from students that had an initially large gap
between their understanding and the topic but started to really "get it"
after some weeks of struggle.

### CS140e vs CS140

CS140e can count as a substitute for CS140 (including for PhD students
that need to fulfill a OS breadth requirement).  It is perhaps 80% of
the coding work of CS140, but likely requires a more strenuous type of
thinking at some points.  The main differences:

 1. We work with raw hardware (the r/pi A+) versus a simulator.  At the
       end you should have a simple, but actually useful embedded system
       you can do interesting tricks with.

 2. Most of the reading involves primary hardware documents, rather
       than high level discussions.   The ability to read and act on
       such documents is a super-power that will serve you well the rest of
       your career:  once you get good at it, almost anything is easy.

 3. Most of the code at the end (I hope) will be written by you, rather
       than you having to deal with a large, complicated code base.
       The downside is that we give up functionality that CS140
       implements, the hoped-for upside is that you will have a much
       more thorough understanding of what we do cover.

 4. We focus much more on checking, somewhat-verifying your code rather
       than tossing it over the wall to TA's to grade at the end of
       an assignment.  Most of these techniques are simple (since we
       only have 10 weeks) but effective and, unfortunately, appear not
       widely-known/used by most programmers.

With that said, you are strongly encouraged to sit in the cs140
lectures: Mazieres is incredibly knowledgeable, and will serve as a
great counter-point to what we do.

### Lab policies.

   - You should be able to complete almost all of the lab in one sitting.
     If not, you *must* complete the lab within a week.  We will not
     accept a lab after this.

   - Before each lab there will be some preparatory reading, which will
     typically include a short set of questions you need to answer and
     turn in *before* the lab begins.  This is an attempt to make the
     labs more information-dense by handling basic stuff outside of it,
     and focusing mostly on any tricky details in it.

   - Labs are not optional!   If you are going to miss a lab you need
     to make arrangements with us before hand, and missing more than
     one or two is going to affect your grade even if you turn in the
     work on time.

     We had a more flexible policy in the past.  Unfortunately, since
     labs typically build on each other, once people fell behind they
     got seriously lost.  Further, this year we short-staffed for the
     amount of students taking the course, so don't have the resources
     to walk a few students each week through makeup labs.

### What to do now

First things first: 

   1. clone the class repository:
            git clone git@github.com:dddrrreee/cs140e-20win.git

      You may need to install `git`, create a `github` account and
      register a key (if you don't want to do the latter, use the `http`
      clone method).

   2. Make sure you have access to the [cs14e
       newsgroup](https://groups.google.com/forum/#!forum/cs140e-win20).
       Email us if not!

   3. Look in the `labs` directory and read through the labs in `0-blink`
      and `1-gpio` which are our startup labs to make sure everyone is
      on the same page.
