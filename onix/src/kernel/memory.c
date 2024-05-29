#include<onix/memory.h>
#include<onix/types.h>
#include<onix/debug.h>
#include<onix/assert.h>
#include<onix/memory.h>
#include<onix/string.h>
#include<onix/stdlib.h>

#define LOGK(fmt,args...) DEBUGK(fmt,##args)

#define ZONE_VALID 1 //ards 可用内存区域
#define ZONE_RESERVED 2 //ards 不可用区域

#define IDX(addr) ((u32)addr >> 12) //获取addr的页索引
#define DIDX(addr) (((u32)addr>>22)&0x3ff) //获取addr的页目录索引
#define TIDX(addr) (((u32)addr>>12)&0x3ff)//获取addr的页表索引
#define PAGE(idx) ((u32)idx << 12)  // 获取页索引 idx 对应的页开始的位置
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)  //检查地址是否对齐代码

//内核页目录索引                                                       //通过检查地址的最低12位是否为0来确保地址是以4KB为边界对齐的
#define KERNEL_PAGE_DIR 0x1000

//内核页表索引
static u32 KERNEL_PAGE_TABLE[] = {
    0x2000,
    0x3000,
};

#define KERNEL_MEMORY_SIZE (0x100000*sizeof(KERNEL_PAGE_TABLE))

typedef struct ards_t
{
    u64 base;//内存基地址
    u64 size;//内存长度
    u32 type;//类型
}_packed ards_t;

static u32 memory_base = 0;//可用内存基地址 应该等于1M
static u32 memory_size = 0;//可用内存大小
static u32 total_pages = 0;//所有内存页数
static u32 free_pages = 0;//空闲内存页数


#define used_pages (total_pages-free_pages)//已用页数

void memory_init(u32 magic, u32 addr)
{
    u32 count;
    ards_t* ptr;

    // 如果是 onix loader 进入的内核
    if (magic == ONIX_MAGIC)
    {
        count = *(u32*)addr;//取描述符的数量
        ptr = (ards_t*)(addr + 4);//指针指向了ARDS表中第一个描述符的位置
        for (size_t i = 0; i < count; i++, ptr++)
        {
            LOGK("Memory base 0x%p size 0x%p type %d\n",
                (u32)ptr->base, (u32)ptr->size, (u32)ptr->type);
            if (ptr->type == ZONE_VALID && ptr->size > memory_size)
            {
                //如果新的内存区域大小大于 memory_size，
                //则更新 memory_base 和 memory_size，
                //将其设为新发现的内存区域的基地址和大小(指向1M后的内存)
                memory_base = (u32)ptr->base;
                memory_size = (u32)ptr->size;
            }
        }
    }
    else
    {
        panic("Memory init magic unknown 0x%p\n", magic);
    }

    LOGK("ARDS count %d\n", count);
    LOGK("Memory base 0x%p\n", (u32)memory_base);
    LOGK("Memory size 0x%p\n", (u32)memory_size);

    assert(memory_base == MEMORY_BASE); // 内存开始的位置为 1M

    /*目的是检查内存大小的低12位是否为0。这是因为在典型的x86系统中，页的大小通常是4KB，
    而12个二进制位正好可以表示一个4KB的偏移量。
    因此，通过将内存大小的低12位与0xfff（二进制为12个1）进行按位与操作，
    可以得到内存大小的低12位的值。
    如果结果为0，则表示内存大小是4KB的整数倍，即4KB对齐。*/
    assert((memory_size & 0xfff) == 0); // 要求按页对齐

    //计算可用内存的总页数和空闲页数
    total_pages = IDX(memory_size) + IDX(MEMORY_BASE);
    free_pages = IDX(memory_size);

    LOGK("Total pages %d\n", total_pages);
    LOGK("Free pages %d\n", free_pages);

    if (memory_size < KERNEL_MEMORY_SIZE) {
        panic("System memory is %dM too small,at least %dM needed\n", memory_size / MEMORY_BASE, KERNEL_MEMORY_SIZE / MEMORY_BASE);
    }
}

static u32 start_page = 0;//可分配物理内存起始位置
static u8* memory_map;    //物理内存数组
static u32 memory_map_pages;//物理内存数组占用的页数

void memory_map_init() {
    //初始化物理内存数组
    memory_map = (u8*)memory_base;

    //计算物理内存数组占用的页数
    memory_map_pages = div_round_up(total_pages, PAGE_SIZE);
    LOGK("Memory map page count %d\n", memory_map_pages);

    free_pages -= memory_map_pages;

    //清空物理内存数组
    memset((void*)memory_map, 0, memory_map_pages * PAGE_SIZE);

    //前1M的内存位置 以及 物理内存数组已占用的页,已被占用
    start_page = IDX(MEMORY_BASE) + memory_map_pages;
    for (size_t i = 0;i < start_page;i++) {
        memory_map[i] = 1;
    }
    LOGK("Total pages %d free pages %d\n", total_pages, free_pages);
}

