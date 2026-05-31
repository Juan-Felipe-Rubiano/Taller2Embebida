#include "apm32f10x.h"
#include "apm32_config.h"
/* USER CODE BEGIN Includes */
#include "system_util.h"
#include "cola.h"
#include "manejador_general.h"
/* USER CODE END Includes */


FSM f_manejo = {.name = "MANEJO", .current_state = ST_IDLE, .n=STD__N, .m = EVD__N, .table = &table_drive[0][0]};
FSM f_seguridad = {.name = "SEGURIDAD", .current_state = ST_DISARMED, .n=STS__N, .m = EVS__N, .table = &table_seguridad[0][0]};
FSM f_camaras = {.name = "CAMARAS", .current_state = ST_CAM_OFF, .n=STC__N, .m = EVC__N, .table = &table_camaras[0][0]};
FSM f_testigos = {.name = "TESTIGOS", .current_state = ST_OK, .n=STT__N, .m = EVT__N, .table = &table_testigos[0][0]};

Queue_t btn_event_queue;

int main(void) {
    // Configures clocks and selected components
    APM32_Init();
    SysTick_Init();
    
    /* USER CODE BEGIN Init */
    config();
    queue_init(&btn_event_queue);
    EventBtn cur_event;
    /* USER CODE END Init */

    while(1) {
        /* USER CODE BEGIN While */
        enqueue_btn_event(&btn_event_queue);
        if(queue_dequeue(&cur_event, &btn_event_queue)) {
            manejador_general(cur_event);
        }
        update_system();
        /* USER CODE END While */
    }
    
    return 0;
}
