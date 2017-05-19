#pragma once

static const int RAM_SIZE = 0x2000 * 4;

class gb;

class serializer;

union pare_reg {
    uint16_t w{0};
    struct {
        uint8_t l, h;
    } b;
};

struct cpu_regs {
    pare_reg AF;
    pare_reg BC;
    pare_reg DE;
    pare_reg HL;
    uint16_t SP{0};
    uint16_t PC{0};
    uint8_t I{0};
};

class cpu {
    friend class gb;

public:
    explicit cpu(gb &ref);

    uint8_t read(uint16_t adr) const;

    uint8_t read_direct(uint16_t adr) const;

    void write(uint16_t adr, uint8_t dat);

    uint16_t inline readw(uint16_t adr) const { return read(adr) | (read((uint16_t) (adr + 1)) << 8); }

    void inline writew(uint16_t adr, uint16_t dat) {
        write(adr, (uint8_t) dat);
        write((uint16_t) (adr + 1), (uint8_t) (dat >> 8));
    }

    void exec(int clocks);

    void irq(int irq_type);

    void inline irq_process();

    void reset();

    const uint8_t *get_vram() const { return vram; }

    const uint8_t *get_ram() const { return ram; }

    const uint8_t *get_oam() const { return oam; }

    uint8_t get_ram_bank_number() const { return (uint8_t) ((ram - ram_bank) / 0x1000); }

    int get_clock() const { return total_clock; }

    bool get_speed() const { return speed; }

    void serialize(serializer &s);

private:
    uint8_t inline io_read(uint16_t adr) const;

    void inline io_write(uint16_t adr, uint8_t dat);

    uint8_t op_read() { return read(regs.PC++); }

    uint16_t op_readw() {
        regs.PC += 2;
        return readw((uint16_t) (regs.PC - 2));
    }

    gb &ref_gb;
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
    int total_clock, rest_clock, sys_clock, seri_occer, div_clock;
    bool halt, speed, speed_change, dma_executing;
    int dma_src;
    int dma_dest;
    int dma_rest;
    int gdma_rest;
    bool b_dma_first;

    bool int_desable;

    uint8_t *dma_src_bank;
    uint8_t *dma_dest_bank;

    uint8_t _ff6c, _ff72, _ff73, _ff74, _ff75;
};