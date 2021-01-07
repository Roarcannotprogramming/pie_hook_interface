#include "pie_interface.h"

extern int optind, opterr, optopt;
extern char *optargi;

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"text", required_argument, NULL, 't'},
    {"stack", required_argument, NULL, 's'},
    {"heap", required_argument, NULL, 'p'},
    {"stackmagic", required_argument, NULL,'g'},
    {NULL, 0, NULL, 0}
};

static char short_options[] = "ht:s:p:g:";

static char help_msg[] = "Usage: %s [-h|--help] [-t|--text TEXT_SEG_BASE] [-s|--stack STACK_BASE] [-p|--heap HEAP_OFFSET] [-g|--stackmagic]\n"
                       "    -h, --help\tShow this help message\n"
                       "    -t, --text TEXT_SEG_BASE\tEnable text base PIE hook (TEXT_SEG_BASE means 0x555555554000 + OFFSET)\n"
                       "    -s, --stack STACK_BASE\tEnable stack base hook (STACK_BASE means the top of user stack area)\n"
                       "    -p, --heap HEAP_OFFSET\tEnable heap base hook (HEAP_OFFSET means OFFSET from the end of text area)\n"
                       "    -g, --stackmagic STACK_OFFSET\tEnable stack offset hook(the offset need to be fine-tuning manually)\n";

size_t string2Addr(char *s);
void errorMsg(const char *format, ...);
void infoMsg(const char *format, ...); 
int doTextHook(size_t addr);
int doStackBaseHook(size_t addr);
int doStackOffsetHook(size_t offset);
int doHeapHook(size_t offset);

int main(int argc, char *argv[]) {
    int opt;
    int long_index = -1;
    size_t addr = 0;
    while(EOF != (opt = getopt_long(argc, argv, short_options, long_options, &long_index))) {
        switch (opt) {
        case 'h':
            fprintf(stderr, help_msg, argv[0]);
            break;
        
        case 't':
            addr = string2Addr(optarg);
            doTextHook(addr);
           break;

        case 's':
            addr = string2Addr(optarg);
            doStackBaseHook(addr);
            break;
        
        case 'p':
            // addr -> heap offset
            addr = string2Addr(optarg);
            doHeapHook(addr);
            break;

        case 'g':
            // addr -> stack offset
            addr = string2Addr(optarg);
            doStackOffsetHook(addr);
            break;
        
        default:
            fprintf(stderr, "\n");
            fprintf(stderr, help_msg, argv[0]);
            break;
        }
    }
}

size_t string2Addr(char *s) {
    int len = strlen(s);
    if(len == 0) {
        return 0;
    }
    if ( len > 2 && (!strncmp(s, "0x", 2) || !strncmp(s, "0X", 2))) {
        return strtoull(s, NULL, 16);
    }
    if ( len > 2 && (!strncmp(s, "0b", 2) || !strncmp(s, "0B", 2))) {
        return strtoull(s + 2, NULL, 2);
    }
    return strtoull(s, NULL, 10);
}

void errorMsg(const char *format, ...) {
    va_list ap;
    char prefix[] = "[-] ERROR: ";
    int len = strlen(prefix) + strlen(format) + 1;
    char *temp_str = (char *)malloc(len + 1);
    strcpy(temp_str, prefix);
    strcat(temp_str, format);
    temp_str[len - 1] = '\n';

    va_start(ap, format);
    vfprintf(stderr, temp_str, ap);    

    free(temp_str);
    va_end(ap);
}

void infoMsg(const char *format, ...) {
    va_list ap;
    char prefix[] = "[-] INFO: ";
    int len = strlen(prefix) + strlen(format) + 1;
    char *temp_str = (char *)malloc(len + 1);
    strcpy(temp_str, prefix);
    strcat(temp_str, format);
    temp_str[len - 1] = '\n';

    va_start(ap, format);
    vfprintf(stderr, temp_str, ap);    

    free(temp_str);
    va_end(ap);
}

