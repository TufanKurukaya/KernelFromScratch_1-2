
#ifndef KEY_H
# define KEY_H
# include <stddef.h>
# include <stdint.h>

# define CMD_QUEUE_SIZE 64
# define MAX_KEYS 128
# define REPEAT_DELAY 20
# define REPEAT_RATE 5

typedef struct
{
	unsigned char	type;
	unsigned char	scancode;
}					input_command_t;

typedef struct
{
	uint8_t			key;
	uint32_t		counter;
	uint8_t			active;
}					repeat_state_t;

extern repeat_state_t	repeat;
typedef void			(*f_key_handler_t)(void);
extern f_key_handler_t	f_keys[12];

void				keyboard_isr(uint8_t scancode);
int					input_poll(input_command_t *out);
void				update_key_state(input_command_t *cmd);
int					is_key_down(uint8_t scancode);
int					is_key_toggled(uint8_t scancode);
void				enqueue(input_command_t cmd);
void				handle_key_repeat(void);
void				keyboard_handler(void);
void				handeler_arrow(char c);
char	scancode_to_char(uint8_t scancode);

// F-key handler functions
void				f1_handler(void);
void				f2_handler(void);
void				f3_handler(void);
void				f4_handler(void);
void				f5_handler(void);
void				f6_handler(void);
void				f7_handler(void);
void				f8_handler(void);
void				f9_handler(void);
void				f10_handler(void);

#endif
