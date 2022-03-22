#ifndef __INC_COMMAND_H__
#define __INC_COMMAND_H__

#define TOKEN_EMPTY         256
#define TOKEN_SYMBOL        257
#define TOKEN_STRING        258
#define TOKEN_NUMBER        259

typedef struct  {
    const   char    *command;
    void    (*func)(void);
    const   char    *help;
}   Command_t;

extern  "C++" {
extern  void    exec_device(void);
extern  void    exec_reset(void);
extern  void    exec_schedule_control(void);
extern  void    exec_sensor(void);

extern  double  get_float(void);
extern  int     get_int(void);
extern  char    *get_word(void);
extern  int     get_token(void);
extern  void    skip_statement(void);
extern  void    skip_statement(const char *msg);
extern  void    set_input(char *s);

extern  size_t  read_input(const char *prompt, char *buff, size_t length);
}
extern  "C" {
extern  void    start_command(void);
}
#endif
