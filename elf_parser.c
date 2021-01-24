#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include "elf.h"
// #include <malloc.h>


void help();
void get_file_header(const char *pbuff); // get ELF file header [-h]
void get_section_headers(const char *pbuff); // get section headers [-S]
void get_program_headers(const char *pbuff); // get program headers [-l]
void get_symbol_table(const char *pbuff); // get symbol table entries [-s]
void outputsyminfo(const Elf64_Sym *psym, const char *pbuffstr, int ncount); //
void get_relocation_entries(const char *pbuff); // get relocation entries [-r]


int main(int argc, char * argv[]) {
    /*
    Usage: ./elf_parser <option(s)> elf-file(s)
    */
    if (argc != 3) {
        printf("[elf_parser] [Warning]: Nothing to do.\n");
        printf("[Note the usage]: ./elf_parser <option> <ELF file path>\n");
        help();
        exit(0);
    }

    /*
    open the file
    */
    int fd = open(argv[2], O_RDONLY, 0);
    if (fd < 0) {
        perror("Open");
        exit(-1);
    }

    /*
    malloc the size of the file
    */
    long int end = lseek(fd, 0, SEEK_END); // R|W in the end of the file
    long int begin = lseek(fd, 0, SEEK_SET); // R|W in the begin of the file
    char *pbuff = malloc(end);

    if (!pbuff) {
        perror("Malloc");
        exit(-1);
    }

    /*
    set the initial memery to zero
    */
    memset(pbuff, 0, end);
    if (-1 == read(fd, pbuff, end)) {
        perror("Read file");
        exit(-1);
    }

    /*
    map the <option> into functions
    */
    if (!strcmp(argv[1], "-h")) {
        get_file_header(pbuff);
    }
    else if (!strcmp(argv[1], "-S")) {
        get_section_headers(pbuff);
    }
    else if (!strcmp(argv[1], "-l")) {
        get_program_headers(pbuff);
    }
    else if (!strcmp(argv[1], "-s")) {
        get_symbol_table(pbuff);
    }
    else if (!strcmp(argv[1], "-r")) {
        get_relocation_entries(pbuff);
    }
    return 0;
}

void help() {
    printf("Display information about the contents of ELF format files\n");
    printf("Options are:\n");
    printf("-h            : Display the ELF file header\n");
    printf("-S            : Display the sections' headers\n");
    printf("-l            : Display the program headers\n");
    printf("-s            : Display the symbol table\n");
    printf("-r            : Display the relocations (if present)\n");
}

