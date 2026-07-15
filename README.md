# Iris - x86_64 Operating System Kernel

The Iris project was born just like many other ambitious undertakings: by looking at a huge, decades-old system developed and maintained by thousands of professionals (the Linux kernel) and going:

> "Eh, I could do that in a few weeks. How hard could it possibly be?"

Oh, Past Connor...

Very, very difficult.

Future Connor is now learning that the hard way.

## Why is it called Iris?

Is there a special reason you named the project that?

Is it named after someone you love? Does it have a connection to the project?

Nope.

It literally just sounded cool, and all the other good names were taken.

(*And* I like the blue-purple color that irises sometimes are. Bonus points!)

## Wait, wasn't it named something else before?

Actually, yes! Wait, how'd you know that...

When I was beginning this project, there were two huge differences: the kernel was `i386` (still `x86`, but 32-bit), and the entire operating system was being built at the same time as the kernel. I was planning on this being an easy peasy, month-long-ish project, so I named it TLOS (pronounced "tee-loss"), which stands for "Tiny Little Operating System".

Then, I decided that I needed a separate name for the kernel! So, given the central nature of kernels, I chose the name "Core Kernel", which was then shortened to "CorK".

Since I *love* uniform naming schemes, I then renamed the entire project to Vial.

Why, you ask?

Because vials are experimental equipment, sure...

But also...

...you put CorKs in Vials.

Just give Past Connor a courtesy laugh and keep reading.

I know, it's awful.

After the project gained steam, I decided that CorK and Vial were... relatively childish-sounding names for a project that will probably last multiple years. So, after going through tons of ideas (and learning that *all* of the good ones were already taken), I settled on Iris!

I love the color that some irises are (which will become the accent color for the future UI), and it's just a really nice name in general anyway. Short, punchy, easy to remember, and most importantly: *not* taken!

The future operating system built around Iris will be named Lavender. I told you already: I *love* uniform naming schemes! Same color palette, same vibe, while also sounding more flowing.

It all just felt perfect.

## Dude, hurry up and tell me what it can do

Okay, okay, fine.

Iris is an `x86_64` OS kernel written mostly in C, with an assembly stub for entry to set up things like the stack.

Iris boots via its custom `UEFI` bootloader (`CUB v2.1`), which passes a `BootInfo` struct to the kernel. After CUB exits `UEFI` boot services, Iris takes control of the machine and no longer relies on `UEFI` or CUB.

Iris currently uses the `QEMU` debug port at `0xE9` to print to a terminal as its main source of communication with the user. In the future, a framebuffer text printer will be developed and replace the debug port.

On the developer's side, Iris is built using a `Makefile` that compiles using `GCC` and `LD`, producing a kernel `ELF` file and CUB `EFI` executable. You can use the Dockerfile in the repository to create a container and run `make` inside of it (instructions are at the bottom of this README).

As always, this project evolves quickly, so it's best to check the source files yourself instead of relying on my poorly-made descriptions here!

## Just let me build Iris already!

In Docker, run these commands:

* To build the Docker image, move into the root of this repository and run:

    * On x84_64 systems (Linux)

        ```
        docker build -t iris-i .
        ```

    * If you are running Docker on an ARM machine (including a Mac):

        ```
        docker build --platform linux/amd64 -t iris-i .
        ```

* Then, to spin up a temporary container that will delete itself upon exit:

    ```
    docker run --rm -it --name iris-c --user $(id -u):$(id -g) -v "$(pwd)":/home/iris iris-i
    ```

(Remove `--rm` to create a persistent container instead)

* Then, inside the container, run:

    ```
    make clean && make
    ```

* Now, choose your own adventure!

    * If you have QEMU installed on the host computer (`qemu-system-x86`), on the host run: 

        ```
        make run
        ```

    * If not, no worries! Inside of the container, you can use:

        ```
        make run-c
        ```
