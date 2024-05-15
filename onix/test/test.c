int main() {
    // 使用系统调用 write 输出消息到标准输出
    const char msg[] = "Hello, world!\n";
    asm(
        "movl $4, %%eax\n"   // syscall number for sys_write
        "movl $1, %%ebx\n"   // file descriptor for stdout
        "movl %0, %%ecx\n"   // address of the message
        "movl $14, %%edx\n"  // message length
        "int $0x80\n"        // invoke syscall
        :
        : "r" (msg)
        : "%eax", "%ebx", "%ecx", "%edx"
    );

    return 0;
}
