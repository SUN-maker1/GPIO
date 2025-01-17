#include "GPIO.h"


/*
    该实例展示了如何配置一个GPIO，并使用中断来处理GPIO的输入。
    重点阐述一下GPIO中断的配置和使用：
    1. 在对应的IO口配置好中断触发方式
    2. 创建一个中断回调函数，用于处理中断事件 static void IRAM_ATTR gpio_isr_handler(void* arg) 
       回调函数可以指定是否在IRAM中运行获得更快的相应，参数是GPIO的编号，也就是add绑定的引脚号
    3. 为所有的GPIO注册一个中断服务 gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT); 参数是优先级
    4. 逐个为相应的GPIO与对应的中断回调函数进行绑定gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
        一旦绑定成功，当GPIO满足中断条件就会触发一次回调函数
    5. 需要的话可以移除中断服务 gpio_isr_handler_remove(GPIO_INPUT_IO_0);

    中断服务优先级：
    #define ESP_INTR_FLAG_LEVEL1        (1<<1)    // 优先级1
    #define ESP_INTR_FLAG_LEVEL2        (1<<2)    // 优先级2
    #define ESP_INTR_FLAG_LEVEL3        (1<<3)    // 优先级3
    #define ESP_INTR_FLAG_LEVEL4        (1<<4)    // 优先级4
    #define ESP_INTR_FLAG_LEVEL5        (1<<5)    // 优先级5
    #define ESP_INTR_FLAG_LEVEL6        (1<<6)    // 优先级6
    #define ESP_INTR_FLAG_NMI           (1<<7)    // 不可屏蔽中断
    #define ESP_INTR_FLAG_SHARED        (1<<8)    // 共享中断
    #define ESP_INTR_FLAG_EDGE          (1<<9)    // 边沿触发
    #define ESP_INTR_FLAG_IRAM          (1<<10)   // ISR可以在IRAM中运行
    #define ESP_INTR_FLAG_INTRDISABLED  (1<<11)   // ISR运行时禁用其他中断
    #define ESP_INTR_FLAG_DEFAULT       (ESP_INTR_FLAG_LEVEL1) // 默认使用优先级1

*/
static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {   //portMAX_DELAY表示永久等待，直到队列中有数据
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void app_main(void)
{
    GPIO_Init();
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    //remove isr handler for gpio number.
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin again
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    int cnt = 0;
    while(1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
    }
}                                                                                                                                                                                                                                                                                                                                           