int doTextHook(size_t addr) {
    if(addr < TEXT_MIN) {
        errorMsg("Text base out of range");
        return -1;
    }   

    int fd = open("/dev/piehook", O_RDWR);
    struct piehook_param p = {0, 0, 0};

    if(fd < 0){
        errorMsg("Cannot open /dev/piehook, may use \"sudo\" or \"insmod\"");
        return -1;
    }

    if(ioctl(fd, PIEHOOK_ENABLE) < 0) {
        errorMsg("Cannot enable PIE hook");
        close(fd);
        return -1;
    }

    p.rnd_offset = addr - TEXT_MIN;
    if(ioctl(fd, PIEHOOK_CONFIG, &p) < 0) {
        errorMsg("Cannot config PIE hook");
        return -1;
    }

    if(p.result & 2) {
        errorMsg("Address is invalid, check your input");
        return -1;
    }
    if (p.result & 1) {
        infoMsg("Text base hooked, but address is not aligned, TEXT_BASE: %#lx", addr & (~0xfff));
        return 0;
    }
    infoMsg("Text base hooked successfully, TEXT_BASE: %#lx", addr & (~0xfff));
    return 0;
} 

int doStackBaseHook(size_t addr){
    if(addr > STACKTOP_MAX) {
        errorMsg("Stack base out of range");
        return -1;
    }   
    int fd = open("/dev/piehook", O_RDWR);
    struct piehook_param p = {0, 0, 0};

    if(fd < 0){
        errorMsg("Cannot open /dev/piehook, may use \"sudo\" or \"insmod\"");
        return -1;
    }

    if(ioctl(fd, STACKHOOK_ENABLE) < 0) {
        errorMsg("Cannot enable Stack hook");
        close(fd);
        return -1;
    }

    p.rnd_base = addr;
    if(ioctl(fd, STACKHOOK_CONFIG_BASE, &p) < 0) {
        errorMsg("Cannot config Stack hook");
        return -1;
    }
    if(p.result & 2) {
        errorMsg("Address is invalid, check your input");
        return -1;
    }
    if (p.result & 1) {
        infoMsg("Stack base hooked, but address is not aligned, STACK_BASE: %#lx", addr & (~0xfff));
        return 0;
    }
    infoMsg("Stack base hooked successfully, STACK_BASE: %#lx", addr & (~0xfff));
    return 0;
}

int doStackOffsetHook(size_t offset){
    int fd = open("/dev/piehook", O_RDWR);
    struct piehook_param p = {0, 0, 0};

    if(fd < 0){
        errorMsg("Cannot open /dev/piehook, may use \"sudo\" or \"insmod\"");
        return -1;
    }

    if(ioctl(fd, STACKHOOK_ENABLE) < 0) {
        errorMsg("Cannot enable Stack Offset hook");
        close(fd);
        return -1;
    }

    p.rnd_offset = offset;
    if(ioctl(fd, STACKHOOK_CONFIG_OFFSET, &p) < 0) {
        errorMsg("Cannot config Stack Offset hook");
        return -1;
    }
    if(p.result & 2) {
        errorMsg("Address is invalid, check your input");
        return -1;
    }
    infoMsg("Stack offset hooked successfully, STACK_OFFSET: %#lx", offset);
    return 0;
}

int doHeapHook(size_t offset){
    if(offset > HEAPOFF_MAX) {
        errorMsg("Heap offset out of range");
        return -1;
    }   

    int fd = open("/dev/piehook", O_RDWR);
    struct piehook_param p = {0, 0, 0};

    if(fd < 0){
        errorMsg("Cannot open /dev/piehook, may use \"sudo\" or \"insmod\"");
        return -1;
    }

    if(ioctl(fd, HEAPHOOK_ENABLE) < 0) {
        errorMsg("Cannot enable Heap hook");
        close(fd);
        return -1;
    }

    p.rnd_offset = offset;
    if(ioctl(fd, HEAPHOOK_CONFIG, &p) < 0) {
        errorMsg("Cannot config Heap hook");
        return -1;
    }
    if(p.result & 2) {
        errorMsg("Address is invalid, check your input");
        return -1;
    }
    if (p.result & 1) {
        infoMsg("Heap base hooked, but address is not aligned, HEAP_BASE: %#lx", offset & (~0xfff));
        return 0;
    }
    infoMsg("Heap base hooked successfully, HEAP_BASE: %#lx", offset & (~0xfff));
    return 0;
}