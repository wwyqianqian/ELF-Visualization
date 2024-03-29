#include <stdio.h>
#include <stdlib.h>
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

/*
funtion of get the section headers
*/
void get_section_headers(const char *pbuff) {
    // (pbuff + 60) contains the number of section headers
    int SH_num = *(Elf64_Half*) (pbuff + 60);

    // get shstrndex (section header string table index)
    Elf64_Ehdr* pfilehead = (Elf64_Ehdr*) pbuff; // Elf64_Ehdr: struct of ELF file header
    Elf64_Half eshstrndx = pfilehead->e_shstrndx;

    // get the offset of section header string
    Elf64_Shdr* psecheader = (Elf64_Shdr*) (pbuff + pfilehead->e_shoff); // Elf64_Shdr: struct of section header
    Elf64_Shdr* pshstr = (Elf64_Shdr*) (psecheader + eshstrndx);
    char* pshstrbuff = (char *)(pbuff + pshstr->sh_offset);

    // output info
    printf("There are %d section headers, starting at offset 0x%llx:\n\n", SH_num, (unsigned long long)(*(Elf64_Off*) (pbuff + 40)));
    printf("Section Headers:\n");
    printf("  [Nr] %-16s  %-16s  %-16s  %-16s\n", "Name", "Type", "Address", "Offset");
    printf("       %-16s  %-16s  %-5s  %-5s  %-5s  %-5s\n", "Size", "EntSize", "Flags", "Link", "Info", "Align");


    for (int i = 0; i < SH_num; ++i) {
        // Name
        printf("  [%2d] %-16s  ", i, (char *)(psecheader[i].sh_name + pshstrbuff)); // sh_name: index of shdr string table; pshstrbuff: string table

        // Type
        switch (psecheader[i].sh_type) {
            case SHT_NULL:
                printf("%-16s  ", "NULL");break;
            case SHT_PROGBITS:
                printf("%-16s  ", "PROGBITS");break;
            case SHT_SYMTAB:
                printf("%-16s  ", "SYMTAB");break;
            case SHT_STRTAB:
                printf("%-16s  ", "STRTAB");break;
            case SHT_RELA:
                printf("%-16s  ", "RELA");break;
            case SHT_HASH:
                printf("%-16s  ", "GNU_HASH");break;
            case SHT_DYNAMIC:
                printf("%-16s  ", "DYNAMIC");break;
            case SHT_NOTE:
                printf("%-16s  ", "NOTE");break;
            case SHT_NOBITS:
                printf("%-16s  ", "NOBITS");break;
            case SHT_REL:
                printf("%-16s  ", "REL");break;
            case SHT_SHLIB:
                printf("%-16s  ", "SHLIB");break;
            case SHT_DYNSYM:
                printf("%-16s  ", "DYNSYM");break;
            case SHT_INIT_ARRAY:
                printf("%-16s  ", "INIT_ARRY");break;
            case SHT_FINI_ARRAY:
                printf("%-16s  ", "FINI_ARRY");break;
            case SHT_PREINIT_ARRAY:
                printf("%-16s  ", "PREINIT_ARRAY");break;
            case SHT_GROUP:
                printf("%-16s  ", "GROUP");break;
            case SHT_SYMTAB_SHNDX:
                printf("%-16s  ", "SYMTAB_SHNDX");break;
            case SHT_NUM:
                printf("%-16s  ", "NUM");break;
            case SHT_LOOS:
                printf("%-16s  ", "LOOS");break;
            case SHT_GNU_ATTRIBUTES:
                printf("%-16s  ", "GNU_ATTRIBUTES");break;
            case SHT_GNU_HASH:
                printf("%-16s  ", "GNU_HASH");break;
            case SHT_GNU_LIBLIST:
                printf("%-16s  ", "GNU_LIBLIST");break;
            case SHT_CHECKSUM:
                printf("%-16s  ", "CHECKSUM");break;
            case SHT_LOSUNW:
                printf("%-16s  ", "LOSUNW");break;
            // case SHT_SUNW_move:
            //     printf("%-16s  ", "SUNW_move");break;
            case SHT_SUNW_COMDAT:
                printf("%-16s  ", "SUNW_COMDAT");break;
            case SHT_SUNW_syminfo:
                printf("%-16s  ", "SUNW_syminfo");break;
            case SHT_GNU_verdef:
                printf("%-16s  ", "GNU_verdef");break;
            case SHT_GNU_verneed:
                printf("%-16s  ", "GNU_verneed");break;
            case SHT_GNU_versym:
                printf("%-16s  ", "GNU_versym");break;
            // case SHT_HISUNW:
            //     printf("%-16s  ", "HISUNW");break;
            // case SHT_HIOS:
            //     printf("%-16s  ", "HIOS");break;
            case SHT_LOPROC:
                printf("%-16s  ", "LOPROC");break;
            case SHT_HIPROC:
                printf("%-16s  ", "HIPROC");break;
            case SHT_LOUSER:
                printf("%-16s  ", "LOUSER");break;
            case SHT_HIUSER:
                printf("%-16s  ", "HIUSER");break;
            default:
                printf("%-16s  ", "NONE");break;
        }

        // Address & Offset
        printf("%016llX  %08llX\n", psecheader[i].sh_addr, psecheader[i].sh_offset);

        // Size & EntSize
        printf("       %016llX  %016llx  ", psecheader[i].sh_size, psecheader[i].sh_entsize);

        // Flags
        /*
        ------------------------------- elf.h sh_flags define -------------------------------
        #define SHF_WRITE             (1 << 0)               1     1      W (write)
        #define SHF_ALLOC             (1 << 1)              10     2      A (alloc)
        #define SHF_EXECINSTR         (1 << 2)             100     4      X (execute)

        #define SHF_MERGE             (1 << 4)           10000     16     M (merge)
        #define SHF_STRINGS           (1 << 5)          100000     32     S (strings)
        #define SHF_INFO_LINK         (1 << 6)         1000000     64     I (info)
        #define SHF_LINK_ORDER        (1 << 7)        10000000     128    L (link order)
        #define SHF_OS_NONCONFORMING  (1 << 8)       100000000     256    O (extra OS processing required)
        #define SHF_GROUP             (1 << 9)      1000000000     512    G (group)
        #define SHF_TLS               (1 << 10)    10000000000     1024   T (TLS)
        #define SHF_COMPRESSED        (1 << 11)   100000000000     2048   C (compressed)
        #define SHF_MASKOS            0x0ff00000                          o (OS specific)
        #define SHF_MASKPROC          0xf0000000                          p (processor specific)
        #define SHF_ORDERED           (1 << 30)
        #define SHF_EXCLUDE           (1U << 31)                          E (exclude)
        -------------------------------------------------------------------------------------
        Key to Flags:
        W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
        L (link order), O (extra OS processing required), G (group), T (TLS),
        C (compressed), x (unknown), o (OS specific), E (exclude),
        l (large), p (processor specific), D(SHF_ORDERED).

        */


        switch (psecheader[i].sh_flags) {
            case 0:
                printf("");
                break;
            case SHF_MASKOS:
                printf("o");
                break;
            case SHF_MASKPROC:
                printf("p");
                break;
            case SHF_EXCLUDE:
                printf("E");
                break;
            case SHF_ORDERED:
                printf("D");
                break;

            default:

                if ((psecheader[i].sh_flags & 0b000000000001) != 0) {
                    printf("W");
                }
                if ((psecheader[i].sh_flags & 0b000000000010) != 0) {
                    printf("A");
                }
                if ((psecheader[i].sh_flags & 0b000000000100) != 0) {
                    printf("X");
                }
                if ((psecheader[i].sh_flags & 0b000000010000) != 0) {
                    printf("M");
                }
                if ((psecheader[i].sh_flags & 0b000000100000) != 0) {
                    printf("S");
                }
                if ((psecheader[i].sh_flags & 0b000001000000) != 0) {
                    printf("I");
                }
                if ((psecheader[i].sh_flags & 0b000010000000) != 0) {
                    printf("L");
                }
                if ((psecheader[i].sh_flags & 0b000100000000) != 0) {
                    printf("O");
                }
                if ((psecheader[i].sh_flags & 0b001000000000) != 0) {
                    printf("G");
                }
                if ((psecheader[i].sh_flags & 0b010000000000) != 0) {
                    printf("T");
                }
                if ((psecheader[i].sh_flags & 0b100000000000) != 0) {
                    printf("C");
                }

                break;
        }

        // Link  Info  Align
        printf("    %4u  %4u  %4llu\n", psecheader[i].sh_link, psecheader[i].sh_info, psecheader[i].sh_addralign);
    }

    // print map
    printf("Key to Flags:\n");
    printf("  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n");
    printf("  L (link order), O (extra OS processing required), G (group), T (TLS), C (compressed),\n");
    printf("  x (unknown), o (OS specific), E (exclude), l (large), p (processor specific), D (SHF_ORDERED).\n");
}

