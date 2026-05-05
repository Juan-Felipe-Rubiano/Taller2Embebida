#include "apm32f10x.h"
#include "apm32_config.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/**
 * 1. ¿Por qué el microcontrolador tienen todos sus periféricos
(GPIO, TIM, ADC) desactivados por defecto? Explique la importancia del registro RCM->APB2CLKEN
    R/ El objetivo de usar una tarjeta de desarrollo como esta es utilizar su poder para realizar las funciones que deseemos.
    Dependiendo de los componentes que usemos, está a nuestra potestad y responsabilidad "activar/desactivar" los pines que necesitamos.
    Adicionalmente, es inútil mantener activados pines que no se están utilizando. Es decir, es un desperdicio de energía.
    Este registro es el encargado de manejar el reloj. En cada tick del reloj, se actualiza la instrucción a ejecutar.
    Esta se suele almacenar en el registro PC donde se encuentra la dirección de memoria de la instrucción a ejecutar.
    Si el reloj no está activo, el microcontrolador no podrá ejecutar ninguna instrucción, por lo que es necesario activar
    el reloj para que el microcontrolador funcione correctamente.
    Resolución ejercicio: En la página 29 del manual dice: RCM_APB2CLKEN registro de APB2; RCM_APB1CLKEN registro de APB1.
    En la página 10 se ve que GPIO A-E están en el bus APB2, por lo que para activar el reloj de estos pines, es necesario
    activar el bit correspondiente en el registro. 
    En la página 37-40 se muestra que PA y PB se activan con un 1 en el bit 2 y 3.
 * 2. Sabesmo que los GPIO están en APB2. En la página 79 vemos que los registros altos y bajos se configuran en GPIOx_CFGLOW/HIG
     y que cada uno tiene 32 bits. Con 8 GPIOs por puerto, eso da 4 bits por GPIO. En la página 80 vemos que los birts 1:0 definen
     el modo como entrada en 00 y salida en varias combinaciones, siendo la más rápida 11. Luego, en 3:2 configuramos el modo, que
     el que nos intersa en salida push-pull, con 00 y en entrada pull-up con 10 también.
     Por tanto setearemos PA0-9, PB8-9 U 12-15 como salida push-pull y PA10-15 como entrada pull-up.
     Por ejemplo, en los pines de datos de 7 segmentos estamos en GPIOA->CFGLOW y colocamos 0011 en cada pin o 0x3.
     Por tanto podemos pasar 0x33333333 directamente para los 8 pines. La siguiente tanda larga de pines es la de PA
     en HIG, 8-9 son salidas push-pull y 10-15 son entradas pull-up.
     Es decir:
     8-9: 0011 -> 0x3 => 0x00000033
     10-15: 1000 -> 0x8 => 0x88888800 con la excepción 13 y 14 que debemos dejar quietos
     Luego, PB tiene:
     8-9: 0011 -> 0x3 => 0x00000033
     12-15: 0011 -> 0x3 => 0x33330000 con la excepción de 10 y 11 que debemos dejar quietos
     El problema está en lo de dejar quietos porque tocaría a punta de máscaras pero el enunciado dice que sea lo más corto posible.
     Entonces la manera de dejarlos quietos sin hacer máscaras es dejarlos en lo que estaban antes manualmente, que sabesmos por esa misma
     página que es 0x4.
 * 3. Generalmente, la multiplexación dinámica trata de prender y apagar los segmentos del, valga la redundancia, display 7 segmentos doble, de manera tan
    rápida que el ojo humano no pueda distinguir el cambio y piense que ambos dígitos están encedidos simultáneamente. Una definición formal de google dice
    que es una técnica que combina múltiples señales en una sola señal a través de un canal. Recordando una investigación que hice en redes; en wifi, se utilizan diferentes
    tipos de multiplexación como OFDM y MIMO, dividiendo el canal en varias frecuencias transportadoras (que se decodifican con transformadas de Fourier) y en múltiples
    antenas, respectivamente. En nuestro caso, tenemos dos siete segmentos que comparten los pines de datos, por lo que solo se podría mostrar el mismo número en los dos
    dígitos al mismo tiempo. Lo que haremos es prender un dígito, esperar, prender el otro, pero tan rápido que nuestro cerebro lo procesará como una imagen continua, ya
    que la persistencia de la visión es de aproximadamente 1/10 a 1/16 de segundo. Justamente por eso, si el retartdo es muy alto, el cerebro se dará cuenta de que no se están 
    prendiendo ambos digitos al tiempo, pues este retardo en el cambio será mayor a la persistencia de la visión. Si por lo contrario es muy bajo, se verán muy poco los dígitos
    ya que no estarán encendidos el tiempo suficiente para que el ojo los processe y no tendrán tiempo de encenderse completamente. Para suplir la demanda de corriente, empleamos los
    transistores como interruptores, ya que estos pueden manejar corrientes más altas que los pines del microcontrolador, evitando así dañarlos.
    PS - El arreglo de números se ha desarrollado a punta de probar todo, el del tutorial no es correcto, al menos para este 7 segmentos.
 * 4. Este fenómeno ocurre ya que al oprimir un interruptor mecánico, los contactos rebotan varias veces en cuestión de milisegundos antes de llegar a la posición final.
    Esto me es más familiar en software, donde por ejemplo, cuando implementamos una barra de búsqueda que responde a la entrada del usuario, el evento se activa apenas el usuario coloca
    una letra, pero si el usuario escribe muy rápido, el evento se activa varias veces por cada letra, lo que puede causar problemas de rendimient o llamadas innecesarias a una api y por
    tanto costos. Si el usuario escribe muy rápido, el número de eventos crece rápidamente, dando una mala experiencia para el usuario. 
    Para solucionar esto mediante software en este contexto, después de detectar el cambio de estado del interruptor, se puede esperar unos ms y verificar si sigue presionado el botón, 
    llegando así a la conclusión de que el cambio de estado es real. Mediante hardware, se puede hacer uso de un condensador y una resistencia, de manera que el condensador se cargue o
    descargue lentamente, evitando así los rebotes.

 */

