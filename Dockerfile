FROM ubuntu:24.04
RUN apt update && apt install -y build-essential gnu-efi qemu-system-x86 ovmf
WORKDIR /home/vial
CMD ["bash"]