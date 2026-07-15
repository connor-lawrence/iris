FROM ubuntu:24.04
RUN apt update && apt install -y git gcc make binutils gnu-efi qemu-system-x86 ovmf
WORKDIR /home/iris
CMD ["bash"]
