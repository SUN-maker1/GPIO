/*
    GPIO使用流程：
    1.定义所使用的GPIO引脚，然后用掩码进行宏定义
    2.定义GPIO的配置结构体（gpio_app_config_t），并对相应结构体成员（default_gpio_config）进行配置
    3.调用gpio_config_init()函数进行GPIO初始化
    4.调用gpio_task_example()函数进行GPIO中断处理（可有可无）

*/

/*
    GPIO.Mode 一共有6种模式
    GPIO_MODE_DISABLE                                   完全禁用GPIO引脚模式 ：用于降低功耗,当引脚不需要使用时的默认状态
    GPIO_MODE_INPUT                                     纯输入模式：   应用场景：按键输入、传感器数据读取、外部信号检测                                                         
    GPIO_MODE_OUTPUT                                    纯输出模式：   应用场景： LED控制、蜂鸣器控制、继电器控制
    GPIO_MODE_INPUT_OUTPUT                              双向模式（既可输入也可输出）应用场景：双向数据传输，SPI通信，需要读写操作的通信协议
    GPIO_MODE_OUTPUT_OD                                 开漏输出模式    应用场景：I2C通信、多设备共享总线、电平转换                     
    GPIO_MODE_INPUT_OUTPUT_OD                           开漏输入输出模式： 应用场景：双向通信总线、需要电平转换的双向通信、多主机通信系统                            
    
    特别说明：开漏(Open-Drain)模式特别适用于需要不同电压电平转换的场合
             输入输出模式可以动态切换引脚方向
    选择模式时需要考虑：通信协议要求、电压电平匹配、功耗需求、信号方向需求

    intr_type：（GPIO中断类型）
    GPIO_INTR_DISABLE：禁用中断
    GPIO_INTR_POSEDGE：上升沿触发
    GPIO_INTR_NEGEDGE：下降沿触发
    GPIO_INTR_ANYEDGE：任意边沿触发
    GPIO_INTR_LOW_LEVEL：低电平触发
    GPIO_INTR_HIGH_LEVEL：高电平触发

    对于多类型GPIO的配置，最好一类GPIO类型定义一个config结构体，配置好后分别初始化;
*/

/* gpio_set_intr_type（）：动设置GPIO中断类型 不需要配置结构体再初始化的方法  即在运动的过程种就可以改变中断触发方式
    gpio_set_intr_type(对应GPIO引脚（不用掩码）, 中断触发模式);
    GPIO_INTR_DISABLE：禁用中断
    GPIO_INTR_POSEDGE：上升沿触发
    GPIO_INTR_NEGEDGE：下降沿触发
    GPIO_INTR_ANYEDGE：任意边沿触发
    GPIO_INTR_LOW_LEVEL：低电平触发
    GPIO_INTR_HIGH_LEVEL：高电平触发
*/
#include "GPIO.h"

typedef struct {
    gpio_config_t output_conf;
    gpio_config_t input_conf;
} gpio_app_config_t;

static const gpio_app_config_t default_gpio_config = {
    .output_conf = {
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    },
    .input_conf = {
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .intr_type = GPIO_INTR_POSEDGE
    }
};

 esp_err_t gpio_config_init(const gpio_app_config_t* config)
{
    esp_err_t ret;

    // 检查参数
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // 配置输出GPIO
    ret = gpio_config(&config->output_conf);
    if (ret != ESP_OK) {
        return ret;
    }

    // 配置输入GPIO
    ret = gpio_config(&config->input_conf);
    if (ret != ESP_OK) {
        return ret;
    }

    /* 取消注释这部分代码，安装中断服务
    if (config->input_conf.intr_type != GPIO_INTR_DISABLE) {
        // 安装GPIO中断服务
        ret = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {  
            return ret;
        }
    }  */
    
    return ESP_OK;
}

void GPIO_Init(void)
{
    gpio_config_init(&default_gpio_config);
}


