#include<onix/io.h>
#include<onix/interrupt.h>
#include<onix/assert.h>
#include<onix/debug.h>
#include<onix/task.h>

#define PIT_CHAN0_REG 0x40
#define PIT_CHAN2_REG 0x42
#define PIT_CTRL_REG 0x43

#define HZ 100
#define OSCILLATOR 1193182
#define CLOCK_COUNTER (OSCILLATOR / HZ)
#define JIFFY (1000 / HZ)

#define SPEAKER_REG 0x61
#define BEEP_HZ 400
#define BEEP_COUNTER (OSCILLATOR / BEEP_HZ)

//时间片计数器
u32 volatile jiffies = 0;//告诉编译器不要对 jiffies 变量进行优化
u32 jiffy = JIFFY;

u32 volatile beeping = 0;

void start_beep(){
    if(!beeping)
    {
        outb(SPEAKER_REG,inb(SPEAKER_REG)|0b11);
    }
    beeping = jiffies+5;
}

void stop_beep(){
    if(beeping && jiffies > beeping){
        outb(SPEAKER_REG,inb(SPEAKER_REG)& 0xfc);
        beeping = 0;
    }
}

void clock_handler(int vector)
{
    assert(vector == 0x20);
    send_eoi(vector);
    stop_beep();
    jiffies++;
    // DEBUGK("clock jiffies %d ...\n", jiffies);

    task_t* task = running_task();
    assert(task->magic == ONIX_MAGIC);

    task->jiffies = jiffies;
    task->ticks--;
    if(!task->ticks){
        schedule();
    }
}

void pit_init() {
    //配置计数器0时钟
    outb(PIT_CTRL_REG, 0b00110100);
    outb(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);
    outb(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);

    //配置计数器 2 蜂鸣器
    outb(PIT_CTRL_REG,0b10110110);
    outb(PIT_CHAN2_REG,(u8)BEEP_COUNTER);
    outb(PIT_CHAN2_REG,(u8)(BEEP_COUNTER>>8));
}

void clock_init() {
    pit_init();
    set_interrupt_handler(IRQ_CLOCK, clock_handler);
    set_interrupt_mask(IRQ_CLOCK, true);
}