//分配一页的物理内存
static u32 get_page() {
    for (size_t i = start_page;i < total_pages;i++) {
        //如果物理内存没用占用
        if (!memory_map[i]) {
            memory_map[i] = 1;
            free_pages--;
            assert(free_pages >= 0);
            u32 page = ((u32)i) << 12;
            LOGK("GET page 0x%p\n", page);
            return page;
        }
    }
    panic("Out of Memory!!!");
}

//释放一页物理内存
static void put_page(u32 addr) {
    ASSERT_PAGE(addr);

    u32 idx = IDX(addr);

    //idx 大于1M并且小于总页面数
    assert(memory_map[idx] >= 1);

    //物理引用减一
    memory_map[idx]--;

    //若为0 则空闲页加一
    if (!memory_map[idx]) {
        free_pages++;
    }

    assert(free_pages > 0 && free_pages < total_pages);
    LOGK("PUT page 0x%p\n", addr);
}

//得到cr3寄存器
u32 inline get_cr3() {
    //直接将mov eax,cr3返回值在eax中
    asm volatile("movl %cr3,%eax\n");
}

//设置cr3寄存器,参数是页目录的地址
void set_cr3(u32 pde) {
    ASSERT_PAGE(pde);  //确保传入的页目录条目 pde 是按页面对齐的
    asm volatile("movl %%eax,%%cr3\n"::"a"(pde));  //
}

//将cr0寄存器最高位PE置为1,启动分页
static _inline void enable_page() {
    //0b1000_0000_0000_0000_0000_0000_0000_0000
    //0x800000000
    asm volatile("movl %cr0,%eax\n"
        "orl $0x80000000,%eax\n"
        "movl %eax,%cr0\n");
}

//初始化页表项
static void entry_init(page_entry_t* entry, u32 index) {
    *(u32*)entry = 0;
    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}

//内核页目录
#define KERNEL_PAGE_DIR 0x200000

//内核页表
#define KERNEL_PAGE_ENTRY 0x201000

//初始化内存映射
void mapping_init()
{
    page_entry_t *pde = (page_entry_t *)KERNEL_PAGE_DIR;
    memset(pde, 0, PAGE_SIZE);

    idx_t index = 0;

    for (idx_t didx = 0; didx < (sizeof(KERNEL_PAGE_TABLE) / 4); didx++)
    {
        page_entry_t *pte = (page_entry_t *)KERNEL_PAGE_TABLE[didx];
        memset(pte, 0, PAGE_SIZE);

        page_entry_t *dentry = &pde[didx];
        entry_init(dentry, IDX((u32)pte));

        for (idx_t tidx = 0; tidx < 1024; tidx++, index++)
        {
            // 第 0 页不映射，为造成空指针访问，缺页异常，便于排错
            if (index == 0)
                continue;

            page_entry_t *tentry = &pte[tidx];
            entry_init(tentry, index);
            memory_map[index] = 1; // 设置物理内存数组，该页被占用
        }
    }

    // 将最后一个页表指向页目录自己，方便修改
    page_entry_t *entry = &pde[1023];
    entry_init(entry, IDX(KERNEL_PAGE_DIR));

    // 设置 cr3 寄存器
    set_cr3((u32)pde);

    BMB;
    // 分页有效
    enable_page();
}

static page_entry_t *get_pde()
{
    return (page_entry_t *)(0xfffff000);
}

static page_entry_t *get_pte(u32 vaddr)
{
    return (page_entry_t *)(0xffc00000 | (DIDX(vaddr) << 12));
}

// 刷新虚拟地址 vaddr 的 块表 TLB
static void flush_tlb(u32 vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
                 : "memory");
}

void memory_test()
{
    BMB;

    // 将 20 M 0x1400000 内存映射到 64M 0x4000000 的位置

    // 我们还需要一个页表，0x900000

    u32 vaddr = 0x4000000; // 线性地址几乎可以是任意的
    u32 paddr = 0x1400000; // 物理地址必须要确定存在
    u32 table = 0x900000;  // 页表也必须是物理地址

    page_entry_t *pde = get_pde();

    page_entry_t *dentry = &pde[DIDX(vaddr)];
    entry_init(dentry, IDX(table));

    page_entry_t *pte = get_pte(vaddr);
    page_entry_t *tentry = &pte[TIDX(vaddr)];

    entry_init(tentry, IDX(paddr));

    BMB;

    char *ptr = (char *)(0x4000000);
    ptr[0] = 'a';

    BMB;

    entry_init(tentry, IDX(0x1500000));
    flush_tlb(vaddr);

    BMB;

    ptr[2] = 'b';

    BMB;
}