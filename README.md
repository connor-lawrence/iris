# Iris - My 64-bit Kernel
This repository contains the code and files for my hobby kernel, `Iris`! It is a `x86_64` kernel that boots from my Custom UEFI Bootloader named `CUB`!

Expect a mess in the repo—because that's what you're going to get! What did you expect? I'm a teenager...

## Wasn't it named something else?
When I was starting this project, the OS's name wasn't `Iris` at all! It was actually `TLOS` (pronounced "tee-loss"), which stands for Tiny Little Operating System.

Later, I decided I needed a separate name for the kernel! So I picked `Core Kernel`, which shortens to `CorK` (like the kind in bottles)!

Since I love uniform naming schemes, I then renamed the entire project to `vial`. Why, you ask? Because it's experimental, sure... but also because... there's a cork in vials... get it...?

Just laugh and let me feel funny.

After the project had gained steam, I began to become uncomfortable with the name `CorK`, so after lots of brainstorming (and discovering that *all* of the good names were taken) I chose the name 'Iris'!

## Details
* 64-bit x86 kernel (`x86_64`)
* Compiles to ELF
* Boots through my Custom UEFI Bootloader, named `CUB`
* `CUB` passes boot info to `Iris`
* Right now, `Iris` can't do much... but the boot process is in place and functional through `CUB`!

## Useful Commands
* Build the Docker Image: `docker build -t iris-i .`
* Run the Docker Container: `docker run -it --name iris-c -v "$(pwd)":/home/iris iris-i`
    * Add `-rm` after `docker run` for a temporary container
* In the container: `make clean && make`
* Now, choose your own adventure:
    * If you have QEMU installed on the host computer (`qemu-system-x86`), on the host run: `make run`
    * If not, no worries! You wont get much though, but you can use: `make run-c`
