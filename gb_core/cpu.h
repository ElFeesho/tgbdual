#pragma once

static const int RAM_SIZE = 0x2000 * 4;

class gb;
class serializer;

union pare_reg {
    uint16_t w;
    struct {
        uint8_t l, h;
    } b;
};

struct cpu_regs {
    pare_reg AF;
    pare_reg BC;
    pare_reg DE;
    pare_reg HL;
    uint16_t SP;
    uint16_t PC;
    uint8_t I;
};

class cpu {
    friend class gb;

   public:
    cpu(gb *ref);

    uint8_t read(uint16_t adr);

    uint8_t read_direct(uint16_t adr);
    void write(uint16_t adr, uint8_t dat);
    uint16_t inline readw(uint16_t adr) { return read(adr) | (read(adr + 1) << 8); }
    void inline writew(uint16_t adr, uint16_t dat) {
        write(adr, (uint8_t)dat);
        write(adr + 1, dat >> 8);
    }

    void exec(int clocks);
    uint8_t seri_send(uint8_t dat);
    void irq(int irq_type);
    void inline irq_process();
    void reset();
    void set_trace(bool trace) { b_trace = trace; }

    uint8_t *get_vram() { return vram; }
    uint8_t *get_ram() { return ram; }
    uint8_t *get_oam() { return oam; }
    uint8_t *get_stack() { return stack; }

    uint8_t *get_ram_bank() { return ram_bank; }
    void set_ram_bank(int bank) { ram_bank = ram + bank * 0x1000; }
    uint8_t get_ram_bank_number() { return (ram - ram_bank) / 0x1000; }

    cpu_regs *get_regs() { return &regs; }

    int get_clock() { return total_clock; }
    bool get_speed() { return speed; }

    bool *get_halt() { return &halt; }

    void save_state(int *dat);
    void restore_state(int *dat);
    void save_state_ex(int *dat);
    void restore_state_ex(int *dat);

    void serialize(serializer &s);

   private:
    uint8_t inline io_read(uint16_t adr);
    void inline io_write(uint16_t adr, uint8_t dat);
    uint8_t op_read() { return read(regs.PC++); }
    uint16_t op_readw() {
        regs.PC += 2;
        return readw(regs.PC - 2);
    }

    int dasm(char *S, uint8_t *A);
    void log();

    gb *ref_gb;
    cpu_regs regs;

    uint8_t ram[RAM_SIZE];
    uint8_t vram[0x2000 * 2];
    uint8_t stack[0x80];
    uint8_t oam[0xA0];
    uint8_t spare_oam[0x18];
    uint8_t ext_mem[16];

    uint8_t *vram_bank;
    uint8_t *ram_bank;

    uint8_t z802gb[256], gb2z80[256];
    uint32_t rp_que[256];
    int que_cur;
    //	uint16_t org_pal[16][4];
    int total_clock, rest_clock, sys_clock, seri_occer, div_clock;
    bool halt, speed, speed_change, dma_executing;
    bool b_trace;
    int dma_src;
    int dma_dest;
    int dma_rest;
    int gdma_rest;
    bool b_dma_first;

    int last_int;
    bool int_desable;

    uint8_t *dma_src_bank;
    uint8_t *dma_dest_bank;

    uint8_t _ff6c, _ff72, _ff73, _ff74, _ff75;

    //	FILE *file;
};