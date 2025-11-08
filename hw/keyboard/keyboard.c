#include "../../arch/boot/io.h"
#include "../screen/screen.h"
#include "keyboard.h"

static volatile input_command_t	cmd_queue[CMD_QUEUE_SIZE];
static volatile int				head = 0, tail = 0;
static volatile uint8_t			key_state[128];
volatile repeat_state_t			repeat = {0};

extern volatile uint16_t		*vga_buffer;
extern volatile uint8_t			vga_color;

static int	queue_is_full(void)
{
	return (((tail + 1) % CMD_QUEUE_SIZE) == head);
}

static int	queue_is_empty(void)
{
	return (head == tail);
}

void	enqueue(input_command_t cmd)
{
	if (queue_is_full())
		return ;
	cmd_queue[tail] = cmd;
	tail = (tail + 1) % CMD_QUEUE_SIZE;
}

int	dequeue(input_command_t *out)
{
	if (queue_is_empty())
		return (0);
	*out = cmd_queue[head];
	head = (head + 1) % CMD_QUEUE_SIZE;
	return (1);
}

void	keyboard_isr(unsigned char scancode)
{
	input_command_t	cmd;

	cmd.scancode = scancode & 0x7F;
	cmd.type = (scancode & 0x80) ? 1 : 0;
	enqueue(cmd);
}

void	keyboard_handler(void)
{
	uint8_t	scancode;

	scancode = inb(0x60);
	keyboard_isr(scancode);
}

int	input_poll(input_command_t *out)
{
	return (dequeue(out));
}

void	update_key_state(input_command_t *cmd)
{
	uint8_t	sc;
	uint8_t	state;
	uint8_t	pressed;

	sc = cmd->scancode;
	if (sc >= MAX_KEYS)
		return ;
	state = key_state[sc];
	pressed = (cmd->type == 0);
	if (sc == 0x3A || sc == 0x45 || sc == 0x46)
	{
		if (pressed && !(state & 0x01))
			state ^= (1 << 1);
	}
	if (pressed)
	{
		state |= (1 << 0);
		repeat.key = sc;
		repeat.counter = 0;
		repeat.active = 1;
	}
	else
	{
		state &= ~(1 << 0);
		if (repeat.key == sc)
		{
			repeat.active = 0;
		}
	}
	key_state[sc] = state;
}

void	handle_key_repeat(void)
{
	input_command_t	cmd;

	if (!repeat.active)
		return ;
	repeat.counter++;
	if (repeat.counter < REPEAT_DELAY)
		return ;
	if ((repeat.counter - REPEAT_DELAY) % REPEAT_RATE == 0)
	{
		cmd.scancode = repeat.key;
		cmd.type = 0;
		enqueue(cmd);
	}
}

int	is_key_down(uint8_t scancode)
{
	return ((scancode < MAX_KEYS) ? (key_state[scancode] & 0x01) : 0);
}

int	is_key_toggled(uint8_t scancode)
{
	return ((scancode < MAX_KEYS) ? ((key_state[scancode] >> 1) & 0x01) : 0);
}