/*
funtion of get the program headers
*/
void get_program_headers(const char *pbuff) {
    Elf64_Ehdr* pfilehead = (Elf64_Ehdr*) pbuff;
    Elf64_Phdr* pproheader = (Elf64_Phdr*) (pbuff + pfilehead->e_phoff);
    Elf64_Shdr* psecheader = (Elf64_Shdr*) (pbuff + pfilehead->e_shoff);
    Elf64_Shdr* pshstr = (Elf64_Shdr*) (psecheader + pfilehead->e_shstrndx);
    char* pstrbuff = (char*) (pbuff + pshstr->sh_offset);
    printf("Elf file type is: ");

    // Legal values for e_type (object file type)
    switch (pfilehead->e_type) {
        case 0:
            printf("No file type\n");
            break;
        case 1:
            printf("Relocatable file\n");
            break;
        case 2:
            printf("Executable file\n");
            break;
        case 3:
            printf("Shared object file\n");
            break;
        case 4:
            printf("Core file\n");
            break;
        default:
            printf(" ERROR\n");
            break;
    }

    // Entry point virtual address e_entry
    printf("Entry point: 0X%0llX\n", pfilehead->e_entry);

    // e_phnum, e_phoff
    printf("There are %d program headers, starting at offset %llu\n\n", pfilehead->e_phnum, pfilehead->e_phoff);

    // Print program headers' details
    printf("Program Headers:\n");
    printf("  %-14s  %-16s  %-16s  %-16s\n", "Type", "Offset", "VirtAddr", "PhysAddr");
    printf("  %-14s  %-16s  %-16s  %-6s  %-6s\n", "", "FileSiz", "MemSiz", "Flags", "Align");


    for (int i = 0; i < pfilehead->e_phnum; ++i) {

        // Type, Offset, VirAddr, PhyAddr, FileSiz, MemSiz
        switch (pproheader[i].p_type) {
            case PT_NULL:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_LOAD:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "LOAD", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_DYNAMIC:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "DYNAMIC", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_INTERP:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "INTERP", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_NOTE:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "NOTE", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_SHLIB:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "SHLIB", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_PHDR:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "PHDR", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_TLS:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "TLS", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_NUM:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "NUM", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_LOOS:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "LOOS", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_GNU_EH_FRAME:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "GNU_EH_FRAME", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_GNU_STACK:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "GNU_STACK", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_GNU_RELRO:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "GNU_RELRO", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_LOSUNW:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "LOSUNW", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            // case PT_SUNWBSS:
            //     printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "SUNWBSS", pproheader[i].p_offset, pproheader[i].p_vaddr,
            //            pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_SUNWSTACK:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "SUNWSTACK", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_HISUNW:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "HISUNW", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            // case PT_HIOS:
            //     printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "HIOS", pproheader[i].p_offset, pproheader[i].p_vaddr,
            //            pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_LOPROC:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "LOPROC", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            case PT_HIPROC:
                printf("  %-14s  %016llX  %016llX  %016llX\n  %-14s  %016llX  %016llX  ", "HIPROC", pproheader[i].p_offset, pproheader[i].p_vaddr,
                       pproheader[i].p_paddr, "", pproheader[i].p_filesz, pproheader[i].p_memsz);break;
            default:
                break;
        }

        // Flags: Legal values for p_flags (segment flags)
        // Align
        switch (pproheader[i].p_flags) {
            case PF_X:
                printf("%-6s  0X%-llX\n", "  E", pproheader[i].p_align);break;
            case PF_W:
                printf("%-6s  0X%-llX\n", " W ", pproheader[i].p_align);break;
            case PF_R:
                printf("%-6s  0X%-llX\n", "R  ", pproheader[i].p_align);break;
            case PF_X|PF_W:
                printf("%-6s  0X%-llX\n", " WE", pproheader[i].p_align);break;
            case PF_X|PF_R:
                printf("%-6s  0X%-llX\n", "R E", pproheader[i].p_align);break;
            case PF_W|PF_R:
                printf("%-6s  0X%-llX\n", "RW ", pproheader[i].p_align);break;
            case PF_X|PF_R|PF_W:
                printf("%-6s  0X%-llX\n", "RWE", pproheader[i].p_align);break;
            default:
                printf("\n");
                break;
        }

        // Segment file offset
        if(PT_INTERP == pproheader[i].p_type)
            printf("      [Requesting program interpreter: %s]\n", (char*) (pbuff + pproheader[i].p_offset));
    }

    // Print mapping
    printf("\n Section to Segment mapping:\n");
    printf("  Segment Sections...\n");
    for (int i = 0; i < pfilehead->e_phnum; ++i) {
        printf("   %-7d", i);
        for (int n = 0; n < pfilehead->e_shnum; ++n) {
            Elf64_Off temp = psecheader[n].sh_addr + psecheader[n].sh_size;
            if ((psecheader[n].sh_addr > pproheader[i].p_vaddr && psecheader[n].sh_addr < pproheader[i].p_vaddr + pproheader[i].p_memsz) ||
                (temp > pproheader[i].p_vaddr && temp <= pproheader[i].p_vaddr + pproheader[i].p_memsz)) {
                printf("%s ", (char*) (psecheader[n].sh_name + pstrbuff));
            }
        }
        printf("\n");
    }
}

