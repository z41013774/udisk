#ifndef __S3C2440_PWM_H__
#define __S3C2440_PWM_H__
#define START 		1
#define STOP		0
#define TIMER0DIV	4
#define TIMER1DIV	4
#define TIMER2DIV	4
#define TIMER3DIV	4
#define TIMER4DIV	4
#define INT_TIMER2	12
#define INT_TIMER1	11
void cfg_timer(void);
void timer0_start(void);
void timer0_stop(void);
void timer0_init(void);
void start_timer1(void);
void stop_timer1(void);
void timer1_init(void);

void timer2_init(void);
void timer2_start(void);
void timer2_stop(void);
void delay_ms(int ms);
#define wait_ms(ms)	delay_ms(ms)

#endif
