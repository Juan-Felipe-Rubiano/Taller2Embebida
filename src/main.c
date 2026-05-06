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
     página que es 0x4. Aunque como los pines A13-14 son de SWDIO y SWCLK, toca mantenerlos en 8.
     NOTA IMPORTANTE: El tutorial de pinout entrega un código que modifica los valores de AFIO->PCFG1. Esto no es correcto para esta placa con este sdk.
     La página 87-89 del manual nos indica que el registro que configura la funcionalidad der JTAG es AFIO_REMAP1->SWJCFG; con un 0b010 en este registro para
     desabilitar JTAG, sin desactivar SWD, que es lo que necesitamos para comunicarnos con la placa. 
     Revisando en apm32f10x.h, encontramos la unión con la estructura, que nos permite acceder a los bits individuales del registro, por lo que podemos colocar un 0b010 o 0x2
     directamente en el campo SWJCFG sin afectar los demás bits.
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
 * 5. Finalmente, implementamos el manipulador binario, que pondrá en uso todos los elementos del circuito. Para esto, tomamos en consideración que tenemos seis leds, que podemos considerar
    ordenados de manera "little endian" siempre que visto del lado de la A en la protoboard. Es decir, el led de la izquierda (verde ahora aunque sujeto a cambio) será el MSB, mientras que el
    de la completa derecha (6ta posición y rojo) será el LSB. Sabemos que todos los leds están seguidos; entre PB12-15 - PA8-9, por lo que pensamos en empaquetar el conjunto de leds en un solo byte.
    Este se entiende de esta manera X_X_B12_B13_B14_B15_A8_A9, donde los X son valores que siempre estarán en 0 pues van más allá de nuestro conjunto de 6 leds, que solo pueden estar prendidos o apagados.
    Considerando esto, el valor máximo es 63 si los 6 bits de leds están en 1, lo que es 0x3f. Con esto, ya encontramos una forma de representar el estado completo de los leds, usando la técnica del empaquetado
    vista en clase, aunque en otro contexto. Simplemente agregamos reglas como: no pasarnos de los límites al mover el cursor (que empieza en 0), el toggle del estado de un led se hace mediante un xor de un 1 
    shifteado la posición del cursor veces, y bueno, en cada "iteración" de nuestro programa, imprimiremos el número en el 7 segmentos con la función anteriormente creada, mediante el número sacado del byte de
    empaquetado.

 */

extern volatile uint32_t msTicks;

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

uint8_t cursor = 0;
uint8_t leds_empaquetados = 0x3f; //Inciamos con todos prendidos, deberiamos ver un 63 en el 7 segmentos


int main(void) {
    // Configures clocks and selected components
    APM32_Init();
    SysTick_Init();
    
    /* USER CODE BEGIN Init */
    RCM->APB2CLKEN |= (1 << 2) | (1 << 3) | (1 << 0);
    //AFIO->PCFG1 = (AFIO->PCFG1 & ~(0x7 << 24)) | (0x2 << 24); //Desactivar JTAG Este es el del ejemplo, no sirve
    AFIO->REMAP1_B.SWJCFG = 0x2; //Desactivar JTAG real, sin afectar SWD;
    GPIOA->CFGLOW = 0x33333333; // PA0-7
    GPIOA->CFGHIG = 0x88888833; // PA8-15
    GPIOB->CFGHIG = 0x33334433; // PB8-15
    //botones
    GPIOA->ODATA |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 15);
     
    /* USER CODE END Init */

    while(1) {
        /* USER CODE BEGIN While */
        //GPIOB->ODATA ^= (1 << 2); // Toggle LED PB2
        //delay_ms(500);
        //  for(uint8_t i = 0; i < 100; i++)
        //      for(int t = 0; t < 100; t++)
        //          mostrar_numero(i);
        //mostrar_numero(63);
        manipulador_binario();
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

    GPIOB->ODATA |= (1 << 8) | (1 << 9);//apagado para prevenir ghosting


}

uint8_t debounce(uint16_t pin){
    static uint16_t estado_boton = 0xFFFF;//0 si presionado
    static uint32_t ultimo_cambio[16] = {0};
    uint8_t presionado = !(GPIOA->IDATA & (1 << pin));//1 si presionado
    uint8_t soltado = (estado_boton & (1 << pin)) != 0;
    if(presionado){
        if(soltado && (msTicks - ultimo_cambio[pin]) > 20){
            estado_boton &= ~(1 << pin);//marca como presionado
            ultimo_cambio[pin] = msTicks;
            return 1;
        }
    } else {
        if(!soltado && (msTicks - ultimo_cambio[pin]) > 20){
            estado_boton |= (1 << pin);//marca como soltado
            ultimo_cambio[pin] = msTicks;
        }
    }
    //if(!(GPIOA->IDATA & (1 << pin))){//si 0, presionado
        //delay_ms(20);
      //  if(!(GPIOA->IDATA & (1 << pin))) return 1;//si presionado
    //}
    return 0;//no espichado
}

void manejador_leds(uint8_t empaquetado){
    GPIOB->ODATA &= ~((1 << 12) | (1 << 13) | (1 << 14) | (1 << 15));
    GPIOB->ODATA |= ((empaquetado & 0x0F) << 12);

    GPIOA->ODATA &= ~((1 << 8) | (1 << 9));
    GPIOA->ODATA |= (((empaquetado >> 4) & 0x03) << 8);
}

void manipulador_binario(){
    if(debounce(10) && cursor > 0) cursor--;//izq
    if(debounce(11) && cursor < 5) cursor++;//der
    if(debounce(12)) leds_empaquetados ^= (1 << cursor);//toggle
    if(debounce(15)){//borrado
        leds_empaquetados = 0; 
        cursor = 0;
    } 

    mostrar_numero(leds_empaquetados);
    manejador_leds(leds_empaquetados);    
}