/*
funtion of get the symbol table
*/
void get_symbol_table(const char *pbuff) {
    Elf64_Ehdr* pfilehead = (Elf64_Ehdr*) pbuff;
    uint16_t eshstrndx = pfilehead->e_shstrndx;
    Elf64_Shdr* psecheader = (Elf64_Shdr*) (pbuff + pfilehead->e_shoff);
    Elf64_Shdr* pshstr = (Elf64_Shdr*) (psecheader + eshstrndx);
    char* pshstrbuff = (char *)(pbuff + pshstr->sh_offset);

    for (int i = 0;i < pfilehead->e_shnum; ++i) {
        if (!strcmp(psecheader[i].sh_name + pshstrbuff, ".dynsym") || !strcmp(psecheader[i].sh_name + pshstrbuff, ".symtab")) {
            Elf64_Sym* psym = (Elf64_Sym*) (pbuff + psecheader[i].sh_offset);
            int ncount = psecheader[i].sh_size / psecheader[i].sh_entsize; // sh_entsize: Entry size if section holds table
            char* pbuffstr = (char*) ((psecheader + psecheader[i].sh_link)->sh_offset + pbuff); // sh_link: Link to another section
            printf("Symbol table '%s' contains %d entries:\n", psecheader[i].sh_name + pshstrbuff, ncount);
            outputsyminfo(psym, pbuffstr, ncount);
            continue;
        }
    }
}
void outputsyminfo(const Elf64_Sym *psym, const char *pbuffstr, int ncount) {
    printf("%7s  %-8s          %s  %s    %s    %s        %s  %s\n",
           "Num:", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");

    for (int i = 0; i < ncount; ++i) {
        // Print num, value, size
        printf("%6d:  %016llx  %-6llu", i, psym[i].st_value, psym[i].st_size);

        // How to extract and insert information held in the st_info field.
        // Print type, bind
        char typelow = ELF64_ST_TYPE(psym[i].st_info);
        char bindhigh = ELF64_ST_BIND(psym[i].st_info);

        // Legal values for ST_TYPE subfield of st_info (symbol type)
        switch (typelow) {
            case STT_NOTYPE:
                printf("%-8s", "NOTYPE");break;
            case STT_OBJECT:
                printf("%-8s", "OBJECT");break;
            case STT_FUNC:
                printf("%-8s", "FUNC");break;
            case STT_SECTION:
                printf("%-8s", "SECTION");break;
            case STT_FILE:
                printf("%-8s", "FILE");break;
            case STT_COMMON:
                printf("%-8s", "COMMON");break;
            case STT_TLS:
                printf("%-8s", "TLS");break;
            case STT_NUM:
                printf("%-8s", "NUM");break;
            case STT_LOOS:
                printf("%-8s", "LOOS");break;
            // case STT_GNU_IFUNC:
            //     printf("%-8s", "GNU_IFUNC");break;
            case STT_HIOS:
                printf("%-8s", "HIOS");break;
            case STT_LOPROC:
                printf("%-8s", "LOPROC");break;
            case STT_HIPROC:
                printf("%-8s", "HIPROC");break;
            default:
                break;
        }

        // Legal values for ST_BIND subfield of st_info (symbol binding)
        switch (bindhigh) {
            case STB_LOCAL:
                printf("%-8s", "LOCAL"); break;
            case STB_GLOBAL:
                printf("%-8s", "GLOBAL"); break;
            case STB_WEAK:
                printf("%-8s", "WEAK"); break;
            case STB_NUM:
                printf("%-8s", "NUM");break;
            case STB_LOOS:
                printf("%-8s", "LOOS");break;
            // case STB_GNU_UNIQUE:
            //     printf("%-8s", "GNU_UNIQUE");break;
            case STB_HIOS:
                printf("%-8s", "HIOS");break;
            case STB_LOPROC:
                printf("%-8s", "LOPROC");break;
            case STB_HIPROC:
                printf("%-8s", "HIPROC");break;
            default:
                break;
        }

        // Print vis: Symbol visibility specification encoded in the st_other field
        switch (psym[i].st_other) {
            case STV_DEFAULT:
                printf("%-8s", "DEFAULT");break;
            case STV_INTERNAL:
                printf("%-8s", "INTERNAL");break;
            case STV_HIDDEN:
                printf("%-8s", "HIDDEN");break;
            case STV_PROTECTED:
                printf("%-8s", "PROTECTED");break;
            default:
                break;
        }

        // Print ndx, name
        switch (psym[i].st_shndx) {
            case SHN_ABS:
                printf("   %s  %s\n", "ABS", psym[i].st_name + pbuffstr);break;
            case SHN_COMMON:
                printf("   %s  %s\n", "COM", psym[i].st_name + pbuffstr);break;
            case SHN_UNDEF:
                printf("   %s  %s\n", "UND", psym[i].st_name + pbuffstr);break;
            default:
                printf("   %3d  %s\n", psym[i].st_shndx, psym[i].st_name + pbuffstr);break;
        }
    }
}

/*
funtion of get the relocation entries
*/
void get_relocation_entries(const char *pbuff) {
    Elf64_Ehdr* pfilehead = (Elf64_Ehdr*) pbuff;
    Elf64_Shdr* psecheader = (Elf64_Shdr*) (pbuff + pfilehead->e_shoff);
    Elf64_Shdr* pshstr = (Elf64_Shdr*) (psecheader + pfilehead->e_shstrndx);
    char* pstrbuff = (char*) (pbuff + pshstr->sh_offset);

    for (int i = 0; i < pfilehead->e_shnum; ++i) {
        if (!strncmp(psecheader[i].sh_name + pstrbuff, ".rel", 4)) {
            int ncount = psecheader[i].sh_size / psecheader[i].sh_entsize;

            // Print sh_name, sh_offset, count
            printf("\nRelocation section'%s' at offset %0llX contains %d entrie(s):\n", psecheader[i].sh_name + pstrbuff, psecheader[i].sh_offset, ncount);
            Elf64_Rela* preltable = (Elf64_Rela*) (pbuff + psecheader[i].sh_offset);

            // sh_link: Link to another section
            printf("%-16s  %-16s  %-16s   %-16s  %-16s\n", "Offset", "Info", "Type", "Sym.Value", "Sym.Name + Addend");
            int symnum = psecheader[i].sh_link;
            int strnum = psecheader[symnum].sh_link;

            // relocated string buffer
            char* prelstrbuf = (char*) (psecheader[strnum].sh_offset + pbuff);

            // symbol
            Elf64_Sym* psym = (Elf64_Sym*) (pbuff + psecheader[symnum].sh_offset);
            for (int n = 0; n < ncount; ++n) {

                // Print offset, info
                printf("%016llX  %016llX  ", preltable[n].r_offset, preltable[n].r_info);

                // Print relocations: elf.h:2676
                // Only for AMD x86-64 relocations
                switch (ELF64_R_TYPE(preltable[n].r_info)) {
                    case R_X86_64_NONE:
                        printf("%-16s", "R_X86_64_NONE");break;
                    case R_X86_64_64:
                        printf("%-16s", "R_X86_64_64");break;
                    case R_X86_64_PC32:
                        printf("%-16s", "R_X86_64_PC32");break;
                    case R_X86_64_GOT32:
                        printf("%-16s", "R_X86_64_GOT32");break;
                    case R_X86_64_PLT32:
                        printf("%-16s", "R_X86_64_PLT32");break;
                    case R_X86_64_COPY:
                        printf("%-16s", "R_X86_64_COPY");break;
                    case R_X86_64_GLOB_DAT:
                        printf("%-16s", "R_X86_64_GLOB_DAT");break;
                    case R_X86_64_JUMP_SLOT:
                        printf("%-16s", "R_X86_64_JUMP_SLOT");break;
                    case R_X86_64_RELATIVE:
                        printf("%-16s", "R_X86_64_RELATIVE");break;
                    case R_X86_64_GOTPCREL:
                        printf("%-16s", "R_X86_64_GOTPCREL");break;
                    case R_X86_64_32:
                        printf("%-16s", "R_X86_64_32");break;
                    case R_X86_64_32S:
                        printf("%-16s", "R_X86_64_32S");break;
                    case R_X86_64_16:
                        printf("%-16s", "R_X86_64_16");break;
                    case R_X86_64_PC16:
                        printf("%-16s", "R_X86_64_PC16");break;
                    case R_X86_64_8:
                        printf("%-16s", "R_X86_64_8");break;
                    case R_X86_64_PC8:
                        printf("%-16s", "R_X86_64_PC8");break;
                    case R_X86_64_DTPMOD64:
                        printf("%-16s", "R_X86_64_DTPMOD64");break;
                    case R_X86_64_DTPOFF64:
                        printf("%-16s", "R_X86_64_DTPOFF64");break;
                    case R_X86_64_TPOFF64:
                        printf("%-16s", "R_X86_64_TPOFF64");break;
                    case R_X86_64_TLSGD:
                        printf("%-16s", "R_X86_64_TLSGD");break;
                    case R_X86_64_TLSLD:
                        printf("%-16s", "R_X86_64_TLSLD");break;
                    case R_X86_64_DTPOFF32:
                        printf("%-16s", "R_X86_64_DTPOFF32");break;
                    case R_X86_64_GOTTPOFF:
                        printf("%-16s", "R_X86_64_GOTTPOFF");break;
                    case R_X86_64_TPOFF32:
                        printf("%-16s", "R_X86_64_TPOFF32");break;
                    case R_X86_64_PC64:
                        printf("%-16s", "R_X86_64_PC64");break;
                    case R_X86_64_GOTOFF64:
                        printf("%-16s", "R_X86_64_GOTOFF64");break;
                    case R_X86_64_GOTPC32:
                        printf("%-16s", "R_X86_64_GOTPC32");break;
                    case R_X86_64_GOT64:
                        printf("%-16s", "R_X86_64_GOT64");break;
                    case R_X86_64_GOTPCREL64:
                        printf("%-16s", "R_X86_64_GOTPCREL64");break;
                    case R_X86_64_GOTPC64:
                        printf("%-16s", "R_X86_64_GOTPC64");break;
                    case R_X86_64_GOTPLT64:
                        printf("%-16s", "R_X86_64_GOTPLT64");break;
                    case R_X86_64_PLTOFF64:
                        printf("%-16s", "R_X86_64_PLTOFF64");break;
                    case R_X86_64_SIZE32:
                        printf("%-16s", "R_X86_64_SIZE32");break;
                    case R_X86_64_SIZE64:
                        printf("%-16s", "R_X86_64_SIZE64");break;
                    case R_X86_64_GOTPC32_TLSDESC:
                        printf("%-16s", "R_X86_64_GOTPC32_TLSDESC");break;
                    case R_X86_64_TLSDESC_CALL:
                        printf("%-16s", "R_X86_64_TLSDESC_CALL");break;
                    case R_X86_64_TLSDESC:
                        printf("%-16s", "R_X86_64_TLSDESC");break;
                    case R_X86_64_IRELATIVE:
                        printf("%-16s", "R_X86_64_IRELATIVE");break;
                    case R_X86_64_NUM:
                        printf("%-16s", "R_X86_64_NUM");break;
                    default:
                        break;
                }
                printf("  %016llX  ", (psym + ELF64_R_SYM(preltable[n].r_info))->st_value);

                printf("%s + %llu\n", (char*) (prelstrbuf + (psym + ELF64_R_SYM(preltable[n].r_info))->st_name), preltable[n].r_addend);
            }
        }
    }
}
