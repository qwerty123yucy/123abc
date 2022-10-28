#include <gpio.h>
#include <stdint.h>

// adopted from repository: EdwardLu2018/STM32-From-Scratch 

static volatile uint32_t led_mode = 0;

static gpio_t *const gpios[] = {(gpio_t *)GPIOA_BASE, (gpio_t *)GPIOB_BASE, (gpio_t *)GPIOC_BASE};

static gpio_port_t get_gpio_port(unsigned char pin) {
    switch(pin >> 4) {
        case 0: return GPIO_A;
        case 1: return GPIO_B;
        // case 2: return gpio_c;
        default: return GPIO_C;
    }
}

void pin_mode(pin_t pin, uint8_t mode, uint8_t up) {
    gpio_port_t gpio_port = get_gpio_port(pin);
    gpio_t *gpio = gpios[gpio_port];
    uint8_t bit = pin & PIN_MASK; // mask the input to get the pin number
    uint8_t shift_by = (bit % 8) * 4;
    uint8_t reg_idx = bit / 8; // get lo/hi control register index
    uint32_t config = gpio->cr[reg_idx] & ~(0xf << shift_by);
    gpio->cr[reg_idx] = (config | (mode << shift_by));

    if(up){
	gpio->odr |= (0x1 << pin);
    }

}

void gpio_toggle(pin_t pin) {
    gpio_port_t gpio_port = get_gpio_port(pin);
    gpio_t *gpio = gpios[gpio_port];
    gpio->odr ^= LED_MASK(pin & PIN_MASK);
}

void gio_on(pin_t pin) {
    gpio_port_t gpio_port = get_gpio_port(pin);
    gpio_t *gpio = gpios[gpio_port];
    if (pin == PC13) gpio->odr &= ~LED_MASK(pin & PIN_MASK);
    else gpio->odr |= LED_MASK(pin & PIN_MASK);
}

void gio_off(pin_t pin) {
    gpio_port_t gpio_port = get_gpio_port(pin);
    gpio_t *gpio = gpios[gpio_port];
    if (pin == PC13) gpio->odr |= LED_MASK(pin & PIN_MASK);
    else gpio->odr &= ~LED_MASK(pin & PIN_MASK);
}


void init_gpio(){
	// for usart1
	pin_mode(PC13, OUT_GP_PUSH_PULL_50, 0);
	pin_mode(PA9, OUT_ALT_PUSH_PULL_50, 0);
	pin_mode(PA10, INPUT_FLOATING_PT, 0);
	
	// for buttoms
	pin_mode(PA0, INPUT_PULL_UP_DOWN, 1);
	pin_mode(PC13, INPUT_PULL_UP_DOWN, 1);

	// for spi1
	pin_mode(PA7, OUT_ALT_PUSH_PULL_50, 0);	// PA7 for MOSI
	pin_mode(PA6, INPUT_FLOATING_PT, 0);	// PA6 for MISO
	pin_mode(PA5, OUT_ALT_PUSH_PULL_50, 0);	// PA5 for SCK
	pin_mode(PA4, OUT_ALT_PUSH_PULL_50, 0);	// PA4 for CS
	
	

}


/* 
 * leds are connected to gpio pins.
 * PB5	red
 * PB0	green
 * PB1	blue
 */

void led_trigger(){
	led_mode = (led_mode + 1) % 8;
	switch(led_mode){
		case 0:
			pin_mode(PB5, INPUT_FLOATING_PT, 0);
			pin_mode(PB0, INPUT_FLOATING_PT, 0);
			pin_mode(PB1, INPUT_FLOATING_PT, 0);
			break;
		case 1:
			pin_mode(PB5, OUT_GP_PUSH_PULL_50, 0);
			pin_mode(PB0, INPUT_FLOATING_PT, 0);
			pin_mode(PB1, INPUT_FLOATING_PT, 0);
			break;
		case 2:	
			pin_mode(PB0, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB5, INPUT_FLOATING_PT, 0);
                        pin_mode(PB1, INPUT_FLOATING_PT, 0);
                        break;
		case 3:
			pin_mode(PB1, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB5, INPUT_FLOATING_PT, 0);
                        pin_mode(PB0, INPUT_FLOATING_PT, 0);
                        break;
		case 4:
			pin_mode(PB5, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB0, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB1, INPUT_FLOATING_PT, 0);
                        break;
		case 5:
			pin_mode(PB5, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB1, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB0, INPUT_FLOATING_PT, 0);
                        break;
		case 6:
                        pin_mode(PB1, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB0, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB5, INPUT_FLOATING_PT, 0);
                        break;
		case 7:
                        pin_mode(PB5, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB1, OUT_GP_PUSH_PULL_50, 0);
                        pin_mode(PB0, OUT_GP_PUSH_PULL_50, 0);
                        break;
		default:
			pin_mode(PB5, INPUT_FLOATING_PT, 0);
                        pin_mode(PB0, INPUT_FLOATING_PT, 0);
                        pin_mode(PB1, INPUT_FLOATING_PT, 0);
                        break;
	}
	return;
}


void led_off(){
	pin_mode(PB5, INPUT_FLOATING_PT, 0);
	pin_mode(PB0, INPUT_FLOATING_PT, 0);
	pin_mode(PB1, INPUT_FLOATING_PT, 0);
	led_mode = 0;	
	return;
}
