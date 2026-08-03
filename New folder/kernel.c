#include <stdint.h>

// Raspberry Pi 5 Mailbox Hardware Addresses
#define MMIO_BASE_1     0x107C000000UL
#define MBOX_REG_OFFSET (MMIO_BASE_1 + 0x13880)

#define MBOX0_READ      (*(volatile uint32_t *)(MBOX_REG_OFFSET + 0x00))
#define MBOX0_STATUS    (*(volatile uint32_t *)(MBOX_REG_OFFSET + 0x18))
#define MBOX1_WRITE     (*(volatile uint32_t *)(MBOX_REG_OFFSET + 0x20))
#define MBOX1_STATUS    (*(volatile uint32_t *)(MBOX_REG_OFFSET + 0x38))

#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000
#define MBOX_RESPONSE   0x80000000
#define MBOX_REQUEST    0x00000000

// GPU Mailbox Tags
#define MBOX_TAG_SET_PHYS_WH  0x00048003
#define MBOX_TAG_SET_VIRT_WH  0x00048004
#define MBOX_TAG_SET_DEPTH    0x00048005
#define MBOX_TAG_ALLOC_BUF    0x00040001
#define MBOX_TAG_GET_PITCH    0x00040008
#define MBOX_TAG_LAST         0x00000000

// 16-byte aligned mailbox request buffer
volatile uint32_t __attribute__((aligned(16))) mbox[36];

uint32_t *framebuffer = 0;
uint32_t screen_width = 1024;
uint32_t screen_height = 768;
uint32_t screen_pitch = 0;

int mbox_call(uint8_t ch) {
    uint32_t addr = (uint32_t)((uintptr_t)mbox & ~0xF) | (ch & 0xF);
    
    while (MBOX1_STATUS & MBOX_FULL);
    MBOX1_WRITE = addr;

    while (1) {
        while (MBOX0_STATUS & MBOX_EMPTY);
        if (MBOX0_READ == addr) {
            return mbox[1] == MBOX_RESPONSE;
        }
    }
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x < screen_width && y < screen_height && framebuffer) {
        framebuffer[y * (screen_pitch / 4) + x] = color;
    }
}

void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t i = 0; i < h; i++) {
        for (uint32_t j = 0; j < w; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}

void init_framebuffer(void) {
    mbox[0] = 35 * 4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_SET_PHYS_WH;
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;  // Resolution Width
    mbox[6] = 768;   // Resolution Height

    mbox[7] = MBOX_TAG_SET_VIRT_WH;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;
    mbox[11] = 768;

    mbox[12] = MBOX_TAG_SET_DEPTH;
    mbox[13] = 4;
    mbox[14] = 4;
    mbox[15] = 32;   // 32-bit ARGB Color Mode

    mbox[16] = MBOX_TAG_ALLOC_BUF;
    mbox[17] = 8;
    mbox[18] = 8;
    mbox[19] = 16;
    mbox[20] = 0;

    mbox[21] = MBOX_TAG_GET_PITCH;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 0;

    mbox[25] = MBOX_TAG_LAST;

    if (mbox_call(8)) {
        framebuffer = (uint32_t *)(uintptr_t)(mbox[19] & 0x3FFFFFFF);
        screen_pitch = mbox[24];
    }
}

void kernel_main(void) {
    init_framebuffer();

    if (framebuffer) {
        // Fill dark background (Slate Blue)
        draw_rect(0, 0, 1024, 768, 0xFF0F172A);

        // Draw central Nexus OS window card
        draw_rect(212, 234, 600, 300, 0xFF1E293B);
        
        // Cyan top accent bar
        draw_rect(212, 234, 600, 12, 0xFF38BDF8);

        // Glowing blue status block
        draw_rect(252, 280, 520, 80, 0xFF0284C7);

        // Green system active bar
        draw_rect(252, 400, 200, 30, 0xFF22C55E);
    }

    while (1) {
        __asm__ volatile("wfe");
    }
}