const uint8_t nums[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void mostrar_numero(uint8_t num);

int main(void) {
    // Configures clocks and selected components
    APM32_Init();
    
    /* USER CODE BEGIN Init */
    RCM->APB2CLKEN |= (1 << 2) | (1 << 3) | (1 << 0);
    AFIO->PCFG1 = (AFIO->PCFG1 & ~(0x7 << 24)) | (0x2 << 24); //Desactivar JTAG
    GPIOA->CFGLOW = 0x33333333; // PA0-7
    GPIOA->CFGHIG = 0x84488833; // PA8-15
    GPIOB->CFGHIG = 0x33334433; // PB8-15
    //botones
    GPIOA->ODATA |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 15);
     
    /* USER CODE END Init */

    while(1) {
        /* USER CODE BEGIN While */
        //GPIOB->ODATA ^= (1 << 2); // Toggle LED PB2
        //delay_ms(500);
        for(uint8_t i = 0; i < 100; i++)
            mostrar_numero(i);

        /* USER CODE END While */
    }
    
    return 0;
}


void mostrar_numero(uint8_t num) {
    uint8_t unidades = num % 10;
    uint8_t decenas = num /10;

    /*
    * Ponemos PB8-9 en 1 para apagar los transistores
    * y por ende los dígitos y prevenir el ghosting
     */
    GPIOB->ODATA |= (1 << 8) | (1 << 9);
    /**
     * Modificamos solo los bits necesarios y colocamos
     * la representación del número invertido por ánodo común
     */
    GPIOA->ODATA = (GPIOA->ODATA & ~0XFF) | (~nums[unidades] & 0xFF);
    //Encendemos el transistor de unidades
    GPIOB->ODATA &= ~(1 << 8);
    //Delay de 5ms para lograr engañar al cerebro
    delay_ms(5);

    GPIOB->ODATA |= (1 << 8) | (1 << 9);
    GPIOA->ODATA = (GPIOA->ODATA & ~0xFF) | (~nums[decenas] & 0xFF);
    //Encendemos el transistor de decenas
    GPIOB->ODATA &= ~(1<<9);
    delay_ms(5);


}

uint8_t debounce(uint16_t pin){
    if(!(GPIOA->IDATA & (1 << pin))){//si 0, presionado
        delay_ms(20);//La estrategia de sw
        if(!(GPIOA->IDATA & (1 << pin))) return 1;//si espichado
    }
    return 0;//no espichado
}