/*
funtion of get the ELF file header
*/
void get_file_header(const char *pbuff) {
    printf("ELF Header:\n");
    // unsigned char e_ident[EI_NIDENT]

    // Magic number pbuff[0]-[3]
    printf("%-33s:", "Magic");
    putchar(' ');
    for (int i = 0; i < EI_NIDENT; ++i) {
        printf("%02X", pbuff[i]);
        putchar(' ');
    }
    printf("\n");

    // Class pbuff[4]
    printf("%-33s:", "Class");
    switch (pbuff[4]) {
        case 0:
            printf(" None (Invalid)\n");
            break;
        case 1:
            printf(" ELF32\n");
            break;
        case 2:
            printf(" ELF64\n");
            break;
        default:
            printf(" ERROR\n");
            break;
    }

    // Data pbuff[5]
    printf("%-33s:", "Data");
    switch (pbuff[5]) {
        case 0:
            printf(" Invalid data encoding\n");
            break;
        case 1:
            printf(" 2's complement, little endian (LSB)\n");
            break;
        case 2:
            printf(" 2's complement, big endian (MSB)\n");
            break;
        default:
            printf(" ERROR\n");
            break;
    }

    // Version pbuff[6] (the object file version)
    printf("%-33s: %s\n", "Version", "always 1(current)");

    // OS/ABI pbuff[7]
    // ref: https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html
    printf("%-33s:", "OS/ABI");
    switch (pbuff[7]) {
        case 0:
            printf(" UNIX System V\n");
            break;
        case 1:
            printf(" Hewlett-Packard HP-UX\n");
            break;
        case 2:
            printf(" NetBSD\n");
            break;
        case 3:
            printf(" Linux\n");
            break;
        case 6:
            printf(" Sun Solaris\n");
            break;
        case 7:
            printf(" AIX\n");
            break;
        case 8:
            printf(" IRIX\n");
            break;
        case 9:
            printf(" FreeBSD\n");
            break;
        case 10:
            printf(" Compaq TRU64 UNIX\n");
            break;
        case 11:
            printf(" Novell Modesto\n");
            break;
        case 12:
            printf(" Open BSD\n");
            break;
        case 13:
            printf(" Open VMS\n");
            break;
        case 14:
            printf(" Hewlett-Packard Non-Stop Kernel\n");
            break;
        default:
            printf(" Architecture-specific\n");
            break;

    }

    // ABI Version pbuff[8] (the version of the ABI to which the object is targeted)
    printf("%-33s: %s\n", "ABI Version", "0"); // Almost never used in the wild
    pbuff += EI_NIDENT; // the pointer go to 'Halfword e_type'

    // Type: Halfword e_type
    printf("%-33s:", "Type");
    switch (*(uint16_t*) pbuff) { // 'type Halfword' is 'uint16_t' now
        case 0:
            printf(" No file type\n"); // ET_NONE
            break;
        case 1:
            printf(" Relocatable file\n"); // ET_REL
            break;
        case 2:
            printf(" Executable file\n"); // ET_EXEC
            break;
        case 3:
            printf(" Shared object file\n"); // ET_DYN
            break;
        case 4:
            printf(" Core file\n"); // ET_CORE
            break;
        default:
            printf(" ERROR\n"); // ET_LOOS, ET_HIOS, ET_LOPROC, ET_HIPROC
            break;
    }
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_machine'

    // Machine: Halfword e_machine
    printf("%-33s:", "Machine");
    switch (*(uint16_t*) pbuff) {
        case EM_M32:
            printf(" AT&T WE 32100\n");
            break;
        case EM_SPARC:
            printf(" SPARC\n");
            break;
        case EM_386:
            printf(" Intel 80386\n");
            break;
        case EM_68K:
            printf(" Motorola 68000\n");
            break;
        case EM_88K:
            printf(" Motorola 88000\n");
            break;
        case EM_860:
            printf(" Intel 80860\n");
            break;
        case EM_MIPS:
            printf(" MIPS I Architecture\n");
            break;
        case EM_S370:
            printf(" IBM System/370 Processor\n");
            break;
        case EM_MIPS_RS3_LE:
            printf(" MIPS RS3000 Little-endian\n");
            break;
        case EM_PARISC:
            printf(" Hewlett-Packard PA-RISC\n");
            break;
        case EM_VPP500:
            printf(" Fujitsu VPP500\n");
            break;
        case EM_SPARC32PLUS:
            printf(" Enhanced instruction set SPARC\n");
            break;
        case EM_960:
            printf(" intel 80960\n");
            break;
        case EM_PPC:
            printf(" PowerPC\n");
            break;
        case EM_PPC64:
            printf(" 64-bit PowerPC\n");
            break;
        case EM_S390:
            printf(" IBM System/390 Processor\n");
            break;
        case EM_V800:
            printf(" NEC V800\n");
            break;
        case EM_FR20:
            printf(" Fujitsu FR20\n");
            break;
        case EM_RH32:
            printf(" TRW RH-32\n");
            break;
        case EM_RCE:
            printf(" Motorola RCE\n");
            break;
        case EM_ARM:
            printf(" Advanced RISC Machines ARM\n");
            break;
        case EM_ALPHA:
            printf(" Digital Alpha\n");
            break;
        case EM_SH:
            printf(" Hitachi SH\n");
            break;
        case EM_SPARCV9:
            printf(" SPARC Version 9\n");
            break;
        case EM_TRICORE:
            printf(" Siemens TriCore embedded processor\n");
            break;
        case EM_ARC:
            printf(" Argonaut RISC Core, Argonaut Technologies Inc.\n");
            break;
        case EM_H8_300:
            printf(" Hitachi H8/300\n");
            break;
        case EM_H8_300H:
            printf(" Hitachi H8/300H\n");
            break;
        case EM_H8S:
            printf(" Hitachi H8S\n");
            break;
        case EM_H8_500:
            printf(" Hitachi H8/500\n");
            break;
        case EM_IA_64:
            printf(" Intel IA-64 processor architecture\n");
            break;
        case EM_MIPS_X:
            printf(" Stanford MIPS-X\n");
            break;
        case EM_COLDFIRE:
            printf(" Motorola ColdFire\n");
            break;
        case EM_68HC12:
            printf(" Motorola M68HC12\n");
            break;
        case EM_MMA:
            printf(" Fujitsu MMA Multimedia Accelerator\n");
            break;
        case EM_PCP:
            printf(" Siemens PCP\n");
            break;
        case EM_NCPU:
            printf(" Sony nCPU embedded RISC processor\n");
            break;
        case EM_NDR1:
            printf(" Denso NDR1 microprocessor\n");
            break;
        case EM_STARCORE:
            printf(" Motorola Star*Core processor\n");
            break;
        case EM_ME16:
            printf(" Toyota ME16 processor\n");
            break;
        case EM_ST100:
            printf(" STMicroelectronics ST100 processor\n");
            break;
        case EM_TINYJ:
            printf(" Advanced Logic Corp. TinyJ embedded processor family\n");
            break;
        case EM_X86_64:
            printf(" AMD x86-64 architecture\n"); // value == 62
            break;
        case EM_PDSP:
            printf(" Sony DSP Processor\n");
            break;
        case EM_PDP10:
            printf(" Digital Equipment Corp. PDP-10\n");
            break;
        case EM_PDP11:
            printf(" Digital Equipment Corp. PDP-11\n");
            break;
        case EM_FX66:
            printf(" Siemens FX66 microcontroller\n");
            break;
        case EM_ST9PLUS:
            printf(" STMicroelectronics ST9+ 8/16 bit microcontroller\n");
            break;
        case EM_ST7:
            printf(" STMicroelectronics ST7 8-bit microcontroller\n");
            break;
        case EM_68HC16:
            printf(" Motorola MC68HC16 Microcontroller\n");
            break;
        case EM_68HC11:
            printf(" Motorola MC68HC11 Microcontroller\n");
            break;
        case EM_68HC08:
            printf(" Motorola MC68HC08 Microcontroller\n");
            break;
        case EM_68HC05:
            printf(" Motorola MC68HC05 Microcontroller\n");
            break;
        case EM_SVX:
            printf(" Silicon Graphics SVx\n");
            break;
        case EM_ST19:
            printf(" STMicroelectronics ST19 8-bit microcontroller\n");
            break;
        case EM_VAX:
            printf(" Digital VAX\n");
            break;
        case EM_CRIS:
            printf(" Axis Communications 32-bit embedded processor\n");
            break;
        case EM_JAVELIN:
            printf(" Infineon Technologies 32-bit embedded processor\n");
            break;
        case EM_FIREPATH:
            printf(" Element 14 64-bit DSP Processor\n");
            break;
        case EM_ZSP:
            printf(" LSI Logic 16-bit DSP Processor\n");
            break;
        case EM_MMIX:
            printf(" Donald Knuth's educational 64-bit processor\n");
            break;
        case EM_HUANY:
            printf(" Harvard University machine-independent object files\n");
            break;
        case EM_PRISM:
            printf(" SiTera Prism\n");
            break;
        case EM_AVR:
            printf(" Atmel AVR 8-bit microcontroller\n");
            break;
        case EM_FR30:
            printf(" Fujitsu FR30\n");
            break;
        case EM_D10V:
            printf(" Mitsubishi D10V\n");
            break;
        case EM_D30V    :
            printf(" Mitsubishi D30V\n");
            break;
        case EM_V850:
            printf(" NEC v850\n");
            break;
        case EM_M32R    :
            printf(" Mitsubishi M32R\n");
            break;
        case EM_MN10300:
            printf(" Matsushita MN10300\n");
            break;
        case EM_MN10200:
            printf(" Matsushita MN10200\n");
            break;
        case EM_PJ:
            printf(" picoJava\n");
            break;
        case EM_OPENRISC:
            printf(" OpenRISC 32-bit embedded processor\n");
            break;
        case EM_ARC_A5:
            printf(" ARC Cores Tangent-A5\n");
            break;
        case EM_XTENSA:
            printf(" Tensilica Xtensa Architecture\n");
            break;
        case EM_VIDEOCORE:
            printf(" Alphamosaic VideoCore processor\n");
            break;
        case EM_TMM_GPP:
            printf(" Thompson Multimedia General Purpose Processor\n");
            break;
        case EM_NS32K:
            printf(" National Semiconductor 32000 series\n");
            break;
        case EM_TPC :
            printf(" Tenor Network TPC processor\n");
            break;
        case EM_SNP1K:
            printf(" Trebia SNP 1000 processor\n");
            break;
        case EM_ST200:
            printf(" STMicroelectronics (www.st.com) ST200 microcontroller\n");
            break;
        default:
            printf(" Other architecture [Reserved for future use]\n");
            break;
    }
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_version'

    // Version: Word e_version
    printf("%-33s: %s\n", "version", "0X1"); // 'type Word' is 'uint32_t' now (always 1)
    pbuff += sizeof(uint32_t); // the pointer go to 'Address e_entry'

    // Entry point address: Address e_entry
    printf("%-33s: 0X%llx\n", "Entry point address", *(uint64_t*) pbuff);
    pbuff += sizeof(uint64_t); // the pointer go to 'Offset e_phoff'

    // Start of program headers: Offset e_phoff
    printf("%-33s: %llu (bytes into file)\n", "Start of program headers", *(uint64_t*) pbuff);
    pbuff += sizeof(uint64_t); // the pointer go to 'Offset e_shoff'

    // Start of section headers: Offset e_shoff
    printf("%-33s: %llu (bytes into file)\n", "Start of section headers", *(uint64_t*) pbuff);
    pbuff += sizeof(uint64_t); // the pointer go to 'Word e_flags'

    // Flags: Word e_flags (This member holds processor-specific flags associated with the file. Flag names take the form EF_machine_flag.)
    printf("%-33s: 0X0\n", "Flags");
    pbuff += sizeof(uint32_t); // the pointer go to 'Halfword e_ehsize'

    // Size of this header: Halfword e_ehsize
    printf("%-33s: %d (bytes)\n", "Size of this header", *(uint16_t*) pbuff);
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_phentsize'

    // Size of program headers: Halfword e_phentsize
    printf("%-33s: %d (bytes)\n", "Size of program headers", *(uint16_t*) pbuff);
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_phnum'

    // Number of program headers: Halfword e_phnum
    printf("%-33s: %d\n", "Number of program headers", *(uint16_t*) pbuff);
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_shentsize'

    // Size of section headers: Halfword e_shentsize
    printf("%-33s: %d (bytes)\n", "Size of section headers", *(uint16_t*) pbuff);
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_shnum'

    // Number of section headers: Halfword e_shnum
    printf("%-33s: %d\n", "Number of section headers", *(uint16_t*) pbuff);
    pbuff += sizeof(uint16_t); // the pointer go to 'Halfword e_shstrndx'

    // Section header string table index : Halfword e_shstrndx
    printf("%-33s: %d\n", "Section header string table index", *(uint16_t*) pbuff);
}
