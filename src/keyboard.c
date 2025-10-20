#include "inc/keyboard.h"

static input_command_t cmd_queue[CMD_QUEUE_SIZE];
static int head = 0, tail = 0;
static uint8_t key_state[128];
repeat_state_t repeat = {0}; // Initialize the repeat state

static int queue_is_full(void) {
    return ((tail + 1) % CMD_QUEUE_SIZE) == head;
}

static int queue_is_empty(void) {
    return head == tail;
}

void enqueue(input_command_t cmd) {
    if (queue_is_full())
        return; // taşma kontrolü
    cmd_queue[tail] = cmd;
    tail = (tail + 1) % CMD_QUEUE_SIZE;
}

int dequeue(input_command_t *out) {
    if (queue_is_empty())
        return 0;
    *out = cmd_queue[head];
    head = (head + 1) % CMD_QUEUE_SIZE;
    return 1;
}

void keyboard_isr(unsigned char scancode)
{
    input_command_t cmd;

    cmd.scancode = scancode & 0x7F;
    cmd.type = (scancode & 0x80) ? 1 : 0; // 0 = pressed, 1 = released

    enqueue(cmd);
    // Just enqueue and return immediately
}

int input_poll(input_command_t *out) 
{
    return dequeue(out);
}


void update_key_state(input_command_t *cmd)
{
    uint8_t sc = cmd->scancode;
    if (sc >= MAX_KEYS)
        return;

    uint8_t state = key_state[sc];
    uint8_t pressed = (cmd->type == 0); // 0 = pressed, 1 = released

    // toggle keys (CapsLock, NumLock, ScrollLock)
    if (sc == 0x3A || sc == 0x45 || sc == 0x46)
    {
        // sadece ilk basıldığında toggle değiştir
        if (pressed && !(state & 0x01))  // sadece önce basılı değilse
            state ^= (1 << 1);           // toggle bitini tersle
    }

    // her durumda basılı bitini güncelle
    if (pressed) {
        state |=  (1 << 0);
        // Setup repeat state for this key
        repeat.key = sc;
        repeat.counter = 0;
        repeat.active = 1;
    }
    else {
        state &= ~(1 << 0);
        // If this is the key currently repeating, disable repeat
        if (repeat.key == sc) {
            repeat.active = 0;
        }
    }

    key_state[sc] = state;
}

// Handle key repeat logic - call this from main loop
void handle_key_repeat(void) {
    if (!repeat.active)
        return;
    
    repeat.counter++;
    
    // Initial delay before repeating
    if (repeat.counter < REPEAT_DELAY)
        return;
    
    // Generate repeat at the specified rate
    if ((repeat.counter - REPEAT_DELAY) % REPEAT_RATE == 0) {
        input_command_t cmd;
        cmd.scancode = repeat.key;
        cmd.type = 0; // Key press
        enqueue(cmd);
    }
}


int is_key_down(uint8_t scancode)
{
    return (scancode < MAX_KEYS) ? (key_state[scancode] & 0x01) : 0;
}

int is_key_toggled(uint8_t scancode)
{
    return (scancode < MAX_KEYS) ? ((key_state[scancode] >> 1) & 0x01) : 0;
}
